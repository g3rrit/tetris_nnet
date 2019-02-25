#include <iostream>

#include "nnet.h"
#include "mstd.h"

using ML::Nnet_Structure;
using ML::Nnet;

int main() {
  std::cout << "Starting Tetris Neural Net Server\n";

  Nnet_Structure structure;
  structure.push_back(10);
  structure.push_back(10);
  Nnet net{structure};
  
  std::cout << "network:" << net << "\n";

  getchar();

  return 0;
}
