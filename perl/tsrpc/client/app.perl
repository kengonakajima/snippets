use tsrpc_client;






$hoge = tsrpc_client->new( "ss001" , 10000 );
$fuck = tsrpc_client->new( "lsdev" , 33 );


$hoge->out();

$fuck->out();