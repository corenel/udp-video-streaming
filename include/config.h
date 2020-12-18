#pragma once

#define FRAME_HEIGHT 720
#define FRAME_WIDTH 1280
#define FRAME_INTERVAL (1000 / 30)
#define PACK_SIZE 4096  // udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

#define PACKET_SPLIT_BEGIN_END_FLAG 0
#define PACKET_SPLIT_NUMBER 1
#define PACKET_SPLIT_METHOD PPACKET_SPLIT_NUMBER