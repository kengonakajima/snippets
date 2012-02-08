require "mysql"

m = Mysql.new("localhost","storage","","brain")

res = m.query( "show tables" )

res.each { |row|
	print row[0],"\n"
}

res = m.query( "select body from entry limit 100")

res.each { |row|
	print row[0],"\n"
}
