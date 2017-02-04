

var g_renderer;
var g_camera
var g_scene;





function createSpriteGeometry(width,height) {
    var geometry = new THREE.Geometry();
    var sizeHalfX = width / 2;
    var sizeHalfY = height / 2;
    geometry.vertices.push(new THREE.Vector3(-sizeHalfX, sizeHalfY, 0));
    geometry.vertices.push(new THREE.Vector3(sizeHalfX, sizeHalfY, 0));
    geometry.vertices.push(new THREE.Vector3(sizeHalfX, -sizeHalfY, 0));
    geometry.vertices.push(new THREE.Vector3(-sizeHalfX, -sizeHalfY, 0));
    geometry.faces.push(new THREE.Face3(0, 2, 1));
    geometry.faces.push(new THREE.Face3(0, 3, 2));
    return geometry;
}

function createSpriteUV(geometry, texture, cellWidth, cellHeight, cellNo) {
    var width = texture.image.width;
    var height = texture.image.height;
    
    var uCellCount = (width / cellWidth) | 0;
    var vCellCount = (height / cellHeight) | 0;
    var vPos = vCellCount - ((cellNo / uCellCount) | 0);
    var uPos = cellNo % uCellCount;
    var uUnit = cellWidth / width;
    var vUnit = cellHeight / height;
    
    geometry.faceVertexUvs[0].push([
        new THREE.Vector2((uPos) * cellWidth / width, (vPos) * cellHeight / height),
        new THREE.Vector2((uPos + 1) * cellWidth / width, (vPos - 1) * cellHeight / height),
        new THREE.Vector2((uPos + 1) * cellWidth / width, (vPos) * cellHeight / height)
    ]);
    geometry.faceVertexUvs[0].push([
        new THREE.Vector2((uPos) * cellWidth / width, (vPos) * cellHeight / height),
        new THREE.Vector2((uPos) * cellWidth / width, (vPos - 1) * cellHeight / height),
        new THREE.Vector2((uPos + 1) * cellWidth / width, (vPos - 1) * cellHeight / height)
    ]);
}

function updateSpriteUV(geometry, texture, cellWidth, cellHeight, cellNo) {
    var width = texture.image.width;
    var height = texture.image.height;
    
    var uCellCount = (width / cellWidth) | 0;
    var vCellCount = (height / cellHeight) | 0;
    var vPos = vCellCount - ((cellNo / uCellCount) | 0);
    var uPos = cellNo % uCellCount;
    var uUnit = cellWidth / width;
    var vUnit = cellHeight / height;
    var uvs = geometry.faceVertexUvs[0][0];
    
    uvs[0].x = (uPos) * uUnit;
    uvs[0].y = (vPos) * vUnit;
    uvs[1].x = (uPos + 1) * uUnit;
    uvs[1].y = (vPos - 1) * vUnit;
    uvs[2].x = (uPos + 1) * uUnit;
    uvs[2].y = (vPos) * vUnit;
    
    uvs = geometry.faceVertexUvs[0][1];
    
    uvs[0].x = (uPos) * uUnit;
    uvs[0].y = (vPos) * vUnit;
    uvs[1].x = (uPos) * uUnit;
    uvs[1].y = (vPos - 1) * vUnit;
    uvs[2].x = (uPos + 1) * uUnit;
    uvs[2].y = (vPos - 1) * vUnit;
    
    
    geometry.uvsNeedUpdate = true;

}

function createSpriteMaterial(texture) {
    var material = new THREE.MeshBasicMaterial({ map: texture /*,depthTest:true*/, transparent: true });
    material.shading = THREE.FlatShading;
    material.side = THREE.FrontSide;
    material.alphaTest = 0.5;
    material.needsUpdate = true;
    return material;
}
/*
function loadTexture(info) {
  var defer = Q.defer();
  THREE.ImageUtils.loadTexture(info.path, {},
  function (texture) {
    info.texture = texture;
    if (info.path.match(/\.png/i)) {
      texture.premultiplyAlpha = true;
    }
    texture.magFilter = THREE.NearestFilter;
    texture.minFilter = THREE.LinearMipMapLinearFilter;
    defer.resolve(info);
    },// Error
    function (e) {
    defer.reject(e);
  });
  return defer.promise;
}
*/

var g_base_map;

function init() {

	var width = window.innerWidth;
	var height = window.innerHeight;

	// renderer
	g_renderer = new THREE.WebGLRenderer( { antialias:false, sortObjects:true });
	g_renderer.setPixelRatio( window.devicePixelRatio );
    g_renderer.setClearColor(0x000000,1);
	g_renderer.setSize( window.innerWidth, window.innerHeight );
	g_renderer.autoClear = false; // To allow render overlay on top of sprited sphere
    document.body.appendChild( g_renderer.domElement );
    
	g_camera = new THREE.OrthographicCamera( - width / 2, width / 2, height / 2, - height / 2, -1, 10 );
	g_camera.position.z = 10;


	// create sprites
	var textureLoader = new THREE.TextureLoader();
	g_base_map = textureLoader.load( "base.png", createSprites );
    g_base_map.magFilter = THREE.NearestFilter;
    g_base_map.minFilter = THREE.LinearMipMapLinearFilter;
    g_renderer.clear();

    g_scene = new THREE.Scene();
}


function clearScene(scene) {
    scene.children.forEach(function(object){
        scene.remove(object);
    });
}


   
var g_sprites=[];

function createSprite() {
    var mat = createSpriteMaterial(g_base_map);
    var geom = createSpriteGeometry(10+50*Math.random(),10+50*Math.random());
    createSpriteUV(geom,g_base_map,8,8,0);
    var mesh = new THREE.Mesh(geom,mat);
    mesh.position.x=10;
    mesh.position.y=10;
    var vv=100;
    mesh.velocity=new THREE.Vector2( -vv+vv*2*Math.random(), -vv+vv*2*Math.random() );
    mesh.deck_index=0;
    mesh.poll_count=0
    return mesh;
}

function createSprites() {
    var spr = createSprite();
    g_sprites.push(spr);
}


function updateGame(dt) {
    for(var i in g_sprites) {
        var spr = g_sprites[i];
        spr.poll_count++;
        spr.position.x += spr.velocity.x*dt;
        spr.position.y += spr.velocity.y*dt;
        if( spr.position.x < -window.innerWidth/2 ) spr.velocity.x *= -1;
        if( spr.position.x > window.innerWidth/2 ) spr.velocity.x *= -1;
        if( spr.position.y < -window.innerHeight/2 ) spr.velocity.y *= -1;
        if( spr.position.y > window.innerHeight/2 ) spr.velocity.y *= -1;        
        if(spr.poll_count%10==0) {
            spr.deck_index++;
            if(spr.deck_index>3)spr.deck_index=0;
            if( g_base_map && g_base_map.image) {
                updateSpriteUV( spr.geometry, g_base_map, 8,8, spr.deck_index );
            }
        }
    }    
}


window.onmousedown = function(ev) {
    for(var i=0;i<50;i++) {
        var spr = createSprite();
        g_sprites.push(spr);
    }
    console.log("sprites:",g_sprites.length);
}

function animate() {
    var dt = 1.0/60.0;
    updateGame(dt);

    clearScene(g_scene);
    for(var i in g_sprites) {
        var spr = g_sprites[i];
        g_scene.add(spr);
    }
    
	requestAnimationFrame( animate );
	render();

}

function render() {


	g_renderer.clear();
	g_renderer.clearDepth();    
	g_renderer.render( g_scene, g_camera );

}


init();
animate();