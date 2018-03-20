// NCAPI v2
#include <mvnc.h>

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

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

float* loadImageFromFile(const char *path, int reqsize, float *mean) {
	int width, height, cp, i;
	unsigned char *img, *imgresized;
	float *imgfp32;

	img = stbi_load(path, &width, &height, &cp, 3);
	if(!img) {
		printf("The picture %s could not be loaded\n", path);
		return 0;
	}
	imgresized = (unsigned char*) malloc(3*reqsize*reqsize);
	if(!imgresized) {
		free(img);
		perror("malloc");
		return 0;
	}
	stbir_resize_uint8(img, width, height, 0, imgresized, reqsize, reqsize, 0, 3);
	free(img);
	imgfp32 = (float*) malloc(sizeof(*imgfp32) * reqsize * reqsize * 3);
	if(!imgfp32) {
		free(imgresized);
		perror("malloc");
		return 0;
	}
	for(i = 0; i < reqsize * reqsize * 3; i++) {
		imgfp32[i] = imgresized[i];
	}
	free(imgresized);
	for(i = 0; i < reqsize*reqsize; i++) {
		float blue, green, red;
		blue = imgfp32[3*i+2];
		green = imgfp32[3*i+1];
		red = imgfp32[3*i+0];

		imgfp32[3*i+0] = blue-mean[0];
		imgfp32[3*i+1] = green-mean[1];
		imgfp32[3*i+2] = red-mean[2];
	}
	return imgfp32;
}

int main() {
    // You can check that this return code is equal to NC_OK after each API function call
    // This is omitted in this example for better readability
    ncStatus_t retCode;

    // Initialize and open a device
    struct ncDeviceHandle_t* deviceHandle;
    retCode = ncDeviceInit(0, &deviceHandle);
    retCode = ncDeviceOpen(deviceHandle);

    // Load a graph from file at some GRAPH_FILEPATH
    unsigned int graphBufferLength;
    void* graphBuffer = loadGraphFromFile(GRAPH_FILEPATH, &graphBufferLength);

    // Initialize and allocate the graph to the device
    struct ncGraphHandle_t* graphHandle;
    retCode = ncGraphInit("my graph", &graphHandle);
    retCode = ncGraphAllocate(deviceHandle, graphHandle, graphBuffer, graphBufferLength);
    free(graphBuffer);
    
    // Get the graph TensorDescriptor structs (they describe expected graph input/output)
    struct ncTensorDescriptor_t *inputDescriptor;
    struct ncTensorDescriptor_t *outputDescriptor;
    unsigned int length;
    ncGraphGetOption(graphHandle, NC_RO_GRAPH_INPUT_TENSOR_DESCRIPTORS, &inputDescriptor, &length);
    ncGraphGetOption(graphHandle, NC_RO_GRAPH_OUTPUT_TENSOR_DESCRIPTORS, &outputDescriptor, &length);
    
    // Create input/output FIFOs
    struct ncFifoHandle_t* inputFIFO;
    struct ncFifoHandle_t* outputFIFO;
    int datatype = NC_FIFO_FP32;
    retCode = ncFifoInit(NC_FIFO_HOST_WO, &inputFIFO);
    retCode = ncFifoInit(NC_FIFO_HOST_RO, &outputFIFO);
    retCode = ncFifoSetOption(inputFIFO, NC_RW_FIFO_DATA_TYPE, &datatype, sizeof(datatype)); // optional, if needed
    retCode = ncFifoSetOption(outputFIFO, NC_RW_FIFO_DATA_TYPE, &datatype, sizeof(datatype)); // optional, if needed
    retCode = ncFifoCreate(inputFIFO, deviceHandle, inputDescriptor, 2);
    retCode = ncFifoCreate(outputFIFO, deviceHandle, outputDescriptor, 2);

    // Read and preprocess input
    float* imageBuffer = loadImageFromFile("cat.jpg", networkDim, networkMean);
    unsigned int imageBufferLength = 3 * networkDim * networkDim * sizeof(*imageBuffer);

    // Write the image to the input FIFO
    retCode = ncFifoWriteElem(inputFIFO, imageBuffer, inputDescriptor, 0);
    free(imageBuffer);
    
    // Queue the inference
    retCode = ncGraphQueueInference(graphHandle, &inputFIFO, &outputFIFO);

    // Get the results from the output FIFO
    void* result;
    void* userParam;
    struct ncTensorDescriptor_t resultDescriptor;
    retCode = ncFifoReadElem(outputFIFO, &result, &resultDescriptor, &userParam);

    // Do something with the results...
    int dataTypeSize = outputDescriptor->totalSize/( outputDescriptor->w* outputDescriptor->h*outputDescriptor->c*outputDescriptor->n);
    unsigned int numResults = resultDescriptor.totalSize / dataTypeSize;
    printf("resultData length is %d \n", numResults);
    float *fresult = (float*) result;
    float maxResult = 0.0;
    int maxIndex = -1;
    for (int index = 0; index < numResults; index++) {
        if (fresult[index] > maxResult) {
            maxResult = fresult[index];
            maxIndex = index;
        }
    }
    printf("Index of top result is: %d\n", maxIndex);
    printf("Probability of top result is: %f\n", fresult[maxIndex]);

    // Clean up
    retCode = ncFifoDelete(inputFIFO);
    inputFIFO = NULL;
    retCode = ncFifoDelete(outputFIFO);
    outputFIFO = NULL;
    retCode = ncGraphDeallocate(graphHandle);
    graphHandle = NULL;
    retCode = ncDeviceClose(deviceHandle);
    deviceHandle = NULL;
}
