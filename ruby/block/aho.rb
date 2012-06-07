def a(hoge, &blk)
  blk.call(100,hoge)
end


a(10) do |arg1,arg2|
  print("aa:",arg1,arg2)
end


