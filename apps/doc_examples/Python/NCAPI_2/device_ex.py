from mvnc import mvncapi

# Get a list of valid device handles
device_handles = mvncapi.enumerate_devices()

# Create a Device object for the first device found
device = mvncapi.Device(device_handles[0])

# Get the device name and output to console
print(device.get_option(mvncapi.DeviceOption.RO_DEVICE_NAME))

# Open communication with the device
device.open()

#
# Use the device...
#

# Close the device and destroy the device handle
device.close()
device.destroy()