#!/usr/bin/env node
const React = require('react');
const { useState, useEffect } = React;
const { render, Box, Text, useInput, useApp } = require('ink');

const CANVAS_WIDTH = 80;
const CANVAS_HEIGHT = 24;

const WireframeTool = () => {
    const { exit } = useApp();
    
    const [cursor, setCursor] = useState({ x: 0, y: 0 });
    const [canvas, setCanvas] = useState(() => {
        return Array(CANVAS_HEIGHT).fill(null).map(() => 
            Array(CANVAS_WIDTH).fill(' ')
        );
    });
    const [mode, setMode] = useState('draw'); // draw, select, button, icon, interact
    const [selectedElement, setSelectedElement] = useState(null);
    const [elements, setElements] = useState([]);
    const [drawing, setDrawing] = useState(false);
    const [interactions, setInteractions] = useState({});
    const [simulationMode, setSimulationMode] = useState(false);
    const [message, setMessage] = useState('');

    useInput((input, key) => {
        if (input === 'q') {
            exit();
        }

        if (key.escape) {
            setMode('draw');
            setSelectedElement(null);
            setSimulationMode(false);
        }

        // モード切り替え
        if (!simulationMode) {
            if (input === 'd') setMode('draw');
            if (input === 'b') setMode('button');
            if (input === 'i') setMode('icon');
            if (input === 's') setMode('select');
            if (input === 'n') setMode('interact');
            if (input === 'p') setSimulationMode(true);
        }

        // カーソル移動
        if (key.upArrow && cursor.y > 0) {
            setCursor(prev => ({ ...prev, y: prev.y - 1 }));
        }
        if (key.downArrow && cursor.y < CANVAS_HEIGHT - 1) {
            setCursor(prev => ({ ...prev, y: prev.y + 1 }));
        }
        if (key.leftArrow && cursor.x > 0) {
            setCursor(prev => ({ ...prev, x: prev.x - 1 }));
        }
        if (key.rightArrow && cursor.x < CANVAS_WIDTH - 1) {
            setCursor(prev => ({ ...prev, x: prev.x + 1 }));
        }

        // 描画モード
        if (mode === 'draw' && !simulationMode) {
            if (input === ' ') {
                setDrawing(prev => !prev);
            }
            if (drawing || input === '*') {
                setCanvas(prev => {
                    const newCanvas = prev.map(row => [...row]);
                    newCanvas[cursor.y][cursor.x] = '*';
                    return newCanvas;
                });
            }
            if (input === 'c') {
                // クリア
                setCanvas(Array(CANVAS_HEIGHT).fill(null).map(() => 
                    Array(CANVAS_WIDTH).fill(' ')
                ));
                setElements([]);
                setInteractions({});
            }
        }

        // ボタン配置モード
        if (mode === 'button' && key.return && !simulationMode) {
            const button = {
                type: 'button',
                x: cursor.x,
                y: cursor.y,
                width: 10,
                height: 3,
                text: 'Button',
                id: Date.now()
            };
            setElements(prev => [...prev, button]);
            drawElement(button);
        }

        // アイコン配置モード
        if (mode === 'icon' && key.return && !simulationMode) {
            const icon = {
                type: 'icon',
                x: cursor.x,
                y: cursor.y,
                symbol: '◆',
                id: Date.now()
            };
            setElements(prev => [...prev, icon]);
            drawElement(icon);
        }

        // 選択モード
        if (mode === 'select' && !simulationMode) {
            const element = findElementAtCursor();
            if (key.return && element) {
                setSelectedElement(element);
            }
        }

        // インタラクション定義モード
        if (mode === 'interact' && selectedElement && !simulationMode) {
            if (input === '1') {
                // クリック時のメッセージ
                const action = prompt('Enter message for click action:') || 'Clicked!';
                setInteractions(prev => ({
                    ...prev,
                    [selectedElement.id]: { type: 'message', action }
                }));
                setMessage(`Interaction added to ${selectedElement.type}`);
            }
            if (input === '2') {
                // ページ遷移
                const target = prompt('Enter target page:') || 'page2';
                setInteractions(prev => ({
                    ...prev,
                    [selectedElement.id]: { type: 'navigate', target }
                }));
                setMessage(`Navigation added to ${selectedElement.type}`);
            }
        }

        // シミュレーションモード
        if (simulationMode) {
            const element = findElementAtCursor();
            if (key.return && element && interactions[element.id]) {
                const interaction = interactions[element.id];
                if (interaction.type === 'message') {
                    setMessage(`Action: ${interaction.action}`);
                } else if (interaction.type === 'navigate') {
                    setMessage(`Navigate to: ${interaction.target}`);
                }
            }
        }
    });

    const findElementAtCursor = () => {
        return elements.find(el => {
            if (el.type === 'button') {
                return cursor.x >= el.x && cursor.x < el.x + el.width &&
                       cursor.y >= el.y && cursor.y < el.y + el.height;
            }
            if (el.type === 'icon') {
                return cursor.x === el.x && cursor.y === el.y;
            }
            return false;
        });
    };

    const drawElement = (element) => {
        setCanvas(prev => {
            const newCanvas = prev.map(row => [...row]);
            
            if (element.type === 'button') {
                // ボタンの枠を描画
                for (let i = 0; i < element.width; i++) {
                    if (element.y >= 0 && element.y < CANVAS_HEIGHT) {
                        if (element.x + i < CANVAS_WIDTH) {
                            newCanvas[element.y][element.x + i] = '─';
                        }
                    }
                    if (element.y + element.height - 1 >= 0 && element.y + element.height - 1 < CANVAS_HEIGHT) {
                        if (element.x + i < CANVAS_WIDTH) {
                            newCanvas[element.y + element.height - 1][element.x + i] = '─';
                        }
                    }
                }
                for (let i = 0; i < element.height; i++) {
                    if (element.y + i >= 0 && element.y + i < CANVAS_HEIGHT) {
                        if (element.x < CANVAS_WIDTH) {
                            newCanvas[element.y + i][element.x] = '│';
                        }
                        if (element.x + element.width - 1 < CANVAS_WIDTH) {
                            newCanvas[element.y + i][element.x + element.width - 1] = '│';
                        }
                    }
                }
                // 角
                if (element.y >= 0 && element.y < CANVAS_HEIGHT && element.x < CANVAS_WIDTH) {
                    newCanvas[element.y][element.x] = '┌';
                }
                if (element.y >= 0 && element.y < CANVAS_HEIGHT && element.x + element.width - 1 < CANVAS_WIDTH) {
                    newCanvas[element.y][element.x + element.width - 1] = '┐';
                }
                if (element.y + element.height - 1 >= 0 && element.y + element.height - 1 < CANVAS_HEIGHT && element.x < CANVAS_WIDTH) {
                    newCanvas[element.y + element.height - 1][element.x] = '└';
                }
                if (element.y + element.height - 1 >= 0 && element.y + element.height - 1 < CANVAS_HEIGHT && element.x + element.width - 1 < CANVAS_WIDTH) {
                    newCanvas[element.y + element.height - 1][element.x + element.width - 1] = '┘';
                }
                // テキスト
                const textY = element.y + Math.floor(element.height / 2);
                const textX = element.x + Math.floor((element.width - element.text.length) / 2);
                for (let i = 0; i < element.text.length; i++) {
                    if (textY >= 0 && textY < CANVAS_HEIGHT && textX + i < CANVAS_WIDTH) {
                        newCanvas[textY][textX + i] = element.text[i];
                    }
                }
            } else if (element.type === 'icon') {
                if (element.y >= 0 && element.y < CANVAS_HEIGHT && element.x < CANVAS_WIDTH) {
                    newCanvas[element.y][element.x] = element.symbol;
                }
            }
            
            return newCanvas;
        });
    };

    // 要素の再描画
    useEffect(() => {
        // キャンバスをクリアして全要素を再描画
        const newCanvas = Array(CANVAS_HEIGHT).fill(null).map(() => 
            Array(CANVAS_WIDTH).fill(' ')
        );
        setCanvas(newCanvas);
        
        // 全要素を再描画
        elements.forEach(element => {
            drawElement(element);
        });
    }, [elements]);

    // メッセージを3秒後にクリア
    useEffect(() => {
        if (message) {
            const timer = setTimeout(() => {
                setMessage('');
            }, 3000);
            return () => clearTimeout(timer);
        }
    }, [message]);

    return (
        <Box flexDirection="column">
            <Box marginBottom={1}>
                <Text>
                    Wireframe Tool - Mode: {simulationMode ? 'SIMULATION' : mode} | 
                    {selectedElement && <Text> Selected: {selectedElement.type} (ID: {selectedElement.id})</Text>}
                </Text>
            </Box>
            <Box marginBottom={1}>
                <Text>
                    Controls: Arrows (move), Space (draw), D/B/I/S/N (modes), P (simulate), C (clear), Q (quit)
                </Text>
                {mode === 'interact' && selectedElement && (
                    <Text> | Press 1 (message) or 2 (navigate) to add interaction</Text>
                )}
            </Box>
            
            <Box borderStyle="single" width={CANVAS_WIDTH + 2} height={CANVAS_HEIGHT + 2}>
                <Box flexDirection="column">
                    {canvas.map((row, y) => (
                        <Box key={y}>
                            <Text>
                                {row.map((cell, x) => {
                                    const isSelected = selectedElement && 
                                        ((selectedElement.type === 'button' && 
                                          x >= selectedElement.x && x < selectedElement.x + selectedElement.width &&
                                          y >= selectedElement.y && y < selectedElement.y + selectedElement.height) ||
                                         (selectedElement.type === 'icon' && 
                                          x === selectedElement.x && y === selectedElement.y));
                                    
                                    if (x === cursor.x && y === cursor.y) {
                                        return <Text key={x} inverse>{cell}</Text>;
                                    } else if (isSelected) {
                                        return <Text key={x} color="green">{cell}</Text>;
                                    }
                                    return cell;
                                }).join('')}
                            </Text>
                        </Box>
                    ))}
                </Box>
            </Box>
            
            <Box marginTop={1}>
                <Text>Cursor: ({cursor.x}, {cursor.y}) | Drawing: {drawing ? 'ON' : 'OFF'}</Text>
                {message && <Text color="yellow"> | {message}</Text>}
            </Box>
        </Box>
    );
};

// 簡易的なプロンプト実装（実際のReact Inkでは別の方法が必要）
const prompt = (message) => {
    // 実際の実装では、別のUIコンポーネントや状態管理が必要
    return 'example';
};

render(<WireframeTool />);