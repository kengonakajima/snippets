require "./lumino/lumino"

local n = 10000000
cnt=0


tbl={}
for i=1,1000  do
  tbl[i] = function() cnt = cnt + 1 end
end
function checkit2(v)
  f = tbl[v]
  if f then f() end
end

  

function checkit1(v)
  if v==1 then
    cnt = cnt + 1
  elseif v==2 then
    cnt = cnt + 1    
  elseif v==3 then
    cnt = cnt + 1    
  elseif v==4 then
    cnt = cnt + 1    
  elseif v==5 then
    cnt = cnt + 1    
  elseif v==6 then
    cnt = cnt + 1    
  elseif v==7 then
    cnt = cnt + 1    
  elseif v==8 then
    cnt = cnt + 1    
  elseif v==9 then
    cnt = cnt + 1        
  elseif v==10 then
    cnt = cnt + 1        
  elseif v==11 then
    cnt = cnt + 1        
  elseif v==12 then
    cnt = cnt + 1        
  elseif v==13 then
    cnt = cnt + 1        
  elseif v==14 then
    cnt = cnt + 1        
  elseif v==15 then
    cnt = cnt + 1        
  elseif v==16 then
    cnt = cnt + 1        
  elseif v==17 then
    cnt = cnt + 1        
  elseif v==18 then
    cnt = cnt + 1        
  elseif v==18 then
    cnt = cnt + 1        
  elseif v==19 then
    cnt = cnt + 1        
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1        
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1
  elseif v==20 then
    cnt = cnt + 1            
  end  
end

function doit1()
  for i=1,n do
    checkit1(i)
  end
end

function doit2()
  for i=1,n do
    checkit2(i)
  end
end


t = measure( function() doit1() end)
print("T1:", t )

t = measure( function() doit2() end)
print("T2:", t )