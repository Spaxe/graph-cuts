/// Graph Cuts Algorithm, referencing Ch. 29 of the GPU Computing Gems, 2011
/// For Brisbane GPU Users, 16 June 2011
/// Xavie Ho [contact@xavierho.com]
#pragma once

class GraphCuts
{
public:
  // final input and output
  unsigned char *image;         
  unsigned char *result_image;  
  unsigned int w, h, wh;

  // Variables
  int *height;                 
  int *excess_flow;           
  int *edge_capacity; // Bidirectional between every two pixels

  // Constructor initiates a new computation by loading the image.
  GraphCuts( const char* image_path // Path to the input image
           , int tolerance          // Selection threshold
           );
  // Destructor releases the memory
  ~GraphCuts();

  // CPU Implementation for Graph Cuts
  void graph_cuts();
  void push();
  void relabel();
  bool finished_yet();
  bool active(int x);

  // Trace functions
  void print_image();
  void print_result_image();
  void print_height();
  void print_excess_flow();
  void print_edge_capacity();
};