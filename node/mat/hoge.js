function findRectangles(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  const visited = Array(rows).fill(null).map(() => Array(cols).fill(false));
  const rectangles = [];

  for (let i = 0; i < rows; i++) {
    for (let j = 0; j < cols; j++) {
      if (matrix[i][j] === 1 && !visited[i][j]) {
        const topLeft = [i, j];
        const bottomRight = findBottomRight(matrix, i, j, topLeft, visited);
        rectangles.push([topLeft, bottomRight]);
      }
    }
  }

  return rectangles;
}

function findBottomRight(matrix, i, j, topLeft, visited) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  let bottomRight = [i, j];

  // 右方向に探索
  while (j + 1 < cols && matrix[i][j + 1] === 1) {
    j++;
    bottomRight = [i, j];
  }

  // 下方向に探索
  while (i + 1 < rows && matrix[i + 1][j] === 1) {
    i++;
    bottomRight = [i, j];
  }

  // 見つかった四角形の領域を訪問済みとしてマーク
  for (let x = topLeft[0]; x <= bottomRight[0]; x++) {
    for (let y = topLeft[1]; y <= bottomRight[1]; y++) {
      visited[x][y] = true;
    }
  }

  return bottomRight;
}

// テストデータの生成
function generateTestData(rows, cols, density) {
  const matrix = Array(rows).fill(null).map(() => Array(cols).fill(0));

  for (let i = 0; i < rows; i++) {
    for (let j = 0; j < cols; j++) {
      if (Math.random() < density) {
        matrix[i][j] = 1;
      }
    }
  }

  return matrix;
}

function displayMatrix(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;

  for (let i = 0; i < rows; i++) {
    let row = "";
    for (let j = 0; j < cols; j++) {
      row += matrix[i][j] + " ";
    }
    console.log(row);
  }
  console.log("");
}


// rectanglesの情報を基に、空の行列に1を設定する関数
function setRectanglesInMatrix(rectangles, rows, cols) {
  const matrix = Array(rows).fill(null).map(() => Array(cols).fill(0));

  rectangles.forEach(rect => {
    const [topLeft, bottomRight] = rect;
    const [startRow, startCol] = topLeft;
    const [endRow, endCol] = bottomRight;

    for (let i = startRow; i <= endRow; i++) {
      for (let j = startCol; j <= endCol; j++) {
        matrix[i][j] = 1;
      }
    }
  });

  return matrix;
}

// 2つの行列を比較する関数
function compareMatrices(matrix1, matrix2) {
  const rows1 = matrix1.length;
  const cols1 = matrix1[0].length;
  const rows2 = matrix2.length;
  const cols2 = matrix2[0].length;

  if (rows1 !== rows2 || cols1 !== cols2) {
    return false;
  }

  for (let i = 0; i < rows1; i++) {
    for (let j = 0; j < cols1; j++) {
      if (matrix1[i][j] !== matrix2[i][j]) {
        return false;
      }
    }
  }

  return true;
}


// テストの実行
function runTest(testData) {
//  console.log("Input Matrix:");
//  console.log(testData);

  const rectangles = findRectangles(testData);

  console.log("Detected Rectangles:");
  rectangles.forEach(rect => {
    console.log(`Top-Left: (${rect[0][0]}, ${rect[0][1]}), Bottom-Right: (${rect[1][0]}, ${rect[1][1]})`);
  });
}



// テストデータの生成とテストの実行
const testData = generateTestData(10, 10, 0.2);
displayMatrix(testData);

// 検出された矩形を空の行列に適用
const rows = testData.length;
const cols = testData[0].length;
const rectangles = findRectangles(testData);
const rectangleMatrix = setRectanglesInMatrix(rectangles, rows, cols);

displayMatrix(rectangleMatrix);
const comp=compareMatrices(testData,rectangleMatrix);
console.log("compare:",comp);


runTest(testData);
