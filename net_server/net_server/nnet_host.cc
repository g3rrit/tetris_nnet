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
  host->input.set(210, (f64)req->drop_time);
  // --------

  Vec<f64> *out = host->nnet.get_output(host->input);
  res->move = out->get(0) >= out->get(1) ? 1 : 0;
  res->rotate = out->get(2) >= out->get(3) ? 1 : 0;

  print_response(res);
}

Nnet_Host::Nnet_Host(Nnet_Structure &structure) 
  : nnet(structure),
    input(structure.at(0)) {}

Nnet_Host::~Nnet_Host() {}

void Nnet_Host::run() {
  start_host(request_callback, this);
}
