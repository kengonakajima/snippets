#
# 長い時間かかるunixコマンドの並列実行をruby threadでできるのか・
#

def doslp(x)
  t = Thread.new do
    STDERR.print "start sleep: #{x}\n"
    a = `scp /Users/ringo/Downloads/Growl-1.2.2.dmg chundev@202.213.207.211:/home/chundev/tmp_#{x}.dmg`
    STDERR.print "end sleep\n"
  end
  return t
end

ary = []

10.times do |i|
  ary[i] = doslp(i)
end

10.times do |i|
  ary[i].join
end



STDERR.print "end\n"
