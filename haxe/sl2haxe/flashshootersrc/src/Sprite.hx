import flash.MovieClip;

class Sprite
{
   public var Image : MovieClip;
   public var Active(getActive,setActive) : Bool;
   public var x(getX,setX) : Int;
   public var y(getY,setY) : Int;
   public var offX(getoffX,setoffX) : Int;
   public var offY(getoffY,setoffY) : Int;

	/*
		Constructor
	*/
	public function new(mc : MovieClip, sx : Int, sy : Int)
        {
          trace("Sprite:"+mc);
          Active = true;
          Image = mc;
          Image._x = sx;
          Image._y = sy;
          fy = sy;
          fx = sx;
          foffx = 0;
          foffy = 0;
        }
        
  public function free()
  {
    Active = false;
    Image._visible = false;
    Image.removeMovieClip();
  }

   private var fx : Int;
   private var fy : Int;
   private var foffx : Int;
   private var foffy : Int;
   private var fActive : Bool;

   private function getX() {
       return fx;
   }

   private function setX( value : Int ) {
      fx = value;
      Image._x = fx - foffx;
      return fx;
   }

   private function getY() {
       return fy;
   }

   private function setY( value : Int ) {
      fy = value;
      Image._y = fy-foffy;
      return fy;
   }

   private function getoffX() {
       return foffx;
   }

   private function setoffX( value : Int ) {
      foffx = value;
      Image._x = fx-foffx;
      return foffx;
   }

   private function getoffY() {
       return foffy;
   }

   private function setoffY( value : Int ) {
      foffy = value;
      Image._y = fy-foffy;
      return foffy;
   }

   private function getActive() {
       return fActive;
   }

   private function setActive( value : Bool ) {
      fActive = value;
      Image._visible = fActive;
      return fActive;
   }
}