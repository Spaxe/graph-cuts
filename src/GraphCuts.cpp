#include <cstdio>
#include <cstring>
#include <climits>
#include "GraphCuts.h"

GraphCuts::GraphCuts(const char* image_path, int tolerance)
{
  // @TODO Actually loading a real image from file
  w = 16;
  h = 16;
  unsigned int wh = w*h;

  image = new unsigned char[wh];
  result_image = new unsigned char[wh];

  height = new int[wh];
  excess_flow = new int[wh];
  edge_capacity = new int[wh*wh]; // A "sparse" matrix for simplicity. 
                                  // However this is horrible at memory.

  // initialise values
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      image[i*h+j] = i*j;
    } 
  }
  memset(result_image, 0, wh*sizeof(unsigned char)); 
  memset(height, INT_MAX, wh*sizeof(int)); // Pretending INT_MAX = Infinity
  memset(excess_flow, 0, wh*sizeof(int));
  memset(edge_capacity, tolerance, wh*wh*sizeof(int));
}

GraphCuts::~GraphCuts()
{
  delete[] image;
  delete[] result_image;
  delete[] height;
  delete[] excess_flow;
  delete[] edge_capacity;
}

void GraphCuts::print_image()
{
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      printf("%4u", image[i*h+j]);
    }
    printf("\n");
  }
}
