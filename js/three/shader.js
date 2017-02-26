var renderer;
var cameraOrtho, sceneOrtho;

var vertex_glsl =
    "void main(void){\n" +
    "gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);\n" +
    "}";

var fragment_glsl =
    "void main(void) {\n" +
    "gl_FragColor = vec4(1.0, .0, .0, 1.);\n"+
    "}";

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
function createRectGeometry(width,height) {
    var geometry = new THREE.Geometry();
    var sizeHalfX = width / 2;
    var sizeHalfY = height / 2;
    /*
      0--1
      |\ |
      | \|
      3--2
     */
    geometry.vertices.push(new THREE.Vector3(-sizeHalfX, sizeHalfY, 0)); //0
    geometry.vertices.push(new THREE.Vector3(sizeHalfX, sizeHalfY, 0)); //1
    geometry.vertices.push(new THREE.Vector3(sizeHalfX, -sizeHalfY, 0)); //2
    geometry.vertices.push(new THREE.Vector3(-sizeHalfX, -sizeHalfY, 0)); //3
    geometry.faces.push(new THREE.Face3(0, 2, 1));
    geometry.faces.push(new THREE.Face3(0, 3, 2));
    return geometry;
}

function createHUDSprites ( texture ) {
    uniforms={}
    var material = new THREE.ShaderMaterial( {
        uniforms : uniforms,
        vertexShader : vertex_glsl,
        fragmentShader : fragment_glsl
    });
//    var material = new THREE.SpriteMaterial( { map: texture } );
	var geom = createRectGeometry(1,1);
    var mesh = new THREE.Mesh(geom,material);
	mesh.scale.set( 50, 50, 1 );
	mesh.position.set(0,0,1);
	sceneOrtho.add(mesh);
}

function animate() {
	requestAnimationFrame( animate );
	render();

}

function render() {
	renderer.clear();
	renderer.render( sceneOrtho, cameraOrtho );
}


