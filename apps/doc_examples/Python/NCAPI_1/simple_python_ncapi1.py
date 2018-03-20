"""NCAPI v1"""
from mvnc import mvncapi

import cv2
import numpy as np

def get_image():
    ilsvrc_mean = np.load('ilsvrc_2012_mean.npy').mean(1).mean(1)
    dim=(224,224)
    input_tensor = cv2.imread('cat.jpg')
    input_tensor = cv2.resize(input_tensor, dim)
    input_tensor = input_tensor.astype(np.float32)
    input_tensor[:,:,0] = (input_tensor[:,:,0] - ilsvrc_mean[0])
    input_tensor[:,:,1] = (input_tensor[:,:,1] - ilsvrc_mean[1])
    input_tensor[:,:,2] = (input_tensor[:,:,2] - ilsvrc_mean[2])
    return input_tensor.astype(np.float16)

def do_something(results):
    labels = np.loadtxt('synset_words.txt', str, delimiter='\t')
    order = output.argsort()[::-1][:6]
    print('\n------- predictions --------')
    for i in range(0,5):
        print ('prediction ' + str(i) + ' (probability ' + str(output[order[i]]) 
               + ') is ' + labels[order[i]] + '  label index is: ' + str(order[i]))

# Open a device
device_list = mvncapi.EnumerateDevices()
device = mvncapi.Device(device_list[0])
device.OpenDevice()

# Load a graph from file at some GRAPH_FILEPATH
GRAPH_FILEPATH = './graph'
with open(GRAPH_FILEPATH, mode='rb') as f:
	graph_buffer = f.read()

# Allocate the graph to the device
graph = device.AllocateGraph(graph_buffer)

# Read and pre-process input (16 bit floating point data type required)
input_tensor = get_image()

# Load the image to the device and trigger an inference
graph.LoadTensor(input_tensor, 'user object')

# Get the results from the device
output, userobj = graph.GetResult()

# Do something with the results...
do_something(output)

# Clean up
graph.DeallocateGraph()
device.CloseDevice()
