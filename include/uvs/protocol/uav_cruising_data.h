#pragma once

#define UAV_CRUISING_DATA_PORT 5001
#define UAV_CRUISING_STRUCT_HEADER "UavC"
#define UAV_CRUISING_STRUCT_VERSION 1

struct UavCruisingData {
  char header[4];    // header to identify the structure
  uint16_t version;  // version of the data structure

  // AircraftLocation
  double latitude;   // Latitude in degrees
  double longitude;  // Longitude in degrees
  float altitude;    // Altitude in meters

  // TakeoffLocationAltitude
  float altitude_takeoff;  // Altitude in meters

  // Attitude
  // Gets the attitude of the aircraft, where the pitch, roll, and yaw values
  // will be in the range of [-180, 180] degrees. If its pitch, roll, and yaw
  // values are 0, the aircraft will be hovering level with a True North
  // heading.
  float pitch;  // Pitch value in degrees.
  float roll;   // Roll value in degrees.
  float yaw;    // Yaw value in degrees.

  // GimbalAttitude
  // The current gimbal attitude in degrees. Roll, pitch and yaw are 0 if the
  // gimbal is level with the aircraft and points in the forward direction of
  // North Pole.
  float pitch_gimbal;  // Pitch value in degrees.
  float roll_gimbal;   // Roll value in degrees.
  float yaw_gimbal;    // Yaw value in degrees.
};
