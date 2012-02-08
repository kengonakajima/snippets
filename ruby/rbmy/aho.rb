require "mysql"

m = Mysql.new( "localhost", "toycvs", "", "numtable" )

res = m.query( "select * from rule" )

fields = res.fetch_fields # .filter { |f|
#    f.name 
#}

puts fields.join("\t")

res.each { |row|
    puts row.join("\t")
}

