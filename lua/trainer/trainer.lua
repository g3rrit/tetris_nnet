

---- MEMORY UTIL ----

function read_byte(addr)
   return memory.readbyteunsigned(addr)
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
   
   input.field = read_bytes(0x400, 0xc7)
   
   local piece = read_byte(0x62)
   -- TODO

   input.x_pos = read_byte(0x60)
   input.y_pos = read_byte(0x61)

   input.drop_time = read_byte(0x65)

   return input
end

function Input:save()
   local file = io.open(output_file, "wb")

   file:write(self.field)
   file:write(self.x_pos)
   file:write(self.y_pos)
   file:write(self.drop_time)

   io.close(file)
end

---- PLAYER_INPUT ----

Player_Input = {}
Player_Input.__index = Player_Input

function Player_Input:new() 
   local player_input = joypad.get(1)
   setmetatable(player_input, Player_Input)
   return player_input
end

--------------------

function wait_on_menue()
   for i=0, 100 do
      emu.frameadvance()
   end
   while((read_byte(0x10c3) ~= 0) or (read_byte(0x10a8) ~= 0)) do
      emu.frameadvance()
   end
end

function start()
   emu.poweron()
   update()
end


function update() 
   local running = true
   wait_on_menue()
   emu.print("done waiting")
   while(running) do
      local jp = joypad.get(1)
      emu.print(jp)
      emu.frameadvance()
   end
end


start()
