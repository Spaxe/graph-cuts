#pragma once
#include "HPTime.h"

class Main
{
  cl_context context;
  cl_context_properties properties[3];
  cl_kernel kernel;
  cl_command_queue command_queue;
  cl_program program;
  cl_int err;
  cl_uint num_of_platforms;
  cl_platform_id platform_id;
  cl_device_id device_id;
  cl_uint num_of_devices;
  cl_mem input, output;
  size_t global;

  float *data, *result;
  static const int data_size = 16;
  Time timer;
  double total_time, this_time;

public:
  Main();
  ~Main();

  void start();
  void init();
  void preprocess();
  void compute();
  void print();
  void cleanup();
  const char* readFile(const char* filepath);
};