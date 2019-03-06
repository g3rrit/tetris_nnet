#include <iostream>

#include "nnet.h"
#include "mstd.h"
#include "nnet_host.h"

using ML::Nnet_Structure;
using ML::Nnet;

int main() {
  std::cout << "Starting Tetris Neural Net Server\n";

  Nnet_Structure structure;
  structure.push_back(211);
  structure.push_back(190);
  structure.push_back(140);
  structure.push_back(100);
  structure.push_back(50);
  structure.push_back(20);
  structure.push_back(10);
  structure.push_back(4);
  Nnet_Host host{structure};
  host.run();

  return 0;
}


