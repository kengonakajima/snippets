#

require "rubygems"
require "sqlite3"

$db = SQLite3::Database.new("_mp.db")



def do_query(q)
  while true
    begin
      $db.execute(q)
    rescue Exception => ex
      if ex.code == 5 then
        sleep 0.1
        STDERR.print "waiting for lock #{Process.pid}\n"
        next
      end
      STDERR.print ex,"\n"
    end
    break
  end
end

do_query( "create table if not exists entries ( id varchar(80), doc_id varchar(16) )" )
do_query( "create index idindex on entries(id)")
do_query( "create index docindex on entries(doc_id)")

do_query("pragma journal_mode=memory")


100.times do |j|
  STDERR.print "tr begin #{j} pid:#{Process.pid}\n"  
  do_query("begin transaction")
  10000.times do |i|
    k = (rand()*65536).to_i
    do_query( "insert into entries values ( 'id.#{k}', 'doc.#{Process.pid}' ) " )
  end
  do_query("end transaction")
  STDERR.print "tr end\n"
  sleep 2*rand()
end


$db.close


