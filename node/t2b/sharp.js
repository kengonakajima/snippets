const fs = require('fs');
const sharp = require('sharp');

// Specify your image dimensions and the path to your binary file
const width = 1600; // Replace with your image's width
const height = 900; // Replace with your image's height
const inputPath = 'unity.raw';
const outputPath = 'output.png';

// Read the binary data
const data = fs.readFileSync(inputPath);

// Use sharp to create and save the PNG
sharp(data, {
  raw: {
    width: width,
    height: height,
    channels: 3
  }
})
.toFile(outputPath)
.then(() => console.log('Image saved successfully.'))
.catch(err => console.error(err));
