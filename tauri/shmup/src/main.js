// ===== 定数 =====
const CANVAS_WIDTH = 480;
const CANVAS_HEIGHT = 640;
const PLAYER_WIDTH = 30;
const PLAYER_HEIGHT = 20;
const PLAYER_SPEED = 5;
const BULLET_WIDTH = 4;
const BULLET_HEIGHT = 10;
const BULLET_SPEED = 8;
const ENEMY_WIDTH = 30;
const ENEMY_HEIGHT = 30;
const ENEMY_SPEED = 2;
const ENEMY_SPAWN_INTERVAL = 60;

// ===== ゲーム状態 =====
let canvas, ctx;
let player;
let bullets = [];
let enemies = [];
let score = 0;
let gameOver = false;
let keys = {};
let frameCount = 0;

// ===== 初期化 =====
function init() {
  canvas = document.getElementById('game');
  ctx = canvas.getContext('2d');

  player = {
    x: CANVAS_WIDTH / 2 - PLAYER_WIDTH / 2,
    y: CANVAS_HEIGHT - PLAYER_HEIGHT - 20,
    width: PLAYER_WIDTH,
    height: PLAYER_HEIGHT
  };

  document.addEventListener('keydown', (e) => {
    keys[e.code] = true;
    if (e.code === 'Space') e.preventDefault();
  });
  document.addEventListener('keyup', (e) => {
    keys[e.code] = false;
  });

  requestAnimationFrame(gameLoop);
}

// ===== ゲームループ =====
function gameLoop() {
  if (gameOver) {
    document.getElementById('gameover').style.display = 'block';
    return;
  }

  update();
  render();
  frameCount++;
  requestAnimationFrame(gameLoop);
}

// ===== 更新処理 =====
function update() {
  // プレイヤー移動
  if (keys['ArrowLeft'] || keys['KeyA']) {
    player.x = Math.max(0, player.x - PLAYER_SPEED);
  }
  if (keys['ArrowRight'] || keys['KeyD']) {
    player.x = Math.min(CANVAS_WIDTH - PLAYER_WIDTH, player.x + PLAYER_SPEED);
  }

  // 弾発射（スペースキー）
  if (keys['Space'] && frameCount % 10 === 0) {
    bullets.push({
      x: player.x + PLAYER_WIDTH / 2 - BULLET_WIDTH / 2,
      y: player.y,
      width: BULLET_WIDTH,
      height: BULLET_HEIGHT
    });
  }

  // 弾の移動
  for (let i = bullets.length - 1; i >= 0; i--) {
    bullets[i].y -= BULLET_SPEED;
    if (bullets[i].y + BULLET_HEIGHT < 0) {
      bullets.splice(i, 1);
    }
  }

  // 敵の出現
  if (frameCount % ENEMY_SPAWN_INTERVAL === 0) {
    enemies.push({
      x: Math.random() * (CANVAS_WIDTH - ENEMY_WIDTH),
      y: -ENEMY_HEIGHT,
      width: ENEMY_WIDTH,
      height: ENEMY_HEIGHT
    });
  }

  // 敵の移動
  for (let i = enemies.length - 1; i >= 0; i--) {
    enemies[i].y += ENEMY_SPEED;
    if (enemies[i].y > CANVAS_HEIGHT) {
      enemies.splice(i, 1);
    }
  }

  // 衝突判定: 弾 vs 敵
  for (let i = bullets.length - 1; i >= 0; i--) {
    for (let j = enemies.length - 1; j >= 0; j--) {
      if (isColliding(bullets[i], enemies[j])) {
        bullets.splice(i, 1);
        enemies.splice(j, 1);
        score += 100;
        document.getElementById('score').textContent = 'SCORE: ' + score;
        break;
      }
    }
  }

  // 衝突判定: プレイヤー vs 敵
  for (let enemy of enemies) {
    if (isColliding(player, enemy)) {
      gameOver = true;
      return;
    }
  }
}

// ===== 衝突判定（矩形同士） =====
function isColliding(a, b) {
  return a.x < b.x + b.width &&
         a.x + a.width > b.x &&
         a.y < b.y + b.height &&
         a.y + a.height > b.y;
}

// ===== 描画処理 =====
function render() {
  ctx.clearRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

  // プレイヤー描画（三角形）
  ctx.fillStyle = '#0f0';
  ctx.beginPath();
  ctx.moveTo(player.x + PLAYER_WIDTH / 2, player.y);
  ctx.lineTo(player.x, player.y + PLAYER_HEIGHT);
  ctx.lineTo(player.x + PLAYER_WIDTH, player.y + PLAYER_HEIGHT);
  ctx.closePath();
  ctx.fill();

  // 弾描画（矩形）
  ctx.fillStyle = '#ff0';
  for (let bullet of bullets) {
    ctx.fillRect(bullet.x, bullet.y, bullet.width, bullet.height);
  }

  // 敵描画（矩形）
  ctx.fillStyle = '#f00';
  for (let enemy of enemies) {
    ctx.fillRect(enemy.x, enemy.y, enemy.width, enemy.height);
  }
}

// ===== 起動 =====
init();
