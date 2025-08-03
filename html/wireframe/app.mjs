#!/usr/bin/env node
import React from 'react';
import { render, Box, Text, useInput, useApp } from 'ink';

const { useState, useEffect } = React;

const CANVAS_WIDTH = 60;
const CANVAS_HEIGHT = 20;

const WireframeTool = () => {
    const { exit } = useApp();
    
    const [cursor, setCursor] = useState({ x: 0, y: 0 });
    const [canvas, setCanvas] = useState(() => {
        return Array(CANVAS_HEIGHT).fill(null).map(() => 
            Array(CANVAS_WIDTH).fill(' ')
        );
    });
    const [mode, setMode] = useState('draw'); 
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
            if (input === 'p') {
                setSimulationMode(true);
                setMessage('Simulation mode ON');
            }
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
            if ((drawing || input === '*') && cursor.y < CANVAS_HEIGHT && cursor.x < CANVAS_WIDTH) {
                setCanvas(prev => {
                    const newCanvas = prev.map(row => [...row]);
                    newCanvas[cursor.y][cursor.x] = '*';
                    return newCanvas;
                });
            }
            if (input === 'c') {
                setCanvas(Array(CANVAS_HEIGHT).fill(null).map(() => 
                    Array(CANVAS_WIDTH).fill(' ')
                ));
                setElements([]);
                setInteractions({});
                setMessage('Canvas cleared');
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
            setMessage('Button placed');
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
            setMessage('Icon placed');
        }

        // 選択モード
        if (mode === 'select' && !simulationMode) {
            const element = findElementAtCursor();
            if (key.return && element) {
                setSelectedElement(element);
                setMessage(`Selected: ${element.type}`);
            }
        }

        // インタラクション定義モード
        if (mode === 'interact' && selectedElement && !simulationMode) {
            if (input === '1') {
                setInteractions(prev => ({
                    ...prev,
                    [selectedElement.id]: { type: 'message', action: 'Clicked!' }
                }));
                setMessage(`Click action added`);
            }
            if (input === '2') {
                setInteractions(prev => ({
                    ...prev,
                    [selectedElement.id]: { type: 'navigate', target: 'page2' }
                }));
                setMessage(`Navigation added`);
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

    // キャンバスの再描画
    const redrawCanvas = () => {
        const newCanvas = Array(CANVAS_HEIGHT).fill(null).map(() => 
            Array(CANVAS_WIDTH).fill(' ')
        );
        
        // 要素を描画
        elements.forEach(element => {
            if (element.type === 'button') {
                for (let i = 0; i < element.width && element.x + i < CANVAS_WIDTH; i++) {
                    if (element.y < CANVAS_HEIGHT) newCanvas[element.y][element.x + i] = '─';
                    if (element.y + element.height - 1 < CANVAS_HEIGHT) newCanvas[element.y + element.height - 1][element.x + i] = '─';
                }
                for (let i = 0; i < element.height && element.y + i < CANVAS_HEIGHT; i++) {
                    if (element.x < CANVAS_WIDTH) newCanvas[element.y + i][element.x] = '│';
                    if (element.x + element.width - 1 < CANVAS_WIDTH) newCanvas[element.y + i][element.x + element.width - 1] = '│';
                }
                if (element.y < CANVAS_HEIGHT && element.x < CANVAS_WIDTH) newCanvas[element.y][element.x] = '┌';
                if (element.y < CANVAS_HEIGHT && element.x + element.width - 1 < CANVAS_WIDTH) newCanvas[element.y][element.x + element.width - 1] = '┐';
                if (element.y + element.height - 1 < CANVAS_HEIGHT && element.x < CANVAS_WIDTH) newCanvas[element.y + element.height - 1][element.x] = '└';
                if (element.y + element.height - 1 < CANVAS_HEIGHT && element.x + element.width - 1 < CANVAS_WIDTH) newCanvas[element.y + element.height - 1][element.x + element.width - 1] = '┘';
                
                const textY = element.y + Math.floor(element.height / 2);
                const textX = element.x + Math.floor((element.width - element.text.length) / 2);
                for (let i = 0; i < element.text.length && textX + i < CANVAS_WIDTH; i++) {
                    if (textY < CANVAS_HEIGHT && textX + i >= 0) {
                        newCanvas[textY][textX + i] = element.text[i];
                    }
                }
            } else if (element.type === 'icon') {
                if (element.y < CANVAS_HEIGHT && element.x < CANVAS_WIDTH) {
                    newCanvas[element.y][element.x] = element.symbol;
                }
            }
        });
        
        return newCanvas;
    };

    useEffect(() => {
        setCanvas(redrawCanvas());
    }, [elements]);

    // 描画
    const canvasRows = [];
    for (let y = 0; y < CANVAS_HEIGHT; y++) {
        let rowText = '';
        for (let x = 0; x < CANVAS_WIDTH; x++) {
            const isSelected = selectedElement && 
                ((selectedElement.type === 'button' && 
                  x >= selectedElement.x && x < selectedElement.x + selectedElement.width &&
                  y >= selectedElement.y && y < selectedElement.y + selectedElement.height) ||
                 (selectedElement.type === 'icon' && 
                  x === selectedElement.x && y === selectedElement.y));
            
            if (x === cursor.x && y === cursor.y) {
                rowText += '@'; // カーソル位置
            } else {
                rowText += canvas[y][x];
            }
        }
        canvasRows.push(
            React.createElement(Box, { key: y },
                React.createElement(Text, null, rowText)
            )
        );
    }

    return React.createElement(Box, { flexDirection: "column" },
        React.createElement(Box, { marginBottom: 1 },
            React.createElement(Text, null,
                "Wireframe Tool - Mode: " + (simulationMode ? 'SIMULATION' : mode) + 
                (selectedElement ? " | Selected: " + selectedElement.type : "")
            )
        ),
        React.createElement(Box, { marginBottom: 1 },
            React.createElement(Text, null,
                "Controls: Arrows(move) Space(draw) D/B/I/S/N(modes) P(simulate) C(clear) Q(quit)"
            )
        ),
        
        React.createElement(Box, { borderStyle: "single", width: CANVAS_WIDTH + 2, height: CANVAS_HEIGHT + 2 },
            React.createElement(Box, { flexDirection: "column" }, canvasRows)
        ),
        
        React.createElement(Box, { marginTop: 1 },
            React.createElement(Text, null,
                "Cursor: (" + cursor.x + ", " + cursor.y + ") | Drawing: " + (drawing ? 'ON' : 'OFF') +
                (message ? " | " + message : "")
            )
        )
    );
};

render(React.createElement(WireframeTool));