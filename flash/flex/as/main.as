package
{

	import flash.display.*;
    import flash.events.*;
    import flash.net.*;
    import flash.text.*;
    import flash.system.*;
    import flash.utils.ByteArray;
    import caurina.transitions.Tweener;
    import flash.text.Font;
    
	[SWF(width="952", height="600", backgroundColor="#ffffff", frameRate="30")]

   	class Node {
    	private var absX:int;
    };

    public class main extends Sprite{

	    private var socket:Socket;
    
    	private function closeHandler(event:Event):void {
		    trace("closeHandler: " + event);
	    }
    
		private function connectHandler(event:Event):void {
        	trace("connectHandler: " + event);
            var out:ByteArray = new ByteArray();
            out.writeByte(110);
            out.writeByte(10);
            socket.writeBytes(out);
            socket.flush();
	    }
        private function ioErrorHandler(event:IOErrorEvent):void {
	        trace("ioErrorHandler: " + event);
        }
        private function securityErrorHandler(event:SecurityErrorEvent):void {
        	trace("securityErrorHandler: " + event);
        }
        private function socketDataHandler(event:ProgressEvent):void {
	        trace("socketDataHandler: " + event);
        }

		private function drawArrow( sx:Number, sy:Number, ex:Number, ey:Number ):void {			
           	var rad:Number = Math.atan2( ex - sx, ey-sy ) * -1.0;
			trace(rad);
			
			this.graphics.lineStyle(1, 0x999999 );
			this.graphics.moveTo(sx,sy);
			this.graphics.lineTo(ex,ey);

			var len:Number = 7;			
			this.graphics.moveTo(ex,ey);
			var angle1:Number = rad + Math.PI/6 + Math.PI/2*3;
			var angle2:Number = rad - Math.PI/6 + Math.PI/2*3;
			var p1x:Number = ex + Math.cos( angle1 ) * len;
			var p1y:Number = ey + Math.sin( angle1 ) * len;
			this.graphics.lineTo( p1x, p1y );
			this.graphics.moveTo( ex, ey );
			var p2x:Number = ex + Math.cos( angle2 ) * len;
			var p2y:Number = ey + Math.sin( angle2 ) * len;
			this.graphics.lineTo( p2x, p2y );
			this.graphics.lineStyle();
		}

		public function main()
        {
	        // from bankoma
            stage.scaleMode = "noScale";

            // switch host
            var url:String = LoaderInfo(this.root.loaderInfo).url;
            var host:String;
            if( url.match("file://")){
            	host = "localhost";
            } else {
            	host = url.split("//")[1].split(":")[0];
            }
            trace(host);

            Security.loadPolicyFile( "file://./crossdomain.xml");

            

            socket = new Socket( host, 9999 );
            socket.addEventListener(Event.CLOSE, closeHandler);
            socket.addEventListener(Event.CONNECT, connectHandler);
            socket.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
            socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
            socket.addEventListener(ProgressEvent.SOCKET_DATA, socketDataHandler);

            // put text in window
            var tf:TextField = new TextField();
            tf.text = "Hello World!　ほげ";
            addChild(tf);

			var format:TextFormat = new TextFormat();
            format.font ="ヒラギノ丸ゴ Pro W4";
            format.size = 10;
            format.color=0x000000;
            format.bold =true;
            tf.setTextFormat(format);
            this.graphics.beginFill(0x0000FF);
            this.graphics.drawRect(10,10,20,20);
            this.graphics.beginFill(0x00ff00);
            this.graphics.drawRoundRect(30,30,50,50,8);
            
                
            // text input field
            var itf:TextField = new TextField();
            itf.type = "input";
            itf.border = true;
            itf.height=20;
            itf.width=110;
            itf.maxChars=10;
            var get_str:String;
            
            addChild( itf );
            
            
            stage.addEventListener( MouseEvent.CLICK, function(e:MouseEvent):void {
            	Tweener.addTween(tf, {x:mouseX, y:mouseY, time:1, transition:"linear"});
            	trace(itf.text);
            	
            	drawArrow( 100,100,mouseX,mouseY);
                } );

			var icon:Loader = new Loader();
			icon.load(new URLRequest( "./f.png"));
			addChild(icon);
			
			var icon2:Loader = new Loader();
			icon2.load(new URLRequest( "http://s3.amazonaws.com/twitter_production/profile_images/29315832/tokuriki2_mini.jpg"));
			addChild(icon2);			
			icon2.x = 100;
			
			var icon3:Loader = new Loader();
			icon3.load(new URLRequest( "./othello_black.png"));
			addChild(icon3);
			icon3.x = 200;
			icon3.scaleX = 0.7;
			icon3.scaleY = 0.5;
			Tweener.addTween( icon3, { x:300, y:300, time:1, transition:"linear"} );
			
			// 線をひく
			this.graphics.lineStyle(1, 0x999999);
			this.graphics.moveTo(100,100);
			this.graphics.lineTo(200,250);
			
			drawArrow( 100,100,300,400);

			// font test
			trace("デバイスフォントと埋め込みフォントの両方を含む、すべてのフォントのリスト------------------------------");
			var allFonts:Array = Font.enumerateFonts(true);
			allFonts.sortOn("fontName", Array.CASEINSENSITIVE);
			for(var i:int=0;i<allFonts.length;i++){
				trace(allFonts[i].fontName);
			}

            // App code test
        }
    }
}
