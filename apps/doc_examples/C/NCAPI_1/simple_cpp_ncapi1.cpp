// NCAPI v1
#include <stdio.h>
#include <stdlib.h>

#include <mvnc.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "fp16.h"

typedef unsigned short half;

// Network config
#define GRAPH_FILEPATH "graph"
const int networkDim = 224;
float networkMean[] = {0.40787054*255.0, 0.45752458*255.0, 0.48109378*255.0};

void* loadGraphFromFile(char* graphFilepath, unsigned int* graphBufferLength) {
    FILE* fp;
    char* graphBuffer;

    // Open the file
    fp = fopen(graphFilepath, "rb");
    if (fp == NULL) return 0;

    // Get the length
    fseek(fp, 0, SEEK_END);
    *graphBufferLength = ftell(fp);
    rewind(fp);

    // Allocate a data buffer
    if (!(graphBuffer = (char*)malloc(*graphBufferLength))) {
        fclose(fp);
        return 0;
    }

    // Read data into the buffer
    if (fread(graphBuffer, 1, *graphBufferLength, fp) != *graphBufferLength) {
        fclose(fp);
        free(graphBuffer);
        return 0;
    }

    fclose(fp);
    
    return graphBuffer;
}

half* loadImageFromFile(const char *path, int reqSize, float *mean)
{
    int width, height, cp, i;
    unsigned char *img, *imgresized;
    float *imgfp32;
    half *imgfp16;

    img = stbi_load(path, &width, &height, &cp, 3);
    if(!img)
    {
        printf("Error - the image file %s could not be loaded\n", path);
        return NULL;
    }
    imgresized = (unsigned char*) malloc(3*reqSize*reqSize);
    if(!imgresized)
    {
        free(img);
        perror("malloc");
        return NULL;
    }
    stbir_resize_uint8(img, width, height, 0, imgresized, reqSize, reqSize, 0, 3);
    free(img);
    imgfp32 = (float*) malloc(sizeof(*imgfp32) * reqSize * reqSize * 3);
    if(!imgfp32)
    {
        free(imgresized);
        perror("malloc");
        return NULL;
    }
    for(i = 0; i < reqSize * reqSize * 3; i++)
        imgfp32[i] = imgresized[i];
    free(imgresized);
    imgfp16 = (half*) malloc(sizeof(*imgfp16) * reqSize * reqSize * 3);
    if(!imgfp16)
    {
        free(imgfp32);
        perror("malloc");
        return NULL;
    }
    for(i = 0; i < reqSize*reqSize; i++)
    {
        float blue, green, red;
                blue = imgfp32[3*i+2];
                green = imgfp32[3*i+1];
                red = imgfp32[3*i+0];

                imgfp32[3*i+0] = blue-mean[0];
                imgfp32[3*i+1] = green-mean[1];
                imgfp32[3*i+2] = red-mean[2];
    }
    floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqSize*reqSize);
    free(imgfp32);
    return imgfp16;
}

int main() {
    // You can check that this return code is MVNC_OK after each API function call
    // This is omitted in this example for better readability
    mvncStatus retCode;

    // Initialize and open a device
    const unsigned int NAME_SIZE = 100;
    char deviceName[NAME_SIZE];
    void* deviceHandle;
    retCode = mvncGetDeviceName(0, deviceName, NAME_SIZE);
    retCode = mvncOpenDevice(deviceName, &deviceHandle);

    // Load the graph from file
    unsigned int graphBufferLength;
    void* graphBuffer = loadGraphFromFile(GRAPH_FILEPATH, &graphBufferLength);

    // Allocate the graph to the device
    void* graphHandle;
    retCode = mvncAllocateGraph(deviceHandle, &graphHandle, graphBuffer, graphBufferLength);
    free(graphBuffer);

    // Read and preprocess input
    half* imageBuffer = loadImageFromFile("cat.jpg", networkDim, networkMean);
    unsigned int imageBufferLength = 3 * networkDim * networkDim * sizeof(*imageBuffer);

    // Load the image to the device
    retCode = mvncLoadTensor(graphHandle, imageBuffer, imageBufferLength, NULL);
    free(imageBuffer);

    // Get the results from the device
    void* resultData16;
    void* userParam;
    unsigned int resultDataLength;
    retCode = mvncGetResult(graphHandle, &resultData16, &resultDataLength, &userParam);

    // Do something with the results...
    int numResults = resultDataLength / sizeof(half);
    float* resultData32;
    resultData32 = (float*)malloc(numResults * sizeof(*resultData32));
    fp16tofloat(resultData32, (unsigned char*)resultData16, numResults);
    float maxResult = 0.0;
    int maxIndex = -1;
    for (int index = 0; index < numResults; index++)
    {
        // printf("Category %d is: %f\n", index, resultData32[index]);
        if (resultData32[index] > maxResult)
        {
            maxResult = resultData32[index];
            maxIndex = index;
        }
    }
    printf("Index of top result is: %d\n", maxIndex);
    printf("Probability of top result is: %f\n", resultData32[maxIndex]);

    // Clean up
    retCode = mvncDeallocateGraph(graphHandle);
    graphHandle = NULL;
    retCode = mvncCloseDevice(deviceHandle);
    deviceHandle = NULL;
}
