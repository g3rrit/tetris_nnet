#pragma once

#include "nnet.h"
#include "vmath.h"
#include "mstd.h"
#include <mutex>

using ML::Nnet;
using ML::Nnet_Structure;
using Math::Vec;

struct Nnet_Host {
  Nnet nnet;
  Vec<f64> input;

  std::mutex h_mutex;

  Nnet_Host(Nnet_Structure &structure);
  ~Nnet_Host();

  void run();

  void lock();
  void unlock();
};
