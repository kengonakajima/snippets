
pid = Process.pid()
fn = "/tmp/hogehoge_#{pid}"
system( "rm -f #{fn}")

pids = []
10.times { |i|
    pids[i] = Process.fork() {
        system( "ruby base.rb >> #{fn} &")
    }
}
pids.each { |i|
    Process.waitpid(i)
}
 
system( "cat #{fn}" )   
system( "rm -f #{fn}")
