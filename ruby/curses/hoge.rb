require "curses"

Curses.init_screen

cnt=0
s = "Hello World!"

while true
  Curses.clear
  Curses.setpos(Curses.lines / 2, Curses.cols / 2 - (s.length / 2) + cnt )
  Curses.addstr(s)
  Curses.refresh
  Curses.timeout=1000
  ch=Curses.getch
  cnt+=1
  break if ch == 'q' 
end

Curses.close_screen

