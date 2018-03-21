#include <stdio.h>
#include <stdlib.h>
#include <mvnc.h>

int main(int argc, char** argv)
{
        ncStatus_t retCode;
        struct ncDeviceHandle_t* deviceHandlePtr;
        
        // Initialize the device
        retCode = ncDeviceInit(0, &deviceHandlePtr);
        if (retCode != NC_OK)
        {
                // Failed to initialize the device... maybe it isn't plugged in to the host
                printf("ncDeviceInit Failed [%d]: No devices found for initialization.\n", retCode);
                exit(-1);
        }

	// Get the device state
	int deviceState;
	unsigned int dataLength = sizeof(deviceState);
	retCode = ncDeviceGetOption(deviceHandlePtr, NC_RO_DEVICE_STATE, &deviceState, &dataLength);
        if (retCode != NC_OK)
        {
                // Failed to get device state
                printf("ncDeviceGetOption Failed [%d]: Could not get the device option.\n", retCode);
                exit(-1);
        }
	printf("Device state: %d\n", deviceState);

        // Open the device
        retCode = ncDeviceOpen(deviceHandlePtr);
        if (retCode != NC_OK)
        {
                // Failed to open the device
                printf("ncDeviceOpen Failed [%d]: Could not open the device.\n", retCode);
                exit(-1);
        }

	// Get the device state again
	retCode = ncDeviceGetOption(deviceHandlePtr, NC_RO_DEVICE_STATE, &deviceState, &dataLength);
        if (retCode != NC_OK)
        {
                // Failed to get device state
                printf("ncDeviceGetOption Failed [%d]: Could not get the device option.\n", retCode);
                exit(-1);
        }
	printf("Device state: %d\n", deviceState);

        // Device handle is ready to use now, pass it to other API calls as needed...

        // Close the device
        retCode = ncDeviceClose(deviceHandlePtr);
        if (retCode != NC_OK)
        {
                // Failed to close the device
                printf("ncDeviceClose Failed [%d]: Could not close the device.\n", retCode);
                exit(-1);
        }

	// Get the device state again
	retCode = ncDeviceGetOption(deviceHandlePtr, NC_RO_DEVICE_STATE, &deviceState, &dataLength);
        if (retCode != NC_OK)
        {
                // Failed to get device state
                printf("ncDeviceGetOption Failed [%d]: Could not get the device option.\n", retCode);
                exit(-1);
        }
	printf("Device state: %d\n", deviceState);

        deviceHandlePtr = NULL;
}
