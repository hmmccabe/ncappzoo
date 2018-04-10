#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncDeviceHandle_t* deviceHandle;
    char* deviceName; 
    unsigned int dataLength;
    
    /* Initialize a device handle */
    retCode = ncDeviceCreate(0, &deviceHandle);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Call ncDeviceGetOption once to get the correct dataLength */
    deviceName = NULL;
    dataLength = 0;
    retCode = ncDeviceGetOption(deviceHandle, NC_RO_DEVICE_NAME, deviceName, &dataLength);

    /* retCode should be NC_INVALID_DATA_LENGTH unless there was another problem */
    if(retCode != NC_INVALID_DATA_LENGTH)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Now the value of dataLength is correctly set */
    /* Allocate the array buffer */
    deviceName = (char*)malloc(dataLength);
    
    /* Get the device name option value */
    retCode = ncDeviceGetOption(deviceHandle, NC_RO_DEVICE_NAME, deviceName, &dataLength);
    
    /* Use the device name as needed */
    printf("The device name is %s.\n", deviceName);
    
    /* When you are done, free the device handle and set the pointer to NULL */
    retCode = ncDeviceDestroy(deviceHandle);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    deviceHandle = NULL;
    
    return 0;
}
