// Create context
var width   = 64
var height  = 64
var gl = require('gl')(width, height, { preserveDrawingBuffer: true })

//Clear screen to red
gl.clearColor(1, 0, 0, 1)
gl.clear(gl.COLOR_BUFFER_BIT)

//Write output as a PPM formatted image
var pixels = new Uint8Array(width * height * 4)
gl.readPixels(0, 0, width, height, gl.RGBA, gl.UNSIGNED_BYTE, pixels)
process.stdout.write(['P3\n# gl.ppm\n', width, " ", height, '\n255\n'].join(''))

for(var i = 0; i < pixels.length; i += 4) {
  for(var j = 0; j < 3; ++j) {
    process.stdout.write(pixels[i + j] + ' ')
  }
}
