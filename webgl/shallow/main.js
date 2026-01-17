// Shallow Water Simulation - WebGL2 Implementation
// Based on Kurganov-Petrova scheme (2007)

const canvas = document.getElementById('canvas');
const gl = canvas.getContext('webgl2');

if (!gl) {
    alert('WebGL2 not supported');
    throw new Error('WebGL2 not supported');
}

const extColorBufferFloat = gl.getExtension('EXT_color_buffer_float');
if (!extColorBufferFloat) {
    alert('EXT_color_buffer_float not supported');
    throw new Error('EXT_color_buffer_float not supported');
}

// Simulation parameters
const NX = 128;
const NY = 128;
const DOMAIN_WIDTH = 100.0;  // meters
const DOMAIN_LENGTH = 200.0; // meters
const DX = DOMAIN_WIDTH / NX;
const DY = DOMAIN_LENGTH / NY;
const G = 9.81;
const THETA = 1.3;
const FRICTION = 0.05;
const EPSILON = 1e-6;

// State
let simTextures = {};
let currentStateIdx = 0;
let dt = 0.001;  // Smaller timestep for stability
let time = 0;
let mouseDown = false;
let mouseX = 0, mouseY = 0;

// Camera
let cameraAngleX = 0.5;
let cameraAngleY = 0.5;
let cameraDistance = 150;
let cameraPanX = 0;
let cameraPanY = 50;
let lastMouseX = 0, lastMouseY = 0;
let rightMouseDown = false;

// Programs
let pass1Program, pass2Program, pass3Program, mouseProgram;
let terrainProgram, waterProgram;
let quadVAO, meshVAO;
let framebuffers = {};

// Vertex shader for simulation (full-screen quad)
const quadVS = `#version 300 es
layout(location = 0) in vec2 aPosition;
out vec2 vTexCoord;
void main() {
    vTexCoord = aPosition * 0.5 + 0.5;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}`;

// Pass 1: Reconstruct h, u, v at cell edges
const pass1FS = `#version 300 es
precision highp float;

uniform sampler2D uState;
uniform sampler2D uBottom;
uniform float uTwoTheta;
uniform float uEpsilon;
uniform float uOneOverDx;
uniform float uOneOverDy;

in vec2 vTexCoord;
layout(location = 0) out vec4 outH;
layout(location = 1) out vec4 outU;
layout(location = 2) out vec4 outV;
layout(location = 3) out vec4 outNormal;

float minmod(float a, float b, float c) {
    if (a > 0.0 && b > 0.0 && c > 0.0) return min(min(a, b), c);
    if (a < 0.0 && b < 0.0 && c < 0.0) return max(max(a, b), c);
    return 0.0;
}

void reconstruct(float west, float here, float east, out float outWest, out float outEast) {
    float dxGradOverTwo = 0.25 * minmod(
        uTwoTheta * (here - west),
        east - west,
        uTwoTheta * (east - here)
    );
    outEast = here + dxGradOverTwo;
    outWest = here - dxGradOverTwo;
}

void correctW(float bWest, float bEast, float wBar, inout float wWest, inout float wEast) {
    if (wEast < bEast) {
        wEast = bEast;
        wWest = max(bWest, 2.0 * wBar - bEast);
    } else if (wWest < bWest) {
        wEast = max(bEast, 2.0 * wBar - bWest);
        wWest = bWest;
    }
}

void calcUV(vec4 h, vec4 hu, vec4 hv, out vec4 u, out vec4 v) {
    vec4 h2 = h * h;
    vec4 h4 = h2 * h2;
    vec4 divideByH = sqrt(2.0) * h / sqrt(h4 + max(h4, vec4(uEpsilon)));
    u = divideByH * hu;
    v = divideByH * hv;
}

void main() {
    ivec2 idx = ivec2(gl_FragCoord.xy);

    vec4 inHere = texelFetch(uState, idx, 0);
    vec4 inSouth = texelFetch(uState, idx + ivec2(0, -1), 0);
    vec4 inNorth = texelFetch(uState, idx + ivec2(0, 1), 0);
    vec4 inWest = texelFetch(uState, idx + ivec2(-1, 0), 0);
    vec4 inEast = texelFetch(uState, idx + ivec2(1, 0), 0);

    vec4 B;
    vec3 bHere = texelFetch(uBottom, idx, 0).rgb;
    B.r = bHere.r;
    B.g = bHere.g;
    B.b = texelFetch(uBottom, idx + ivec2(0, -1), 0).r;
    B.a = texelFetch(uBottom, idx + ivec2(-1, 0), 0).g;

    vec4 w, hu, hv;
    reconstruct(inWest.r, inHere.r, inEast.r, w.a, w.g);
    reconstruct(inSouth.r, inHere.r, inNorth.r, w.b, w.r);
    reconstruct(inWest.g, inHere.g, inEast.g, hu.a, hu.g);
    reconstruct(inSouth.g, inHere.g, inNorth.g, hu.b, hu.r);
    reconstruct(inWest.b, inHere.b, inEast.b, hv.a, hv.g);
    reconstruct(inSouth.b, inHere.b, inNorth.b, hv.b, hv.r);

    correctW(B.a, B.g, inHere.r, w.a, w.g);
    correctW(B.b, B.r, inHere.r, w.b, w.r);

    outH = max(vec4(0.0), w - B);
    calcUV(outH, hu, hv, outU, outV);

    vec3 normal;
    normal.x = (inWest.r - inEast.r) * uOneOverDx;
    normal.y = (inSouth.r - inNorth.r) * uOneOverDy;
    normal.z = 2.0;
    outNormal = vec4(normalize(normal), 0.0);
}`;

