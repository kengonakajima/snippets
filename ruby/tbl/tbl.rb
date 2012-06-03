# input: 2D array
# a = [ [1,"helllllo",3], [500,100, "bb" ] ] 
# 
# outout: text table
#   1 helllllo  3
# 500      100 bb
#
# 

def gentbl(t)
  colszs=[]
  t.each do |line|
    line.size.times do |i|
      elem = line[i]
      sz = elem.to_s.size
      colszs[i] = sz if !colszs[i] or colszs[i] < sz 
    end 
  end
  out = ""
  t.each do |line|
    line.size.times do |i|
      elem = line[i].to_s
      col = " " * colszs[i]
      elem.size.times do |j|
        col[j] = elem[j]
      end
      out += col + " "
    end
    out += "\n"
  end
  return out
end


t = [
  [ 1, "hello", 3 ],
  [ 500, 100, "hoge" ]
]
print gentbl(t)

