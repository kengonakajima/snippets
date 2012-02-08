require "rubygems"
#gem 'mysql'
require "mysql"


#my = Mysql.new("localhost", "ringo", "", "hogehoge")

#Mysql.connect("mysql://user:passwd@server:3306/dbname") do |my|
Mysql.connect("mysql://ringo:hoge@server:3306/dbname") do |my|
  my.query("select col1,col2 from tblname").each do |col1, col2|
    p col1, col2
  end
#  my.query("insert into tblname (col1,col2) values (?,?)", 123, "abc")
end