// Pass 2: Calculate fluxes
const pass2FS = `#version 300 es
precision highp float;

uniform sampler2D uH;
uniform sampler2D uU;
uniform sampler2D uV;
uniform float uG;
uniform float uHalfG;

in vec2 vTexCoord;
layout(location = 0) out vec4 outXFlux;
layout(location = 1) out vec4 outYFlux;

float numericalFlux(float aplus, float aminus, float Fplus, float Fminus, float Udiff) {
    float denom = aplus - aminus;
    if (denom > 0.0) {
        return (aplus * Fminus - aminus * Fplus + aplus * aminus * Udiff) / denom;
    }
    return 0.0;
}

void main() {
    ivec2 idx = ivec2(gl_FragCoord.xy);

    vec4 hHere = texelFetch(uH, idx, 0);
    float hW_east = texelFetch(uH, idx + ivec2(1, 0), 0).a;
    float hS_north = texelFetch(uH, idx + ivec2(0, 1), 0).b;

    vec4 uHere = texelFetch(uU, idx, 0);
    float uW_east = texelFetch(uU, idx + ivec2(1, 0), 0).a;
    float uS_north = texelFetch(uU, idx + ivec2(0, 1), 0).b;

    vec4 vHere = texelFetch(uV, idx, 0);
    float vW_east = texelFetch(uV, idx + ivec2(1, 0), 0).a;
    float vS_north = texelFetch(uV, idx + ivec2(0, 1), 0).b;

    vec2 cNE = sqrt(max(vec2(0.0), uG * hHere.rg));
    float cW = sqrt(max(0.0, uG * hW_east));
    float cS = sqrt(max(0.0, uG * hS_north));

    float aplus = max(max(uHere.g + cNE.g, uW_east + cW), 0.0);
    float aminus = min(min(uHere.g - cNE.g, uW_east - cW), 0.0);
    float bplus = max(max(vHere.r + cNE.r, vS_north + cS), 0.0);
    float bminus = min(min(vHere.r - cNE.r, vS_north - cS), 0.0);

    outXFlux.r = numericalFlux(aplus, aminus, hW_east * uW_east, hHere.g * uHere.g, hW_east - hHere.g);
    outXFlux.g = numericalFlux(aplus, aminus,
        hW_east * (uW_east * uW_east + uHalfG * hW_east),
        hHere.g * (uHere.g * uHere.g + uHalfG * hHere.g),
        hW_east * uW_east - hHere.g * uHere.g);
    outXFlux.b = numericalFlux(aplus, aminus,
        hW_east * uW_east * vW_east,
        hHere.g * uHere.g * vHere.g,
        hW_east * vW_east - hHere.g * vHere.g);

    outYFlux.r = numericalFlux(bplus, bminus, hS_north * vS_north, hHere.r * vHere.r, hS_north - hHere.r);
    outYFlux.g = numericalFlux(bplus, bminus,
        hS_north * uS_north * vS_north,
        hHere.r * uHere.r * vHere.r,
        hS_north * uS_north - hHere.r * uHere.r);
    outYFlux.b = numericalFlux(bplus, bminus,
        hS_north * (vS_north * vS_north + uHalfG * hS_north),
        hHere.r * (vHere.r * vHere.r + uHalfG * hHere.r),
        hS_north * vS_north - hHere.r * vHere.r);
}`;

