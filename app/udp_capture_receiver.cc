#include "opencv2/opencv.hpp"
#include "udp_capture.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <Server Port>" << std::endl;
    exit(1);
  }

  unsigned short servPort = atoi(argv[1]);
  cv::namedWindow("recv", cv::WINDOW_AUTOSIZE);
  try {
    UdpCapture cap(servPort);
    cv::Mat frame;
    while (true) {
      cap >> frame;
      if (!frame.empty()) {
        cv::imshow("recv", frame);
        cv::waitKey(1);
      }
    }
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  return 0;
}
