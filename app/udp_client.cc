/*
 *   C++ UDP socket client for live image upstreaming
 *   Modified from
 * http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoClient.cpp
 *   Copyright (C) 2015
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdlib>   // For atoi()
#include <iostream>  // For std::cout and std::cerr

#include "opencv2/opencv.hpp"
#include "uvs/util/config.h"
#include "uvs/util/practical_socket.h"  // For UDPSocket and SocketException

#define USE_WEBCAM false

int main(int argc, char* argv[]) {
  if ((argc < 3) || (argc > 3)) {  // Test for correct number of arguments
    std::cerr << "Usage: " << argv[0] << " <Server> <Server Port>\n";
    exit(1);
  }

  std::string servAddress = argv[1];  // First arg: server address
  unsigned short servPort = Socket::resolveService(argv[2], "udp");

  try {
    UDPSocket sock;
    int jpegqual = ENCODE_QUALITY;  // Compression Parameter

    cv::Mat frame, send;
    std::vector<uchar> encoded;
    cv::namedWindow("send", cv::WINDOW_AUTOSIZE);

#if USE_WEBCAM
    cv::VideoCapture cap(0);  // Grab the camera
    if (!cap.isOpened()) {
      std::cerr << "OpenCV Failed to open camera";
      exit(1);
    }
#endif

    clock_t last_cycle = clock();
    while (true) {
#if USE_WEBCAM
      cap >> frame;
#else
      frame = cv::Mat(1920, 1080, CV_8UC3);
      cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
#endif
      if (frame.size().width == 0)
        continue;  // simple integrity check; skip erroneous data...
      cv::resize(frame, send, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0,
             cv::INTER_LINEAR);
      std::vector<int> compression_params;
      compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
      compression_params.push_back(jpegqual);

      cv::imencode(".jpg", send, encoded, compression_params);
      cv::imshow("send", send);
      int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

      int ibuf[1];
      ibuf[0] = total_pack;
      sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

      for (int i = 0; i < total_pack; i++)
        sock.sendTo(&encoded[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);

      cv::waitKey(FRAME_INTERVAL);

      clock_t next_cycle = clock();
      double duration = static_cast<double>(next_cycle - last_cycle) /
                        static_cast<double>(CLOCKS_PER_SEC);
      std::cout << "\teffective FPS:" << (1 / duration) << " \tMbps:"
                << (PACK_SIZE * total_pack / duration / 1024 / 1024 * 8)
                << std::endl;

      std::cout << next_cycle - last_cycle;
      last_cycle = next_cycle;
    }
    // Destructor closes the socket

  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  return 0;
}
