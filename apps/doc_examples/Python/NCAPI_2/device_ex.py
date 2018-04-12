"""NCAPI v2"""
from mvnc import mvncapi

# Initialize and open a device
device_list = mvncapi.enumerate_devices()
device = mvncapi.Device(device_list[0])
device.open()

# Read a graph from file at some GRAPH_FILEPATH and initialize the Graph
GRAPH_FILEPATH = 'graph'
with open(GRAPH_FILEPATH, mode='rb') as f:
    graph_buffer = f.read()
graph = mvncapi.Graph('my graph')

# Allocate the graph to the device
device.graph_allocate(graph, graph_buffer)

# Clean up
graph.destroy()
device.close()
device.destroy()
