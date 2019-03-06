--output_file = "tetris_net.in"
--input_file = "tetris_net.out"

socket = require "socket.core"
--connect to nnet server
s = socket.tcp()
s:connect("127.0.0.1", 7777)
if(not s) then
   emu.print("unable to connect to host")
   return
end
emu.print("connected to host")

---- MEMORY UTIL ----

function read_byte(addr)
   return memory.readbyte(addr)
end

function read_bytes(addr, count)
   return memory.readbyterange(addr, count)
end

function write_byte(addr, val)
   memory.writebyte(addr, val)
end

function write_bytes(addr, val_arr, size)
   for i=0, size do
      write_byte(addr + i, var_arr[i])
   end
end

---- NNET_INPUT ----

Input = {}
Input.__index = Input

function Input:new() 
   local input = {}
   setmetatable(input, Input)
   
   input.field = read_bytes(0x400, 200)
   
   local piece = read_byte(0x62)
   input.t_piece = 0
   input.j_piece = 0
   input.z_piece = 0
   input.o_piece = 0
   input.s_piece = 0
   input.l_piece = 0
   input.i_piece = 0

   if(piece <= 3) then
      input.t_piece = piece + 1
   elseif(piece <= 7) then
      input.j_piece = piece - 2
   elseif(piece <= 9) then
      input.z_piece = piece - 6
   elseif(piece <= 0xa) then
      input.o_piece = 1
   elseif(piece <= 0xc) then
      input.s_piece = piece - 9
   elseif(piece <= 0x10) then
      input.l_piece = piece - 0xb
   elseif(piece <= 0x12) then
      input.i_piece = piece - 0xf
   else
      print("input error piece: ", piece)
   end

   input.x_pos = read_byte(0x60)
   input.y_pos = read_byte(0x61)

   local next_piece = read_byte(0xbf)
   if(next_piece <= 3) then
      input.next_piece = 0
   elseif(next_piece <= 7) then
      input.next_piece = 1
   elseif(next_piece <= 9) then
      input.next_piece = 2
   elseif(next_piece <= 0xa) then
      input.next_piece = 3
   elseif(next_piece <= 0xc) then
      input.next_piece = 4
   elseif(next_piece <= 0x10) then
      input.next_piece = 5
   elseif(next_piece <= 0x12) then
      input.next_piece = 6
   else
      print("input error next_piece: ", next_piece)
   end

   input.drop_time = read_byte(0x65)

   return input
end

function Input:send()

   s:send(self.field)
   s:send(self.t_piece)
   s:send(self.j_piece)
   s:send(self.z_piece)
   s:send(self.o_piece)
   s:send(self.s_piece)
   s:send(self.l_piece)
   s:send(self.i_piece)
   s:send(self.x_pos)
   s:send(self.y_pos)
   s:send(self.next_piece)
   s:send(self.drop_time)

end

--- NNET_OUTPUT ----

Output = {}
Output.__index = Output

function Output:new()
   local output = {}
   setmetatable(output, Output)

   local move = nil
   while(move == nil) do
      move = s:receive(1)
   end
   -- | 0 - dont move
   -- | 1 - move left
   -- | 2 - move right
   local rotate = nil
   while(rotate == nil) do
      rotate = s:receive(1)
   end
   -- | 0 - dont rotate
   -- | 1 - rotate left 
   -- | 2 - rotate right

   if(move == 0) then
      output.move_left = false
      output.move_right = false
   elseif(move == 1) then
      output.move_left = true
      output.move_right = false
   elseif(move == 2) then
      output.move_left = false
      output.move_right = true
   else
      emu.print("input error move: ", move)
   end

   if(rotate == 0) then
      output.rotate_left = false
      output.rotate_right = false
   elseif(rotate == 1) then
      output.rotate_left = true
      output.rotate_right = false
   elseif(rotate == 2) then
      output.rotate_left = false
      output.rotate_right = true
   else 
      print("input error rotate: ", rotate)
   end

   return output
end

function Output:exec()
   local jp = joypad.get(1)
   jp.left = self.move_left
   jp.right = self.move_right
   jp.A = self.rotate_left
   jp.B = self.rotate_right
   joypad.set(1, jp)
end
--------------------

function file_exists(path) 
   local f=io.open(path,"r")
   if f~=nil then io.close(f) return true else return false end
end

function wait_on_menue()
   for i=0, 100 do
      emu.frameadvance()
   end
   while((read_byte(0x10c3) ~= 0) or (read_byte(0x10a8) ~= 0)) do
      emu.frameadvance()
   end
end

function skip_frames(n) 
   for i = 0, n do
      emu.frameadvance()
   end
end

function press_start()
   local jp = joypad.get(1)
   jp.start = true
   joypad.set(1, jp)
   emu.frameadvance()
   skip_frames(10)
end


function new_game()
   local running = true
   emu.poweron()
   wait_on_menue()
   skip_frames(10)

   -- select menue
   press_start()
   
   -- select music
   press_start()
   -- select level
   press_start()

   while(running) do
      -- set line_count so transition doesnt happen
      write_byte(0x70, 0)
      
      -- check for gameover
      if(read_byte(0x400) == 0x4f) then
         running = false
         break
      end

      emu.frameadvance()
      local input = Input:new()
      input:send()
      print(input)

      local output = Output:new()
      output:exec()
   end
end

while(true) do new_game() end

