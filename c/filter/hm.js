function buildHuffmanTree(frequencyMap) {
  const priorityQueue = Array.from(frequencyMap.entries())
    .map(([value, frequency]) => ({ value, frequency }))
    .sort((a, b) => a.frequency - b.frequency);

  while (priorityQueue.length > 1) {
    const node1 = priorityQueue.shift();
    const node2 = priorityQueue.shift();
    priorityQueue.push({ value: null, frequency: node1.frequency + node2.frequency, left: node1, right: node2 });
    priorityQueue.sort((a, b) => a.frequency - b.frequency);
  }

  return priorityQueue[0];
}

function buildEncodingMap(huffmanTree, prefix = "") {
  if (huffmanTree.value !== null) {
    return new Map([[huffmanTree.value, prefix]]);
  }
  return new Map([
    ...buildEncodingMap(huffmanTree.left, prefix + "0"),
    ...buildEncodingMap(huffmanTree.right, prefix + "1")
  ]);
}

function compressHuffman(input) {
  const frequencyMap = new Map();
  for (const byte of input) {
    frequencyMap.set(byte, (frequencyMap.get(byte) || 0) + 1);
  }

  const huffmanTree = buildHuffmanTree(frequencyMap);
  const encodingMap = buildEncodingMap(huffmanTree);

  const compressedData = Array.from(input)
    .map((byte) => encodingMap.get(byte))
    .join("");

  const paddingLength = (8 - (compressedData.length % 8)) % 8;
  const paddedData = compressedData + "0".repeat(paddingLength);

  const outputBytes = new Uint8Array(paddedData.length / 8);
  for (let i = 0; i < outputBytes.length; i++) {
    outputBytes[i] = parseInt(paddedData.slice(i * 8, (i + 1) * 8), 2);
  }

  return { data: outputBytes, paddingLength, encodingMap };
}


function serializeEncodingMap(encodingMap) {
  const entries = Array.from(encodingMap.entries());
  const numEntries = entries.length;
  const bufferSize = 2+numEntries * 2 + entries.reduce((sum, [_, code]) => sum + Math.ceil(code.length / 8), 0);
  const buffer = new ArrayBuffer(bufferSize);
  const view = new DataView(buffer);

  let offset = 0;
  view.setUint16(offset, numEntries);
  offset += 2;

  for (const [byte, code] of entries) {
    view.setUint8(offset, byte);
    offset++;
    const codeLength = code.length;
    view.setUint8(offset, codeLength);
    offset++;

    for (let i = 0; i < Math.ceil(codeLength / 8); i++) {
      const startBit = i * 8;
      const endBit = Math.min(startBit + 8, codeLength);
      const bitString = code.slice(startBit, endBit).padEnd(8, '0');
      view.setUint8(offset, parseInt(bitString, 2));
      offset++;
    }
  }

  return new Uint8Array(buffer);
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
const h=compressHuffman(data);
const encMapBytes=serializeEncodingMap(h.encodingMap);

//余計に長くなってしまう。
console.log(data,h,encMapBytes);
