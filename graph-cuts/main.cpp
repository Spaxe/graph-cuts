///
/// An implementation of a graph cuts algorithm
/// See Ch. 29, "Fast Graph Cuts for Computer Vision"
/// GPU Computing Gems, 2011
/// Xavier Ho [contact@xavierho.com]
///
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "CL/cl.h"
#include "main.h"
using namespace std;

#define KERNEL_FILE "test.opencl"

int main(int argc, char* argv[])
{
  Main m;
  m.start();
  return 0;
}

Main::Main()
{
  data = new float[data_size];
  result = new float[data_size];

  for (int i = 0; i < data_size; ++i)
    data[i] = (float) i;
  memset(result, 0, data_size);
  total_time = this_time = 0.0;
}

Main::~Main()
{
  delete[] data;
  delete[] result;
}

void Main::start()
{
  init();
  preprocess();
  compute();
  print();
  cleanup();
  cin.get();
}

void Main::init()
{
  timer.start();
  // Test for computing platform
  if (clGetPlatformIDs(1, &platform_id, &num_of_platforms) != CL_SUCCESS)
  {
    cerr << "ERROR: Unable to get platform_id." << endl;
    cin.get();
  }
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Platform detected in %.2f ms.\n", total_time);

  timer.start();
  // Test for supported GPU devices
  if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id,
                      &num_of_devices) != CL_SUCCESS)
  {
    cerr << "ERROR: Unable to get device_id." << endl;
    cin.get();
  }
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Device ID obtained in %.2f ms.\n", this_time);
}

void Main::preprocess()
{
  timer.start();
  // Context properties list - must be terminated with 0 (Er, okay).
  properties[0] = CL_CONTEXT_PLATFORM;
  properties[1] = (cl_context_properties) platform_id;
  properties[2] = 0;

  // Create a context with the device
  context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Context created in %.2f ms.\n", this_time);

  timer.start();
  // Create a command queue using the context and device
  command_queue = clCreateCommandQueue(context, device_id, 0, &err);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Command queue created in %.2f ms.\n", this_time);

  timer.start();
  // Create a program from the kernel source code
  const char *kernel_source = readFile(KERNEL_FILE);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Kernel read in %.2f ms.\n", this_time);

  timer.start();
  // Compiles the kernel
  program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
  if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) != CL_SUCCESS)
    cerr << "ERROR: Unable to build program: " << program << endl;
  kernel = clCreateKernel(program, "test", &err);
  delete[] kernel_source;
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Kernel compiled in %.2f ms.\n", this_time);
}

void Main::compute()
{
  timer.start();
  // Create buffers for the input and output data
  size_t buffer_size = data_size * sizeof(float);
  input = clCreateBuffer(context, CL_MEM_READ_ONLY, buffer_size, NULL, NULL);
  output = clCreateBuffer(context, CL_MEM_READ_ONLY, buffer_size, NULL, NULL);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Buffers for data created in %.2f ms.\n", this_time);

  timer.start();
  // Load input data
  clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, buffer_size, data, 
                                                                0, NULL, NULL);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Input data loaded in %.2f ms.\n", this_time);

  timer.start();
  // Set argument lists for the kernel
  clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
  global = data_size;
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Kernel arguments configured in %.2f ms.\n", this_time);

  timer.start();
  // Enqueue for execution
  clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global, NULL
                                                              , 0, NULL, NULL);
  printf("Kernel queued for execution ...\n");
  clFinish(command_queue);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Computation completed in %.2f ms.\n", this_time);

  timer.start();
  // read output data
  clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, buffer_size, result, 
                                                                0, NULL, NULL);
  this_time = timer.getMilliseconds();
  total_time += this_time;
  printf("Results gathered in %.2f ms.\n", this_time);
}

void Main::print()
{
  cout << endl << "Input:" << endl << "  ";
  for (int i = 0; i < data_size; ++i)
    cout << data[i] << " ";
  cout << endl << "Output:" << endl << "  "; 
  for (int i = 0; i < data_size; ++i)
    cout << result[i] << " ";
  cout << endl;
  printf("Time accumulated: %.2f ms\n", total_time);
}

void Main::cleanup()
{
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(command_queue);
  clReleaseContext(context);
}

// Reads the entire file content at once. The caller must free returned memory.
const char* Main::readFile(const char* filepath)
{
  char* kernel;
  // Open the file
  ifstream f(filepath, ios_base::in | ios_base::binary);
  if (!f.is_open()) {
    cerr << "ERROR: Unable to open file: " << filepath << endl;
    return NULL;
  }
  // Get the file size
  f.seekg (0, ios::end);
  int size = (int) f.tellg();
  f.seekg(0, ios::beg);
  // Read the entire content at once
  kernel = new char[size+1];
  f.read(kernel, size);
  // Replaces the EOF character with a terminal.
  kernel[size] = '\0';
  f.close();
  return kernel;
}
