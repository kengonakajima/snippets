function getRandomElements(array, count) {
  if (count > array.length) {
    throw new Error("取り出す要素数が配列の長さを超えています");
  }

  const result = [];
  const tempArray = [...array];

  for (let i = 0; i < count; i++) {
    const randomIndex = Math.floor(Math.random() * tempArray.length);
    result.push(tempArray[randomIndex]);
    tempArray.splice(randomIndex, 1);
  }

  return result;
}

// 使用例
const array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
const randomElements = getRandomElements(array, 4);
console.log(randomElements);

