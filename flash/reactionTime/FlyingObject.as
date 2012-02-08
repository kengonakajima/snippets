package {
	import flash.display.*;
	import flash.net.*;   
	import flash.events.*;
	import flash.filters.*;
    import flash.geom.*;
    
    import flash.text.*;
    import flash.system.*;
    import flash.utils.ByteArray;

    public class FlyingObject extends Sprite {
        private var dx:Number;
        private var dy:Number;
        private var counter:int;
        private var icon:Loader;
        private var tf:TextField;

        private var f:Function;

        private var toRemove:Boolean;
        
        // if sz==0 then use it as a filename.
        public function FlyingObject( x:int, y:int, sz:int, col:int ){
            this.x = x;
            this.y = y;
            this.dx = this.dy = 0;
            this.graphics.lineStyle( undefined, 0 );
            this.graphics.beginFill( col );
            this.graphics.drawCircle( 0,0, sz );
            counter = 0;
            toRemove = false;
        }
        public function setDxDy( dx:Number, dy:Number ):void {
            this.dx = dx;
            this.dy = dy;
        }
        public function getDxDy():Point {
            return new Point( dx,dy );
        }
        public function getCounter():int {
            return counter;
        }
        public function setCounter(c:int ):int {
            return counter = c;
        }
        public function move():void{
            x += dx;
            y += dy;
            counter++;
            if(f != null ){
                f.call(this,this);
            }
        }
        public function setThinkFunction( f:Function  ):void {
            this.f = f;
        }
        public function getToRemove():Boolean {
            return toRemove;
        }
        public function setToRemove( f:Boolean ):void {
            toRemove = f;
        }
    }
}
