#include "RemoteViewer.h"
#include "utilities.h"
#include "types.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

using namespace std;
using namespace cv;

void RemoteViewer::initSocket() {
    sockaddr_in serv_addr;

    mSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocketFD < 0) {
        error("Failed to open new socket file descriptor");
    }

    // initialize the socket of this machine
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_NUM);

    if (bind(mSocketFD, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
}

void RemoteViewer::listenClient() {
	int clientSocket;
    socklen_t client;
    sockaddr_in cli_addr;

    listen(mSocketFD, 1);

    client = sizeof(cli_addr);
	clientSocket = accept(mSocketFD, (sockaddr *)&cli_addr, &client);
	if (clientSocket < 0) {
		error("ERROR on accept");
	}

	cout << "Client is connected" << endl;

    clientSession(clientSocket);

    close(clientSocket);
}

void RemoteViewer::clientSession(int &clientSocket) {
    bool run = true;
    char small_buff[SMALL_BUF_SIZE];
	int num_bytes;
    String windowName = "Default View";

    // Initialize json object for communication
    json_object *jobj = json_object_new_object();
    json_object *jval = json_object_new_string("Successively connected!");
    json_object_object_add(jobj, "welcome", jval);

	if (strcpy(small_buff, json_object_to_json_string(jobj)) == NULL) {
        error("ERROR copy json object");
	}

	if (send(clientSocket, small_buff, strlen(small_buff), 0) == -1) {
		error("ERROR write json object");
	}

    while (run) {
		if ((num_bytes = read(clientSocket, small_buff, SMALL_BUF_SIZE)) <= 0) {
			cout << "No byte received" << endl;
			break;
		}

		int width = 0, height = 0, bytes_to_receive = 0;
		int request_code = parseRequest(small_buff);
		Mat image;

		switch (request_code) {
		case JSON_REQUEST_STREAMING:
			parseImageData(small_buff, width, height, bytes_to_receive);

			sendAcknowledgement(clientSocket, JSON_VALUE_OK);

			image = receiveImageData(clientSocket, bytes_to_receive);

			imshow(windowName, image);
			waitKey(10);

			sendAcknowledgement(clientSocket, JSON_VALUE_OK);

            break;

		case JSON_REQUEST_DISCONNECT:
			run = false;
			break;

		default:
			cout << "No request received" << endl;
			break;
        }
    }
}

void RemoteViewer::closeSocket() {
	close(mSocketFD);
}

int RemoteViewer::parseRequest(char *buffer) {
	json_object *jobj = json_tokener_parse(buffer);
	cout << buffer << endl;
	int request = -1;
	json_object_object_foreach(jobj, key, val) {
		if (strcmp(key, JSON_KEY_REQUEST) == 0) {
			request = json_object_get_int(val);
		}
	}

	return request;
}

void RemoteViewer::parseImageData(char *buffer, int &width, int &height, int &bytes) {
	json_object *jobj = json_tokener_parse(buffer);

	json_object_object_foreach(jobj, key, val) {
        // parse given data
		if (strcmp(key, JSON_KEY_BYTES) == 0) {
			bytes = json_object_get_int(val);
		}
		else if (strcmp(key, JSON_KEY_WIDTH) == 0) {
			width = json_object_get_int(val);

		}
		else if (strcmp(key, JSON_KEY_HEIGHT) == 0) {
			height = json_object_get_int(val);
		}
	}
}

void RemoteViewer::sendAcknowledgement(int clientSocket, int code) {
    char temp_buff[1024];
    json_object *jobj = json_object_new_object();
	json_object *jval = json_object_new_int(code);
    json_object_object_add(jobj, JSON_KEY_ACK, jval);

	if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL) {
        error("ERROR copy json object");
	}

	// send an acknowledgement
	if (send(clientSocket, temp_buff, strlen(temp_buff), 0) == -1) {
		error("ERROR write json object");
	}
}

int RemoteViewer::receiveData(int clientSocket, char *buffer, int buf_size) {
	int offset = 0, num_bytes = 0;
	while ((num_bytes = recv(clientSocket, buffer + offset, buf_size - offset, 0)) > 0) {
		cout << "Received " << num_bytes << "bytes." << endl;
		offset += num_bytes;
		if (offset >= buf_size) {
			break;
		}
	}
	return offset;
}

Mat RemoteViewer::receiveImageData(int clientSocket, int size) {
	int offset = 0, num_bytes = 0;
	uchar *imageBuffer = new uchar[size];
    char large_buff[LARGE_BUF_SIZE];
	memset(imageBuffer, 0, size);

	// receive an image data
	while ((num_bytes = read(clientSocket, large_buff, LARGE_BUF_SIZE)) > 0) {
		memcpy(imageBuffer + offset, large_buff, num_bytes);
		offset += num_bytes;
		if (offset >= size) {
			break;
		}
	}

	cout << "received " << size << "bytes as a image data" << endl;

	vector<uchar> bytesBuffer(imageBuffer, imageBuffer + size);

	Mat image = imdecode(Mat(1, size, CV_8UC1, imageBuffer), CV_LOAD_IMAGE_UNCHANGED);

	delete[] imageBuffer;

	return image;
}

void RemoteViewer::InternalThreadEntry() {
    initSocket();

    listenClient();

    closeSocket();
}

RemoteViewer::RemoteViewer(String windowName) : mWindowName(windowName) {
	
}