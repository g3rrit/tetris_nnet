#pragma once

#include "nstd.h"

#define REQUEST_SIZE 210
struct Request {
  u8 field[20 * 10];
  u8 t_piece;
  u8 j_piece;
  u8 z_piece;
  u8 o_piece;
  u8 s_piece;
  u8 l_piece;
  u8 i_piece;
  u8 x_pos;
  u8 y_pos;
  u8 next_piece;
  u8 drop_time;
};

#define RESPONSE_SIZE 2
struct Response {
  u8 move;
  u8 rotate;
};

int start_host(void(*)(void*, Request*, Response*), void*);
