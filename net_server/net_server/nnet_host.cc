#include "nnet_host.h"

#include <iostream>
#include "ipc.h"

void print_request(Request *req) {
  std::cout << "Request: \n";
  for(int i = 0; i < 10 * 20; i++) {
    std::cout << (int)req->field[i] << " ";
  }
  std::cout << "T_PIECE: " << (int)req->t_piece << "\n"
    << "J_PIECE: " << (int)req->j_piece << "\n"
    << "Z_PIECE: " << (int)req->z_piece << "\n"
    << "O_PIECE: " << (int)req->o_piece << "\n"
    << "S_PIECE: " << (int)req->s_piece << "\n"
    << "L_PIECE: " << (int)req->l_piece << "\n"
    << "I_PIECE: " << (int)req->i_piece << "\n"
    << "X_POS: " << (int)req->x_pos << "\n"
    << "Y_POS: " << (int)req->y_pos << "\n"
    << "NEXT_PIECE: " << (int)req->next_piece << "\n" 
    << "DROP_TIME: " << (int)req->drop_time << "\n"
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
  res->move = 1;
  res->rotate = 1;
  print_response(res);
}

void Nnet_Host::run() {
  start_host(request_callback, this);
}
