struct Ball {
  mut x : Double
  mut y : Double
  mut vx : Double
  mut vy : Double
}

let screen_width : Double = 800.0
let screen_height : Double = 600.0
let ball_size : Double = 32.0

fn random_double(seed : Int, min : Double, max : Double) -> Double {
  let a : Int64 = 1103515245L
  let c : Int64 = 12345L
  let m : Int64 = 2147483648L
  let next = (a * seed.to_int64() + c) % m
  let normalized = next.to_double() / m.to_double()
  min + normalized * (max - min)
}

fn create_ball(seed : Int) -> Ball {
  let x = random_double(seed, ball_size, screen_width - ball_size)
  let y = random_double(seed * 7, ball_size, screen_height - ball_size)
  let vx = random_double(seed * 13, -5.0, 5.0)
  let vy = random_double(seed * 17, -5.0, 5.0)
  Ball::{ x, y, vx, vy }
}

fn update_ball(ball : Ball) -> Unit {
  ball.x = ball.x + ball.vx
  ball.y = ball.y + ball.vy
  if ball.x < 0.0 {
    ball.x = 0.0
    ball.vx = -ball.vx
  }
  if ball.x > screen_width - ball_size {
    ball.x = screen_width - ball_size
    ball.vx = -ball.vx
  }
  if ball.y < 0.0 {
    ball.y = 0.0
    ball.vy = -ball.vy
  }
  if ball.y > screen_height - ball_size {
    ball.y = screen_height - ball_size
    ball.vy = -ball.vy
  }
}

fn distance(b1 : Ball, b2 : Ball) -> Double {
  let dx = (b1.x + ball_size / 2.0) - (b2.x + ball_size / 2.0)
  let dy = (b1.y + ball_size / 2.0) - (b2.y + ball_size / 2.0)
  (dx * dx + dy * dy).sqrt()
}

fn collide_balls(b1 : Ball, b2 : Ball) -> Unit {
  let dist = distance(b1, b2)
  if dist < ball_size && dist > 0.0 {
    let dx = (b2.x + ball_size / 2.0) - (b1.x + ball_size / 2.0)
    let dy = (b2.y + ball_size / 2.0) - (b1.y + ball_size / 2.0)
    let nx = dx / dist
    let ny = dy / dist
    let dvx = b1.vx - b2.vx
    let dvy = b1.vy - b2.vy
    let dvn = dvx * nx + dvy * ny
    if dvn > 0.0 {
      b1.vx = b1.vx - dvn * nx
      b1.vy = b1.vy - dvn * ny
      b2.vx = b2.vx + dvn * nx
      b2.vy = b2.vy + dvn * ny
      let overlap = ball_size - dist
      b1.x = b1.x - overlap / 2.0 * nx
      b1.y = b1.y - overlap / 2.0 * ny
      b2.x = b2.x + overlap / 2.0 * nx
      b2.y = b2.y + overlap / 2.0 * ny
    }
  }
}

fn main_err() -> Unit raise {
  let ctx = @sdl3.Context::new()
  let window = ctx.createWindow("Sprite Bounce Demo", width=800, height=600)
  let renderer = window.createRenderer()
  let timer = ctx.getTimer()
  let texture = @lib.img_LoadTexture(renderer.inner(), "ball.png")
  println("Sprite demo started! Press any key to add 10 balls.")
  let balls : Array[Ball] = []
  let mut seed = 12345
  for i = 0; i < 5; i = i + 1 {
    balls.push(create_ball(seed + i * 100))
  }
  let mut quit = false
  while not(quit) {
    let events = ctx.getEvents()
    for e in events {
      match e.getType() {
        Quit => quit = true
        KEY_DOWN => {
          for i = 0; i < 10; i = i + 1 {
            seed = seed + 1
            balls.push(create_ball(seed * 31))
          }
          println("Added 10 balls. Total: " + balls.length().to_string())
        }
        _ => ()
      }
    }
    for ball in balls {
      update_ball(ball)
    }
    let len = balls.length()
    for i = 0; i < len; i = i + 1 {
      for j = i + 1; j < len; j = j + 1 {
        collide_balls(balls[i], balls[j])
      }
    }
    renderer.setDrawColor(@sdl3.Black)
    renderer.clear()
    for ball in balls {
      let src = @lib.SDL_FRect::{ x: 0.0, y: 0.0, w: 32.0, h: 32.0 }
      let dst = @lib.SDL_FRect::{
        x: ball.x.to_float(),
        y: ball.y.to_float(),
        w: ball_size.to_float(),
        h: ball_size.to_float(),
      }
      let _ = @lib.sdl_RenderTexture(renderer.inner(), texture, src, dst)
    }
    renderer.present()
    timer.delay(16)
  }
  println("Cleaning up...")
  ctx.quit()
}

fn main {
  match (try? main_err()) {
    Ok(_) => ()
    Err(e) => println(e)
  }
}
