require "rubygems"
require "mysql"


my = Mysql.connect("localhost","root","","test")

#print my.methods.sort,"\n"

#print my.escape_string("';"),"\n"

my.query("drop table if exists poo")

my.query("create table poo ( hoge char(100)) ")
my.query("insert into poo set hoge='zzz'")
res=my.query("select * from poo")
print res.fetch_row(),"\n"

def wrapped_query(my,fmt,*args)
  print args.class,"\n"
  q = fmt % args
  print "query:",q,"\n"
end

wrapped_query(my,"insert into poo set hoge='%s' fuga='%s'", "zoh", "bar")
wrapped_query(my,"insert into poo set hoge='aho'")


