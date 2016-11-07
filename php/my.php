<!DOCTYPE html>
<html>
<body>

<h1>php mysql test</h1>

<?php

   echo "mysql test\n";

   mysql_connect( "localhost", "root", "" );
   
   $res = mysql_select_db("test");
   if(!$res) { die("error:".mysql_error()); }
   
   echo "selected db\n";

   $res = mysql_query( "show tables");
   print($res);
   while ($row = mysql_fetch_array($res, MYSQL_NUM)) {
       printf("table name: %s\n", $row[0]);
   }
   mysql_free_result($res);

   $res = mysql_query( "drop table if exists hoge");
   $res = mysql_query( "create table hoge (id int, name char(32))" );
   if(!$res) { die("cant create table"); } else { print("created table\n");}

   $res = mysql_query("insert into hoge values( 100, 'poo')");
   $res = mysql_query("insert into hoge values( 200, 'ahpoo')");
   $res = mysql_query("insert into hoge values( 3000, 'hasdkfoo')");   

   $res = mysql_query("select * from hoge");
   
   while ($row = mysql_fetch_array($res, MYSQL_NUM)) {
       printf("selected. id:%s name:%s\n", $row[0], $row[1]);
   }
   
   mysql_close();
?>

</body>
</html>
