#include <mvnc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    ncStatus_t retCode;
    int logLevel;
    unsigned int dataLength = sizeof(logLevel);

    // Get the logging level option value
    retCode = ncGlobalGetOption(NC_RW_LOG_LEVEL, &logLevel, &dataLength);

    // retCode should be NC_OK unless there was a problem
    if(retCode != NC_OK)
    {
        printf("Error: %d\n", retCode);
        exit(-1);
    }

    // Use the log level as needed
    printf("LOGGING LEVEL IS %d\n", logLevel);
}