// Pass 3: Time step
const pass3FS = `#version 300 es
precision highp float;

uniform sampler2D uState;
uniform sampler2D uBottom;
uniform sampler2D uXFlux;
uniform sampler2D uYFlux;
uniform float uDt;
uniform float uOneOverDx;
uniform float uOneOverDy;
uniform float uGOverDx;
uniform float uGOverDy;
uniform float uFriction;
uniform float uEpsilon;
uniform int uNX;
uniform int uNY;

in vec2 vTexCoord;
out vec4 outState;

float calcUV_scalar(float h, float hu, float hv, out float u, out float v) {
    float h2 = h * h;
    float h4 = h2 * h2;
    float divideByH = sqrt(2.0) * h / sqrt(h4 + max(h4, uEpsilon));
    u = divideByH * hu;
    v = divideByH * hv;
    return divideByH;
}

float frictionCalc(float h, float u) {
    return max(h * u * uDt * 0.2, uFriction * h * abs(u) * u);
}

void main() {
    ivec2 idx = ivec2(gl_FragCoord.xy);

    if (idx.x <= 1 || idx.x >= uNX - 2 || idx.y <= 1 || idx.y >= uNY - 2) {
        vec4 state = texelFetch(uState, idx, 0);
        if (idx.x <= 1) state.g = abs(state.g);
        if (idx.x >= uNX - 2) state.g = -abs(state.g);
        if (idx.y <= 1) state.b = abs(state.b);
        if (idx.y >= uNY - 2) state.b = -abs(state.b);
        outState = state;
        return;
    }

    vec3 bHere = texelFetch(uBottom, idx, 0).rgb;
    vec3 inState = texelFetch(uState, idx, 0).rgb;

    vec3 xfluxHere = texelFetch(uXFlux, idx, 0).rgb;
    vec3 xfluxWest = texelFetch(uXFlux, idx + ivec2(-1, 0), 0).rgb;
    vec3 yfluxHere = texelFetch(uYFlux, idx, 0).rgb;
    vec3 yfluxSouth = texelFetch(uYFlux, idx + ivec2(0, -1), 0).rgb;

    float BX_west = texelFetch(uBottom, idx + ivec2(-1, 0), 0).g;
    float BY_south = texelFetch(uBottom, idx + ivec2(0, -1), 0).r;

    float h = max(0.0, inState.r - bHere.b);
    float u, v;
    calcUV_scalar(h, inState.g, inState.b, u, v);

    vec3 sourceTerm = vec3(
        0.0,
        -uGOverDx * h * (bHere.g - BX_west) - frictionCalc(h, u),
        -uGOverDy * h * (bHere.r - BY_south) - frictionCalc(h, v)
    );

    vec3 dByDt = (xfluxWest - xfluxHere) * uOneOverDx
               + (yfluxSouth - yfluxHere) * uOneOverDy
               + sourceTerm;

    vec3 result = inState + dByDt * uDt;
    outState = vec4(result, 0.0);
}`;

// Mouse interaction shader
const mouseFS = `#version 300 es
precision highp float;

uniform sampler2D uState;
uniform vec2 uMousePos;
uniform float uRadius;
uniform float uStrength;
uniform int uNX;
uniform int uNY;

in vec2 vTexCoord;
out vec4 outState;

void main() {
    ivec2 idx = ivec2(gl_FragCoord.xy);
    vec4 state = texelFetch(uState, idx, 0);

    vec2 pos = vec2(float(idx.x) / float(uNX), float(idx.y) / float(uNY));
    float dist = length(pos - uMousePos);

    if (dist < uRadius) {
        float factor = (1.0 - dist / uRadius);
        state.r += uStrength * factor * factor;
    }

    outState = state;
}`;

