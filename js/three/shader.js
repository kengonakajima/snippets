// http://qiita.com/_nabe/items/1657d9556591284a43c8

var renderer;
var cameraOrtho, sceneOrtho;

var vertex_v_glsl =
    "void main(void){\n" +
    "gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);\n" +
    "}";

var vertex_col_v_glsl =
    "attribute vec3 color;\n"+
    "varying vec4 vColor;\n"+
    "void main(){\n"+
    "  vColor = vec4(color.r,color.g,color.b,1);\n"+
    "  gl_Position = vec4(position, 1.0);\n"+
    "}\n";

var vertex_uv_glsl =
    "varying vec2 vUv;// fragmentShaderに渡すためのvarying変数\n"+  
    "void main()\n"+
    "{\n"+
    "  // 処理する頂点ごとのuv(テクスチャ)座標をそのままfragmentShaderに横流しする\n"+
    "  vUv = uv;\n"+
    "  // 変換：ローカル座標 → 配置 → カメラ座標\n"+
    "  vec4 mvPosition = modelViewMatrix * vec4(position, 1.0);\n"+
    "  // 変換：カメラ座標 → 画面座標\n"+
    "  gl_Position = projectionMatrix * mvPosition;\n"+
    "}\n";

var vertex_uv_color_glsl =
    "varying vec2 vUv;\n"+
    "varying vec4 vColor;\n"+
    "attribute vec3 color;\n"+
    "void main()\n"+
    "{\n"+
    "  vUv = uv;\n"+
    "  vColor = vec4(color,1);\n"+
    "  vec4 mvPosition = modelViewMatrix * vec4(position, 1.0);\n"+
    "  gl_Position = projectionMatrix * mvPosition;\n"+
    "}\n";

var vertex_fixed_glsl = 
	"uniform float time;\n"+
	"uniform vec2 resolution;\n"+
	"void main()	{\n"+
	"	gl_Position = vec4( position, 1.0 );\n"+
	"}\n";


var fragment_fixed_glsl =
    "void main(void) {\n" +
    "gl_FragColor = vec4(1.0, .0, .0, 1.);\n"+
    "}";

var fragment_uv_glsl =
    "//uniform 変数としてテクスチャのデータを受け取る\n"+
    "uniform sampler2D texture;\n"+
    "// vertexShaderで処理されて渡されるテクスチャ座標\n"+
    "varying vec2 vUv;\n"+
    "void main()\n"+
    "{\n"+
    "  // テクスチャの色情報をそのままピクセルに塗る\n"+
    "  vec4 ccc = texture2D(texture,vUv);\n"+
    "  gl_FragColor = texture2D(texture, vUv);\n"+
    "}\n";

var fragment_uv_color_glsl =
    "uniform sampler2D texture;\n"+
    "varying vec2 vUv;\n"+
    "varying vec4 vColor;\n"+    
    "void main()\n"+
    "{\n"+
    "  vec4 tc = texture2D(texture,vUv);\n"+
    "  gl_FragColor = vec4( tc.r * vColor.r, tc.g * vColor.g, tc.b * vColor.b, vColor.a);\n"+
    "}\n";


var fragment_mod_glsl = 
	"uniform float time;\n"+
	"uniform vec2 resolution;\n"+
	"void main()	{\n"+
	"	float x = mod(time + gl_FragCoord.x, 20.) < 10. ? 1. : 0.;\n"+
	"	float y = mod(time + gl_FragCoord.y, 20.) < 10. ? 1. : 0.;\n"+
	"	gl_FragColor = vec4(vec3(min(x, y)), 1.);\n"+
	"}\n";

var fragment_fixed_glsl = 
	"void main()	{\n"+
	"	gl_FragColor = vec4(1,0,1,1);\n"+
	"}\n";

var fragment_color_glsl =
    "varying vec4 vColor;\n"+
	"void main()	{\n"+
	"	gl_FragColor = vColor;\n"+
	"}\n";

