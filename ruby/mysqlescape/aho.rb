require "mysql"


a = "ランボー物"

print a, "\n"

print Mysql.escape_string(a),"\n"
