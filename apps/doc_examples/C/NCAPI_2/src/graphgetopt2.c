#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncGraphHandle_t* graphHandle;
    char* graphName; 
    unsigned int dataLength;
    
    /* Initialize a graph handle */
    retCode = ncGraphCreate("graph1", &graphHandle);
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Call ncGraphGetOption once to get the correct dataLength */
    graphName = NULL;
    dataLength = 0;
    retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_NAME, graphName, &dataLength);

    /* retCode should be NC_INVALID_DATA_LENGTH unless there was another problem */
    if(retCode != NC_INVALID_DATA_LENGTH) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Now the value of dataLength is correctly set */
    /* Allocate the array buffer */
    graphName = (char*)malloc(dataLength);
    
    /* Get the graph name option value */
    retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_NAME, graphName, &dataLength);
    
    /* This time retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Use the graph name as needed */
    printf("The graph name is %s.\n", graphName);
    
    /* When you are done, free the graph handle and set the pointer to NULL */
    retCode = ncGraphDestroy(graphHandle);    
    if(retCode != NC_OK)  {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    graphHandle = NULL;
    
    return 0;
}
