

var camera, scene, renderer;
var cameraOrtho, sceneOrtho;

var spriteTL, spriteTR, spriteBL, spriteBR, spriteC;

var mapC;

var group;

init();
animate();


function gentex() {
    var sz=50;
    var dummyRGBA = new Uint8Array(sz * sz * 4);
    for(var i=0; i< sz * sz; i++){
        // RGB from 0 to 255
        dummyRGBA[4*i] = Math.random()*255;
        dummyRGBA[4*i + 1] = Math.random()*255;
        dummyRGBA[4*i + 2] = Math.random()*255;
        // OPACITY
        dummyRGBA[4*i + 3] = 255;
    }

    dummyDataTex = new THREE.DataTexture( dummyRGBA, sz, sz, THREE.RGBAFormat );
    dummyDataTex.needsUpdate = true;
    return dummyDataTex;
}

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

    var tttt = gentex();
    
	group = new THREE.Group();

//    mapB.offset = new THREE.Vector2( 0.25, 0); これでずらせるが全部ずれる
//    mapB.repeat = new THREE.Vector2( 0.25, 1);
	var materialC = new THREE.SpriteMaterial( { map: mapC, color: 0xffffff, fog: true } );
	var materialB = new THREE.SpriteMaterial( { map: mapB, color: 0xffffff, fog: true } );
    var materialT = new THREE.SpriteMaterial( { map: tttt, color: 0xffffff, fog: false } );
    
	for ( var a = 0; a < amount; a ++ ) {

		var x = Math.random() - 0.5;
		var y = Math.random() - 0.5;
		var z = Math.random() - 0.5;

		if ( z < 0 ) {
            if(Math.random() < 0.5 ) {
                material = materialT;
            } else {
			    material = materialB.clone();
            }
		} else {

			material = materialC.clone();
			material.color.setHSL( 0.5 * Math.random(), 0.75, 0.5 );
			material.map.offset.set( -0.5, -0.5 );
			material.map.repeat.set( 2, 2 );

		}

		var sprite = new THREE.Sprite( material );

		sprite.position.set( x, y, z );
		sprite.position.normalize();
		sprite.position.multiplyScalar( radius );

		group.add( sprite );

	}

	scene.add( group );

	// renderer

	renderer = new THREE.WebGLRenderer();
	renderer.setPixelRatio( window.devicePixelRatio );
	renderer.setSize( window.innerWidth, window.innerHeight );
	renderer.autoClear = false; // To allow render overlay on top of sprited sphere

	document.body.appendChild( renderer.domElement );

	//

	window.addEventListener( 'resize', onWindowResize, false );

}

function createHUDSprites ( texture ) {

	var material = new THREE.SpriteMaterial( { map: texture } );

	var width = material.map.image.width;
	var height = material.map.image.height;

	spriteTL = new THREE.Sprite( material );
	spriteTL.scale.set( width, height, 1 );
	sceneOrtho.add( spriteTL );

	spriteTR = new THREE.Sprite( material );
	spriteTR.scale.set( width, height, 1 );
	sceneOrtho.add( spriteTR );

	spriteBL = new THREE.Sprite( material );
	spriteBL.scale.set( width, height, 1 );
	sceneOrtho.add( spriteBL );

	spriteBR = new THREE.Sprite( material );
	spriteBR.scale.set( width, height, 1 );
	sceneOrtho.add( spriteBR );

	spriteC = new THREE.Sprite( material );
	spriteC.scale.set( width, height, 1 );
	sceneOrtho.add( spriteC );

	updateHUDSprites();

}

function updateHUDSprites () {

	var width = window.innerWidth / 2;
	var height = window.innerHeight / 2;

	var material = spriteTL.material;

	var imageWidth = material.map.image.width / 2;
	var imageHeight = material.map.image.height / 2;

	spriteTL.position.set( - width + imageWidth,   height - imageHeight, 1 ); // top left
	spriteTR.position.set(   width - imageWidth,   height - imageHeight, 1 ); // top right
	spriteBL.position.set( - width + imageWidth, - height + imageHeight, 1 ); // bottom left
	spriteBR.position.set(   width - imageWidth, - height + imageHeight, 1 ); // bottom right
	spriteC.position.set( 0, 0, 1 ); // center

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

	var time = Date.now() / 1000;

	for ( var i = 0, l = group.children.length; i < l; i ++ ) {

		var sprite = group.children[ i ];
		var material = sprite.material;
		var scale = Math.sin( time + sprite.position.x * 0.01 ) * 0.3 + 1.0;

		var imageWidth = 1;
		var imageHeight = 1;

		if ( material.map && material.map.image && material.map.image.width ) {

			imageWidth = material.map.image.width;
			imageHeight = material.map.image.height;

		}

		sprite.material.rotation += 0.1 * ( i / l );
		sprite.scale.set( scale * imageWidth, scale * imageHeight, 1.0 );

		if ( material.map !== mapC ) {

			material.opacity = Math.sin( time + sprite.position.x * 0.01 ) * 0.4 + 0.6;

		}

	}

	group.rotation.x = time * 0.5;
	group.rotation.y = time * 0.75;
	group.rotation.z = time * 1.0;

	renderer.clear();
	renderer.render( scene, camera );
	renderer.clearDepth();
	renderer.render( sceneOrtho, cameraOrtho );

}


