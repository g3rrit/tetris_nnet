#include "nnet_host.h"

#include <iostream>
#include "ipc.h"

void print_request(Request *req) {
  std::cout << "Request: \n";
  for(int i = 0; i < 10 * 20; i++) {
    std::cout << (int)req->field[i] << " ";
  }
  std::cout << "\nT_PIECE: " << (int)req->t_piece << "\n"
    << "J_PIECE: " << (int)req->j_piece << "\n"
    << "Z_PIECE: " << (int)req->z_piece << "\n"
    << "O_PIECE: " << (int)req->o_piece << "\n"
    << "S_PIECE: " << (int)req->s_piece << "\n"
    << "L_PIECE: " << (int)req->l_piece << "\n"
    << "I_PIECE: " << (int)req->i_piece << "\n"
    << "X_POS: " << (int)req->x_pos << "\n"
    << "Y_POS: " << (int)req->y_pos << "\n"
    << "NEXT_PIECE: " << (int)req->next_piece << "\n" 
    << "---------------------\n";
}

void print_response(Response *res) {
  std::cout << "Response: \n" 
    << "MOVE: " << (int)res->move << "\n"
    << "ROTATE: " << (int)res->rotate << "\n";
}

void request_callback(void *_host, Request *req, Response *res) {
  Nnet_Host *host = (Nnet_Host*)_host;
  print_request(req);

  // fill input vector
  for(int i = 0; i < 200; i++) {
    host->input.set(i, (f64)req->field[i]);
  }
  host->input.set(200, (f64)req->t_piece);
  host->input.set(201, (f64)req->j_piece);
  host->input.set(202, (f64)req->z_piece);
  host->input.set(203, (f64)req->o_piece);
  host->input.set(204, (f64)req->s_piece);
  host->input.set(205, (f64)req->l_piece);
  host->input.set(206, (f64)req->i_piece);
  host->input.set(207, (f64)req->x_pos);
  host->input.set(208, (f64)req->y_pos);
  host->input.set(209, (f64)req->next_piece);
  // --------

  host->lock();
  Vec<f64> *out = host->nnet.get_output(host->input);
  host->unlock();
  u8 move = 0;
  u8 rotate = 0;
  f64 move_left = out->get(0);
  f64 move_right = out->get(1);
  f64 rotate_left = out->get(2);
  f64 rotate_right = out->get(3);

  std::cout << "OUTPUT:\n"
    << "move_left: " << move_left << "\n"
    << "move_right: " << move_right << "\n"
    << "rotate_left: " << rotate_left << "\n"
    << "rotate_right: " << rotate_right << "\n";

  if(move_left >= 0.5 &&  move_left > move_right + 0.2) {
    move = 1;
  } else if(move_right >= 0.5 && move_right > move_left + 0.2) {
    move = 2;
  } else {
    move = 0;
  }

  if(rotate_left >= 0.5  && rotate_left > rotate_right + 0.2) {
    rotate = 1;
  } else if(rotate_right >= 0.5 && rotate_right > rotate_left + 0.2) {
    rotate = 2;
  } else {
    rotate = 0;
  }

  res->move = move;
  res->rotate = rotate;

  print_response(res);
}

Nnet_Host::Nnet_Host(Nnet_Structure &structure) 
  : nnet(structure),
    input(structure.at(0)) {}

Nnet_Host::~Nnet_Host() {}

void Nnet_Host::run() {
  start_host(request_callback, this);
}

void Nnet_Host::lock() {
  h_mutex.lock();
}

void Nnet_Host::unlock() {
  h_mutex.unlock();
}
