/*
 This example describes how to use USB UVCD api with OBJECT_DETECTION.
 In this example, the device is setup to function as a USB camera.
 Connect to PC and use the device as a USB camera.
 Perform objects prediction when the device is connect to PC and PotPlayer.

 Example guide: TBD


 NN Model Selection
 Select Neural Network(NN) task and models using modelSelect(nntask, objdetmodel, facedetmodel, facerecogmodel).
 Replace with NA_MODEL if they are not necessary for your selected NN Task.

 NN task
 =======
 OBJECT_DETECTION/ FACE_DETECTION/ FACE_RECOGNITION

 Models
 =======
 YOLOv3 model         DEFAULT_YOLOV3TINY   / CUSTOMIZED_YOLOV3TINY
 YOLOv4 model         DEFAULT_YOLOV4TINY   / CUSTOMIZED_YOLOV4TINY
 YOLOv7 model         DEFAULT_YOLOV7TINY   / CUSTOMIZED_YOLOV7TINY
 SCRFD model          DEFAULT_SCRFD        / CUSTOMIZED_SCRFD
 MobileFaceNet model  DEFAULT_MOBILEFACENET/ CUSTOMIZED_MOBILEFACENET
 No model             NA_MODEL
 */

#include "StreamIO.h"
#include "VideoStream.h"
#include "UVCD.h"
#include "NNObjectDetection.h"
#include "VideoStreamOverlay.h"
#include "ObjectClassList.h"



#define STREAM_CHANNEL 0
#define CHANNELNN      3

int reconnect = 0;
int disconnect = 0;

Video camera_uvcd;
NNObjectDetection ObjDet;
UVCD usb_uvcd;

// Lower resolution for NN processing
#define NNWIDTH  1280
#define NNHEIGHT 720

