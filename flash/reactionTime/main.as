package
{
    import flash.display.*;
    import flash.events.*;
    import flash.net.*;
    import flash.text.*;
    import flash.system.*;
    import flash.utils.ByteArray;
    import flash.utils.Timer;
    import flash.text.Font;
    import flash.geom.*;
    import flash.ui.Keyboard;
                
    public class main extends Sprite{
        private var timer:Timer;
        private var format:TextFormat;

        private var sprnum:int = 0 ;
        private var sprites:Array;

        private const WIDTH:int = 500;
        private const HEIGHT:int = 400;
        private const AIMDISTANCE:int = 150;
        
        private function mouseDownHandler( event:MouseEvent ):void {
            var f:FlyingObject = findFlyingObject( "aim" );
            f.x = event.stageX;
            f.y = event.stageY;

        }
        private function keydownHandler(event:KeyboardEvent):void {
            var i:int;
            switch(event.keyCode){
            case Keyboard.SPACE:
                addEnemy( 100, 0 );
                break;
            case Keyboard.ENTER:
                break;
            default:
                break;
            }
        }

        public function effectThink( tgt:FlyingObject ):void {
            if( tgt.getCounter() > 3 ){
                tgt.setToRemove(true);
            }
        }
        public function addEffect( x:int, y:int , col:int):FlyingObject {
            var f:FlyingObject = addFlyingObject( x,y, 2, col );
            f.setThinkFunction( effectThink );
            f.name = "effect";
            return f;
        }
        
        public function enemyThink(tgt:FlyingObject):void{
            checkBoundary(tgt);

            if( ( tgt.getCounter() % 20 ) == 0 ){
                var myship:FlyingObject = findFlyingObject( "myship");
                addBullet( tgt.x, tgt.y,
                           myship.x - 50 + (100*Math.random()),
                           myship.y - 50 + (100*Math.random()) );
            }

        }
        public function addEnemy( x:int, y:int ):FlyingObject {
            var f:FlyingObject = addFlyingObject( x,y,15,0xff0000 );
            f.setDxDy(-3 + (6*Math.random()),5 + (3*Math.random()) );
            f.setThinkFunction( enemyThink );
            f.name = "enemy";
            return f;
        }

        public function checkBoundary(tgt:FlyingObject):void {
            if( tgt.x < 0 || tgt.y < 0 || tgt.x > WIDTH || tgt.y > HEIGHT ){
                tgt.setToRemove(true);
            }
        }
                
        public function addBullet( x:int, y:int, tox:int, toy:int ):FlyingObject {
            var f:FlyingObject = addFlyingObject( x, y, 4, 0x101010 );
            var len:Number = Math.sqrt( (tox-x)*(tox-x) + (toy-y)*(toy-y));
            var normV:Point = new Point( (tox - x )/len, (toy-y)/len);
            //            trace("vxvx", normV.x, ",", normV.y , "xy:", x, ",", y );

            f.setThinkFunction( checkBoundary );
            f.setDxDy( normV.x*2, normV.y*2 );
            f.name = "bullet";
            return f;
        }
        public function myShipThink( tgt:FlyingObject ):void {

            // dot product
            var bullets:Array = findFlyingObjects( "bullet" );
            var i:int;
            
            var dotProducts:Array = new Array();
            var dxTable:Array = new Array();
            dxTable[0] = -1;
            dxTable[1] = 0;
            dxTable[2] = 1;
            dxTable[3] = -1;
            dxTable[4] = 0;
            dxTable[5] = 1;
            dxTable[6] = -1;
            dxTable[7] = 0;
            dxTable[8] = 1;
            var dyTable:Array = new Array();
            dyTable[0] =1;
            dyTable[1] =1;
            dyTable[2] =1;
            dyTable[3] =0;
            dyTable[4] =0;
            dyTable[5] =0;
            dyTable[6] =-1;
            dyTable[7] =-1;
            dyTable[8] =-1;
            

            for(var k:int=0;k<9;k++){
                var nextPos:Point = new Point( tgt.x + dxTable[k],
                                                   tgt.y + dyTable[k]);
                var dotTotal:Number = 0;
                for(i=0;i<bullets.length;i++){                    
                    var btomV:Point = new Point(bullets[i].x - nextPos.x,
                                                bullets[i].y - nextPos.y );
                    var bV:Point = bullets[i].getDxDy();
                    var dot:Number = bV.x * btomV.x + bV.y * btomV.y;
                    var len:Number = Math.sqrt(btomV.x*btomV.x+btomV.y*btomV.y);
                    dotTotal += Math.abs(dot) / len;
                }
                if( dotTotal == 0 ){
                    dotTotal = 999999999999999;
                }
                dotProducts.push(dotTotal);                
            }
            var min:Number = 999999999999999;
            var idealInd:int = 4;
            for(i=0;i<9;i++){
                if( dotProducts[i] < min ){
                    min = dotProducts[i];
                    idealInd = i;
                }
            }
            trace("dotPs:", dotProducts[idealInd] );
            if( dotProducts[idealInd] < 5 ){
                var idealP:Point = new Point( tgt.x + dxTable[idealInd],
                                              tgt.y + dyTable[idealInd] );


                goToIdealPoint(tgt,idealP);
            }

            //                var f:FlyingObject = findFlyingObject( "aim" );
            //                goToIdealPoint(tgt,new Point( f.x, f.y + AIMDISTANCE));

            
            /*            
            // ideal point
            var bullets:Array = findFlyingObjects( "bullet" );
            var i:int;
            var p:Point = new Point(0,0);
            var idealP:Point = new Point(0,0);
            
            for(i=0;i<bullets.length;i++){
                p.x += bullets[i].x;
                p.y += bullets[i].y;
                var dV:Point = new Point( tgt.x - bullets[i].x,
                                          tgt.y - bullets[i].y );
                var len:Number = Math.sqrt( dV.x * dV.x + dV.y * dV.y );
                var pV:Point = new Point( dV.x / len / len,
                                          dV.y / len / len );

                idealP.x += pV.x;
                idealP.y += pV.y;
            }
            p.x /= bullets.length;
            p.y /= bullets.length;
            addEffect( p.x, p.y, 0xffffff );

            //
            idealP.x *= 100;
            idealP.y *= 100;

            idealP.x += tgt.x;
            idealP.y += tgt.y;
            addEffect( idealP.x, idealP.y, 0xff8080 );
            trace("idealp:", idealP.x, ",", idealP.y );

            // 90
            var idealP2:Point = new Point( idealP.x - tgt.x,
                                           idealP.y - tgt.y );
            var rad:Number = Math.atan2( idealP2.x, idealP2.y );
            var il:Number = Math.sqrt( idealP2.x*idealP2.x+idealP2.y*idealP2.y);
            rad += Math.PI/2;
            idealP2 = new Point( Math.cos(rad)*il, Math.sin(rad)*il);
            idealP2.x += tgt.x;
            idealP2.y += tgt.y;
            addEffect( idealP2.x, idealP2.y, 0x00ffff);
            trace("idealp2", idealP2.x, ",", idealP2.y);
            

            if( Math.abs(idealP2.x - tgt.x ) > 2 ||
                Math.abs(idealP2.y - tgt.y ) > 2 ){
                goToIdealPoint(tgt,idealP2);
            } else {
                var f:FlyingObject = findFlyingObject( "aim" );
                goToIdealPoint(tgt,new Point( f.x, f.y + AIMDISTANCE));
            }
            */
        }
        // move from tgt to p
        public function goToIdealPoint( tgt:FlyingObject , p:Point ):void {
            const SPEED:int = 3;
            var dx:int, dy:int;
            dx=0;
            dy=0;
            if( tgt.x > p.x ){
                dx = - SPEED;
                if( tgt.y > p.y ){
                    dy = - SPEED;
                } else if( tgt.y < p.y ){
                    dy = SPEED;
                }
            } else if( tgt.x < p.x ){
                dx = SPEED;
                if( tgt.y > p.y ){
                    dy = - SPEED;
                } else if( tgt.y < p.y ){
                    dy = - SPEED;
                }                    
            } else {
                if( tgt.y > p.y ){
                    dy = - SPEED;
                } else if( tgt.y < p.y ){
                    dy = SPEED;
                }                    
            }

            if( (dx > 0 && tgt.x < WIDTH-20 ) ||
                ( dx < 0 && tgt.x > 20 )){
                tgt.x += dx;
            }
            if( (dy > 0 && tgt.y < HEIGHT-20 ) ||
                ( dy < 0 && tgt.y > 20 )){
                tgt.y += dy;
            }            
        }
        
        public function addMyShip( x:int, y:int ):FlyingObject {
            var f:FlyingObject = addFlyingObject( x,y, 10,0xffffff );
            f.name = "myship";
            f.setThinkFunction( myShipThink );
            return f;
        }
        public function addAim( x:int, y:int ):FlyingObject {
            var f:FlyingObject = addFlyingObject( x,y, 8, 0x00ff00 );
            f.name = "aim";
            return f;
        }
        
        public function addFlyingObject( x:int, y:int, sz:int, col:int ):FlyingObject {
            var f:FlyingObject = new FlyingObject( x,y, sz, col );
            addChild(f);
            sprites.push(f);
            return f;
        }
        public function findFlyingObject( name:String ):FlyingObject {
            for( var i:int = 0; i<sprites.length;i++){
                if(sprites[i] && sprites[i].name == name ){
                    return sprites[i];
                }
            }
            return null;
        }
        public function findFlyingObjects( name:String ):Array {
            var a:Array = new Array();
            for( var i:int = 0; i<sprites.length;i++){
                if(sprites[i] && sprites[i].name == name ){
                    a.push(sprites[i]);
                }
            }
            return a;
        }
        
        public function main()
        {
            // from bankoma
            stage.scaleMode = "noScale";
            stage.frameRate = 60;
            stage.align="TL";

            // text input field
            var thisobj:Sprite = this;

            // sprite num test
            sprites = new Array(2000);


            // keyboard events
            stage.addEventListener( KeyboardEvent.KEY_DOWN, keydownHandler );
            stage.addEventListener( MouseEvent.MOUSE_DOWN, mouseDownHandler );

            // timer
            addEventListener( "enterFrame", enterFrame );


            //tf
            format = new TextFormat();
            format.font ="ヒラギノ丸ゴ Pro W4";
            format.size = 16;
            format.color=0xffffff;
            format.bold =true;

            
            trace(sprites.length);

            var f:FlyingObject = addMyShip(WIDTH/2, HEIGHT-50 );
            addAim( f.x, f.y + AIMDISTANCE );

        }
        private function enterFrame( e:Event ):void {
            for( var i:int = 0; i<sprites.length;i++){
                if(sprites[i]){
                    sprites[i].move();
                }
            }
            for( i = 0; i<sprites.length;i++){
                if( sprites[i] && sprites[i].getToRemove()){
                    removeChild(sprites[i] );
                    sprites[i] = null;
                }
            }
            ///////////////////////////////////////////////////
            // pop enemy
            ///////////////////////////////////////////////////
            if( Math.random() < 0.02 ){
                addEnemy( WIDTH * Math.random(), 0 );
            }

            
        }

    }
}
