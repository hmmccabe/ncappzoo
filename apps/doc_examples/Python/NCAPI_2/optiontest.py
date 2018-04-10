"""Test program for getting option values for Python Global/Device/Fifo/Graph Options."""
from mvnc import mvncapi
import numpy as np

# Turn off logging except for fatal to hide INVALID_DATA_LENGTH messages
mvncapi.global_set_option(mvncapi.GlobalOption.RW_LOG_LEVEL, 4)

print('\n\n----- Global Options -----')
for name, member in mvncapi.GlobalOption.__members__.items():
    try:
        optval = mvncapi.global_get_option(member)
        print(name, type(optval), optval)
    except Exception as e:
        print(e, name)

# Initialize and open a device
device_list = mvncapi.enumerate_devices()
device = mvncapi.Device(device_list[0])
print('\n\nDevice created, state is', device.get_option(mvncapi.DeviceOption.RO_DEVICE_STATE))
device.open()
print('Device opened, state is', device.get_option(mvncapi.DeviceOption.RO_DEVICE_STATE))

print('\n\n----- Device Options -----')
for name, member in mvncapi.DeviceOption.__members__.items():
    try:
        optval = device.get_option(member)
        print(name, type(optval), optval)
    except Exception as e:
        print(e, name)

# Read a graph from file at some GRAPH_FILEPATH and initialize the Graph
GRAPH_FILEPATH = './graph'
with open(GRAPH_FILEPATH, mode='rb') as f:
    graph_buffer = f.read()
graph = mvncapi.Graph('my graph')

# Allocate the graph to the device
print('\n\n')
print('Graph created, state is', graph.get_option(mvncapi.GraphOption.RO_GRAPH_STATE))
device.graph_allocate(graph, graph_buffer)
print('Graph allocated, state is', graph.get_option(mvncapi.GraphOption.RO_GRAPH_STATE))

print('\n\n----- Graph Options -----')
for name, member in mvncapi.GraphOption.__members__.items():
    try:
        optval = graph.get_option(member)
        print(name, type(optval), optval)
    except Exception as e:
        print(e, name)

# Get the graph TensorDescriptor structs
input_descriptor = graph.get_option(mvncapi.GraphOption.RO_INPUT_TENSOR_DESCRIPTORS)
output_descriptor = graph.get_option(mvncapi.GraphOption.RO_OUTPUT_TENSOR_DESCRIPTORS)

# Create input/output Fifos
print('\n\n')
input_fifo = mvncapi.Fifo('input', mvncapi.FifoType.HOST_WO)
output_fifo = mvncapi.Fifo('output', mvncapi.FifoType.HOST_RO)
print('Input fifo created, state is', input_fifo.get_option(mvncapi.FifoOption.RO_STATE))
print('Output fifo created, state is', output_fifo.get_option(mvncapi.FifoOption.RO_STATE))
input_fifo.set_option(mvncapi.FifoOption.RW_DATA_TYPE, mvncapi.FifoDataType.FP32)   # optional, if needed
output_fifo.set_option(mvncapi.FifoOption.RW_DATA_TYPE, mvncapi.FifoDataType.FP32)  # optional, if needed
input_fifo.allocate(device, input_descriptor, 2)
output_fifo.allocate(device, output_descriptor, 2)
print('Input fifo allocated, state is', input_fifo.get_option(mvncapi.FifoOption.RO_STATE))
print('Output fifo allocated, state is', output_fifo.get_option(mvncapi.FifoOption.RO_STATE))

print('\n\n----- Input Fifo Options -----')
for name, member in mvncapi.FifoOption.__members__.items():
    try:
        optval = input_fifo.get_option(member)
        print(name, type(optval), optval)
    except Exception as e:
        print(e, name)

print('\n\n----- Output Fifo Options -----')
for name, member in mvncapi.FifoOption.__members__.items():
    try:
        optval = output_fifo.get_option(member)
        print(name, type(optval), optval)
    except Exception as e:
        print(e, name)

# Read and pre-process input
input_tensor = np.zeros((224, 224), dtype = np.float32)

# Write the image to the input queue
input_fifo.write_elem(input_tensor, input_descriptor, 'user object')

# Queue the inference
graph.queue_inference(input_fifo, output_fifo)
# print('graph:', graph.get_option(mvncapi.GraphOption.RO_GRAPH_STATE))  # HANGS

# Get the results from the output queue
output, user_obj = output_fifo.read_elem()

print('\n\nGraph RO_TIME_TAKEN (after inference)', graph.get_option(mvncapi.GraphOption.RO_TIME_TAKEN))

# Clean up
print('\n\n -----Cleaning up -----')
input_fifo.destroy()
try:
    print('Input fifo destroyed, state is', input_fifo.get_option(mvncapi.FifoOption.RO_STATE))
except Exception as e:
    print('Input fifo destroyed, get_option for state raises', e)
output_fifo.destroy()
try:
    print('Output fifo destroyed, state is', output_fifo.get_option(mvncapi.FifoOption.RO_STATE))
except Exception as e:
    print('Output fifo destroyed, get_option for state raises', e)
graph.destroy()
try:
    print('Graph destroyed, state is', graph.get_option(mvncapi.GraphOption.RO_GRAPH_STATE))
except Exception as e:
    print('Graph destroyed, get_option for state raises', e)
device.close()
print('Device closed, state is', device.get_option(mvncapi.DeviceOption.RO_DEVICE_STATE))
device.destroy()
try:
    print('Device destroyed, state is', device.get_option(mvncapi.DeviceOption.RO_DEVICE_STATE))
except Exception as e:
    print('Device destroyed, get_option for state raises', e)
