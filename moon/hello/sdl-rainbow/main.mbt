fn hsv_to_rgb(h : Double, s : Double, v : Double) -> (Int, Int, Int) {
  let c = v * s
  let x = c * (1.0 - ((h / 60.0) % 2.0 - 1.0).abs())
  let m = v - c
  let (r1, g1, b1) = if h < 60.0 {
    (c, x, 0.0)
  } else if h < 120.0 {
    (x, c, 0.0)
  } else if h < 180.0 {
    (0.0, c, x)
  } else if h < 240.0 {
    (0.0, x, c)
  } else if h < 300.0 {
    (x, 0.0, c)
  } else {
    (c, 0.0, x)
  }
  let r = ((r1 + m) * 255.0).to_int()
  let g = ((g1 + m) * 255.0).to_int()
  let b = ((b1 + m) * 255.0).to_int()
  (r, g, b)
}

fn main_err() -> Unit raise {
  let ctx = @sdl3.Context::new()
  let window = ctx.createWindow("Rainbow Demo", width=800, height=600)
  let renderer = window.createRenderer()
  let timer = ctx.getTimer()
  println("Rainbow demo started!")
  let mut quit = false
  let mut hue = 0.0
  while not(quit) {
    let events = ctx.getEvents()
    if events.iter().any(fn(e) { e.getType() is Quit }) {
      quit = true
    }
    let (r, g, b) = hsv_to_rgb(hue, 1.0, 1.0)
    renderer.setDrawColor(@sdl3.RGB(r, g, b))
    renderer.clear()
    renderer.present()
    hue = hue + 1.0
    if hue >= 360.0 {
      hue = 0.0
    }
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
