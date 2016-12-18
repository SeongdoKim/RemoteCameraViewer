#pragma once

#include "ThreadClass.h"

#include <opencv2/opencv.hpp>
#include <json/json.h>

class RemoteViewer : public ThreadClass {
private:

    static const int PORT_NUM = 1050;

    static const int SMALL_BUF_SIZE = 1024;

    static const int LARGE_BUF_SIZE = 8192;

    /**
     * The name of output window shared by the main thread
     */
    const cv::String mWindowName;

    /**
     * Socket file descriptor, if any
     */
    int mSocketFD = 0;

    /**
     * Initialize the socket
     */
    void initSocket();

    /**
     * Wait until connection request received
     */
    void listenClient();

    /**
     * Close the socket
     */
    void closeSocket();

    /**
     * Client session to communicate with connected client
     */
    void clientSession(int &clientSocket);

    /**
     * Parse the given buffer containing a request code.
     * @param buffer A pointer of {@link json_object}.
     * @return parsed request code.
     */
    int parseRequest(char *buffer);

    /**
     * Parse the given buffer containing a image data.
     * @param buffer A pointer of {@link json_object}.
     * @param width Width of the target image.
     * @param height Height of the target image.
     * @param bytes The number of bytes representing the target image.
     */
    void parseImageData(char *buffer, int &width, int &height, int &bytes);

    /**
     * Send an acknowledge message to the given client.
     * @param clientSocket A connected client to send an acknowledge.
     * @param code An acknowledge code defined in {@code types.h}.
     */
    void sendAcknowledgement(int clientSocket, int code);

    /**
     * Receive an unknown amount of data from the given client.
     * @param clientSocket A connected client to receive data.
     * @param buffer A buffer to store the received data.
     * @param buf_size Size of the buffer.
     * @return The number of received bytes.
     */
    int receiveData(int clientSocket, char *buffer, int buf_size);

    cv::Mat receiveImageData(int clientSocket, int size);

protected:
    void InternalThreadEntry();

public:
    /**
     * Basic constructor
     * @param windowName The name of window shared with the main thread
     */
    RemoteViewer(cv::String windowName);
};