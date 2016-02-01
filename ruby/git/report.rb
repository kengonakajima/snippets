require "rubygems"
require "git"

dir = ARGV[0]
g = Git.open( dir )
g.log.since( "1 week ago" ).each do |l|
  commit = g.gcommit(l.sha)
  print commit.date.strftime("%y-%m-%d"), " ", commit.message, "\n"
end





