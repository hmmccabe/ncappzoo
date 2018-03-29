"""NCAPI v2"""
from mvnc import mvncapi

import cv2
import numpy as np


def get_image():
    ilsvrc_mean = np.load('ilsvrc_2012_mean.npy').mean(1).mean(1)
    dim = (224, 224)
    tensor = cv2.imread('cat.jpg')
    tensor = cv2.resize(tensor, dim)
    tensor = tensor.astype(np.float32)
    tensor[:, :, 0] = (tensor[:, :, 0] - ilsvrc_mean[0])
    tensor[:, :, 1] = (tensor[:, :, 1] - ilsvrc_mean[1])
    tensor[:, :, 2] = (tensor[:, :, 2] - ilsvrc_mean[2])
    return tensor


def do_something(results):
    labels = np.loadtxt('synset_words.txt', str, delimiter='\t')
    order = results.argsort()[::-1][:6]
    print('\n------- predictions --------')
    for i in range(0, 5):
        print('prediction ' + str(i) + ' (probability ' + str(results[order[i]])
              + ') is ' + labels[order[i]] + '  label index is: ' + str(order[i]))


# Initialize and open a device
device_list = mvncapi.enumerate_devices()
device = mvncapi.Device(device_list[0])
device.open()

# Read a graph from file at some GRAPH_FILEPATH and initialize the Graph
GRAPH_FILEPATH = './graph'
with open(GRAPH_FILEPATH, mode='rb') as f:
    graph_buffer = f.read()
graph = mvncapi.Graph('my graph')

# Allocate the graph to the device
device.graph_allocate(graph, graph_buffer)

# Get the graph TensorDescriptor structs (they describe expected graph input/output)
input_descriptor = graph.get_option(mvncapi.GraphOption.RO_INPUT_TENSOR_DESCRIPTORS)
output_descriptor = graph.get_option(mvncapi.GraphOption.RO_OUTPUT_TENSOR_DESCRIPTORS)

# Create input/output Fifos
input_fifo = mvncapi.Fifo('input', mvncapi.FifoType.HOST_WO)
output_fifo = mvncapi.Fifo('output', mvncapi.FifoType.HOST_RO)
input_fifo.set_option(mvncapi.FifoOption.RW_DATA_TYPE, mvncapi.FifoDataType.FP32)   # optional, if needed
output_fifo.set_option(mvncapi.FifoOption.RW_DATA_TYPE, mvncapi.FifoDataType.FP32)  # optional, if needed
input_fifo.create(device, input_descriptor, 2)
output_fifo.create(device, output_descriptor, 2)

# Read and pre-process input
input_tensor = get_image()

# Write the image to the input queue
input_fifo.write_elem(input_tensor, input_descriptor, 'user object')

# Queue the inference
graph.queue_inference(input_fifo, output_fifo)

# Get the results from the output queue
output, user_obj = output_fifo.read_elem()

# Do something with the results...
do_something(output)

# Clean up
input_fifo.delete()
output_fifo.delete()
graph.deallocate()
device.close()
