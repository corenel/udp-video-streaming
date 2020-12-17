/*
 *   C++ UDP socket server for live image upstreaming
 *   Modified from
 * http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoServer.cpp
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

#include "config.h"
#include "opencv2/opencv.hpp"
#include "practical_socket.h"  // For UDPSocket and SocketException

#define BUF_LEN 65540  // Larger than maximum UDP packet size

int main(int argc, char* argv[]) {
  if (argc != 2) {  // Test for correct number of parameters
    std::cerr << "Usage: " << argv[0] << " <Server Port>" << std::endl;
    exit(1);
  }

  unsigned short servPort = atoi(argv[1]);  // First arg:  local port

  namedWindow("recv", cv::WINDOW_AUTOSIZE);
  try {
    UDPSocket sock(servPort);

    char buffer[BUF_LEN];       // Buffer for echo string
    int recvMsgSize;            // Size of received message
    std::string sourceAddress;  // Address of datagram source
    unsigned short sourcePort;  // Port of datagram source

    clock_t last_cycle = clock();

    while (true) {
      // Block until receive message from a client
      do {
        recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
      } while (static_cast<unsigned>(recvMsgSize) > sizeof(int));
      int total_pack = ((int*)buffer)[0];

      std::cout << "expecting length of packs:" << total_pack << std::endl;
      char* longbuf = new char[PACK_SIZE * total_pack];
      for (int i = 0; i < total_pack; i++) {
        recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
        if (recvMsgSize != PACK_SIZE) {
          std::cerr << "Received unexpected size pack:" << recvMsgSize
                    << std::endl;
          continue;
        }
        memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
      }

      std::cout << "Received packet from " << sourceAddress << ":" << sourcePort
                << std::endl;

      cv::Mat rawData = cv::Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
      cv::Mat frame = imdecode(rawData, cv::IMREAD_COLOR);
      if (frame.size().width == 0) {
        std::cerr << "decode failure!" << std::endl;
        continue;
      }
      cv::imshow("recv", frame);
      free(longbuf);

      cv::waitKey(1);
      clock_t next_cycle = clock();
      double duration = static_cast<double>(next_cycle - last_cycle) /
                        static_cast<double>(CLOCKS_PER_SEC);
      std::cout << "\teffective FPS:" << (1 / duration) << " \tMbps:"
                << (PACK_SIZE * total_pack / duration / 1024 / 1024 * 8)
                << std::endl;

      //      std::cout << next_cycle - last_cycle << std::endl;
      last_cycle = next_cycle;
    }
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  return 0;
}
