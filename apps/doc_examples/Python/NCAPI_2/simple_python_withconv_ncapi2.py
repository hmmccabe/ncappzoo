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
    print('top prediction (probability ' + str(results[order[0]]) + ') is ' + labels[order[0]])


# Initialize and open a device
device_list = mvncapi.enumerate_devices()
device = mvncapi.Device(device_list[0])
device.open()

# Initialize a graph from file at some GRAPH_FILEPATH
GRAPH_FILEPATH = './graph'
with open(GRAPH_FILEPATH, mode='rb') as f:
    graph_buffer = f.read()
graph = mvncapi.Graph('my graph')

# CONVENIENCE FUNCTION: Allocate the graph to the device and create input/output Fifos in one call
# Here we are leaving default values for FifoType and the number of elements, but setting the data types to FP32
input_fifo, output_fifo = device.graph_allocate_with_fifos(graph, graph_buffer,
                                                           input_fifo_data_type=mvncapi.FifoDataType.FP32,
                                                           output_fifo_data_type=mvncapi.FifoDataType.FP32)

# Get graph TensorDescriptor struct for input (describes expected graph input)
input_desc = graph.get_option(mvncapi.GraphOption.RO_INPUT_TENSOR_DESCRIPTORS)

# Read and pre-process input
input_tensor = get_image()

# CONVENIENCE FUNCTION: Write the image to the input queue and queue the inference in one call
graph.queue_inference_with_fifo_elem(input_fifo, output_fifo, input_tensor, input_desc, 'user object')

# Get the results from the output queue
output, user_obj = output_fifo.read_elem()

# Do something with the results...
do_something(output)

# Clean up
input_fifo.destroy()
output_fifo.destroy()
graph.destroy()
device.close()
device.destroy()
