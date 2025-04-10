import * as THREE from 'three';
import { PointerLockControls } from 'three/addons/controls/PointerLockControls.js';

// Initialize scene, camera, and renderer
const scene = new THREE.Scene();
scene.background = new THREE.Color(0x87ceeb); // Sky blue background

const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
camera.position.set(0, 2, 5); // Position slightly above ground

const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.shadowMap.enabled = true;
document.body.appendChild(renderer.domElement);

// Add FPS controls with pointer lock
const controls = new PointerLockControls(camera, document.body);

// Click to start controls
const instructions = document.createElement('div');
instructions.style.position = 'absolute';
instructions.style.top = '50%';
instructions.style.width = '100%';
instructions.style.textAlign = 'center';
instructions.style.fontSize = '24px';
instructions.style.fontFamily = 'Arial, sans-serif';
instructions.style.color = 'white';
instructions.style.textShadow = '0 0 5px black';
instructions.innerHTML = 'Click to play<br>WASD = Move, SPACE = Jump, MOUSE = Look<br>1,2 = Switch blocks';
document.body.appendChild(instructions);

// Movement and physics variables
const velocity = new THREE.Vector3();
const direction = new THREE.Vector3();
let playerOnGround = false;
const playerHeight = 1.8;
const jumpStrength = 0.2;
const gravity = 0.01;
const moveSpeed = 0.1;

// Movement state
const moveState = {
  forward: false,
  backward: false,
  left: false,
  right: false,
  jump: false
};

// Event listeners for controls
renderer.domElement.addEventListener('click', function() {
  controls.lock();
});

controls.addEventListener('lock', function() {
  instructions.style.display = 'none';
});

controls.addEventListener('unlock', function() {
  instructions.style.display = 'block';
});

// Add lighting
const ambientLight = new THREE.AmbientLight(0x606060);
scene.add(ambientLight);

const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
directionalLight.position.set(5, 10, 7.5);
directionalLight.castShadow = true;
directionalLight.shadow.camera.near = 0.5;
directionalLight.shadow.camera.far = 50;
directionalLight.shadow.camera.left = -10;
directionalLight.shadow.camera.right = 10;
directionalLight.shadow.camera.top = 10;
directionalLight.shadow.camera.bottom = -10;
scene.add(directionalLight);

// Grid helper
const gridHelper = new THREE.GridHelper(20, 20);
scene.add(gridHelper);

// Voxel data structure (1 for filled, 0 for empty)
const size = 10;
const voxels = {};

// Materials for voxels
const materials = {
  1: new THREE.MeshStandardMaterial({ color: 0x8B4513 }), // Brown for ground
  2: createGrassBlockMaterial() // Grass block with green top and dirt sides
};

// Create a custom material for grass blocks
function createGrassBlockMaterial() {
  const materials = [
    new THREE.MeshStandardMaterial({ color: 0x8B4513 }), // Right side - dirt
    new THREE.MeshStandardMaterial({ color: 0x8B4513 }), // Left side - dirt
    new THREE.MeshStandardMaterial({ color: 0x228B22 }), // Top - grass
    new THREE.MeshStandardMaterial({ color: 0x8B4513 }), // Bottom - dirt
    new THREE.MeshStandardMaterial({ color: 0x8B4513 }), // Front - dirt
    new THREE.MeshStandardMaterial({ color: 0x8B4513 })  // Back - dirt
  ];
  return materials;
}

// Voxel meshes container
let voxelMeshes = new THREE.Group();
scene.add(voxelMeshes);

// Function to set a voxel at a specific position
function setVoxel(x, y, z, value) {
  const key = `${x},${y},${z}`;
  if (value) {
    voxels[key] = value;
  } else {
    delete voxels[key];
  }
  updateVoxelMeshes();
}

// Create a simple ground
for (let x = -5; x <= 5; x++) {
  for (let z = -5; z <= 5; z++) {
    setVoxel(x, 0, z, 1);
  }
}

// Add some random blocks
for (let i = 0; i < 20; i++) {
  const x = Math.floor(Math.random() * 10) - 5;
  const y = Math.floor(Math.random() * 3) + 1;
  const z = Math.floor(Math.random() * 10) - 5;
  setVoxel(x, y, z, 1);
}

// Update the visible voxels
function updateVoxelMeshes() {
  // Remove old meshes
  scene.remove(voxelMeshes);
  voxelMeshes = new THREE.Group();
  
  // Create geometry for each voxel type
  const boxGeometry = new THREE.BoxGeometry(1, 1, 1);
  
  // Add a mesh for each voxel
  for (const key in voxels) {
    const [x, y, z] = key.split(',').map(Number);
    const voxelType = voxels[key];
    
    if (voxelType && materials[voxelType]) {
      let mesh;
      if (Array.isArray(materials[voxelType])) {
        // Multi-material block
        mesh = new THREE.Mesh(boxGeometry, materials[voxelType]);
      } else {
        // Single material block
        mesh = new THREE.Mesh(boxGeometry, materials[voxelType]);
      }
      mesh.position.set(x, y, z);
      mesh.castShadow = true;
      mesh.receiveShadow = true;
      voxelMeshes.add(mesh);
    }
  }
  
  scene.add(voxelMeshes);
}

// Raycasting for interaction
const raycaster = new THREE.Raycaster();
const pointer = new THREE.Vector2();

// Track which block type we're currently placing
let currentBlockType = 1; // Start with normal brown block

// Crosshair for aiming
const crosshair = document.createElement('div');
crosshair.style.position = 'absolute';
crosshair.style.top = '50%';
crosshair.style.left = '50%';
crosshair.style.width = '20px';
crosshair.style.height = '20px';
crosshair.style.marginLeft = '-10px';
crosshair.style.marginTop = '-10px';
crosshair.style.color = 'white';
crosshair.style.fontSize = '20px';
crosshair.style.textAlign = 'center';
crosshair.style.userSelect = 'none';
crosshair.style.lineHeight = '20px';
crosshair.innerHTML = '+';
document.body.appendChild(crosshair);