// 3D terrain vertex shader
const terrain3DVS = `#version 300 es
uniform mat4 uMVP;
uniform sampler2D uBottom;
uniform float uDomainWidth;
uniform float uDomainLength;
uniform int uNX;
uniform int uNY;

layout(location = 0) in vec2 aGridPos;
out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexCoord;

void main() {
    ivec2 idx = ivec2(aGridPos);
    float B = texelFetch(uBottom, idx, 0).b;

    float x = (aGridPos.x / float(uNX) - 0.5) * uDomainWidth;
    float z = (aGridPos.y / float(uNY)) * uDomainLength;
    float y = B;

    vWorldPos = vec3(x, y, z);
    vTexCoord = aGridPos / vec2(float(uNX), float(uNY));

    // Compute normal from neighbors
    float bE = texelFetch(uBottom, idx + ivec2(1, 0), 0).b;
    float bW = texelFetch(uBottom, idx + ivec2(-1, 0), 0).b;
    float bN = texelFetch(uBottom, idx + ivec2(0, 1), 0).b;
    float bS = texelFetch(uBottom, idx + ivec2(0, -1), 0).b;

    float dx = uDomainWidth / float(uNX);
    float dz = uDomainLength / float(uNY);

    vec3 tangentX = vec3(2.0 * dx, bE - bW, 0.0);
    vec3 tangentZ = vec3(0.0, bN - bS, 2.0 * dz);
    vNormal = normalize(cross(tangentZ, tangentX));

    gl_Position = uMVP * vec4(x, y, z, 1.0);
}`;

// 3D terrain fragment shader
const terrain3DFS = `#version 300 es
precision highp float;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(0.3, dot(vNormal, lightDir));

    // Grass-like color with variation
    vec3 baseColor = vec3(0.35, 0.5, 0.25);
    float heightFactor = clamp(vWorldPos.y / 30.0, 0.0, 1.0);
    vec3 highColor = vec3(0.5, 0.45, 0.35);
    vec3 color = mix(baseColor, highColor, heightFactor);

    fragColor = vec4(color * diff, 1.0);
}`;

// 3D water vertex shader
const water3DVS = `#version 300 es
uniform mat4 uMVP;
uniform sampler2D uState;
uniform sampler2D uBottom;
uniform sampler2D uNormal;
uniform float uDomainWidth;
uniform float uDomainLength;
uniform int uNX;
uniform int uNY;

layout(location = 0) in vec2 aGridPos;
out vec3 vWorldPos;
out vec3 vNormal;
out float vWaterDepth;
out vec3 vEyeDir;

uniform vec3 uCameraPos;

void main() {
    ivec2 idx = ivec2(aGridPos);
    float w = texelFetch(uState, idx, 0).r;
    float B = texelFetch(uBottom, idx, 0).b;
    float h = max(0.0, w - B);

    float x = (aGridPos.x / float(uNX) - 0.5) * uDomainWidth;
    float z = (aGridPos.y / float(uNY)) * uDomainLength;
    float y = w;

    // Push water surface down slightly where depth is very small
    if (h < 0.1) {
        y = B + h * 0.5;
    }

    vWorldPos = vec3(x, y, z);
    vWaterDepth = h;
    vNormal = texelFetch(uNormal, idx, 0).rgb;
    vEyeDir = normalize(uCameraPos - vWorldPos);

    gl_Position = uMVP * vec4(x, y, z, 1.0);
}`;

// 3D water fragment shader
const water3DFS = `#version 300 es
precision highp float;

in vec3 vWorldPos;
in vec3 vNormal;
in float vWaterDepth;
in vec3 vEyeDir;
out vec4 fragColor;

void main() {
    if (vWaterDepth < 0.05) {
        discard;
    }

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 normal = normalize(vNormal);

    // Fresnel
    float fresnel = pow(1.0 - max(0.0, dot(normal, vEyeDir)), 3.0);

    // Specular
    vec3 halfVec = normalize(lightDir + vEyeDir);
    float spec = pow(max(0.0, dot(normal, halfVec)), 50.0);

    // Water color based on depth
    vec3 shallowColor = vec3(0.4, 0.6, 0.7);
    vec3 deepColor = vec3(0.1, 0.2, 0.4);
    float depthFactor = clamp(vWaterDepth / 5.0, 0.0, 1.0);
    vec3 waterColor = mix(shallowColor, deepColor, depthFactor);

    // Diffuse lighting
    float diff = max(0.5, dot(normal, lightDir));

    vec3 color = waterColor * diff;
    color += vec3(1.0) * spec * 0.5;
    color += vec3(0.6, 0.7, 0.8) * fresnel * 0.3;

    float alpha = 0.7 + depthFactor * 0.3;
    fragColor = vec4(color, alpha);
}`;

