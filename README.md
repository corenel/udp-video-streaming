# udp-video-streaming
Simple project for streaming video frames by UDP.

## Usage

Two classes:

1. `UdpCapture`
2. `UdpWriter`

## Configs

In `include/config.h`:

- `PACK_SIZE`: packet size for spliting encoded image bytes
- `PACKET_SPLIT_METHOD`: method for defining packets of an image
  - `PACKET_SPLIT_NUMBER`: By sending the number of packets first
  - `PACKET_SPLIT_BEGIN_END_FLAG`: By sending begining and ending flags 

## Examples

1. Receving Images over UDP: `app/udp_capture_receiver.cpp`
2. Sending Images over UDP: `app/udp_writer_sender.cpp`

