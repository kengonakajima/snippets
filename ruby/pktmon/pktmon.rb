dev=ARGV[0]

if !dev then 
  STDERR.print "specify devname\n"
  exit 1
end

$last_sent=0
$last_recv=0
$sent=0
$recv=0

while true
  out=`ifconfig #{dev}`
  out.split("\n").each do |line|
    tks=line.split(" ")
    if(tks[0]=="RX" and tks[1]=="packets") then
      n = tks[2].to_i
      $recv = n - $last_recv
      $last_recv=n
    end
    if(tks[0]=="TX" and tks[1]=="packets") then
      n = tks[2].to_i
      $sent = n - $last_sent
      $last_sent=n
    end
  end
  ts = Time.now.to_i
  print "[#{ts}] Recv:#{$recv} Sent:#{$sent}\n"
  sleep 1
end
