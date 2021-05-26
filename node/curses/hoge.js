var Frame = require("node-ncurses");

let color = {
    byNum: (mess, fgNum, bgNum) => {
        mess = mess || '';
        fgNum = fgNum === undefined ? 31 : fgNum;
        bgNum = bgNum === undefined ? 47 : bgNum;
        return '\u001b[' + fgNum + 'm' + '\u001b[' + bgNum + 'm' + mess + '\u001b[39m\u001b[49m';
    },
    black: (mess, fgNum) => color.byNum(mess, 30, fgNum),
    red: (mess, fgNum) => color.byNum(mess, 31, fgNum),
    green: (mess, fgNum) => color.byNum(mess, 32, fgNum),
    yellow: (mess, fgNum) => color.byNum(mess, 33, fgNum),
    blue: (mess, fgNum) => color.byNum(mess, 34, fgNum),
    magenta: (mess, fgNum) => color.byNum(mess, 35, fgNum),
    cyan: (mess, fgNum) => color.byNum(mess, 36, fgNum),
    white: (mess, fgNum) => color.byNum(mess, 37, fgNum)
};


// https://en.wikipedia.org/wiki/ANSI_escape_code

const BG_BLUE = '\u001b[' + 44 + 'm';
const BG_RED = '\u001b[' + 41 + 'm';
var frame = new Frame(1,1,80,24,BG_BLUE); // blue

frame.open();

var subframe = new Frame(10,10,10,10,BG_RED, frame); // magenta

subframe.open();
subframe.gotoxy(5,5);
subframe.putmsg("1");

frame.close();

setInterval( function() {
    frame.refresh();
},100);


