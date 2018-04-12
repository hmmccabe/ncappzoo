#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncDeviceHandle_t* deviceHandle;
    unsigned int deviceState; 
    unsigned int dataLength; 

    /* Initialize a device handle */
    retCode = ncDeviceCreate(0, &deviceHandle);
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Get the device state option value */  
    dataLength = sizeof(deviceState);
    retCode = ncDeviceGetOption(deviceHandle, NC_RO_DEVICE_STATE, &deviceState, &dataLength);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Use the device state as needed */
    printf("The device state is %d.\n", deviceState);
    
    /* When you are done, free the device handle and set the pointer to NULL */
    retCode = ncDeviceDestroy(deviceHandle);
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }    
    deviceHandle = NULL;
    
    return 0;
}
