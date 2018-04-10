#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncFifoHandle_t* fifoHandle;
    char* fifoName; 
    unsigned int dataLength;
    
    /* Initialize a fifo handle */
    retCode = ncFifoCreate("inputFifo", NC_FIFO_HOST_WO, &fifoHandle);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Call ncFifoGetOption once to get the correct dataLength */
    fifoName = NULL;
    dataLength = 0;
    retCode = ncFifoGetOption(fifoHandle, NC_RO_FIFO_NAME, fifoName, &dataLength);

    /* retCode should be NC_INVALID_DATA_LENGTH unless there was another problem */
    if(retCode != NC_INVALID_DATA_LENGTH)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Now the value of dataLength is correctly set */
    /* Allocate the array buffer */
    fifoName = (char*)malloc(dataLength);
    
    /* Get the fifo name option value */
    retCode = ncFifoGetOption(fifoHandle, NC_RO_FIFO_NAME, fifoName, &dataLength);
    
    /* Use the fifo name as needed */
    printf("The fifo name is %s.\n", fifoName);
    
    return 0;
}