function createShader(type, source) {
    const shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.error('Shader compile error:', gl.getShaderInfoLog(shader));
        gl.deleteShader(shader);
        return null;
    }
    return shader;
}

function createProgram(vsSource, fsSource) {
    const vs = createShader(gl.VERTEX_SHADER, vsSource);
    const fs = createShader(gl.FRAGMENT_SHADER, fsSource);
    const program = gl.createProgram();
    gl.attachShader(program, vs);
    gl.attachShader(program, fs);
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        console.error('Program link error:', gl.getProgramInfoLog(program));
        return null;
    }
    return program;
}

function createTexture(width, height, internalFormat, format, type, data) {
    const tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex);
    gl.texImage2D(gl.TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    return tex;
}

function createFramebuffer(textures) {
    const fb = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
    const drawBuffers = [];
    for (let i = 0; i < textures.length; i++) {
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gl.TEXTURE_2D, textures[i], 0);
        drawBuffers.push(gl.COLOR_ATTACHMENT0 + i);
    }
    gl.drawBuffers(drawBuffers);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    return fb;
}

// Generate valley terrain
function generateTerrain() {
    const bottomData = new Float32Array(NX * NY * 4);

    for (let j = 0; j < NY; j++) {
        const y = j / (NY - 1);  // 0 to 1
        const valleyY = y * DOMAIN_LENGTH;

        // Valley slope (higher at the back)
        const baseHeight = y * 15;

        for (let i = 0; i < NX; i++) {
            const x = (i / (NX - 1) - 0.5) * DOMAIN_WIDTH;  // -50 to 50
            const idx = (j * NX + i) * 4;

            // V-shaped valley
            const valleyWidth = 25;
            const valleyDepth = 10;
            let B;

            const absX = Math.abs(x);
            if (absX < valleyWidth * 0.4) {
                // Flat channel bottom
                B = baseHeight;
            } else if (absX < valleyWidth) {
                // Valley walls - smooth transition
                const t = (absX - valleyWidth * 0.4) / (valleyWidth * 0.6);
                B = baseHeight + t * t * valleyDepth;
            } else {
                // Flat top
                B = baseHeight + valleyDepth;
            }

            // Add a dam in the middle
            const damY = DOMAIN_LENGTH * 0.5;
            const damThickness = 4;
            const damHeight = 5;
            if (valleyY > damY - damThickness && valleyY < damY + damThickness) {
                const damT = 1.0 - Math.abs(valleyY - damY) / damThickness;
                // Dam with a gap in the middle for water to flow
                if (Math.abs(x) > 4) {
                    B = Math.max(B, baseHeight + damHeight * damT);
                }
            }

            // BN, BE, BA (use same value for simplicity)
            bottomData[idx] = B;     // BN
            bottomData[idx + 1] = B; // BE
            bottomData[idx + 2] = B; // BA
            bottomData[idx + 3] = 0;
        }
    }

    return bottomData;
}

// Initialize water state
function generateInitialState(bottomData) {
    const stateData = new Float32Array(NX * NY * 4);

    for (let j = 0; j < NY; j++) {
        for (let i = 0; i < NX; i++) {
            const idx = (j * NX + i) * 4;
            const B = bottomData[idx + 2];  // BA

            // Thin layer of water in the valley
            const y = j / (NY - 1);
            const baseHeight = y * 15;  // Match terrain slope
            const waterLevel = baseHeight + 1.5;  // 1.5 meters above valley floor

            // Only fill where terrain is below water level
            let w = Math.max(B, waterLevel);

            stateData[idx] = w;     // w (water surface)
            stateData[idx + 1] = 0; // hu
            stateData[idx + 2] = 0; // hv
            stateData[idx + 3] = 0;
        }
    }

    return stateData;
}

function createMeshVAO() {
    const vertices = [];
    const indices = [];

    for (let j = 0; j < NY; j++) {
        for (let i = 0; i < NX; i++) {
            vertices.push(i, j);
        }
    }

    for (let j = 0; j < NY - 1; j++) {
        for (let i = 0; i < NX - 1; i++) {
            const idx = j * NX + i;
            indices.push(idx, idx + 1, idx + NX);
            indices.push(idx + 1, idx + NX + 1, idx + NX);
        }
    }

    const vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    const vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    gl.enableVertexAttribArray(0);
    gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);

    const ibo = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(indices), gl.STATIC_DRAW);

    gl.bindVertexArray(null);

    return { vao, indexCount: indices.length };
}

