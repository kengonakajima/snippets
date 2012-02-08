import flash.MovieClip;

class ImageContainer
{
  private var FImages : Array<String>;
  private var ImgCount : Int;
  public var container : MovieClip;
  private var MovieId : Int;

  public function new(Cnt : MovieClip) {
    ImgCount = 0;
    container = Cnt;
    MovieId = 1000000;
    FImages = new Array();
  }
  
  public function LoadImage(ImgName : String)
  {
    FImages[ImgCount] = ImgName;
    ImgCount += 1;
  }

  public function GetImage(ImgID : Int) : MovieClip
  {
	trace(FImages[ImgID]);
    var m : Int = MovieId;
    MovieId += 1;
    return container.attachMovie(FImages[ImgID],""+m,container.getNextHighestDepth());
  }

}
