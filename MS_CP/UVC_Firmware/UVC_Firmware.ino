#include "VideoStream.h"

// Configure: Resolution, FPS, Encoding (H264), No Audio
VideoSetting config(VIDEO_FHD, 30, VIDEO_H264, 0);

void setup() {
  VideoStream.begin(config);  // Correct object name
}

void loop() {
  // Nothing needed here
}
