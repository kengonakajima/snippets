
$total_p=0
$total_p_d=0
$total_delivered_p=0
$total_delivered_p_d=0
$total_e=0
$total_e_d=0

while true
  nsout=`netstat -s`
  nsout.split("\n").each do |line|
    line.strip!
    if line =~ /total packets received/ then
      p=line.split(/\s+/)[0].to_i
      d = p - $total_p
      $total_p=p;
      $total_p_d=d
    end
    if line =~ /incoming packets delivered/ then
      p=line.split(/\s+/)[0].to_i
      d = p - $total_delivered_p
      $total_delivered_p=p
      $total_delivered_p_d=d
    end
    if line =~ /packet receive error/ then
      p=line.split(/\s+/)[0].to_i
      d = p - $total_e
      $total_e=p
      $total_e_d=d      
    end
  end
  ts = Time.now
  discarded=$total_p_d - $total_delivered_p_d
  print "[#{ts}] Packets in:#{$total_p_d} delivered:#{$total_delivered_p_d} discarded:#{discarded} err:#{$total_e_d}\n"
  sleep 1

end