function init() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    // Create simulation programs
    pass1Program = createProgram(quadVS, pass1FS);
    pass2Program = createProgram(quadVS, pass2FS);
    pass3Program = createProgram(quadVS, pass3FS);
    mouseProgram = createProgram(quadVS, mouseFS);

    // Create 3D render programs
    terrainProgram = createProgram(terrain3DVS, terrain3DFS);
    waterProgram = createProgram(water3DVS, water3DFS);

    // Create quad VAO for simulation
    quadVAO = gl.createVertexArray();
    gl.bindVertexArray(quadVAO);
    const quadBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, quadBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-1,-1, 1,-1, -1,1, 1,1]), gl.STATIC_DRAW);
    gl.enableVertexAttribArray(0);
    gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);

    // Create mesh VAO for 3D rendering
    const meshData = createMeshVAO();
    meshVAO = meshData.vao;
    const meshIndexCount = meshData.indexCount;

    // Generate terrain
    const bottomData = generateTerrain();
    simTextures.bottom = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, bottomData);

    // Generate initial state
    const stateData = generateInitialState(bottomData);
    simTextures.state = [
        createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, stateData),
        createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null)
    ];

    // Create intermediate textures
    simTextures.h = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null);
    simTextures.u = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null);
    simTextures.v = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null);
    simTextures.normal = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null);
    simTextures.xflux = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null);
    simTextures.yflux = createTexture(NX, NY, gl.RGBA32F, gl.RGBA, gl.FLOAT, null);

    // Create framebuffers
    framebuffers.pass1 = createFramebuffer([simTextures.h, simTextures.u, simTextures.v, simTextures.normal]);
    framebuffers.pass2 = createFramebuffer([simTextures.xflux, simTextures.yflux]);
    framebuffers.pass3 = [
        createFramebuffer([simTextures.state[1]]),
        createFramebuffer([simTextures.state[0]])
    ];
    framebuffers.mouse = [
        createFramebuffer([simTextures.state[1]]),
        createFramebuffer([simTextures.state[0]])
    ];

    // Store mesh index count
    simTextures.meshIndexCount = meshIndexCount;

    // Mouse events
    canvas.addEventListener('mousedown', (e) => {
        if (e.button === 0) {
            mouseDown = true;
            mouseX = e.clientX / canvas.width;
            mouseY = 1 - e.clientY / canvas.height;
        } else if (e.button === 2) {
            rightMouseDown = true;
        }
        lastMouseX = e.clientX;
        lastMouseY = e.clientY;
    });

    canvas.addEventListener('mouseup', (e) => {
        if (e.button === 0) mouseDown = false;
        if (e.button === 2) rightMouseDown = false;
    });

    canvas.addEventListener('mousemove', (e) => {
        const dx = e.clientX - lastMouseX;
        const dy = e.clientY - lastMouseY;

        if (mouseDown) {
            mouseX = e.clientX / canvas.width;
            mouseY = 1 - e.clientY / canvas.height;
        }

        if (rightMouseDown) {
            cameraAngleX += dx * 0.01;
            cameraAngleY = Math.max(-1.5, Math.min(1.5, cameraAngleY + dy * 0.01));
        }

        lastMouseX = e.clientX;
        lastMouseY = e.clientY;
    });

    canvas.addEventListener('wheel', (e) => {
        cameraDistance *= 1 + e.deltaY * 0.001;
        cameraDistance = Math.max(50, Math.min(500, cameraDistance));
        e.preventDefault();
    });

    canvas.addEventListener('contextmenu', (e) => e.preventDefault());

    requestAnimationFrame(frame);
}

function runPass1() {
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffers.pass1);
    gl.viewport(0, 0, NX, NY);
    gl.useProgram(pass1Program);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.state[currentStateIdx]);
    gl.uniform1i(gl.getUniformLocation(pass1Program, 'uState'), 0);

    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.bottom);
    gl.uniform1i(gl.getUniformLocation(pass1Program, 'uBottom'), 1);

    gl.uniform1f(gl.getUniformLocation(pass1Program, 'uTwoTheta'), 2 * THETA);
    gl.uniform1f(gl.getUniformLocation(pass1Program, 'uEpsilon'), EPSILON);
    gl.uniform1f(gl.getUniformLocation(pass1Program, 'uOneOverDx'), 1 / DX);
    gl.uniform1f(gl.getUniformLocation(pass1Program, 'uOneOverDy'), 1 / DY);

    gl.bindVertexArray(quadVAO);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}

