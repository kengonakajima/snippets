import ImageContainer;
import flash.MovieClip;
import Sprite;


class Test {
	private var imglist : ImageContainer;
  	private var _container : MovieClip;
	private var sp : Sprite;

	private var tick : Int;
	
    static function main() {
		new Test();
	}
	function new(){
		
    	_container = flash.Lib._root.createEmptyMovieClip ("Container", 1);


        var mc : flash.MovieClip = flash.Lib.current;
        mc.beginFill(0xFF0000);
        mc.moveTo(50,50);
        mc.lineTo(100,50);
        mc.lineTo(100,100);
        mc.lineTo(50,100);
        mc.endFill();

		imglist = new ImageContainer(_container);
		imglist.LoadImage( "bullet");
		
		sp = new Sprite( imglist.GetImage(0), 100, 100 );
		sp.offX = 100;
		sp.offY = 100;
		sp.Active = true;
		
		tick = 0;
		
		_container.onEnterFrame = mainLoop;
		_container.focus();
        
        
    }

	
	
	function mainLoop()
	{
		tick += 1;
		trace( "t:" + tick );
		sp.x = tick;
		sp.y = tick;
	
	}
}

