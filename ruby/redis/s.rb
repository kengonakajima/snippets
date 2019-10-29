require "redis"

r=Redis.new(host:"localhost")

10000.times do |i|
  ary=[]
  100.times do 
    ary.push(rand(1000000000))
  end
#  r.set "hoge_#{i}", ary.join(",")
  STDERR.print i,"\n" if i%100==0
  print "hoge_#{i}", ary.join(","),"\n"
end

# printしたら9MB
# bzip2したら4MBちょっと
# redisも9MB
# redisのLZFは大して効かない
