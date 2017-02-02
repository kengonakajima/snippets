

var renderer;
var camera, scene;





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

var base_map;

function init() {

	var width = window.innerWidth;
	var height = window.innerHeight;

	// renderer
	renderer = new THREE.WebGLRenderer( { antialias:false, sortObjects:true });
	renderer.setPixelRatio( window.devicePixelRatio );
    renderer.setClearColor(0x000000,1);
	renderer.setSize( window.innerWidth, window.innerHeight );
	renderer.autoClear = false; // To allow render overlay on top of sprited sphere
    document.body.appendChild( renderer.domElement );
    
	camera = new THREE.OrthographicCamera( - width / 2, width / 2, height / 2, - height / 2, -1, 10 );
	camera.position.z = 10;


	// create sprites
	var textureLoader = new THREE.TextureLoader();
	base_map = textureLoader.load( "base.png", createSprites );
    base_map.magFilter = THREE.NearestFilter;
    base_map.minFilter = THREE.LinearMipMapLinearFilter;
    renderer.clear();

    scene = new THREE.Scene();
}

var g_sprmesh;
var g_geom;

function createSprites() {

    var mat = createSpriteMaterial(base_map);
    console.log("sprite mat:",mat);
    g_geom = createSpriteGeometry(100,100);
    createSpriteUV(g_geom,base_map,8,8,0);
    g_sprmesh = new THREE.Mesh(g_geom,mat);
    scene.add(g_sprmesh);
}


var index=0;
var loopcnt=0;
function animate() {
    loopcnt++;
    if(loopcnt%10==0) {
        if( base_map && base_map.image) {
            index++;
            if(index>3)index=0;
            updateSpriteUV( g_geom, base_map, 8,8, index );
        }
    }
    
	requestAnimationFrame( animate );
	render();

}

function render() {


	renderer.clear();
	renderer.clearDepth();    
	renderer.render( scene, camera );
    console.log("ren");

}


init();
animate();