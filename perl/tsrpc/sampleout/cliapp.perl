

use tsrpc_client;




$tc = tsrpc_client->new( "ss001.ip.media.kyoto-u.ac.jp" , 10000 );


if( 0){
for($i=0;$i<100;$i++){
	$ret = $tc->remotesum2( "answer$i" , (1,2,3,4,5,6) );
	print "Return from the server: $ret \n";
}

}






for( $i=0;$i<1000;$i++){
	$tc->udpcat( "ringo" , "hoge" );
}




$tc->destroy();





