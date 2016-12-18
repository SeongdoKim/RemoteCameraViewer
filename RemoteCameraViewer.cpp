#include <iostream>

#include "RemoteViewer.h"

using namespace std;

int main() {
    cv::String windowName = "Default View";
    RemoteViewer remoteViewer(windowName);

    // The output window must be owned by the main thread
    cv::namedWindow(windowName);

    if (!remoteViewer.start()) {
        cout << "Failed to initialize the network thread" << endl;
        return 0;
    }

    remoteViewer.join();

    cv::destroyWindow(windowName);
}