#include <iostream>

#include "nnet.h"
#include "mstd.h"
#include "nnet_host.h"
#include "trainer.h"
#include "stdio.h"
#include "string.h"
#include "nutil.h"

using ML::Nnet_Structure;
using ML::Nnet;
using ML::Trainer;
using ML::Training_Data;

#define NNET_FILE "nnet.data"
#define NNET_OUT_DIR "D:\\projects\\tetris_nnet\\net_server\\Debug\\out"
#define NNET_TRAINING_DATA "D:\\Programs\\fceux\\traindata\\use\\train.data"

int file_exists(const char *path) {
  FILE *file = 0;
  if(file = fopen(path, "r")) {
    fclose(file);
    return 1;
  }
  return 0;
}

uint line_count(FILE *file) {
  uint lc = 0;
  int c = 0;
  for(;c = fgetc(file);) {
    if(c == '\n') lc++;
    else if(c == EOF) break;
  }
  rewind(file);
  return lc;
}

int main() {
  std::cout << "Starting Tetris Neural Net Server\n";

  Nnet_Structure structure;
  structure.push_back(210);
  structure.push_back(190);
  structure.push_back(140);
  structure.push_back(100);
  structure.push_back(50);
  structure.push_back(20);
  structure.push_back(10);
  structure.push_back(4);
  Nnet_Host host{structure};

  // check if nnet file exists
  if(file_exists(NNET_FILE)) {
    FILE *file = fopen(NNET_FILE, "rb");
    host.nnet.load(file);
    fclose(file);
  }

  // create trainings data
  FILE *file = fopen(NNET_TRAINING_DATA, "r");
  if(!file) {
    error("unable to open training file\n");
    return -1;
  }
  uint lc = line_count(file);
  Training_Data t_data{lc, 210, 4};

  f64 field[200] = {0};
  f64 t_piece = 0;
  f64 j_piece = 0;
  f64 z_piece = 0;
  f64 o_piece = 0;
  f64 s_piece = 0;
  f64 l_piece = 0;
  f64 i_piece = 0;
  f64 x_piece = 0;
  f64 x_pos = 0;
  f64 y_pos = 0;
  f64 next_piece = 0;

  f64 move = 0;
  f64 rotate = 0;
  f64 move_left = 0;
  f64 move_right = 0;
  f64 rotate_left = 0;
  f64 rotate_right = 0;
  
  int c = 0;
  char buffer[32] = {0};
  uint offset = 0;
  for(int i = 0; i < lc; i++) {
    fgetc(file); //"["
    fgetc(file); //" "
    for(int n = 0; n < 200; n++) {
      fscanf(file, "%lf ", field + n);
    }
    fscanf(file, "%lf ", &t_piece);
    fscanf(file, "%lf ", &j_piece);
    fscanf(file, "%lf ", &z_piece);
    fscanf(file, "%lf ", &o_piece);
    fscanf(file, "%lf ", &s_piece);
    fscanf(file, "%lf ", &l_piece);
    fscanf(file, "%lf ", &i_piece);
    fscanf(file, "%lf ", &x_pos);
    fscanf(file, "%lf ", &y_pos);
    fscanf(file, "%lf ", &next_piece);

    fgetc(file); //"]"
    fgetc(file); //" "
    fgetc(file); //"["
    fgetc(file); //" "

    fscanf(file, "%lf ", &move);
    fscanf(file, "%lf ", &rotate);

    fgetc(file); //"]"
    fgetc(file); //"\r"
    fgetc(file); //"\n"

    /*
    std::cout << "READ FROM FILE:\n";
    for(uint z = 0; z < 20; z++) {
      std::cout << field[z] << " ";

    }
    std::cout << "t: " << t_piece << "\n"
      << "j: " << j_piece << "\n"
      << "z: " << z_piece << "\n"
      << "o: " << o_piece << "\n"
      << "s: " << s_piece << "\n"
      << "l: " << l_piece << "\n"
      << "i: " << i_piece << "\n"
      << "x: " << x_pos << "\n"
      << "y: " << y_pos << "\n"
      << "n: " << next_piece << "\n"
      << "m: " << move << "\n"
      << "r: " << rotate << "\n"; */

    if(move <= 0.5) {
      move_left = 0;
      move_right = 0;
    } else if(move <= 1.5) {
      move_left = 1;
      move_right = 0;
    } else if(move <= 2.5) {
      move_left = 0;
      move_right = 1;
    }
      
    if(rotate <= 0.5) {
      rotate_left = 0;
      rotate_right = 0;
    } else if(rotate <= 1.5) {
      rotate_left = 1;
      rotate_right = 0;
    } else if(rotate <= 2.5) {
      rotate_left = 0;
      rotate_right = 1;
    }

    memcpy(t_data.data + offset, field, sizeof(f64) * 200);
    offset += 200;
    memcpy(t_data.data + offset, &t_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &j_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &z_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &o_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &s_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &l_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &i_piece, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &x_pos, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &y_pos, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &next_piece, sizeof(f64));
    offset += 1;

    memcpy(t_data.data + offset, &move_left, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &move_right, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &rotate_left, sizeof(f64));
    offset += 1;
    memcpy(t_data.data + offset, &rotate_right, sizeof(f64));
    offset += 1;
  }

  Trainer trainer{host.nnet, 200};
  trainer.set_training_data(&t_data);
  // start host
  
  std::thread([&host] () {
                host.run();
              }).detach();

  vector<f64> *costv = 0;
  uint iteration = 0;
  for(;;) {
    char nnet_fname[512] = {0};
    costv = trainer.train(20, 5, 0.5);
    std::cout << "COST VECTOR:\n";
    for(f64 cost : *costv) {
      std::cout << "[ " << cost << " ]\n";
    }
    //save net
    sprintf(nnet_fname, "%s\\i_%s", NNET_OUT_DIR, iteration, NNET_FILE);
    FILE *file = fopen(nnet_fname, "wb");
    if(!file) {
      error("unable to save nnet to file\n");
    }
    host.lock();
    host.nnet.save(file);
    fclose(file);
    
    // copy best trainings data
    copy(host.nnet, *trainer.get_best());
    
    host.unlock();
  }

  return 0;
}