StreamIO videoStreamer(1, 1);    // 1 Input Video -> 1 Output USB_CAM
StreamIO videoStreamerNN(1, 1);
VideoSetting stream_config(VIDEO_HD, 5, VIDEO_H264, 0);
VideoSetting configNN(NNWIDTH, NNHEIGHT, 5, VIDEO_RGB, 0);
VideoSetting config(VIDEO_HD, 5, VIDEO_H264, 0);
void setup()
{
    Serial1.begin(31250);
    Serial.begin(115200);
    // Configure camera video channel with video format information
    camera_uvcd.configVideoChannel(STREAM_CHANNEL, stream_config);

    camera_uvcd.configVideoChannel(CHANNELNN, configNN);
    // Configure usb_uvcd with identical video format information
    usb_uvcd.configVideo(stream_config);
    camera_uvcd.videoInit();  

    // Configure object detection with corresponding video format information
    // Select Neural Network(NN) task and models
    ObjDet.configVideo(configNN);
    ObjDet.modelSelect(OBJECT_DETECTION, CUSTOMIZED_YOLOV7TINY, NA_MODEL, NA_MODEL);
    ObjDet.begin();

    // Configure StreamIO object to stream data from camera video channel to usb_uvcd
    videoStreamer.registerInput(camera_uvcd.getStream(STREAM_CHANNEL));
    videoStreamer.registerOutput(usb_uvcd);
    if (videoStreamer.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }
    // Start data stream from video channel
    camera_uvcd.channelBegin(STREAM_CHANNEL);

    // Configure StreamIO object to stream data from RGB video channel to object detection
    videoStreamerNN.registerInput(camera_uvcd.getStream(CHANNELNN));
    videoStreamerNN.setStackSize();
    videoStreamerNN.setTaskPriority();
    videoStreamerNN.registerOutput(ObjDet);
    if (videoStreamerNN.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Start video channel for NN
    camera_uvcd.channelBegin(CHANNELNN);

    // Start usb uvcd for NN
    usb_uvcd.nnbegin(camera_uvcd.getStream(STREAM_CHANNEL), videoStreamer.linker, STREAM_CHANNEL, CHANNELNN, camera_uvcd.videostream_status(STREAM_CHANNEL));

    OSD.configVideo(STREAM_CHANNEL, config);
    OSD.begin();
}

void loop()
{
    if (false) {
        // !usb_uvcd.isUsbUvcConnected(camera_uvcd.videostream_status(STREAM_CHANNEL))
        Serial.println("USB UVC device disconnected");
        // Handle disconnection processes
        if (disconnect == 0) {
            disconnectNN();
        }
        return;
    } else {
        // Handle reconnection or restart processes
        if (reconnect == 1) {
            reconnectNN();
        }
        std::vector<ObjectDetectionResult> results = ObjDet.getResult();
        uint16_t im_h = stream_config.height();
        uint16_t im_w = stream_config.width();

        printf("Total number of objects detected = %d\r\n", ObjDet.getResultCount());
        OSD.createBitmap(STREAM_CHANNEL);
        if (ObjDet.getResultCount() > 0) {
            for (int i = 0; i < ObjDet.getResultCount(); i++) {
                int obj_type = results[i].type();
                if (itemList[obj_type].filter) {    // check if item should be ignored

                    ObjectDetectionResult item = results[i];
                    // Result coordinates are floats ranging from 0.00 to 1.00
                    // Multiply with RTSP resolution to get coordinates in pixels
                    int xmin = (int)(item.xMin() * im_w);
                    int xmax = (int)(item.xMax() * im_w);
                    int ymin = (int)(item.yMin() * im_h);
                    int ymax = (int)(item.yMax() * im_h);
                    /*
                    Serial1.println('<');
                    Serial1.println(itemList[obj_type].objectName);
                    */
                    char simple_message[25];

                    snprintf(simple_message, 25, "%d %d %d %d.", xmin, ymin, xmax, ymax);
                    Serial1.println(simple_message);
                    /*
                    Serial1.println('>');
                    */
                    // Draw boundary box
                    if (usb_uvcd.isUsbUvcConnected(camera_uvcd.videostream_status(STREAM_CHANNEL))) {
                      printf("Item %d %s:\t%d %d %d %d\n\r", i, itemList[obj_type].objectName, xmin, xmax, ymin, ymax);
                      OSD.drawRect(STREAM_CHANNEL, xmin, ymin, xmax, ymax, 3, OSD_COLOR_WHITE);
                      OSD.drawPoint(STREAM_CHANNEL, xmin, ymin, 10, OSD_COLOR_BLUE);
                      OSD.drawPoint(STREAM_CHANNEL, xmax, ymax, 10, OSD_COLOR_RED);
                    

                      // Print identification text
                      char text_str[20];
                      snprintf(text_str, sizeof(text_str), "%s %d", itemList[obj_type].objectName, item.score());
                      OSD.drawText(STREAM_CHANNEL, xmin, ymin - OSD.getTextHeight(STREAM_CHANNEL), text_str, OSD_COLOR_CYAN);

                      char top_str[20];
                      snprintf(top_str, sizeof(top_str), "%d %s %d", xmin, ", ", ymin);
                      OSD.drawText(STREAM_CHANNEL, xmin, ymin + OSD.getTextHeight(STREAM_CHANNEL), top_str, OSD_COLOR_BLUE);

                      char bot_str[20];
                      snprintf(bot_str, sizeof(bot_str), "%d %s %d", xmax, ", ", ymax);
                      OSD.drawText(STREAM_CHANNEL, xmax, ymin + OSD.getTextHeight(STREAM_CHANNEL), bot_str, OSD_COLOR_RED);
                    }
                }
              else {
                Serial1.println("-1 -1 -1 -1.");
              }
            }
        }
      else {
        Serial1.println("-1 -1 -1 -1.");
      }
    }
    OSD.update(STREAM_CHANNEL);

    // delay to wait for new results
    delay(100);
}

void disconnectNN()
{
    camera_uvcd.channelEnd(CHANNELNN);           // Stop video channel from NN
    camera_uvcd.channelBegin(STREAM_CHANNEL);    // Stop video channel from video channel
    reconnect = 1;
    disconnect = 1;
}

void reconnectNN()
{
    camera_uvcd.channelBegin(CHANNELNN);         // Start video channel for NN
    camera_uvcd.channelBegin(STREAM_CHANNEL);    // Start video channel for video channel
    reconnect = 0;
    disconnect = 0;
}
