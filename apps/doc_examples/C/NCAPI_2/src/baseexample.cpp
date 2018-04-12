// NCAPI v2
#include <mvnc.h>

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../include/stb_image_resize.h"

#define IMAGE_FILEPATH "../misc/cat.jpg"
#define GRAPH_FILEPATH "../misc/graph"
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
    // Check that this return code is equal to NC_OK after each API function call
    ncStatus_t retCode;

    // Create a device handle
    struct ncDeviceHandle_t* deviceHandle;
    retCode = ncDeviceCreate(0, &deviceHandle);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not create device.\n", retCode);
        exit(-1);
    }
    
    // Open communication with the device
    retCode = ncDeviceOpen(deviceHandle);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not open device.\n", retCode);
        exit(-1);
    }

    // Load a graph from file at some GRAPH_FILEPATH
    unsigned int graphBufferLength;
    void* graphBuffer = loadGraphFromFile(GRAPH_FILEPATH, &graphBufferLength);

    // Create a graph handle
    struct ncGraphHandle_t* graphHandle;
    retCode = ncGraphCreate("my graph", &graphHandle);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not create graph.\n", retCode);
        exit(-1);
    }
    
    // Allocate the graph to the device
    retCode = ncGraphAllocate(deviceHandle, graphHandle, graphBuffer, graphBufferLength);
    free(graphBuffer);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not allocate graph.\n", retCode);
        exit(-1);
    }
    
    // Get the graph input tensor descriptor
    struct ncTensorDescriptor_t* inputDescriptor = NULL;
    unsigned int inDescLen = 0;
    // Get the correct size first
    retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_INPUT_TENSOR_DESCRIPTORS, inputDescriptor, &inDescLen);
    if (retCode == NC_INVALID_DATA_LENGTH) {
        // Get the input tensor descriptor data
        inputDescriptor = (ncTensorDescriptor_t*)malloc(inDescLen);
        retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_INPUT_TENSOR_DESCRIPTORS, inputDescriptor, &inDescLen);
        if (retCode != NC_OK) {
            printf("ERROR [%d]: Could not get input tensor descriptor.\n", retCode);
            exit(-1);
        }
    } else {
        printf("ERROR [%d]: Could not get input tensor descriptor length.\n", retCode);
        exit(-1);
    }
    
    
    // Get the graph output tensor decsriptor
    struct ncTensorDescriptor_t* outputDescriptor = NULL;
    unsigned int outDescLen = 0;
    // Get the correct size first 
    retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_OUTPUT_TENSOR_DESCRIPTORS, outputDescriptor, &outDescLen);
    if (retCode == NC_INVALID_DATA_LENGTH) {
        // Get the output tensor descriptor data
        outputDescriptor = (ncTensorDescriptor_t*)malloc(outDescLen);
        retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_OUTPUT_TENSOR_DESCRIPTORS, outputDescriptor, &outDescLen);
        if (retCode != NC_OK) {
            printf("ERROR [%d]: Could not get output tensor descriptor.\n", retCode);
            exit(-1);
        }
    } else {
        printf("ERROR [%d]: Could not get output tensor descriptor length.\n", retCode);
        exit(-1);
    }
    
    // Create input/output FIFOs
    struct ncFifoHandle_t* inputFIFO;
    struct ncFifoHandle_t* outputFIFO;
    retCode = ncFifoCreate("", NC_FIFO_HOST_WO, &inputFIFO);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not create input FIFO.\n", retCode);
        exit(-1);
    }
    retCode = ncFifoCreate("", NC_FIFO_HOST_RO, &outputFIFO);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not create output FIFOe.\n", retCode);
        exit(-1);
    }
    
    // Allocate the FIFOs to the device
    retCode = ncFifoAllocate(inputFIFO, deviceHandle, inputDescriptor, 2);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not allocate input FIFO.\n", retCode);
        exit(-1);
    }
    retCode = ncFifoAllocate(outputFIFO, deviceHandle, outputDescriptor, 2);
        if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not allocate output FIFO.\n", retCode);
        exit(-1);
    }

    // Read and preprocess input
    float* imageBuffer = loadImageFromFile(IMAGE_FILEPATH, networkDim, networkMean);
    unsigned int imageBufferLength = 3 * networkDim * networkDim * sizeof(*imageBuffer);

    // Write the image to the input FIFO
    retCode = ncFifoWriteElem(inputFIFO, imageBuffer, inputDescriptor, 0);
    free(imageBuffer);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not write tensor to input FIFO.\n", retCode);
        exit(-1);
    }
    
    // Queue the inference
    retCode = ncGraphQueueInference(graphHandle, &inputFIFO, 1, &outputFIFO, 1);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not queue the inference.\n", retCode);
        exit(-1);
    }

    // Get the results from the output FIFO
    void* result = NULL;
    unsigned int resultLen = 0;
    void* userParam; 
    // Get the correct result size
    retCode = ncFifoReadElem(outputFIFO, &result, &resultLen, &userParam);
    if (retCode == NC_INVALID_DATA_LENGTH) {
        // Get the result value
        result = malloc(resultLen);
        retCode = ncFifoReadElem(outputFIFO, &result, &resultLen, &userParam);
        if (retCode != NC_OK) {
            printf("ERROR [%d]: Could not get output tensor descriptor.\n", retCode);
            exit(-1);
        }
    } else {
        printf("ERROR [%d]: Could not get output tensor descriptor length.\n", retCode);
        exit(-1);
    }

    // Do something with the results...
    int dataTypeSize = outputDescriptor->totalSize/( outputDescriptor->w* outputDescriptor->h*outputDescriptor->c*outputDescriptor->n);
    unsigned int numResults = 1;
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
    retCode = ncFifoDestroy(inputFIFO);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not destroy the input FIFO.\n", retCode);
        exit(-1);
    }
    inputFIFO = NULL;
    retCode = ncFifoDestroy(outputFIFO);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not destroy the output FIFO.\n", retCode);
        exit(-1);
    }
    outputFIFO = NULL;
    retCode = ncGraphDestroy(graphHandle);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not destroy the graph handle.\n", retCode);
        exit(-1);
    }
    graphHandle = NULL;
    retCode = ncDeviceClose(deviceHandle);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not close the device.\n", retCode);
        exit(-1);
    }
    retCode = ncDeviceDestroy(deviceHandle);
    if (retCode != NC_OK) {
        printf("ERROR [%d]: Could not destroy the device handle.\n", retCode);
        exit(-1);
    }
    deviceHandle = NULL;
    
    return 0;

}