function runPass2() {
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffers.pass2);
    gl.viewport(0, 0, NX, NY);
    gl.useProgram(pass2Program);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.h);
    gl.uniform1i(gl.getUniformLocation(pass2Program, 'uH'), 0);

    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.u);
    gl.uniform1i(gl.getUniformLocation(pass2Program, 'uU'), 1);

    gl.activeTexture(gl.TEXTURE2);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.v);
    gl.uniform1i(gl.getUniformLocation(pass2Program, 'uV'), 2);

    gl.uniform1f(gl.getUniformLocation(pass2Program, 'uG'), G);
    gl.uniform1f(gl.getUniformLocation(pass2Program, 'uHalfG'), G / 2);

    gl.bindVertexArray(quadVAO);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}

function runPass3() {
    const nextStateIdx = 1 - currentStateIdx;
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffers.pass3[currentStateIdx]);
    gl.viewport(0, 0, NX, NY);
    gl.useProgram(pass3Program);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.state[currentStateIdx]);
    gl.uniform1i(gl.getUniformLocation(pass3Program, 'uState'), 0);

    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.bottom);
    gl.uniform1i(gl.getUniformLocation(pass3Program, 'uBottom'), 1);

    gl.activeTexture(gl.TEXTURE2);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.xflux);
    gl.uniform1i(gl.getUniformLocation(pass3Program, 'uXFlux'), 2);

    gl.activeTexture(gl.TEXTURE3);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.yflux);
    gl.uniform1i(gl.getUniformLocation(pass3Program, 'uYFlux'), 3);

    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uDt'), dt);
    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uOneOverDx'), 1 / DX);
    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uOneOverDy'), 1 / DY);
    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uGOverDx'), G / DX);
    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uGOverDy'), G / DY);
    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uFriction'), FRICTION);
    gl.uniform1f(gl.getUniformLocation(pass3Program, 'uEpsilon'), EPSILON);
    gl.uniform1i(gl.getUniformLocation(pass3Program, 'uNX'), NX);
    gl.uniform1i(gl.getUniformLocation(pass3Program, 'uNY'), NY);

    gl.bindVertexArray(quadVAO);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);

    currentStateIdx = nextStateIdx;
}

function applyMouse() {
    if (!mouseDown) return;

    const nextStateIdx = 1 - currentStateIdx;
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffers.mouse[currentStateIdx]);
    gl.viewport(0, 0, NX, NY);
    gl.useProgram(mouseProgram);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.state[currentStateIdx]);
    gl.uniform1i(gl.getUniformLocation(mouseProgram, 'uState'), 0);

    gl.uniform2f(gl.getUniformLocation(mouseProgram, 'uMousePos'), mouseX, mouseY);
    gl.uniform1f(gl.getUniformLocation(mouseProgram, 'uRadius'), 0.08);
    gl.uniform1f(gl.getUniformLocation(mouseProgram, 'uStrength'), 0.5);
    gl.uniform1i(gl.getUniformLocation(mouseProgram, 'uNX'), NX);
    gl.uniform1i(gl.getUniformLocation(mouseProgram, 'uNY'), NY);

    gl.bindVertexArray(quadVAO);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);

    currentStateIdx = nextStateIdx;
}

function multiplyMatrices(a, b) {
    const result = new Float32Array(16);
    for (let col = 0; col < 4; col++) {
        for (let row = 0; row < 4; row++) {
            result[col * 4 + row] =
                a[0 * 4 + row] * b[col * 4 + 0] +
                a[1 * 4 + row] * b[col * 4 + 1] +
                a[2 * 4 + row] * b[col * 4 + 2] +
                a[3 * 4 + row] * b[col * 4 + 3];
        }
    }
    return result;
}

function createPerspectiveMatrix(fov, aspect, near, far) {
    const f = 1.0 / Math.tan(fov / 2);
    return new Float32Array([
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far + near) / (near - far), -1,
        0, 0, (2 * far * near) / (near - far), 0
    ]);
}

