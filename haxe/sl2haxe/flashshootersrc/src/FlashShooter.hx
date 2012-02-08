import flash.MovieClip;
import flash.TextField;
import ImageContainer;
import Sprite;
import flash.Key;

class FlashShooter
{
  private var ImageList : ImageContainer;
  private var _container : MovieClip;
  private var mouseDown : Bool;
  private var Player : Sprite;
  private var Enemy : Array<Sprite>;
  private var Water : Array<Sprite>;
  private var EnemyID : Int;
  private var MoveDir : Int;
  private var Bullet : Array<Sprite>;
  private var BulletID : Int;
  private var Explosion : Array<Sprite>;
  private var ExplosionID : Int;
  private var ShootCount : Int;
  private var Islands : Array<Sprite>;
  private var Score : Int;
  private var Tick : Int;
  private var ScoreDisplay : TextField;
  private var GameOverDisplay : TextField;

      public function LoadImages ()
      {
          ImageList.LoadImage("xev_pyr");
          ImageList.LoadImage("Enemy1");
          ImageList.LoadImage("Water");
          ImageList.LoadImage("Bullet");
          ImageList.LoadImage("Island1");
          ImageList.LoadImage("Island2");
          ImageList.LoadImage("Island3");
          ImageList.LoadImage("Explosion");
      }

      public function new ()
      {
          _container = flash.Lib._root.createEmptyMovieClip ("Container", 1);
          Water = new Array();
          Bullet = new Array();
          Enemy = new Array();
          Islands = new Array();
          Explosion = new Array();
          ImageList = new ImageContainer(_container);
          LoadImages();
          EnemyID = 100;
          BulletID = 0;
          MoveDir = 0;
          ShootCount = 0;
          ExplosionID = 0;
          Score = 0;
          Tick = 0;

          // Water is the lowest level of items to be created
          Water[0] = new Sprite(ImageList.GetImage(2),0,-32);
          Water[1] = new Sprite(ImageList.GetImage(2),256,-32);
          Water[2] = new Sprite(ImageList.GetImage(2),512,-32);
          Water[3] = new Sprite(ImageList.GetImage(2),0,-32+256);
          Water[4] = new Sprite(ImageList.GetImage(2),256,-32+256);
          Water[5] = new Sprite(ImageList.GetImage(2),512,-32+256);

          // Islands must be on top of the water
          Islands[0] = new Sprite(ImageList.GetImage(4),Math.round(Math.random() * 160) +40,Math.round(Math.random() * 100) -40);
          Islands[1] = new Sprite(ImageList.GetImage(5),Math.round(Math.random() * 160) +240,Math.round(Math.random() * 100) +140 );
          Islands[2] = new Sprite(ImageList.GetImage(6),Math.round(Math.random() * 160) +140,Math.round(Math.random() * 100) +40);

          // Enemies must be on top of the Islands
          CreateEnemy();

          // Player must be on top of the islands
          Player = new Sprite(ImageList.GetImage(0),270,340);
          Player.offX = 32; Player.offY = 32;

          _container.onEnterFrame = mainLoop;
          _container.focus();

                  _container.createTextField("tf1",_container.getNextHighestDepth(),0,400,300,300);
                  ScoreDisplay = _container.tf1;
                  ScoreDisplay.textColor = 0xFFFFFF;
                  ScoreDisplay.text = "Score: "+Score;
                  _container.createTextField("tf2",_container.getNextHighestDepth(),200,200,300,300);
                  GameOverDisplay = _container.tf2;
                  GameOverDisplay.textColor = 0xFFFFFF;
                  GameOverDisplay._visible = false;
                  GameOverDisplay.text = "!! Game Over !!\n\nPress Enter to play again!";
      }

      private function CreateEnemy()
      {
          Enemy[EnemyID] = new Sprite(ImageList.GetImage(1),Math.round(Math.random() * 600) +20,0);
          Enemy[EnemyID].Active = true;
          EnemyID += 1;
      }

      private function Shoot() : Void
      {
        var B : Sprite;
        var i : Int;
        i  = 0;
        B = null;
        // Check if there is an inactive bullet.
        while (i < BulletID && B == null)
        {
          B = Bullet[i];
          if (B.Active == false)
          {
            B = Bullet[i];
          }
          else
          {
            B = null;
          }
          i += 1;
        }
        if (B == null)
        {
          // if no inactive bullet then create a new one
          B = new Sprite(ImageList.GetImage(3),50,50);
          Bullet[BulletID] = B;
          BulletID += 1;
        }
        B.x = Player.x;
        B.y = Player.y-16;
        B.offX = 16;
        B.offY = 16;
        B.Active = true;
        ShootCount = 4;
      }

