function compressRunLength(input) {
  const output = [];
  let currentValue = input[0];
  let currentLength = 1;

  for (let i = 1; i < input.length; i++) {
    if (input[i] === currentValue && currentLength < 255) {
      currentLength++;
    } else {
      output.push(currentValue);
      output.push(currentLength);
      currentValue = input[i];
      currentLength = 1;
    }
  }

  output.push(currentValue);
  output.push(currentLength);

  return new Uint8Array(output);
}

// 使用例
const inputData = new Uint8Array([1, 1, 1, 2, 2, 3, 3, 3, 3, 4, 5, 5]);
const compressedData = compressRunLength(inputData);

console.log("Input data:", inputData);
console.log("Compressed data:", compressedData);
