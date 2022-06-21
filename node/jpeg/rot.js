const fs = require("fs");
const jo = require("jpeg-autorotate");
const options = {
  quality: 100,
  jpegjsMaxResolutionInMP: 200,
};

const path = '42d187fc360210dd53c05c4f9cb0dade.jpg';
jo.rotate(path, options, (error, buffer, orientation, dimensions, quality) => {
  if (error) {
    console.log('An error occurred when rotating the file: ' + error.message)
    return
  }
  console.log(`Orientation was ${orientation}`)
  console.log(`Dimensions after rotation: ${dimensions.width}x${dimensions.height}`)
  console.log(`Quality: ${quality}`)
    // ...Do whatever you need with the resulting buffer...
    console.log("BU:",buffer);
    fs.writeFileSync("1.jpg", buffer);
})
