#include <cstdio>
#include <cstring>
#include <climits>
#include <algorithm>
#include "GraphCuts.h"
using namespace std;

#define INF 999

GraphCuts::GraphCuts(const char* image_path, int tolerance)
{
  // @TODO Actually loading a real image from file
  w = 16;
  h = 16;
  wh = w*h;

  image = new unsigned char[wh];
  result_image = new unsigned char[wh];

  height = new int[wh];
  excess_flow = new int[wh];

  // A "sparse" matrix for simplicity. 
  // However this is horrible at memory.
  // To access edge capacity from x to y, use:
  //   edge_capacity[x*wh+y]
  // and vice versa.
  edge_capacity = new int[wh*wh];

  // initialise values
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      image[i*h+j] = i*j;
    } 
  }
  memset(result_image, 0, wh*sizeof(unsigned char)); 
  for (int i = 0; i < wh; ++i)
    height[i] = image[i];    
  memset(excess_flow, 0, wh*sizeof(int));

  int top_offset = -h,
      bottom_offset = h,
      left_offset = -1,
      right_offset = 1;
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int this_pixel = i*h+j;
      if (j < w-1)
        edge_capacity[this_pixel*wh+this_pixel+right_offset] = height[this_pixel+right_offset] - height[this_pixel];
      if (j > 0)
        edge_capacity[this_pixel*wh+this_pixel+left_offset] = height[this_pixel+left_offset] - height[this_pixel];
      if (i > 0)
        edge_capacity[this_pixel*wh+this_pixel+top_offset] = height[this_pixel+top_offset] - height[this_pixel];
      if (i < h-1)
        edge_capacity[this_pixel*wh+this_pixel+bottom_offset] = height[this_pixel+bottom_offset] - height[this_pixel];
    } 
  }
  // Hard code for testing
  // Source
  excess_flow[51] = tolerance;
}

GraphCuts::~GraphCuts()
{
  delete[] image;
  delete[] result_image;
  delete[] height;
  delete[] excess_flow;
  delete[] edge_capacity;
}

void GraphCuts::graph_cuts()
{
  while(!finished_yet()) {
    //printf("Calculating...\n");
    relabel();
    push();
    //print_height();
    //print_excess_flow();
    //getchar();
  }
  print_height();
  print_excess_flow();
}

/* Compare all neighbouring pixels of an active pixel by the edge capacity from 
 * the current working pixel towards its neighbour. Takes the next minimum
 * height between all pixels that have a positive edge capacity towards them.
 */
void GraphCuts::relabel()
{
  int this_height, this_pixel;
  int neighbour_pixel, edge_capacity_index;
  int top_offset = -h,
      bottom_offset = h,
      left_offset = -1,
      right_offset = 1;

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {

      this_pixel = i*h+j;
      if (active(this_pixel)) {
        //printf("Active pixel: %d\n", this_pixel);
        this_height = INF;                

        neighbour_pixel = this_pixel + right_offset;
        edge_capacity_index = this_pixel*wh + neighbour_pixel;
        if (j < w-1 && edge_capacity[edge_capacity_index] > 0) { 
          this_height = min(this_height, height[neighbour_pixel]+1);
        }

        neighbour_pixel = this_pixel + left_offset;
        edge_capacity_index = this_pixel*wh + neighbour_pixel;
        if (j > 0 && edge_capacity[edge_capacity_index] > 0) { 
          this_height = min(this_height, height[neighbour_pixel]+1);
        }

        neighbour_pixel = this_pixel + top_offset;
        edge_capacity_index = this_pixel*wh + neighbour_pixel;
        if (i > 0 && edge_capacity[edge_capacity_index] > 0) { 
          this_height = min(this_height, height[neighbour_pixel]+1);
        }

        neighbour_pixel = this_pixel + bottom_offset;
        edge_capacity_index = this_pixel*wh + neighbour_pixel;
        if (i < h-1 && edge_capacity[edge_capacity_index] > 0) { 
          this_height = min(this_height, height[neighbour_pixel]+1);
        }

        height[this_pixel] = this_height;
      }
    }
  }
}

