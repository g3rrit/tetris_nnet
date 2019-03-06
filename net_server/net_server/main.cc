#include <iostream>

#include "nnet.h"
#include "mstd.h"
#include "nnet_host.h"

using ML::Nnet_Structure;
using ML::Nnet;

int main() {
  std::cout << "Starting Tetris Neural Net Server\n";

  Nnet_Host host;
  host.run();

  return 0;
}


