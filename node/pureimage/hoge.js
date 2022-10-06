
const pimg= require("pureimage");
const fs=require("fs");

// make image
const img1 = pimg.make(100, 100)

// get canvas context
const ctx = img1.getContext('2d');

// fill with red
ctx.fillStyle = 'red';
ctx.fillRect(0,0,100,100);

//write to 'out.png'
pimg.encodePNGToStream(img1, fs.createWriteStream('out.png')).then(() => {
    console.log("wrote out the png file to out.png");
}).catch((e)=>{
    console.log("there was an error writing");
});
