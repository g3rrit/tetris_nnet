
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
      input.j_piece = piece - 3
   elseif(piece <= 9) then
      input.z_piece = piece - 7
   elseif(piece <= 0xa) then
      input.o_piece = 1
   elseif(piece <= 0xc) then
      input.s_piece = piece - 0xa
   elseif(piece <= 0x10) then
      input.l_piece = piece - 0xc
   elseif(piece <= 0x12) then
      input.i_piece = piece - 0x10
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

function compare(in1, in2) 
   if(in1 == nil or in2 == nil) then return false end
   for i=1, #in1.field do
      local val1 = in1.field:byte(i)
      local val2 = in2.field:byte(i)
      if(val1 ~= val2) then return false end
   end
   if(in1.t_piece ~= in2.t_piece
      or in1.j_piece ~= in2.j_piece
      or in1.z_piece ~= in2.z_piece
      or in1.o_piece ~= in2.o_piece
      or in1.s_piece ~= in2.s_piece
      or in1.l_piece ~= in2.l_piece
      or in1.i_piece ~= in2.i_piece
      or in1.x_pos ~= in2.x_pos 
      or in1.y_pos ~= in2.y_pos
      or in1.next_piece ~= in2.next_piece) then
      return false
   end
   return true
end

---- PLAYER_INPUT ----

Player_Input = {}
Player_Input.__index = Player_Input

function Player_Input:new() 
   local player_input = {}
   setmetatable(player_input, Player_Input)

   local jp = joypad.get(1)

   if(jp.left and not jp.right) then
      player_input.move = 1
   elseif(jp.right and not jp.left) then
      player_input.move = 2
   else player_input.move = 0 end

   if(jp.A and not jp.B) then
      player_input.rotate = 1
   elseif(jp.B and not jp.A) then
      player_input.rotate = 2
   else player_input.rotate = 0 end

   return player_input
end

--------------------

function save(file, inp, outp) 

   file:write("[ ")
   for i=1, #inp.field do
      local val = inp.field:byte(i)
      if(val == 239) then file:write("0 ")
      else file:write("1 ") end
   end
   file:write(inp.t_piece)
   file:write(" ")
   file:write(inp.j_piece)
   file:write(" ")
   file:write(inp.z_piece)
   file:write(" ")
   file:write(inp.o_piece)
   file:write(" ")
   file:write(inp.s_piece)
   file:write(" ")
   file:write(inp.l_piece)
   file:write(" ")
   file:write(inp.i_piece)
   file:write(" ")
   file:write(inp.x_pos)
   file:write(" ")
   file:write(inp.y_pos)
   file:write(" ")
   file:write(inp.next_piece)
   
   file:write(" ] [ ")
   file:write(outp.move)
   file:write(" ")
   file:write(outp.rotate)
   file:write(" ]\r\n")
   
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

game_count = 0
function new_game()
   local running = true
   local file = io.open(emu.getdir() .. "\\traindata\\" .. tostring(game_count) .. "_train.data", "w")
   if(file == nil) then emu.print("unable to open file") return end
   game_count = game_count + 1
   emu.poweron()
   wait_on_menue()
   skip_frames(10)

   -- select menue
   press_start()
   
   -- select music
   press_start()
   -- select level
   press_start()

   local prev = nil
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
      if(not compare(input, prev)) then
         local player_input = Player_Input:new()
         save(file, input, player_input)
      end
      prev = input
   end
end

while(true) do new_game() end

