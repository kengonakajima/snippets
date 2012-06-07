table = require("table")

mt = getmetatable(table)
print("mt should be nil:", mt )

--mt = {}
--setmetatable(table,mt)
--print("should be empty tbl:", mt )
--p(mt)



table.each = function(self,f)
  for i,v in ipairs(self) do
    f(v)
  end  
end

  
hoge={1,2,3}
hoge:each( function(v)
    print(v)
  end)
