var container;

var camera,  scene, renderer;

var sky, sunSphere;

var controls;

function initSky() {

	// Add Sky
	sky = new THREE.Sky();
	sky.scale.setScalar( 450000 );
	scene.add( sky );

	// Add Sun Helper
	sunSphere = new THREE.Mesh(
		new THREE.SphereBufferGeometry( 20000, 16, 8 ),
		new THREE.MeshBasicMaterial( { color: 0xffffff } )
	);
	sunSphere.position.y = - 700000;
	sunSphere.visible = false;
	scene.add( sunSphere );

	/// GUI

	var effectController  = {
		turbidity: 10,
		rayleigh: 2,
		mieCoefficient: 0.005,
		mieDirectionalG: 0.8,
		luminance: 1,
		inclination: 0.49, // elevation / inclination
		azimuth: 0.25, // Facing front,
		sun: ! true
	};



}

function confChanged() {
    var nt=new Date().getTime()*2;
    var bsec=new Date().getTime() % (24*1000);
    var dayrate=bsec/1000/24; // 0~1
    
    
	var uniforms = sky.material.uniforms;
	uniforms.turbidity.value = 10;
	uniforms.rayleigh.value = 2;
	uniforms.luminance.value = 1;
	uniforms.mieCoefficient.value = 0.005;
	uniforms.mieDirectionalG.value = 0.8;

	var distance = 400000;
    var inclination = 0.49; // 傾斜 0.49にすると白夜っぽく平 0.3にするとふつうな感じ 0にすると垂直。これと方位で位置が決まる
    inclination = 0;
    var azimth = dayrate; // 方位 0:東 1:西
	var theta = Math.PI * ( inclination - 0.5 ); // inclination
	var phi = 2 * Math.PI * ( azimth - 0.5 ); // azimth
    console.log("incl:",inclination, "azi:",azimth);
	sunSphere.position.x = distance * Math.cos( phi );
	sunSphere.position.y = distance * Math.sin( phi ) * Math.sin( theta );
	sunSphere.position.z = distance * Math.sin( phi ) * Math.cos( theta );

	sunSphere.visible = !true;

	uniforms.sunPosition.value.copy( sunSphere.position );



}

/*
  gui.add( effectController, "turbidity", 1.0, 20.0, 0.1 ).onChange( guiChanged );
  gui.add( effectController, "rayleigh", 0.0, 4, 0.001 ).onChange( guiChanged );
  gui.add( effectController, "mieCoefficient", 0.0, 0.1, 0.001 ).onChange( guiChanged );
  gui.add( effectController, "mieDirectionalG", 0.0, 1, 0.001 ).onChange( guiChanged );
  gui.add( effectController, "luminance", 0.0, 2 ).onChange( guiChanged );
  gui.add( effectController, "inclination", 0, 1, 0.0001 ).onChange( guiChanged );
  gui.add( effectController, "azimuth", 0, 1, 0.0001 ).onChange( guiChanged );
  gui.add( effectController, "sun" ).onChange( guiChanged );
*/



function init() {

	camera = new THREE.PerspectiveCamera( 60, window.innerWidth / window.innerHeight, 100, 2000000 );
	camera.position.set( 0, 100, 2000 );

	//camera.setLens(20);

	scene = new THREE.Scene();

	var helper = new THREE.GridHelper( 10000, 2, 0xffffff, 0xffffff );
	scene.add( helper );

	renderer = new THREE.WebGLRenderer();
	renderer.setPixelRatio( window.devicePixelRatio );
	renderer.setSize( window.innerWidth, window.innerHeight );
	document.body.appendChild( renderer.domElement );


	initSky();

	window.addEventListener( 'resize', onWindowResize, false );

	controls = new THREE.OrbitControls( camera, renderer.domElement );
	controls.addEventListener( 'change', render );
	//controls.maxPolarAngle = Math.PI / 2;
	controls.enableZoom = false;
	controls.enablePan = false;
    
}

function onWindowResize() {

	camera.aspect = window.innerWidth / window.innerHeight;
	camera.updateProjectionMatrix();

	renderer.setSize( window.innerWidth, window.innerHeight );

	render();

}

function render() {
    
	renderer.render( scene, camera );

}

function animate() {
    confChanged();
    render();
    requestAnimationFrame(animate);    
}


init();
animate();

