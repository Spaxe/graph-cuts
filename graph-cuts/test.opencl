__kernel void test(__global float *input, __global float *output)
{
  size_t id = get_global_id(0);
  output[id] = input[id] * input[id];
}