#include <iostream>
#include <sstream>

#include "opencv2/opencv.hpp"
#include "uvs/legacy/udp_capture.h"

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
    //    int frame_count = 0;
    while (true) {
      cap >> frame;
      if (!frame.empty()) {
        cv::imshow("recv", frame);
        cv::waitKey(1);
        //        std::ostringstream filename;
        //        filename << std::setw(3) << std::setfill('0') << frame_count;
        //        cv::imwrite(filename.str() + ".png", frame);
        //        frame_count += 1;
      }
    }
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  return 0;
}
