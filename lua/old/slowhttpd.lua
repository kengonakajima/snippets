local MIME = require('mime')
local HTTP = require('http')
local Url = require('url')
local FS = require('fs')
local Response = require('response')
local timer = require("timer")
local coroutine = require("coroutine")
local table = require("table")
local io = require("io")

-- Monkey patch in a helper
function Response.prototype:not_found(reason)
  self:write_head(404, {
    ["Content-Type"] = "text/plain",
    ["Content-Length"] = #reason
  })
  self:write(reason)
  self:close()
end

-- Monkey patch in another
function Response.prototype:error(reason)
  self:write_head(500, {
    ["Content-Type"] = "text/plain",
    ["Content-Length"] = #reason
  })
  self:write(reason)
  self:close()
end

tasks = {}
function newFileSendTask(res,path)
   print("newFileSendTask:",res,path)
   local task = coroutine.wrap(
      function()
         local fp = io.open(path,"rb")
         local unit = 8192*4
         print("start task.res,path:", res,path )
         while true do
            local data = fp:read(unit)
            if not data then
               print("file-read finished")
--               res:finish("")
               coroutine.yield(nil)
            else
               res:write(data)
               print("wrote ", #data, "bytes")
               coroutine.yield()
            end
         end
      end)
   table.insert(tasks, task)
   return task
end


            
local root = "."
HTTP.create_server(
   "0.0.0.0", 8180,
   function(req, res)
      print("req,res:",req,res )
      req.uri = Url.parse(req.url)
      local path = root .. req.uri.pathname
      FS.stat(
         path,
         function (err, stat)
            if err then
               if err.code == "ENOENT" then
                  return res:not_found(err.message .. "\n")
               end
               return res:error(err.message .. "\n")
            end
            if not stat.is_file then
               return res:not_found("Requested url is not a file\n")
            end
            
            res:write_head(200, {
                              ["Content-Type"] = MIME.get_type(path),
                              ["Content-Length"] = stat.size
                           })

            newFileSendTask(res,path)
--            FS.create_read_stream(path):pipe(res)

         end)
   end)


timer.set_interval( 100,
                    function()
                       print("interval. tasks:", #tasks )
                       for i,v in ipairs(tasks) do
                          if v() == false then
                             table.remove(tasks,i)
                             break
                          end
                       end
                    end)
                             

print("Http static file server listening")
