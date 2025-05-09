#ifndef __OBJECTCLASSLIST_H__
#define __OBJECTCLASSLIST_H__

struct ObjectDetectionItem {
    uint8_t index;
    const char* objectName;
    uint8_t filter;
};

// List of objects the pre-trained model is capable of recognizing
// Index number is fixed and hard-coded from training
// Set the filter value to 0 to ignore any recognized objects
ObjectDetectionItem itemList[3] = {
  {0, "Ball", 1},
  {1, "Red Car", 1},
  {2, "Solder", 1}
};

ObjectDetectionItem itemListprevious[80] = {
    {0,  "person",         0},
    {1,  "bicycle",        1},
    {2,  "car",            1},
    {3,  "motorbike",      1},
    {4,  "aeroplane",      1},
    {5,  "bus",            1},
    {6,  "train",          1},
    {7,  "truck",          1},
    {8,  "boat",           1},
    {9,  "traffic light",  1},
    {10, "fire hydrant",   1},
    {11, "stop sign",      1},
    {12, "parking meter",  1},
    {13, "bench",          1},
    {14, "bird",           1},
    {15, "cat",            1},
    {16, "dog",            1},
    {17, "horse",          1},
    {18, "sheep",          1},
    {19, "cow",            1},
    {20, "elephant",       1},
    {21, "bear",           1},
    {22, "zebra",          1},
    {23, "giraffe",        1},
    {24, "backpack",       1},
    {25, "umbrella",       1},
    {26, "handbag",        1},
    {27, "tie",            1},
    {28, "suitcase",       1},
    {29, "frisbee",        1},
    {30, "skis",           1},
    {31, "snowboard",      1},
    {32, "sports ball",    1},
    {33, "kite",           1},
    {34, "baseball bat",   1},
    {35, "baseball glove", 1},
    {36, "skateboard",     1},
    {37, "surfboard",      1},
    {38, "tennis racket",  1},
    {39, "bottle",         1},
    {40, "wine glass",     1},
    {41, "cup",            1},
    {42, "fork",           1},
    {43, "knife",          1},
    {44, "spoon",          1},
    {45, "bowl",           1},
    {46, "banana",         1},
    {47, "apple",          1},
    {48, "sandwich",       1},
    {49, "orange",         1},
    {50, "broccoli",       1},
    {51, "carrot",         1},
    {52, "hot dog",        1},
    {53, "pizza",          1},
    {54, "donut",          1},
    {55, "cake",           1},
    {56, "chair",          0},
    {57, "sofa",           1},
    {58, "pottedplant",    1},
    {59, "bed",            1},
    {60, "diningtable",    1},
    {61, "toilet",         1},
    {62, "tvmonitor",      0},
    {63, "laptop",         1},
    {64, "mouse",          0},
    {65, "remote",         1},
    {66, "keyboard",       0},
    {67, "cell phone",     1},
    {68, "microwave",      1},
    {69, "oven",           1},
    {70, "toaster",        1},
    {71, "sink",           1},
    {72, "refrigerator",   1},
    {73, "book",           1},
    {74, "clock",          1},
    {75, "vase",           1},
    {76, "scissors",       1},
    {77, "teddy bear",     1},
    {78, "hair dryer",     1},
    {79, "toothbrush",     1},
};

#endif
