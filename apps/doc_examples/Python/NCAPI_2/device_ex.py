from mvnc import mvncapi

# Get a list of valid device handles
device_handles = mvncapi.enumerate_devices()

# Create a Device object for the first device found
device = mvncapi.Device(device_handles[0])

# Initialize the device
device.open()

# Initialize a graph container
graph = Graph('graph1')

# Read a compiled network graph from file (set the graph_filepath correctly for your graph file)
graph_filepath = './graph'
with open(graph_filepath, 'rb') as f:
    graph_buffer = f.read()

# Allocate the network graph on the device
device.graph_allocate(graph, graph_buffer)

#
# Use the device...
#

# Deallocate the graph and close the device
graph.destroy()
device.close()