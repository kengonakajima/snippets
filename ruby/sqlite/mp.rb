#

require "rubygems"
require "sqlite3"

$db = SQLite3::Database.new("_mp.db")

$db.execute( "create table if not exists entries ( id varchar(80), doc_id varchar(16) )" )
begin
  $db.execute( "create index idindex on entries(id)")
rescue
end


def do_query(q)
  while true
    begin
      $db.execute(q)
    rescue
      sleep 0.1
      STDERR.print "waiting for lock #{Process.pid}\n"
      next
    end
    break
  end
end

do_query("pragma journal_mode=memory")


300.times do |j|
  STDERR.print "tr begin\n"  
  do_query("begin transaction")
  10000.times do |i|
    k = (rand()*65536).to_i
    do_query( "insert into entries values ( 'id#{k}', 'val=#{Process.pid}' ) " )
  end
  do_query("end transaction")
  STDERR.print "tr end\n"  
end


$db.close


