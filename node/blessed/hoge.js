var blessed = require('blessed');
const screen = blessed.screen({
    smartCSR: true,
    title: 'example-tui' // TUIのタイトル
});


screen.title = 'my window title';

// Create a box perfectly centered horizontally and vertically.
var box = blessed.box({
  top: 'center',
  left: '20%',
  width: '50%',
  height: '50%',
  content: 'Hello {bold}world{/bold}!',
  tags: true,
  border: {
    type: 'line'
  },
  style: {
    fg: 'white',
    bg: '#008000',
    border: {
      fg: '#f0f0f0'
    },
    hover: {
      bg: 'green'
    }
  }
});

screen.append(box);

var dbgbox = blessed.box({
  top: '90%',
  left: 'left',
  width: '100%',
  height: '10%',
  content: 'debug status line',
  tags: true,
  border: {
    type: 'line'
  },
  style: {
    fg: 'white',
    bg: 'blue',
    border: {
      fg: '#f0f0f0'
    },
    hover: {
      bg: 'green'
    }
  }
    
});

screen.append(dbgbox);


box.on('click', function(data) {
  box.setContent('{center}Some different {red-fg}content{/red-fg}.{/center}');
  screen.render();
});

// If box is focused, handle `enter`/`return` and give us some more content.
box.key('enter', function(ch, key) {
    dbgbox.setLine(0,"box:"+box.height+","+box.width + "," + Math.floor(box.height/2));
    for(var y=0;y<box.height-2;y++) { // include frame
        var line="";
        for(var x=0;x<box.width-2;x++) { // include frame
            var ch=' ';
            if(Math.random()<0.1) ch='w';
            if(x==Math.floor((box.width-2)/2) && y==Math.floor((box.height-2)/2)) {
                ch='@';
            }
            line+=ch;
        }
        if(y==1)  dbgbox.setLine(1,line);
        box.setLine(y,line);
//  box.setContent('{right}Even different {black-fg}content{/black-fg}.{/right}\n');
//  box.setLine(1, 'bar');
        //  box.insertLine(1, 'foo');
    }
    
  screen.render();
});

// Quit on Escape, q, or Control-C.
screen.key(['escape', 'q', 'C-c'], function(ch, key) {
  return process.exit(0);
});

// Focus our element.
box.focus();

// Render the screen.
screen.render();