require "mysql"


a = "�����{�[��"

print a, "\n"

print Mysql.escape_string(a),"\n"
