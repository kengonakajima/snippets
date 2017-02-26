var renderer;
var cameraOrtho, sceneOrtho;

init();
animate();


function init() {

	var width = window.innerWidth;
	var height = window.innerHeight;

	cameraOrtho = new THREE.OrthographicCamera( - width / 2, width / 2, height / 2, - height / 2, 1, 10 );
	cameraOrtho.position.z = 10;
	sceneOrtho = new THREE.Scene();

	var textureLoader = new THREE.TextureLoader();

	textureLoader.load( "textures/sprite0.png", createHUDSprites );

	// renderer
	renderer = new THREE.WebGLRenderer();
	renderer.setPixelRatio( window.devicePixelRatio );
	renderer.setSize( window.innerWidth, window.innerHeight );
	renderer.autoClear = false; // To allow render overlay on top of sprited sphere

	document.body.appendChild( renderer.domElement );
}
function createHUDSprites ( texture ) {
	var material = new THREE.SpriteMaterial( { map: texture } );
	var s = new THREE.Sprite( material );
	s.scale.set( 50, 50, 1 );
	sceneOrtho.add(s);
	s.position.set(0,0,1);
}

function animate() {
	requestAnimationFrame( animate );
	render();

}

function render() {
	renderer.clear();
	renderer.render( sceneOrtho, cameraOrtho );
}


