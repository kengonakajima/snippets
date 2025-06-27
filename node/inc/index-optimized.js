const React = require('react');
const { render, Box, Text, useInput, Static } = require('ink');

if (!process.stdin.isTTY) {
  console.error('エラー: このアプリケーションはTTY環境で実行する必要があります。');
  console.error('ターミナルから直接実行してください。');
  process.exit(1);
}

const SpreadsheetApp = () => {
  const cellWidth = 10;
  const cellHeight = 3;
  const cols = Math.floor((process.stdout.columns || 80) / cellWidth);
  const rows = Math.floor((process.stdout.rows || 24) / cellHeight);
  const totalCells = cols * rows;
  
  const [cells] = React.useState({});
  const [cursor, setCursor] = React.useState(0);
  const [inputBuffer, setInputBuffer] = React.useState('');
  
  // 静的なグリッド構造を事前に生成
  const gridStructure = React.useMemo(() => {
    const structure = [];
    for (let row = 0; row < rows; row++) {
      const rowCells = [];
      for (let col = 0; col < cols; col++) {
        const index = row * cols + col;
        if (index < totalCells) {
          rowCells.push(index);
        }
      }
      structure.push(rowCells);
    }
    return structure;
  }, [rows, cols, totalCells]);

  useInput((input, key) => {
    const lastEditableCell = totalCells - 2;
    
    if (input === 'q') {
      process.exit(0);
    } else if (key.tab) {
      setCursor((cursor + 1) % (lastEditableCell + 1));
      setInputBuffer('');
    } else if (key.upArrow) {
      const newCursor = cursor - cols;
      if (newCursor >= 0) {
        setCursor(newCursor);
        setInputBuffer(cells[newCursor] || '');
      }
    } else if (key.downArrow) {
      const newCursor = cursor + cols;
      if (newCursor <= lastEditableCell) {
        setCursor(newCursor);
        setInputBuffer(cells[newCursor] || '');
      }
    } else if (key.leftArrow) {
      if (cursor > 0 && cursor % cols !== 0) {
        setCursor(cursor - 1);
        setInputBuffer(cells[cursor - 1] || '');
      }
    } else if (key.rightArrow) {
      if (cursor < lastEditableCell && (cursor + 1) % cols !== 0) {
        setCursor(cursor + 1);
        setInputBuffer(cells[cursor + 1] || '');
      }
    } else if (key.backspace || key.delete) {
      if (cursor <= lastEditableCell) {
        setInputBuffer(inputBuffer.slice(0, -1));
        cells[cursor] = inputBuffer.slice(0, -1);
      }
    } else if (input >= '0' && input <= '9' && cursor <= lastEditableCell) {
      const newBuffer = inputBuffer + input;
      setInputBuffer(newBuffer);
      cells[cursor] = newBuffer;
    } else if (input === '.' && cursor <= lastEditableCell && !inputBuffer.includes('.')) {
      const newBuffer = inputBuffer + input;
      setInputBuffer(newBuffer);
      cells[cursor] = newBuffer;
    }
  });

  const calculateSum = () => {
    let sum = 0;
    for (let i = 0; i < totalCells - 1; i++) {
      const value = parseFloat(cells[i] || '0');
      if (!isNaN(value)) {
        sum += value;
      }
    }
    return sum.toFixed(2);
  };

  return React.createElement(Box, { flexDirection: "column" },
    gridStructure.map((rowIndices, rowIndex) => 
      React.createElement(Box, { key: rowIndex },
        rowIndices.map(index => {
          const isLastCell = index === totalCells - 1;
          const isCursor = index === cursor;
          const value = isLastCell ? calculateSum() : (cells[index] || '');
          const displayValue = value.toString().padEnd(8, ' ');
          
          return React.createElement(Box, {
            key: index,
            borderStyle: "single",
            width: cellWidth,
            height: cellHeight,
            borderColor: isCursor ? 'green' : 'white'
          },
            React.createElement(Text, {
              color: isCursor ? 'green' : 'white'
            }, displayValue)
          );
        })
      )
    )
  );
};

render(React.createElement(SpreadsheetApp));