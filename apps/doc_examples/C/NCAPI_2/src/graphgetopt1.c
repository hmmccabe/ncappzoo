#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ncStatus_t retCode;
    struct ncGraphHandle_t* graphHandle;
    unsigned int graphState; 
    unsigned int dataLength; 

    /* Initialize a graph handle */
    retCode = ncGraphCreate("", &graphHandle);
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Get the graph state option value */  
    dataLength = sizeof(graphState);
    retCode = ncGraphGetOption(graphHandle, NC_RO_GRAPH_STATE, &graphState, &dataLength);
    
    /* retCode should be NC_OK unless there was a problem */
    if(retCode != NC_OK) {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    
    /* Use the graph state as needed */
    printf("The graph state is %d.\n", graphState);
    
    /* When you are done, free the graph handle and set the pointer to NULL */
    retCode = ncGraphDestroy(graphHandle);    
    if(retCode != NC_OK)  {
        printf("Error: %d\n", retCode);
        exit(-1);
    }
    graphHandle = NULL;
    
    return 0;
}
