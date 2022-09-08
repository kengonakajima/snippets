var blessed = require('blessed');
// Create a screen object.
var screen = blessed.screen({
  autoPadding: true,
  smartCSR: true
});
screen.title = '初めてのWindow Title';
// Create a box perfectly centered horizontally and vertically.
var box = blessed.box({
  top: 'center',
  left: 'center',
  width: '100%',
  height: '100%',
  //日本語????になります。残念
  content: 'Hello {bold}world{/bold}!',
  tags: true,
  border: {
    type: 'line'
  },  
  style: {
    fg: 'white',
    bg: '#202020',
    border: {
      fg: '#ffffff'
    },  
    hover: {
      bg: 'green'
    }   
  }
});



// 終了させるキーの設定。Escape, q, or Control-C.
screen.key(['escape', 'q', 'C-c'], function(ch, key) {
  //タイトルが残ってしまうのでクリア
  screen.title = ''; 
  return process.exit(0);
});

// Append our box to the screen.
screen.append(box);
screen.render();

setInterval(()=>{
  box.setContent("aho\nasdf:"+new Date().getTime());
  screen.render();
},100);
