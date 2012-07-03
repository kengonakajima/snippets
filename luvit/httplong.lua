local table = require("table")

local http = require("http")

local ary = {}
local len = 10000000
for i=1,len do
  if (i%100)==0 then ary[i] = "\n" else ary[i] = "." end  
end
ary[len+1] = "END"
local longstr = table.concat(ary)

local sv = http.createServer(function (req, res)
  res:writeHead(200, {
    ["Content-Type"] = "text/plain",
    ["Content-Length"] = #longstr
  })
  res:finish(longstr)
end):listen(8080)

print("Server listening at http://localhost:8080/")



