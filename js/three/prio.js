

var camera, scene, renderer;
var cameraOrtho, sceneOrtho;

var spriteTL, spriteTR, spriteBL, spriteBR, spriteC;

var mapC;



init();
animate();



function init() {

	var width = window.innerWidth;
	var height = window.innerHeight;

	camera = new THREE.PerspectiveCamera( 60, width / height, 1, 2100 );
	camera.position.z = 1500;

	cameraOrtho = new THREE.OrthographicCamera( - width / 2, width / 2, height / 2, - height / 2, 1, 10 );
	cameraOrtho.position.z = 10;

	scene = new THREE.Scene();
	scene.fog = new THREE.Fog( 0x000000, 1500, 2100 );

	sceneOrtho = new THREE.Scene();

	// create sprites

	var amount = 200;
	var radius = 500;

	var textureLoader = new THREE.TextureLoader();

	var mapA = textureLoader.load( "textures/sprite0.png", createHUDSprites );
	var mapB = textureLoader.load( "textures/sprite1.png" );
	mapC = textureLoader.load( "textures/sprite2.png" );



	// renderer

	renderer = new THREE.WebGLRenderer();
	renderer.setPixelRatio( window.devicePixelRatio );
	renderer.setSize( window.innerWidth, window.innerHeight );
	renderer.autoClear = false; // To allow render overlay on top of sprited sphere

	document.body.appendChild( renderer.domElement );

	//

	window.addEventListener( 'resize', onWindowResize, false );

}

var group_left, group_right;

function createHUDSprites ( texture ) {

    group_left = new THREE.Group();
    group_right = new THREE.Group();
    
	var material = new THREE.SpriteMaterial( { map: texture } );

	var width = material.map.image.width;
	var height = material.map.image.height;

	spriteTL = new THREE.Sprite( material );
	spriteTL.scale.set( width, height, 1 );

	spriteTR = new THREE.Sprite( material );
	spriteTR.scale.set( width, height, 1 );
    
	spriteBL = new THREE.Sprite( material );
	spriteBL.scale.set( width, height, 1 );

	spriteBR = new THREE.Sprite( material );
	spriteBR.scale.set( width, height, 1 );


    group_left.add(spriteTL);
    group_left.add(spriteBL);
    group_right.add(spriteTR);
    group_right.add(spriteBR);


    group_left.renderOrder = 1;
    group_right.renderOrder = 2;    
	sceneOrtho.add( group_left );
	sceneOrtho.add( group_right );
    
	updateHUDSprites();

}

function updateHUDSprites () {

	var width = window.innerWidth / 20;
	var height = window.innerHeight / 15;

	var material = spriteTL.material;

	var imageWidth = material.map.image.width / 4;
	var imageHeight = material.map.image.height / 4;

	spriteTL.position.set( - width + imageWidth,   height - imageHeight, 1 ); // top left
    spriteTL.renderOrder = 1;
	spriteBL.position.set( - width + imageWidth, - height + imageHeight, 1 ); // bottom left    
    spriteBL.renderOrder = 3;    
	spriteTR.position.set(   width - imageWidth,   height - imageHeight, 1 ); // top right
    spriteTR.renderOrder = 0;
	spriteBR.position.set(   width - imageWidth, - height + imageHeight, 1 ); // bottom right
    spriteBR.renderOrder = 2;    

}

function onWindowResize() {

	var width = window.innerWidth;
	var height = window.innerHeight;

	camera.aspect = width / height;
	camera.updateProjectionMatrix();

	cameraOrtho.left = - width / 2;
	cameraOrtho.right = width / 2;
	cameraOrtho.top = height / 2;
	cameraOrtho.bottom = - height / 2;
	cameraOrtho.updateProjectionMatrix();

	updateHUDSprites();

	renderer.setSize( window.innerWidth, window.innerHeight );

}

function animate() {

	requestAnimationFrame( animate );
	render();

}

function render() {


	renderer.clear();
	renderer.render( scene, camera );
	renderer.clearDepth();
	renderer.render( sceneOrtho, cameraOrtho );

}