var fragment_replacer_glsl = 
	"uniform sampler2D texture;\n"+
    "varying vec2 vUv;\n"+
	"varying vec4 vColor;\n"+
	"uniform vec3 color1;\n"+    
	"uniform vec3 replace1;\n"+
	"uniform float eps;\n"+
	"void main() {\n"+
	"	vec4 pixel = texture2D(texture, vUv); \n"+
	"	if( pixel.r > color1.r - eps && pixel.r < color1.r + eps && pixel.g > color1.g - eps && pixel.g < color1.g + eps && pixel.b > color1.b - eps && pixel.b < color1.b + eps ){\n"+
//    "   if(pixel.g>color1.g-eps && pixel.g < color1.g+eps && pixel.r>-0.01 && pixel.r<0.1) {\n"+
	"		pixel = vec4(replace1, pixel.a );\n"+
	"    }\n"+
	"   pixel.r = vColor.r * pixel.r;\n"+
	"   pixel.g = vColor.g * pixel.g;\n"+
	"   pixel.b = vColor.b * pixel.b;\n"+
	"   pixel.a = vColor.a * pixel.a;\n" +   
	"	gl_FragColor = pixel;\n"+
	"}\n";




init();
animate();



function init() {

	var width = window.innerWidth;
	var height = window.innerHeight;

	cameraOrtho = new THREE.OrthographicCamera( - width / 2, width / 2, height / 2, - height / 2, 1, 10 );
	cameraOrtho.position.z = 10;
	sceneOrtho = new THREE.Scene();

	var textureLoader = new THREE.TextureLoader();

	textureLoader.load( "textures/test32.png", createHUDSprites );

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
    geometry.faces[0].vertexColors[0] = new THREE.Color(1,1,1);
    geometry.faces[0].vertexColors[1] = new THREE.Color(1,1,1);
    geometry.faces[0].vertexColors[2] = new THREE.Color(1,1,1);
    geometry.faces[1].vertexColors[0] = new THREE.Color(1,1,1);
    geometry.faces[1].vertexColors[1] = new THREE.Color(1,1,1);
    geometry.faces[1].vertexColors[2] = new THREE.Color(1,1,1);
    var u0=0, v0=0, u1=1,v1=1;
    geometry.faceVertexUvs[0].push([ new THREE.Vector2(u0,v0), new THREE.Vector2(u1,v1), new THREE.Vector2(u1,v0) ]);
    geometry.faceVertexUvs[0].push([ new THREE.Vector2(u0,v0), new THREE.Vector2(u0,v1), new THREE.Vector2(u1,v1) ]);
    
    return geometry;
}

function createHUDSprites ( texture ) {
    console.log(texture);
    texture.magFilter = THREE.NearestFilter;    
    console.log("texture:",texture);
    uniforms_0={}
    uniforms_tex={
        "texture" : { type: "t", value: texture }
    };
    uniforms_replacer={
        "texture" : { type: "t", value: texture },        
        "color1" : { type: "v3", value: new THREE.Vector3(0,1,0) },
        "replace1" : { type: "v3", value: new THREE.Vector3(0,0,1) },
        "eps" : { type: "f", value: 0.2 }
    };
    uniforms_time_reso={
		time: { type: "f", value: 1.0 },
		resolution: { type: "v2", value: new THREE.Vector2() }        
    };
	uniforms_time_reso.resolution.value.x = window.innerWidth;
	uniforms_time_reso.resolution.value.y = window.innerHeight;
    
    var material = new THREE.ShaderMaterial( {
        uniforms : uniforms_replacer,
        vertexShader : vertex_uv_color_glsl,
        fragmentShader : fragment_replacer_glsl
    });
//    var material = new THREE.SpriteMaterial( { map: texture } );
	var geom = createRectGeometry(1,1);
    var mesh = new THREE.Mesh(geom,material);
	mesh.scale.set( 150, 150, 1 );
	mesh.position.set(0,0,1);
	sceneOrtho.add(mesh);
}
var startTime = Date.now();
function animate() {
//	var elapsedMilliseconds = Date.now() - startTime;
//	var elapsedSeconds = elapsedMilliseconds / 1000.;
//	if(uniforms_3) uniforms_3.time.value = 60. * elapsedSeconds;
    
	requestAnimationFrame( animate );
	render();

}

function render() {
	renderer.clear();
	renderer.render( sceneOrtho, cameraOrtho );
}


