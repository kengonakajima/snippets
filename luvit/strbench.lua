#!/usr/bin/lua
io = require("io") 
string = require("string")
os=require("os")
math=require("math")

io.stdout:setvbuf "no";             --  io.flush();

str='abcdefgh'..'efghefgh';
imax=1024/string.len(str)*1024*4;         -- 4mb

starttime=os.time();
print "exec.tm.sec\tstr.length";

gstr='';
i=0;

while i < imax+1000 do
        i=i+1;
        gstr=gstr..str;
        gstr=string.gsub(gstr,"efgh","____");
        lngth=string.len(str)*i;
        if(math.mod(lngth,1024*256)==0) then
                print(os.time()-starttime.."sec\t\t"..(lngth/1024).."kb");
        end
end
