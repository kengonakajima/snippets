(function() {
  let cookies = 0;
  let cps = 0;
  let cursors = 0;
  let cursorCost = 10;

  const cookieCountEl = document.getElementById('cookie-count');
  const cpsEl = document.getElementById('cps');
  const buyCursorBtn = document.getElementById('buy-cursor');
  const cursorCountEl = document.getElementById('cursor-count');

  const scene = new THREE.Scene();
  const camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 1000);
  const renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(window.innerWidth, window.innerHeight);
  document.body.appendChild(renderer.domElement);

  const light = new THREE.DirectionalLight(0xffffff, 1);
  light.position.set(0, 1, 1);
  scene.add(light);

  const radius = 5;
  const canvas = document.createElement('canvas');
  canvas.width = canvas.height = 512;
  const ctx = canvas.getContext('2d');
  ctx.fillStyle = '#D2B48C';
  ctx.beginPath();
  ctx.arc(256, 256, 200, 0, Math.PI * 2);
  ctx.fill();
  ctx.fillStyle = '#8B4513';
  for (let i = 0; i < 50; i++) {
    const angle = Math.random() * Math.PI * 2;
    const r = 150 * Math.sqrt(Math.random());
    const x = 256 + r * Math.cos(angle);
    const y = 256 + r * Math.sin(angle);
    ctx.beginPath();
    ctx.arc(x, y, Math.random() * 10 + 5, 0, Math.PI * 2);
    ctx.fill();
  }
  const texture = new THREE.CanvasTexture(canvas);
  const geometry = new THREE.CylinderGeometry(radius, radius, 1, 64);
  const material = new THREE.MeshPhongMaterial({ map: texture });
  const cookie = new THREE.Mesh(geometry, material);
  cookie.rotation.x = -Math.PI / 2;
  scene.add(cookie);

  camera.position.set(0, 10, 15);
  camera.lookAt(0, 0, 0);

  const raycaster = new THREE.Raycaster();
  const mouse = new THREE.Vector2();

  function onMouseDown(event) {
    mouse.x = (event.clientX / window.innerWidth) * 2 - 1;
    mouse.y = -(event.clientY / window.innerHeight) * 2 + 1;
    raycaster.setFromCamera(mouse, camera);
    const intersects = raycaster.intersectObject(cookie);
    if (intersects.length > 0) {
      cookies++;
      updateUI();
      cookie.scale.set(1.1, 1.1, 1.1);
      setTimeout(() => cookie.scale.set(1, 1, 1), 100);
    }
  }
  window.addEventListener('mousedown', onMouseDown);

  buyCursorBtn.addEventListener('click', () => {
    if (cookies >= cursorCost) {
      cookies -= cursorCost;
      cursors++;
      cps += 0.1;
      cursorCost = Math.ceil(cursorCost * 1.15);
      updateUI();
    }
  });

  function updateUI() {
    cookieCountEl.textContent = 'Cookies: ' + cookies.toFixed(1);
    cpsEl.textContent = 'CPS: ' + cps.toFixed(1);
    buyCursorBtn.textContent = `Buy Cursor (Cost: ${cursorCost})`;
    cursorCountEl.textContent = `Cursors: ${cursors}`;
  }

  setInterval(() => {
    cookies += cps / 10;
    updateUI();
  }, 100);

  function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
  }
  window.addEventListener('resize', onWindowResize);

  function animate() {
    requestAnimationFrame(animate);
    renderer.render(scene, camera);
  }
  animate();

  updateUI();
})();