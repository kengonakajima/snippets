f = function()
  local t = {}


  function t.bb()
    aa()
  end

  function aa()
    print("aa")
  end
  
  return t    
end

outt = f()

outt.bb()
