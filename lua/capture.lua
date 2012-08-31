HOGE = 1
function defa()
  a = function()
    local q = HOGE
    print("1:", s,q )
  end
end

defa()

HOGE = 2


a()