function createLookAtMatrix(eye, target, up) {
    const zAxis = normalize([eye[0] - target[0], eye[1] - target[1], eye[2] - target[2]]);
    const xAxis = normalize(cross(up, zAxis));
    const yAxis = cross(zAxis, xAxis);

    return new Float32Array([
        xAxis[0], yAxis[0], zAxis[0], 0,
        xAxis[1], yAxis[1], zAxis[1], 0,
        xAxis[2], yAxis[2], zAxis[2], 0,
        -dot(xAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1
    ]);
}

function normalize(v) {
    const len = Math.sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return [v[0]/len, v[1]/len, v[2]/len];
}

function cross(a, b) {
    return [
        a[1]*b[2] - a[2]*b[1],
        a[2]*b[0] - a[0]*b[2],
        a[0]*b[1] - a[1]*b[0]
    ];
}

function dot(a, b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

function render() {
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0.5, 0.7, 0.9, 1.0);  // Sky blue
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.enable(gl.DEPTH_TEST);

    // Camera setup - look at center of domain from above and to the side
    const centerX = 0;
    const centerY = 15;
    const centerZ = DOMAIN_LENGTH * 0.5;

    // Spherical coordinates for camera position
    const camX = centerX + cameraDistance * Math.cos(cameraAngleY) * Math.sin(cameraAngleX);
    const camY = centerY + cameraDistance * Math.sin(cameraAngleY) + 50;
    const camZ = centerZ + cameraDistance * Math.cos(cameraAngleY) * Math.cos(cameraAngleX);

    const eye = [camX, camY, camZ];
    const target = [centerX, centerY, centerZ];
    const up = [0, 1, 0];

    const aspect = canvas.width / canvas.height;
    const proj = createPerspectiveMatrix(Math.PI / 4, aspect, 1, 1000);
    const view = createLookAtMatrix(eye, target, up);
    const mvp = multiplyMatrices(proj, view);

    // Draw terrain
    gl.useProgram(terrainProgram);
    gl.uniformMatrix4fv(gl.getUniformLocation(terrainProgram, 'uMVP'), false, mvp);
    gl.uniform1f(gl.getUniformLocation(terrainProgram, 'uDomainWidth'), DOMAIN_WIDTH);
    gl.uniform1f(gl.getUniformLocation(terrainProgram, 'uDomainLength'), DOMAIN_LENGTH);
    gl.uniform1i(gl.getUniformLocation(terrainProgram, 'uNX'), NX);
    gl.uniform1i(gl.getUniformLocation(terrainProgram, 'uNY'), NY);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.bottom);
    gl.uniform1i(gl.getUniformLocation(terrainProgram, 'uBottom'), 0);

    gl.bindVertexArray(meshVAO);
    gl.drawElements(gl.TRIANGLES, simTextures.meshIndexCount, gl.UNSIGNED_INT, 0);

    // Draw water with transparency
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    gl.useProgram(waterProgram);
    gl.uniformMatrix4fv(gl.getUniformLocation(waterProgram, 'uMVP'), false, mvp);
    gl.uniform1f(gl.getUniformLocation(waterProgram, 'uDomainWidth'), DOMAIN_WIDTH);
    gl.uniform1f(gl.getUniformLocation(waterProgram, 'uDomainLength'), DOMAIN_LENGTH);
    gl.uniform1i(gl.getUniformLocation(waterProgram, 'uNX'), NX);
    gl.uniform1i(gl.getUniformLocation(waterProgram, 'uNY'), NY);
    gl.uniform3f(gl.getUniformLocation(waterProgram, 'uCameraPos'), camX, camY, camZ);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.state[currentStateIdx]);
    gl.uniform1i(gl.getUniformLocation(waterProgram, 'uState'), 0);

    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.bottom);
    gl.uniform1i(gl.getUniformLocation(waterProgram, 'uBottom'), 1);

    gl.activeTexture(gl.TEXTURE2);
    gl.bindTexture(gl.TEXTURE_2D, simTextures.normal);
    gl.uniform1i(gl.getUniformLocation(waterProgram, 'uNormal'), 2);

    gl.bindVertexArray(meshVAO);
    gl.drawElements(gl.TRIANGLES, simTextures.meshIndexCount, gl.UNSIGNED_INT, 0);

    gl.disable(gl.BLEND);
}

function frame() {
    // Run simulation (fewer steps = slower)
    const stepsPerFrame = 2;
    for (let i = 0; i < stepsPerFrame; i++) {
        runPass1();
        runPass2();
        runPass3();
    }

    applyMouse();
    render();

    time += dt * stepsPerFrame;
    requestAnimationFrame(frame);
}

init();
