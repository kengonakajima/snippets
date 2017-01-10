var scrw=600,scrh=300;
var score=0;
var id_gen=1;
function createSprite(inix,iniy) {
    var out={ x:inix, y:iniy }
    out.id = id_gen++;
    out.hit=function(){return false;}

    return out;    
}
function createBall() {
    var out=createSprite(Math.random() * scrw,  Math.random() * scrh );
    out.vx = -2+Math.random()*4;
    out.vy = -2+Math.random()*4;
    out.loop=0;
    out.update = function() {
        this.x += this.vx;
        this.y += this.vy;            
        this.loop++;
        if(this.loop<60*10) {
            if(this.x<0||this.x>scrw) this.vx*=-1;
            if(this.y<0||this.y>scrh) this.vy*=-1;
        }
    }
    out.draw = function(processing) {
        processing.fill(255,255,255);
        processing.ellipse(this.x,this.y,25,25);
    }
    out.hit = function(x,y) {
        return (this.x>=x-25&&this.x<=x+25&&this.y>=y-25&&this.y<=y+25);
    }
    out.name = "ball";
    return out;
}

function createPlayer() {
    var out=createSprite(scrw/2,scrh/2);    
    out.x = Math.random() * scrw;
    out.y = Math.random() * scrh;
    out.update = function() {
    }
    out.draw = function(processing) {
        processing.fill(255,0,0);
        processing.rect(this.x-10,this.y-10,20,20);        
    }
    out.name = "player";    
    return out;
}
function createLocalPlayer() {
    return createPlayer();
}
function createRemotePlayer(remote_id) {
    var pl = createPlayer();
    pl.remote_id = remote_id;
    return pl;
}

function sketchProc(processing) {
    function clear() {
        processing.size(scrw, scrh);
        processing.background(125);
        processing.fill(255);
    }

    var sprites=[];
    var local_player;
    processing.setup = function() {
        sprites.push( local_player = createLocalPlayer() );
    }
    processing.mouseMoved = function() {
        local_player.x = processing.mouseX;
        local_player.y = processing.mouseY;
    }
    processing.mouseClicked = function() {
        for(var i in sprites) {
            if( sprites[i].name=="ball" && sprites[i].hit(processing.mouseX,processing.mouseY) ) {
                score += 1;
                sprites[i].to_clean=true;
                break;
            }
        }
    }
    
    function updateGame() {
        clear();
        processing.text("SCORE:"+score, 20, 20);
        if(Math.random()<0.02) {
            sprites.push( createBall() )
        }
        var to_keep=[];        
        for(var i in sprites) {
            var spr = sprites[i];
            spr.update();
            spr.draw(processing);
            if(!spr.to_clean) to_keep.push(spr);
        }
        sprites=[];
        for(var i in to_keep) sprites[i]=to_keep[i];
    }
    processing.draw = function() {
        updateGame();
    }
}

var canvas = document.getElementById("canvas");
var p = new Processing(canvas, sketchProc);
