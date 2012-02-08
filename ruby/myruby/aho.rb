require "/usr/lib/ruby/1.4/i486-linux/mysql.so"

$host = "void"
$user = "test"
$passwd = nil
$db = "test"

Mysql.new ( $host, $user , $passwd )
