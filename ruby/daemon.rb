



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


daemonize()

while true
  sleep 1
  system( "ls > /tmp/aaa" )
end




