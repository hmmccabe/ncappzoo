#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncFifoHandle_t* fifoHandle;
    unsigned int fifoState; 
    unsigned int dataLength; 

    /* Initialize a FIFO handle */
    retCode = ncFifoCreate("", NC_FIFO_HOST_WO, &fifoHandle);
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Get the device state option value */  
    dataLength = sizeof(fifoState);
    retCode = ncFifoGetOption(fifoHandle, NC_RO_FIFO_STATE, &fifoState, &dataLength);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Use the device state as needed */
    printf("The FIFO state is %d.\n", fifoState);
    
    /* When you are done, free the FIFO handle and set the pointer to NULL */
    retCode = ncFifoDestroy(fifoHandle);
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    fifoHandle = NULL;
    
    return 0;
}
