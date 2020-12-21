#pragma once

#define IMAGE_STREAMING_DATA_PORT 5000
#define IMAGE_STREAMING_STRUCT_HEADER "IStr"
#define IMAGE_STREAMING_STRUCT_VERSION 1

struct ImageStreamingData {
  char header[4];        // header to identify the structure
  uint16_t version;      // version of the data structure
  uint8_t total_number;  // number of packets in an entire image
};