const React = require('react');
const { render, Box, Text, useInput } = require('ink');

if (!process.stdin.isTTY) {
  console.error('エラー: このアプリケーションはTTY環境で実行する必要があります。');
  console.error('ターミナルから直接実行してください。');
  process.exit(1);
}

const SpreadsheetApp = () => {
  const [terminalSize, setTerminalSize] = React.useState({
    columns: process.stdout.columns || 80,
    rows: process.stdout.rows || 24
  });
  
  React.useEffect(() => {
    const handleResize = () => {
      setTerminalSize({
        columns: process.stdout.columns || 80,
        rows: process.stdout.rows || 24
      });
    };
    
    process.stdout.on('resize', handleResize);
    return () => process.stdout.off('resize', handleResize);
  }, []);
  
  const cellWidth = 10;
  const cellHeight = 3;
  const cols = Math.floor(terminalSize.columns / cellWidth);
  const rows = Math.floor(terminalSize.rows / cellHeight);
  const totalCells = cols * rows;
  
  const [cells, setCells] = React.useState({});
  const [cursor, setCursor] = React.useState(0);
  const [prevCursor, setPrevCursor] = React.useState(-1);
  const [inputBuffer, setInputBuffer] = React.useState('');
  const processingRef = React.useRef(false);

  useInput((input, key) => {
    // 処理中なら早期リターン
    if (processingRef.current) return;
    
    const lastEditableCell = totalCells - 2;
    
    if (input === 'q') {
      process.exit(0);
    } else if (key.tab) {
      processingRef.current = true;
      setPrevCursor(cursor);
      setCursor((cursor + 1) % (lastEditableCell + 1));
      setInputBuffer('');
      setTimeout(() => { processingRef.current = false; }, 50);
    } else if (key.upArrow) {
      const newCursor = cursor - cols;
      if (newCursor >= 0 && newCursor !== cursor) {
        processingRef.current = true;
        setPrevCursor(cursor);
        setCursor(newCursor);
        setInputBuffer(cells[newCursor] || '');
        setTimeout(() => { processingRef.current = false; }, 50);
      }
    } else if (key.downArrow) {
      const newCursor = cursor + cols;
      if (newCursor <= lastEditableCell && newCursor !== cursor) {
        processingRef.current = true;
        setPrevCursor(cursor);
        setCursor(newCursor);
        setInputBuffer(cells[newCursor] || '');
        setTimeout(() => { processingRef.current = false; }, 50);
      }
    } else if (key.leftArrow) {
      const newCursor = cursor - 1;
      if (newCursor >= 0 && cursor % cols !== 0 && newCursor !== cursor) {
        processingRef.current = true;
        setPrevCursor(cursor);
        setCursor(newCursor);
        setInputBuffer(cells[newCursor] || '');
        setTimeout(() => { processingRef.current = false; }, 50);
      }
    } else if (key.rightArrow) {
      const newCursor = cursor + 1;
      if (newCursor <= lastEditableCell && newCursor % cols !== 0 && newCursor !== cursor) {
        processingRef.current = true;
        setPrevCursor(cursor);
        setCursor(newCursor);
        setInputBuffer(cells[newCursor] || '');
        setTimeout(() => { processingRef.current = false; }, 50);
      }
    } else if (key.backspace || key.delete) {
      if (cursor <= lastEditableCell) {
        const newBuffer = inputBuffer.slice(0, -1);
        setInputBuffer(newBuffer);
        setCells(prev => ({...prev, [cursor]: newBuffer}));
      }
    } else if (input >= '0' && input <= '9' && cursor <= lastEditableCell) {
      const newBuffer = inputBuffer + input;
      setInputBuffer(newBuffer);
      setCells(prev => ({...prev, [cursor]: newBuffer}));
    } else if (input === '.' && cursor <= lastEditableCell && !inputBuffer.includes('.')) {
      const newBuffer = inputBuffer + input;
      setInputBuffer(newBuffer);
      setCells(prev => ({...prev, [cursor]: newBuffer}));
    }
  });

  const calculateSum = React.useMemo(() => {
    let sum = 0;
    for (let i = 0; i < totalCells - 1; i++) {
      const value = parseFloat(cells[i] || '0');
      if (!isNaN(value)) {
        sum += value;
      }
    }
    return sum.toFixed(2);
  }, [cells, totalCells]);

  const Cell = React.memo(({ index, isLastCell, isCursor, value }) => {
    const displayValue = value.toString().padEnd(8, ' ');
    
    return React.createElement(Box, {
      borderStyle: "single",
      width: cellWidth,
      height: cellHeight,
      borderColor: isCursor ? 'green' : 'white'
    },
      React.createElement(Text, {
        color: isCursor ? 'green' : 'white'
      }, displayValue)
    );
  }, (prevProps, nextProps) => {
    return prevProps.isCursor === nextProps.isCursor && 
           prevProps.value === nextProps.value &&
           prevProps.index === nextProps.index;
  });

  const renderCell = (index) => {
    const isLastCell = index === totalCells - 1;
    const isCursor = index === cursor;
    const value = isLastCell ? calculateSum : (cells[index] || '');
    
    return React.createElement(Cell, {
      key: index,
      index,
      isLastCell,
      isCursor,
      value
    });
  };

  const Row = React.memo(({ rowIndex, cols, totalCells }) => {
    const rowCells = [];
    for (let col = 0; col < cols; col++) {
      const index = rowIndex * cols + col;
      if (index < totalCells) {
        rowCells.push(renderCell(index));
      }
    }
    return React.createElement(Box, null, rowCells);
  }, (prevProps, nextProps) => {
    return prevProps.cols === nextProps.cols && 
           prevProps.totalCells === nextProps.totalCells;
  });

  const renderGrid = () => {
    const gridRows = [];
    
    for (let row = 0; row < rows; row++) {
      gridRows.push(
        React.createElement(Row, { 
          key: row,
          rowIndex: row,
          cols,
          totalCells
        })
      );
    }
    
    return React.createElement(Box, { flexDirection: "column" }, gridRows);
  };

  return renderGrid();
};

render(React.createElement(SpreadsheetApp));