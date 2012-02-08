

def daemonize()
  exit!(0) if fork
  Process::setsid
  exit!(0) if fork
#  Dir::chdir("/")
  File::umask(0)
  STDIN.reopen("/dev/null")
  STDOUT.reopen("/dev/null", "w")
  STDERR.reopen("/dev/null", "w")
  yield if block_given?
end


$gdbscript = <<EOF
handle SIGPIPE nostop noprint
run
where
list
quit
EOF

$gdbscriptfile= "autogdb.scr"

def output( path, str )
  f = File.open( path, "a+" )
  f.print "#{Time.now.to_s} #{str}\n"
  f.close
end

def strdate()
  t = Time.now
  return "#{t.year}_#{t.month}_#{t.day}_#{t.hour}_#{t.min}_#{t.sec}"
end

def main
  if ARGV.size != 2 then
    STDERR.print "Usage: ruby endless.rb program logfile\n"
    exit 1
  end

  program = ARGV[0]
  logfile = ARGV[1]

  # start

  daemonize()

  scr = File.open( $gdbscriptfile,"w")
  scr.write( $gdbscript )
  scr.close

  pid = File.open( "./endless.rb.pid", "w")
  pid.write( Process.pid.to_s )
  pid.close

  output( logfile, "start endless" )
  output( logfile, "pwd: #{Dir.pwd}" )
  loop=0
  while 1
    loop += 1
    output( logfile, "loop cnt:#{loop}" )
    output( logfile, "copy #{program} to #{program}_endless" )
    system( "cp #{program} #{program}_endless" )
    output( logfile, "starting gdb #{program}_endless .." )

    cmd = "cat #{$gdbscriptfile} | gdb #{program}_endless  >> gdb.log.#{strdate()} 2>&1"
    output( logfile, cmd )
    system( cmd )

#	gprof zonesv1 > log/prof.`date '+%Y_%m_%d'`
#    echo mailing
#    ruby gdbmailer.rb gdb.log
    output( logfile, "sleeping.." )
    sleep 10
    output( logfile, "finish sleeping" )
  end
    
end

#
#
#
main()
