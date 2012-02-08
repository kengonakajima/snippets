os = require("os")

a={cnt=0}

a.aho = function(self)
           self.cnt = self.cnt + 1
        end


a["hoge"] = function(self)
               self.cnt = self.cnt + 1
            end

st = os.clock()
for i=1,100000000 do
   a:aho()
end
et = os.clock()

print("test1:", et-st, a.cnt)

st = os.clock()
for i=1,100000000 do
   a:hoge()
end
et = os.clock()
print("test2:", et-st, a.cnt)

