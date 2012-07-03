require("./lumino/lumino")


local ch = require("childprocess")

--child = ch.spawn( "bash", { '-c', 'set' }, nil ) -- { env = { TEST1 = 1 } } )




-- globs: { "./*.lua", "./*/*.lua" }
function monitorFiles(globs,callback )
  local lastmtime={}
  every( 1, function()
      for _,tgt in ipairs(globs) do
        local cmd = "ls -1 " .. tgt 
        local child = ch.spawn( "bash", { '-c', cmd }, nil ) -- { env = { TEST1 = 1 } } )
        child.stdout:on("data", function(chunk)
            local ary = split(chunk,"\n")
            for _,path in ipairs(ary) do
              if path ~= "" then
                local s = uv.fsStat(path)
                if not lastmtime[path] then
                  lastmtime[path] = s.mtime
                else
                  if lastmtime[path] ~= s.mtime then
                    lastmtime[path] = s.mtime
                    callback(path)
                  end
                end                
              end
            end
          end)
      end  
    end)
end

  
local pathes = { "./*.lua", "./*/*.lua" }
--local pathes = { "./h*.lua" }

monitorFiles(pathes, function(changed)
    pp("changed!", changed )
  end)




