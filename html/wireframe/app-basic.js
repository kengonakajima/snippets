#!/usr/bin/env node
import React, { useState, useEffect } from 'react';
import { render, Box, Text, useInput, useApp } from 'ink';

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
    const [mode, setMode] = useState('draw'); // draw, select, button, icon
    const [selectedElement, setSelectedElement] = useState(null);
    const [elements, setElements] = useState([]);
    const [drawing, setDrawing] = useState(false);

    useInput((input, key) => {
        if (input === 'q') {
            exit();
        }

        if (key.escape) {
            setMode('draw');
            setSelectedElement(null);
        }

        // モード切り替え
        if (input === 'd') setMode('draw');
        if (input === 'b') setMode('button');
        if (input === 'i') setMode('icon');
        if (input === 's') setMode('select');

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
        if (mode === 'draw') {
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
            }
        }

        // ボタン配置モード
        if (mode === 'button' && key.return) {
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
        if (mode === 'icon' && key.return) {
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
    });

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

    return (
        <Box flexDirection="column">
            <Box marginBottom={1}>
                <Text>
                    Wireframe Tool - Mode: {mode} | 
                    Controls: Arrow keys (move), Space (toggle draw), 
                    D (draw mode), B (button mode), I (icon mode), S (select mode), 
                    C (clear), Q (quit)
                </Text>
            </Box>
            
            <Box borderStyle="single" width={CANVAS_WIDTH + 2} height={CANVAS_HEIGHT + 2}>
                <Box flexDirection="column">
                    {canvas.map((row, y) => (
                        <Box key={y}>
                            <Text>
                                {row.map((cell, x) => {
                                    if (x === cursor.x && y === cursor.y) {
                                        return <Text key={x} inverse>{cell}</Text>;
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
            </Box>
        </Box>
    );
};

render(<WireframeTool />);