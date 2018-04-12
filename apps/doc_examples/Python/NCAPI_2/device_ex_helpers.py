from mvnc import mvncapi

# Get a list of valid device handles
device_handles = mvncapi.enumerate_devices()

# Create a Device object for the first device found
device = mvncapi.Device(device_handles[0])

# Open communication with the device
device.open()

# Create a Graph
graph = Graph('graph1')

# Read a compiled network graph from file (set the graph_filepath correctly for your graph file)
graph_filepath = './graph'
with open(graph_filepath, 'rb') as f:
    graph_buffer = f.read()

# Allocate the graph on the device
input_fifo, output_fifo = device.graph_allocate_with_fifos(graph, graph_buffer)

#
# Use the device...
#

# Deallocate and destroy the graph handle and fifo handles, close the device, and destroy the device handle
input_fifo.destroy()
output_fifo.destroy()
graph.destroy()
device.close()
device.destroy()