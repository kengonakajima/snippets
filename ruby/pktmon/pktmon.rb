dev=ARGV[0]

if !dev then 
  STDERR.print "specify devname\n"
  exit 1
end

$last_sent_b=0
$last_recv_b=0
$last_sent_p=0
$last_recv_p=0
$sent_b=0
$recv_b=0
$sent_p=0
$recv_p=0

while true
  out=`ifconfig #{dev}`
  out.split("\n").each do |line|
    tks=line.split(" ")
    if(tks[0]=="RX" and tks[1]=="packets") then
      pn = tks[2].to_i
      bn = tks[4].to_i
      $recv_p = pn - $last_recv_p
      $recv_b = bn - $last_recv_b
      $last_recv_p=pn
      $last_recv_b=bn
    end
    if(tks[0]=="TX" and tks[1]=="packets") then
      pn = tks[2].to_i
      bn = tks[4].to_i
      $sent_p = pn - $last_sent_p
      $sent_b = bn - $last_sent_b
      $last_sent_p=pn
      $last_sent_b=bn
    end
  end
  ts = Time.now.to_i
  print "[#{ts}] pkt r:#{$recv_p} s:#{$sent_p}  bytes r:#{$recv_b/1000000.0}B s:#{$recv_b/1000000.0}M\n"
  sleep 1
end
