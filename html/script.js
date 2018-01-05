// 要素ら

var el_hitarea = document.getElementById('hitarea');
var el_eventname = document.getElementById('eventname');
var el_x = document.getElementById('x');
var el_y = document.getElementById('y');

// 表示をアップデートする関数群

var updateXY = function(event) {
  el_x.innerHTML = event.changedTouches[0].pageX;
  el_y.innerHTML = event.changedTouches[0].pageY;
};
var updateEventname = function(eventname) {
  el_eventname.innerHTML = eventname;
};

// イベント設定

el_hitarea.addEventListener('touchstart', function(event) {
  updateEventname('touchstart');
  updateXY(event);
  el_hitarea.style.backgroundColor = 'red';
}, false);

el_hitarea.addEventListener('touchmove', function(event) {
  event.preventDefault(); // タッチによる画面スクロールを止める
  updateEventname('touchmove');
  updateXY(event);
}, false);

el_hitarea.addEventListener('touchend', function(event) {
  updateEventname('touchend');
  updateXY(event);
  el_hitarea.style.backgroundColor = 'blue';
}, false);