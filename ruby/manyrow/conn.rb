require "rubygems"
require "mysql"

ary = []
10.times { |i|
  t = Thread.new do
    my = Mysql::new( "192.168.1.121", "root", "", "vcx_dev")
    my.query( "drop table if exists hoge#{i}" )
    my.query( "create table hoge#{i} ( id int )" )
    (1000 * 1000 * 10 / n ).times { |i|
      STDERR.print "." if ( i % (100) ) == 0 
      my.query( "insert into hoge set id = #{i}" )
    }
  end
  ary.push(t)
}

ary.each { |x|
  x.join
}

