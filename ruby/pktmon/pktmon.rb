dev=ARGV[0]

if !dev then 
  STDERR.print "specify devname\n"
  exit 1
end

$uname=`uname`.strip
  

$last_sent_b=0
$last_recv_b=0
$last_sent_p=0
$last_recv_p=0
$sent_b=0
$recv_b=0
$sent_p=0
$recv_p=0

while true
  if $uname=="Darwin" then
    out=""
    nsout=`netstat -ib -I #{dev} | grep #{dev}`
    nsout.split("\n").each do |line|
      tks=line.split(/\s+/)
      ipkts=tks[4].to_i
      ibytes=tks[6].to_i
      opkts=tks[7].to_i
      obytes=tks[9].to_i
      out += "RX packets #{ipkts} bytes #{ibytes}\n"
      out += "TX packets #{opkts} bytes #{obytes}\n"
      print "----\n",out, "\n"
      break
    end
  else
    out=`ifconfig #{dev}`
  end
  out.split("\n").each do |line|
    tks=line.strip.split(/\s+/)
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
  diff_p=$sent_p-$recv_p
  per=sprintf("%.2f", diff_p*100.0/$sent_p)
  print "[#{ts}] pkt: r:#{$recv_p} s:#{$sent_p} s-r:#{diff_p}(#{per}%)  Bytes: r:#{$recv_b/1000000.0}B s:#{$recv_b/1000000.0}M\n"
  sleep 1
end
