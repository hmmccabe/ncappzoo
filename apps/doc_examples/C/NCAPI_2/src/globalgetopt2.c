#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) 
{
    ncStatus_t retCode;
    unsigned int* apiVersion;
    unsigned int dataLength;

    /* Call once to get the correct dataLength */
    apiVersion = NULL;
    dataLength = 0;
    retCode = ncGlobalGetOption(NC_RO_API_VERSION, apiVersion, &dataLength);

    /* retCode should be NC_INVALID_DATA_LENGTH unless there was another problem */
    if(retCode != NC_INVALID_DATA_LENGTH)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }

    /* Now the value of dataLength is correctly set (in this case for an array of 4 unsigned ints) */
    /* Allocate the array buffer */
    apiVersion = (unsigned int*)malloc(dataLength);

    /* Call the function again to get the API version option value */
    retCode = ncGlobalGetOption(NC_RO_API_VERSION, apiVersion, &dataLength);
    
    /* Now retCode should be NC_OK unless there was another problem */
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }

    /* Use the API version as needed */
    printf("API VERSION - Major: %d, Minor: %d, Hotfix: %d, Release: %d\n", apiVersion[0], apiVersion[1], apiVersion[2], apiVersion[3]);

    /* When you are done, free it and set the pointer to NULL */
    free(apiVersion);
    apiVersion = NULL;
    
    return 0;
}
