
const sharp = require('sharp');

// 画像ファイルのパス
const imagePath = process.argv[2];//'path/to/your/image.jpg';

sharp(imagePath)
  .metadata()
  .then(metadata => {
    const width = metadata.width;
    const height = metadata.height;
    console.log(`Image size: ${width}x${height} pixels`);
  })
  .catch(error => {
    console.error('Error:', error);
  });
