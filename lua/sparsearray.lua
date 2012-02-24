local cnt = 0
local a={}

while true do  
  for i=1,1000 do
    if not a[i*10000] then
      a[i*10000] = {}
    end    
    for j=1,1000 do
      local elem = a[i*10000][j*10000]      
      if elem then
        a[i*10000][j*10000] = a[i*10000][j*10000] + 1
      else
        a[i*10000][j*10000] = 0
      end      
    end  
  end
  cnt = cnt + 1
  print(cnt, a[1000*10000][1000*10000] )

end