#include "opencv2/opencv.hpp"
#include "uvs/latency_tester.h"
#include "uvs/udp_capture.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <Server Port>" << std::endl;
    exit(1);
  }
  unsigned short servPort = atoi(argv[1]);

  auto tester = LatencyTester();
  UdpCapture cap(servPort);

  cv::Mat frame;
  while (true) {
    cap >> frame;
    if (frame.empty()) {
      std::cerr << "frame is empty" << std::endl;
      continue;
    }
    // cv::imshow("Camera", frame);
    tester.feed(frame);
  }

  return 0;
}
