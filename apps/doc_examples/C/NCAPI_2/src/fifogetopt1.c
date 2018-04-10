#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncFifoHandle_t* fifoHandle;
    unsigned int fifoState; 
    unsigned int dataLength; 

    /* Initialize a fifo handle */
    retCode = ncFifoCreate("", NC_FIFO_HOST_WO, &fifoHandle);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Get the device state option value */  
    dataLength = sizeof(fifoState);
    retCode = ncFifoGetOption(fifoHandle, NC_RO_FIFO_STATE, &fifoState, &dataLength);
    
    /* Use the device state as needed */
    printf("The FIFO state is %d.\n", fifoState);
    
    return 0;
}