// Event listeners for mouse interaction
window.addEventListener('mousedown', onMouseDown);

function onMouseDown(event) {
  if (!controls.isLocked) return;
  
  // Use camera direction for raycasting instead of mouse position
  raycaster.setFromCamera(new THREE.Vector2(0, 0), camera);
  
  // Calculate objects intersecting the picking ray
  const intersects = raycaster.intersectObjects(voxelMeshes.children);
  
  if (intersects.length > 0) {
    const intersect = intersects[0];
    if (event.button === 0) { // Left click: add voxel
      // Calculate position adjacent to the clicked voxel
      const position = intersect.point.add(
        intersect.face.normal.multiplyScalar(0.5)
      ).floor();
      // Don't place blocks where the player is standing
      if (!checkCollision(position)) {
        setVoxel(position.x, position.y, position.z, currentBlockType);
      }
    } else if (event.button === 2) { // Right click: remove voxel
      // Calculate position of the clicked voxel
      const position = intersect.point.sub(
        intersect.face.normal.multiplyScalar(0.5)
      ).floor();
      setVoxel(position.x, position.y, position.z, 0);
    }
  }
}

// Add key listener to switch block types
window.addEventListener('keydown', (event) => {
  if (event.key === '1') {
    currentBlockType = 1; // Regular brown block
    console.log('Selected brown block');
  } else if (event.key === '2') {
    currentBlockType = 2; // Grass block
    console.log('Selected grass block');
  }
});

// Prevent context menu on right-click
window.addEventListener('contextmenu', (event) => event.preventDefault());

// Handle keyboard controls
document.addEventListener('keydown', (event) => {
  switch (event.code) {
    case 'KeyW':
    case 'ArrowUp':
      moveState.forward = true;
      break;
    case 'KeyA':
    case 'ArrowLeft':
      moveState.left = true;
      break;
    case 'KeyS':
    case 'ArrowDown':
      moveState.backward = true;
      break;
    case 'KeyD':
    case 'ArrowRight':
      moveState.right = true;
      break;
    case 'Space':
      if (playerOnGround) {
        velocity.y += jumpStrength;
        playerOnGround = false;
      }
      break;
  }
});

document.addEventListener('keyup', (event) => {
  switch (event.code) {
    case 'KeyW':
    case 'ArrowUp':
      moveState.forward = false;
      break;
    case 'KeyA':
    case 'ArrowLeft':
      moveState.left = false;
      break;
    case 'KeyS':
    case 'ArrowDown':
      moveState.backward = false;
      break;
    case 'KeyD':
    case 'ArrowRight':
      moveState.right = false;
      break;
  }
});

// Handle window resize
window.addEventListener('resize', () => {
  camera.aspect = window.innerWidth / window.innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(window.innerWidth, window.innerHeight);
});

// Check if a position has a voxel
function hasVoxel(x, y, z) {
  const key = `${Math.floor(x)},${Math.floor(y)},${Math.floor(z)}`;
  return voxels[key] !== undefined;
}

// Check collision with voxels (simple collision detection)
function checkCollision(position) {
  // Check for collisions at feet, body, and head level
  const playerRadius = 0.3;
  
  // Check if we're inside a block
  for (let y = 0; y < playerHeight; y += 0.5) {
    if (hasVoxel(position.x, position.y + y, position.z)) {
      return true;
    }
    
    // Check around the player's body
    if (hasVoxel(position.x + playerRadius, position.y + y, position.z) ||
        hasVoxel(position.x - playerRadius, position.y + y, position.z) ||
        hasVoxel(position.x, position.y + y, position.z + playerRadius) ||
        hasVoxel(position.x, position.y + y, position.z - playerRadius)) {
      return true;
    }
  }
  
  return false;
}

// Animation loop
function animate() {
  requestAnimationFrame(animate);
  
  if (controls.isLocked) {
    // Gravity
    velocity.y -= gravity;
    
    // Check if we're on ground
    const groundPos = new THREE.Vector3(
      camera.position.x,
      camera.position.y - playerHeight/2 - 0.1,
      camera.position.z
    );
    playerOnGround = hasVoxel(groundPos.x, groundPos.y, groundPos.z);
    
    if (playerOnGround && velocity.y < 0) {
      velocity.y = 0;
    }
    
    // Calculate movement direction
    direction.z = Number(moveState.forward) - Number(moveState.backward);
    direction.x = Number(moveState.right) - Number(moveState.left);
    direction.normalize();
    
    // Move player
    if (moveState.forward || moveState.backward) {
      const potentialPosition = camera.position.clone();
      potentialPosition.z += direction.z * moveSpeed;
      
      if (!checkCollision(potentialPosition)) {
        controls.moveForward(direction.z * moveSpeed);
      }
    }
    
    if (moveState.left || moveState.right) {
      const potentialPosition = camera.position.clone();
      potentialPosition.x += direction.x * moveSpeed;
      
      if (!checkCollision(potentialPosition)) {
        controls.moveRight(direction.x * moveSpeed);
      }
    }
    
    // Apply vertical velocity (jumping/falling)
    camera.position.y += velocity.y;
    
    // Prevent falling through ground
    const minHeight = playerHeight/2;
    if (camera.position.y < minHeight) {
      camera.position.y = minHeight;
      velocity.y = 0;
      playerOnGround = true;
    }
  }
  
  renderer.render(scene, camera);
}

// Initialize the voxel meshes and start animation
updateVoxelMeshes();
animate();