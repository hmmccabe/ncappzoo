"""NCAPI v2"""
from mvnc import mvncapi

import cv2
import numpy as np
import threading
import time


def get_image(filepath='cat.jpg'):
    ilsvrc_mean = np.load('ilsvrc_2012_mean.npy').mean(1).mean(1)
    dim = (224, 224)
    tensor = cv2.imread(filepath)
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

# Read graphs from file at some GRAPH_FILEPATH and initialize Graphs
GRAPH_FILEPATH = './graph_6'
with open(GRAPH_FILEPATH, mode='rb') as f:
    graph_buffer_1 = f.read()
with open(GRAPH_FILEPATH, mode='rb') as f:
    graph_buffer_2 = f.read()
graph_1 = mvncapi.Graph('graph_1')
graph_2 = mvncapi.Graph('graph_2')

# Allocate the graph to the device
device.graph_allocate(graph_1, graph_buffer_1)
device.graph_allocate(graph_2, graph_buffer_2)

# Get the graph TensorDescriptor structs (they describe expected graph input/output)
input_descriptor = graph_1.get_option(mvncapi.GraphOption.RO_INPUT_TENSOR_DESCRIPTORS)
output_descriptor = graph_1.get_option(mvncapi.GraphOption.RO_OUTPUT_TENSOR_DESCRIPTORS)

# Create input/output Fifos
input_fifo = mvncapi.Fifo('input', mvncapi.FifoType.HOST_WO)
output_fifo = mvncapi.Fifo('output', mvncapi.FifoType.HOST_RO)
input_fifo.set_option(mvncapi.FifoOption.RW_DATA_TYPE, mvncapi.FifoDataType.FP32)   # optional, if needed
output_fifo.set_option(mvncapi.FifoOption.RW_DATA_TYPE, mvncapi.FifoDataType.FP32)  # optional, if needed
fifo_size = 10
input_fifo.create(device, input_descriptor, fifo_size)
output_fifo.create(device, output_descriptor, fifo_size)

#input_tensor = get_image()
#input_fifo.write_elem(input_tensor, input_descriptor, 'user object')

# Write the image to the input queue
for i in range(fifo_size):
    input_tensor = get_image('cat' + str(i) + '.jpg')
    input_fifo.write_elem(input_tensor, input_descriptor, str(i))
    print('img',str(i),'written')
    print('Input Fill level:', input_fifo.get_option(mvncapi.FifoOption.RO_WRITE_FILL_LEVEL))
    print('Output Fill level:', output_fifo.get_option(mvncapi.FifoOption.RO_READ_FILL_LEVEL))

for j in range(fifo_size):
    if j % 2 == 0:
        graph_1.queue_inference(input_fifo, output_fifo)
    else:
        graph_2.queue_inference(input_fifo, output_fifo)
    print('img',str(j),'queued')
    print('Input Fill level:', input_fifo.get_option(mvncapi.FifoOption.RO_WRITE_FILL_LEVEL))
    print('Output Fill level:', output_fifo.get_option(mvncapi.FifoOption.RO_READ_FILL_LEVEL))

for k in range(fifo_size):
    output, user_obj = output_fifo.read_elem()
    print('read img ', user_obj)
    print('Input Fill level:', input_fifo.get_option(mvncapi.FifoOption.RO_WRITE_FILL_LEVEL))
    print('Output Fill level:', output_fifo.get_option(mvncapi.FifoOption.RO_READ_FILL_LEVEL))

    # Do something with the results...
    # do_something(output)

#output_fifo.remove_elem()

# Clean up
input_fifo.delete()
print('input fifo deleted')
output_fifo.delete()
print('output fifo deleted')
graph_1.deallocate()
graph_2.deallocate()
device.close()
