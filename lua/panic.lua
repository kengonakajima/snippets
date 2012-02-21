function errh(erro)
  print( debug.traceback() )
  for k,v in pairs(_G) do print("GLOBAL:" , k,v) end
  return false
end


function a()
  print(1)
  print(10)
  print(100)
  printttttt(1000)
  print(10000)
end

function b()
  a()
end


--xpcall( function()
    b()
--  end, errh )