      private function mainLoop () : Void
      {
        // Check if we need to add a new plane
        Tick += 1;
        if (Tick > 150 && Player.Active)
        {
          CreateEnemy();
          Tick = 0;
        }
        // Check Keys pressed and effects
         if (flash.Key.isDown(32) && ShootCount <= 0) { Shoot(); }
         if (flash.Key.isDown(37) && ShootCount <= 0) { MoveDir = -1; }
         if (flash.Key.isDown(39) && ShootCount <= 0) { MoveDir = 1; }
         if (flash.Key.isDown(13) && (Player.Active == false))
         {
           Player.Active = true;
           for (i in 0...Enemy.length)
           {
             Enemy[i].Active = false;
             Enemy[i].free;
             Enemy[i] = null;
           }
           EnemyID = 0;
           CreateEnemy();
           Score = 0;
         }
         // Do all movement
                  Player.x = Player.x + (7 * MoveDir);
                  if (Player.x < 32) { Player.x = 32; }
                  if (Player.x > 608) { Player.x = 608; }
                  for (i in 0...Water.length)
                  {
                    Water[i].y += 1;
                  }
                  for (i in 0...Islands.length)
                  {
                    Islands[i].y += 1;
                    if (Islands[i].y > 480) {
                      Islands[i].x = Math.round(Math.random() * 400) + 20;
                      Islands[i].y = Math.round(Math.random() * 100) - 200;
                    }
                  }
                  if (Water[0].y >= 0)
                  {
                    for (i in 0...Water.length)
                    {
                      Water[i].y -= 32;
                    }
                  }
                  for (i in 0...Bullet.length)
                  {
                    Bullet[i].y -= 7;
                    if (Bullet[i].y < 100)
                    {
                      Bullet[i].Active = false;
                      Bullet[i].y = -1000;
                    }
                    // Check if we hit an enemy plane
                    for (j in 0...Enemy.length)
                    {
                      if (Bullet[i].Image.hitTest(Enemy[j].Image))
                      {
                        MakeExplosion(Enemy[j].x, Enemy[j].y);
                        Bullet[i].y = -1000;
                        Enemy[j].y = 1000;
                        Score += 5;
                      }
                    }
                  }
                  // Move Enemies
                  for (i in 0...Enemy.length)
                  {
                    if (Enemy[i].Active)
                    {
                      Enemy[i].y += 2;
                      if (Enemy[i].y > 480)
                      {
                        Enemy[i].x = Math.round(Math.random() * 400) + 20;
                        Enemy[i].y = Math.round(Math.random() * 100) - 140;
                      }
                      // Check if we hit the player
                      if (Enemy[i].Image.hitTest(Player.Image) && Player.Active)
                      {
                        MakeExplosion(Player.x, Player.y);
                        Player.Active = false;
                        // Game Over Screen
                        GameOverDisplay._visible = true;
                        trace(GameOverDisplay._visible);
                      }
                    }
                  }
                  for (i in 0...Explosion.length)
                  {
                    Explosion[i].y += 2;
                    if (Explosion[i].Image._currentFrame >= 6) {
                      Explosion[i].Active = false;
                    }
                  }
                  ShootCount -= 1;
                  ScoreDisplay.text = "Score: "+Score;

      }

      public function MakeExplosion(sx : Int, sy : Int)
      {
        var E : Sprite;
        var i : Int;
        i  = 0;
        E = null;
        // Check if there is an inactive Explosion.
        while (i < ExplosionID && E == null)
        {
          E = Explosion[i];
          if (E.Active == false)
          {
            E = Explosion[i];
          }
          else
          {
            E = null;
          }
          i += 1;
        }
        if (E == null)
        {
          // if no inactive Explosion then create a new one
          E = new Sprite(ImageList.GetImage(7),sx,sy);
          Explosion[ExplosionID] = E;
          ExplosionID += 1;
        }
        E.x = sx;
        E.y = sy;
        E.offX = 8;
        E.offY = 8;
        Explosion[i].Image._currentFrame = 0;      
        E.Active = true;
      }

      public static function main ()
      {
                  new FlashShooter ();
      }

}