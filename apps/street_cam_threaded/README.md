# Introduction
The street_cam_threaded application is an object recognition and classification program.  It uses two or more Neural Compute devices (Intel Neural Compute Stick) along with TinyYolo and GoogLeNet to process a video stream such that each frame in the video first gets bounding boxes for objects via TinyYolo then GoogLeNet is used to further classify the objects found.  It requires the caffe/TinyYolo and caffe/GoogLeNet neural networks in the ncappzoo, but will automatically make these as part of the project Makefile.  It is an improvement on the street_cam application in that it uses a threaded approach so that frames with multiple objects in them can be classified faster.

The provided Makefile does the following:
1. Builds both TinyYolo and GoogLeNet from their respective directories within the repo.
2. Copies the built NCS graph files from TinyYolo and GoogLeNet to the current directory.
3. Runs the provided program that creates a GUI window that shows the camera stream along with boxes around the identified objects. 

# Prerequisites
This program requires:
- 2 or more Neural Compute devices such as [the Intel® Movidius™ NCS](https://developer.movidius.com/buy)
- NCSDK 1.09 or greater
- opencv 3.3 with video for linux support

Note: The OpenCV version that installs with some earlier ncsdk installations does <strong>not</strong> provide V4L support.  To run this application you may need to replace the ncsdk installed opencv with a version built from source.  To remove the old opencv and build and install a compatible version you can run the following command from the app's base directory:
```
   make opencv
```   
Note: All development and testing has been done on Ubuntu 16.04 on an x86-64 machine.


# How it Works
At least two NCS devices are needed to run this application, one executes inferences for the Tiny Yolo network and the rest execute inferences for the googlenet network.  OpenCV is used to open a stored video (or easily modifiable to use a camera stream.) For each frame of video that is processed, the program first runs a Tiny Yolo inference to find all objects in the image.  This will be limited to the 20 categories that Tiny Yolo recognizes.  Those categories can be seen in the code in this line:

```python
   network_classifications = ["aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car",
                               "cat", "chair", "cow", "diningtable", "dog", "horse", "motorbike",
                               "person", "pottedplant", "sheep", "sofa", "train","tvmonitor"]
```

Then for each Tiny Yolo object in the image, the program crops out the bounding rectangle and passes that smaller image to googlenet for a more detailed classification. The program will then display the original frame along with boxes around each detected object and its detailed classification if one was provided by googlenet with sufficient probability.  If GoogLeNet doesn't provide a high enough probability classification then the original Tiny Yolo classification will be used.

# Algorithm Thresholds
There are a few thresholds in the code you may want to tweek if you aren't getting results that you expect:
- <strong>TY_INITIAL_BOX_PROBABILITY_THRESHOLD </strong>: This is the minimum probability allowed for boxes returned from tiny yolo.  This should be between 0.0 and 1.0.  A lower value will allow more boxes to be displayed.
- <strong>TY_INITIAL_MAX_IOU </strong>: Determines which boxes from Tiny Yolo should be separate objects vs identifying the same object.  This is based on the intersection-over-union calculation.  The closer this is to 1.0 the more similar the boxes need to be in order to be considered around the same object.
- <strong>GN_PROBABILITY_MIN</strong>:  This is the minimum probability from googlenet that will be used to override the general tiny yolo classification with a more specific googlenet classification.  It should be between 0.0 and 1.0.  A value of 0.0 will override every tiny yolo classification with the googlenet classification. 

You can modify these thresholds at runtime with hotkeys to see how changes to the values affect the object identification.  

The following hotkeys are available in the program.
* 'B' increase TY_BOX_PROBABILITY_THRESHOLD
* 'b' decrease TY_BOX_PROBABILITY_THRESHOLD
* 'I' increase TY_MAX_IOU
* 'i' decrease TY_MAX_IOU
* 'G' increase GN_PROBABILITY_MIN
* 'g' decrease GN_PROBABILITY_MIN
* '2' toggle the GoogLeNet processing.
* 'q' quit program

# Makefile
Provided Makefile has various targets that help with the above mentioned tasks.

## make help
Shows available targets.

## make all
Builds and/or gathers all the required files needed to run the application except building and installing opencv.  This can be done as a separate step with make opencv if required.  

## make opencv
Removes the version of OpenCV that was installed with the NCSDK and builds and installs a compatible version of OpenCV 3.3 for this app. This will take a while to finish. Once you have done this on your system you shouldn't need to do it again.

## make run_py
Runs the provided python program which shows the camera stream along with the object boxes and classifications.

## make clean
Removes all the temporary files that are created by the Makefile
