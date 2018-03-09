<?php

// roomサーバにつないで部屋を作る。
// 以下のどちらかを返す
// "ERROR 理由の文字列"
// "OK {"room_id":699,"owner_id":0}"
function mrs_room_create_room( $host, $port, $room_id, $owner_id ) {
    $sk=socket_create(AF_INET,SOCK_STREAM,0);
    $ret=socket_connect($sk,$host,$port);
    if(!$ret) {
        socket_close($sk);
        return "ERROR connect error!\n";
    }

    $outval="ERROR internal error\n";
    
    $start_at=time();
    while(true) {
        $now=time();
        if($now-$start_at>1) {
            $outval = "ERROR timeout";
            break;
        }
    
        $rs=array($sk);
        $ws=array($sk);
        $es=array($sk);
        $n=socket_select($rs,$ws,$es,0);
        if($rs[0]) {
            $bytes=socket_read($rs[0],1024,PHP_BINARY_READ);
            var_dump("socket_read:",bin2hex($bytes) );
            $out=unpack("Vlen/Vseq/vopt/vpktype", $bytes);
            $pktype=$out["pktype"];
            if($pktype==0xff00) {
                // version_checkを受信。
                // バージョンデータをそのまま送り返す
                $wlen=strlen(bin2hex($bytes))/2;
                $wr=socket_write($rs[0],$bytes,$wlen);
                if($wr!=$wlen) { $outval="ERROR version write error"; break; }
            
                // create_roomを送る
                $l=4+4+2+2 + 8+4+4;
                $bytes=pack("VVvvPVV",
                            $l-4,
                            0x1, // seq
                            0x0, // opt
                            0x0101, // RPC_ID_CREATE_ROOM
                            $room_id,
                            $owner_id,
                            0x0 );
                $wr = socket_write($sk,$bytes,$l);
                if($wr!=$l) { $outval="ERROR create_room write error"; break; }
            } else if($pktype==0x0101) { // RPC_ID_CREATE_ROOM
                // create_roomの結果を受信             
                $out=unpack("Vlen/Vseq/vopt/vpktype/vresult/Proomid/Vowner/Vdatalen", $bytes);
                echo "res:",$out["result"], "room:",$out["roomid"],"owner:",$out["owner"], "datalen:",$out["datalen"], "\n";                
                $res=$out["result"];
                if($res==0) {
                    $a=array("room_id"=> $out["roomid"], "owner_id"=> $out["owner"] );
                    $outval="OK ".json_encode($a);
                } else {
                    $res-=0x10000;
                    $outval="ERROR room_server error:".(string)$res;
                }
                break;
            }
        }
        if($ws[0]) {
            // TODO: 書き込むときにほんとは$wsの状態を見ないといけないが、めんどくさいのでそのままいってる
        }
        usleep(10*1000);
    }
    
    socket_close($sk);
    return $outval;
}

$result = mrs_room_create_room( "52.192.193.235", 33333, 69, 0 ); // 0にするとじどうさくせい

echo "mrs_room_create_room result:",$result,"\n";




/*
  roomサーバへの送信
  
  4バイト レコード長 常に 0x18 0x00 0x00 0x00
  4バイト 順序番号 0x01 0x00 0x00 0x00
  2バイト オプション 0x00 0x00
  2バイト ペイロードタイプ(RPC_ID_CREATE_ROOM) 0x01 0x01

  8バイト ルームID番号
  4バイト 作成者プレイヤーID番号
  4バイト おまけデータ長 0x00 0x00 0x00 0x00

  roomサーバからの受信

  4バイト レコード長 常に 0x18 0x00 0x00 0x00
  4バイト 順序番号 無視
  2バイト オプション 無視
  2バイト ペイロードタイプ(RPC_ID_CREATE_ROOM) 0x01 0x01　無視

  2バイト 結果コード MRS_ROOM_RESULT_*
  8バイト ルームID番号
  4バイト 作成者プレイヤーID番号 無視
  4バイト おまけデータ長 いつも0

*/
