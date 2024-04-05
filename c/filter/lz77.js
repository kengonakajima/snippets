function compressLZ77(data) {
  const window = [];
  const compressed = [];

  let i = 0;
  while (i < data.length) {
    let maxLength = 0;
    let maxDistance = 0;

    for (let j = 1; j <= Math.min(i, 255); j++) {
      const distance = j;
      let length = 0;

      while (
        i + length < data.length &&
        data[i + length] === data[i - distance + length] &&
        length < 255
      ) {
        length++;
      }

      if (length > maxLength) {
        maxLength = length;
        maxDistance = distance;
      }
    }

    if (maxLength >= 2) {
      compressed.push(maxDistance, maxLength, data[i + maxLength]);
      i += maxLength + 1;
    } else {
      compressed.push(0, 0, data[i]);
      i++;
    }
  }

  return new Uint8Array(compressed);
}

const data=new Uint8Array([
    7,   1,   3,   1,  27, 1, 239, 1, 204, 1,  16, 1,
  230,   1, 209,   1, 252, 1, 249, 1, 237, 1, 248, 2,
  211,   1, 247,   1,   7, 1, 223, 1, 224, 1,   6, 1,
  222,   1,  23,   1,  17, 1,   1, 1,  15, 1,   5, 2,
    6,   1, 255,   1,   2, 1,   7, 1, 240, 1, 253, 1,
  251,   1,   1,   1,   2, 1, 255, 1,   0, 1,   1, 1,
    0, 255,   0, 219
]);
const c=compressLZ77(data);

//余計に長くなってしまう。
console.log(data,c);