void GraphCuts::push()
{
  int this_pixel;
  int neighbour_pixel, to_neighbour, from_neighbour;
  int top_offset = -h,
      bottom_offset = h,
      left_offset = -1,
      right_offset = 1;
  int flow;

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {

      this_pixel = i*h+j;
      if (active(this_pixel)) {

        neighbour_pixel = this_pixel + right_offset;
        to_neighbour = this_pixel*wh + neighbour_pixel;
        from_neighbour = neighbour_pixel*wh + this_pixel;
        if (j < w-1 && height[neighbour_pixel] == height[this_pixel]-1) {
          flow = min(edge_capacity[to_neighbour], excess_flow[this_pixel]);
          excess_flow[this_pixel] -= flow;
          excess_flow[neighbour_pixel] += flow;
          edge_capacity[to_neighbour] -= flow;
          edge_capacity[from_neighbour] += flow;
        }

        neighbour_pixel = this_pixel + left_offset;
        to_neighbour = this_pixel*wh + neighbour_pixel;
        from_neighbour = neighbour_pixel*wh + this_pixel;
        if (j > 0 && height[neighbour_pixel] == height[this_pixel]-1) {
          flow = min(edge_capacity[to_neighbour], excess_flow[this_pixel]);
          excess_flow[this_pixel] -= flow;
          excess_flow[neighbour_pixel] += flow;
          edge_capacity[to_neighbour] -= flow;
          edge_capacity[from_neighbour] += flow;
        }

        neighbour_pixel = this_pixel + top_offset;
        to_neighbour = this_pixel*wh + neighbour_pixel;
        from_neighbour = neighbour_pixel*wh + this_pixel;
        if (i > 0 && height[neighbour_pixel] == height[this_pixel]-1) {
          flow = min(edge_capacity[to_neighbour], excess_flow[this_pixel]);
          excess_flow[this_pixel] -= flow;
          excess_flow[neighbour_pixel] += flow;
          edge_capacity[to_neighbour] -= flow;
          edge_capacity[from_neighbour] += flow;
        }

        neighbour_pixel = this_pixel + bottom_offset;
        to_neighbour = this_pixel*wh + neighbour_pixel;
        from_neighbour = neighbour_pixel*wh + this_pixel;
        if (i < h-1 && height[neighbour_pixel] == height[this_pixel]-1) {
          flow = min(edge_capacity[to_neighbour], excess_flow[this_pixel]);
          excess_flow[this_pixel] -= flow;
          excess_flow[neighbour_pixel] += flow;
          edge_capacity[to_neighbour] -= flow;
          edge_capacity[from_neighbour] += flow;
        }

        //printf("Push complete\n");
      }
    }
  }
}

/* The book mentions a stopping criterion but does not specify what it is,
 * so here I have guessed one -- when there is no more pixels to work on.
 */
bool GraphCuts::finished_yet()
{
  for (int i = 0; i < wh; ++i)
    if (active(i))
      return false;
  return true;
}

/* A pixel is active when:
 *   1. Its height is less than infinity.
 *   2. It has a positive excess flow.
 */
bool GraphCuts::active(int x)
{
  return height[x] < INF && excess_flow[x] > 0;
}

void GraphCuts::print_image()
{
  printf("Image:\n");
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      printf("%4u", image[i*h+j]);
    }
    printf("\n");
  }
}

void GraphCuts::print_result_image()
{  
  printf("Result Image:\n");
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      printf("%4u", result_image[i*h+j]);
    }
    printf("\n");
  }
}

void GraphCuts::print_height()
{
  printf("Height:\n");
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      printf(" %3d", height[i*h+j]);
    }
    printf("\n");
  }
}

void GraphCuts::print_excess_flow()
{
  printf("Excess Flow:\n");
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      printf(" %3d", excess_flow[i*h+j]);
    }
    printf("\n");
  }
}