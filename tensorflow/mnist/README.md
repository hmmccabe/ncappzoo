# Introduction
The mnist network can be used for handwriting recognition for the digits 0-9.  The provided Makefile does the following
1. Downloads a trained model 
2. Downloads test images
3. Compiles the network using the Neural Compute SDK.
4. There is a python example (run.py) which runs an inference for all of the test images to show how to use the network with the Neural Compute API thats provided in the Neural Compute SDK.

# Makefile
Provided Makefile provides targets to do all of the following.

## make help
Shows makefile possible targets and brief descriptions. 

## make all
Makes, downloads, and prepares everything needed to run the example program.

## make model
Downloads the trained model

## make compile
compiles the trained model to generate a Movidius internal 'graph' format file.  This file can be loaded on the Neural Compute Stick for inferencing.  Demonstrates NCSDK tool: mvNCCompile

## make run
Runs the provided program which sends a demonstrates using the NCSDK to run an inference using this network.

## make clean
Removes all the temporary and target files that are created by the Makefile.
