function aho(i)
   local o = {val=i}
   function o:hoge(j)
      print("j:",self.val+j)
   end
   return o
end


local a = aho(2)
a:hoge(100)
