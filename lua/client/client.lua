outputfile = arg[1]

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

--------------------

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

--------------------

function update()
   local running = true
   while(running) do
      emu.frameadvance()
      local input = Input:new()
      input:save()
   end
end

function start()
   emu.poweron()
   update()

end


start()
