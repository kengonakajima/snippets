require "redis"
r=Redis.new(host:"localhost")

cnt=0
while true
  r.set "hoge_#{cnt}", Time.now.to_s
  cnt+=1
end
