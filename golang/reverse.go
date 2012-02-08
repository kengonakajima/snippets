package main

import(
	"fmt";
	"os";
	"image";
	"exp/draw";
	"exp/draw/x11";
)

const (
	AI_Lv		= 7;
	MASU		= 8;
	MAXIMUM		= 10000000;
	MINIMUM int32 = -10000000;
	SIRO		= 1;
	KURO		= -1;
	MUZI		= 0;
	GREEN_SIZE	= 300;
)

type Boards struct {
	data		[8][8]int8;
}

type Games struct {
	turn		int8;
	copy		[8]Boards;
	all			byte;
	board		Boards;
	pointmap	Boards;
	aroundmap	[2][8]int8;
	aroundmapd	[2][4]byte;
	nextstep	[4][2]byte;
	img			draw.Image;
}

func main(){
	context, error := x11.NewWindow();
	if error != nil {
		fmt.Fprintf(os.Stderr, "x11失敗 %s\n", error);
		os.Exit(1);
	}
	// 初期設定
	game := init_game();
	game.img = context.Screen();
	game.print_board();

	go func(){
		// チャネル入力があるまでストップ
		<-context.QuitChan();
		os.Exit(0);
	}();

	block := false;
	for {
		mouse := <-context.MouseChan();
		if (mouse.Buttons & 0x01) == 0x01 {
			x := byte(mouse.Point.X / 31);
			y := byte(mouse.Point.Y / 31);
			if x < MASU && y < MASU {
				if block == false {
					block = true;
					game.start_turn(y, x);
					block = false;
				} else {
					println("AI思考中・・・");
				}
			}
		}
		context.FlushImage();
	}
}

func init_game() (*Games){
	game := new(Games);
	game.all = 0;
	game.board.data = [8][8]int8{[8]int8{ 0, 0, 0, 0, 0, 0, 0, 0},[8]int8{ 0, 0, 0, 0, 0, 0, 0, 0},[8]int8{ 0, 0, 0, 0, 0, 0, 0, 0},[8]int8{ 0, 0, 0,-1, 1, 0, 0, 0},[8]int8{ 0, 0, 0, 1,-1, 0, 0, 0},[8]int8{ 0, 0, 0, 0, 0, 0, 0, 0},[8]int8{ 0, 0, 0, 0, 0, 0, 0, 0},[8]int8{ 0, 0, 0, 0, 0, 0, 0, 0}};
	game.pointmap.data = [8][8]int8{[8]int8 { 30,-12,  0, -1, -1,  0,-12, 30},[8]int8 {-12,-15, -3, -3, -3, -3,-15,-12},[8]int8 {  0, -3,  0, -1, -1,  0, -3,  0},[8]int8 { -1, -3, -1, -1, -1, -1, -3, -1},[8]int8 { -1, -3, -1, -1, -1, -1, -3, -1},[8]int8 {  0, -3,  0, -1, -1,  0, -3,  0},[8]int8 {-12,-15, -3, -3, -3, -3,-15,-12},[8]int8 { 30,-12,  0, -1, -1,  0,-12, 30}};
	game.aroundmap = [2][8]int8 {[8]int8 { -1,  0,  1, -1,  1, -1,  0,  1},[8]int8 { -1, -1, -1,  0,  0,  1,  1,  1}};
	game.aroundmapd = [2][4]byte{[4]byte { 0, 0, 7, 7},[4]byte{0, 7, 0, 7}};
	game.nextstep = [4][2]byte {[2]byte { 1, 2},[2]byte { 1, 4},[2]byte { 3, 2},[2]byte { 3, 4}};
	game.turn = 1;
	return game;
}

func (game *Games) start_turn(y, x byte){
	if game.put_stone_check(y, x) == true {
		game.check_reverse(y, x);
		game.all++;
		game.print_board();
		game.ai_stone_check();
	} else {
		if game.board_stone_check() == true {
			println("そこには置けません。");
		} else {
			game.ai_stone_check();
		}
	}
	game.print_board();
}

func (game *Games) irekae(){
	game.turn *= -1;
}

func (game *Games) keep(lv byte){
	game.copy[lv] = game.board;
}

func (game *Games) back(lv byte){
	game.board = game.copy[lv];
}

func (game *Games) ai_stone_check(){
	game.irekae();
	ij := byte(game.AandB(true, AI_Lv, MINIMUM, MAXIMUM));
	y := ij / MASU;
	x := ij % MASU;
	if y < MASU && y >= 0 && x < MASU && x >= 0 {
		game.check_reverse(y, x);
		fmt.Printf("AIは [%2c,%2d] に置きました。\n", (x + 'a'), y);
		game.all++;
	} else {
		println("AIはパスしました");
	}
	game.irekae();
}

func (game *Games) AandB(flag bool, ai byte, a, b int32) (int32){
	var val, Cval int32 = 0, 0;
	var bestY, bestX byte = 255, 255;
	
	if ai == 0 {
		return game.board_point(flag);
	}
	
	if flag == true {
		val = MINIMUM;
	} else {
		val = MAXIMUM;
	}
	// ターンの保存
	game.keep(ai);
	for i := byte(0); i < MASU; i++ {
		for j := byte(0); j < MASU; j++ {
			if game.board.data[i][j] == MUZI {
				if game.put_stone_check(i, j) == true {
					game.check_reverse(i, j);
					game.irekae();
					Cval = game.AandB(!flag, ai - 1, a, b);
					game.irekae();
					game.back(ai);
					
					if flag == true {
						if Cval > val {
							val = Cval;
							a = val;
							bestY = i;
							bestX = j;
						}
						if val > b {
							return val;
						}
					} else {
						if Cval < val {
							val = Cval;
							b = val;
							bestY = i;
							bestX = j;
						}
						if val < a {
							return val;
						}
					}
				}
			}
		}
	}
	if ai == AI_Lv {
		if (bestY == 255) || (bestX == 255) {
			return -1;
		}
		return int32((bestY * MASU) + bestX);
	}
	if (bestY == 255) || (bestX == 255) {
		return game.board_point(flag);
	}
	return val;
}

func (game *Games) board_point(flag bool) (int32){
	var point int32 = 0;
	for i := 0; i < MASU; i++ {
		for j := 0; j < MASU; j++ {
			point += int32((game.board.data[i][j]) * (game.pointmap.data[i][j]));
		}
	}
	point += game.decision_stone() * 5;
	
	if flag == false {
		point *= -1;
	}
	return point;
}

func (game *Games) stone_check(i, j, y, x int8) (bool){
	i += y;
	j += x;
	
	if (i >= MASU) || (i < 0) || (j >= MASU) || (j < 0) {
		return false;
	}
	if game.board.data[i][j] != (game.turn * -1) {
		return false;
	}
	
	i += y;
	j += x;
	
	for (i < MASU) && (i >= 0) && (j < MASU) && (j >= 0) {
		if game.board.data[i][j] == MUZI {
			return false;
		}
		if game.board.data[i][j] == game.turn {
			return true;
		}
		i += y;
		j += x;
	}
	return false;
}

func (game *Games) put_stone_check(i, j byte) (bool){
	for k := 0; k < MASU; k++ {
		if game.stone_check(int8(i), int8(j), game.aroundmap[0][k], game.aroundmap[1][k]) == true {
			return true;
		}
	}
	return false;
}

func (game *Games) board_stone_check() (bool){
	for i := byte(0); i < MASU; i++ {
		for j := byte(0); j < MASU; j++ {
			if game.board.data[i][j] == MUZI {
				if game.put_stone_check(i, j) == true {
					return true;
				}
			}
		}
	}
	return false;
}

func (game *Games) check_reverse(i, j byte) (int32){
	var ret int32 = 0;
	if game.board.data[i][j] == MUZI {
		for k := 0; k < MASU; k++ {
			if game.stone_check(int8(i), int8(j), game.aroundmap[0][k], game.aroundmap[1][k]) == true {
				ret += game.hanten(int8(i), int8(j), game.aroundmap[0][k], game.aroundmap[1][k]);
			}
		}
	}
	return ret;
}

func (game *Games) hanten(i, j, y, x int8) (int32){
	game.board.data[i][j] = game.turn;
	i += y;
	j += x;
	var ret int32 = 1;
	
	for game.board.data[i][j] == (game.turn * -1) {
		game.board.data[i][j] = game.turn;
		i += y;
		j += x;
		ret++;
	}
	return ret;
}

func (game *Games) decision_stone() (int32){
	var point int32 = 0;
	for i := 0; i < 4; i++ {
		if game.board.data[game.aroundmapd[0][i]][game.aroundmapd[1][i]] != MUZI {
			point += game.decision_count(game.aroundmapd[0][i], game.aroundmapd[1][i], game.nextstep[i][0]);
			point += game.decision_count(game.aroundmapd[0][i], game.aroundmapd[1][i], game.nextstep[i][1]);
		}
	}
	return point;
}

func (game *Games) decision_count(y, x, types byte) (int32){
	var	count, my, my2, enemy, enemy2 int32 = 0, 0, 0, 0, 0;
	
	for game.board.data[y][x] != MUZI {
		if (game.board.data[y][x] == game.turn) && (my == count) {
			my++;
		} else if game.board.data[y][x] == game.turn {
			my2++;
		}
		
		if (game.board.data[y][x] != game.turn) && (enemy == count) {
			enemy++;
		} else if game.board.data[y][x] != game.turn {
			enemy2++;
		}
		
		if count >= 7 {
			return ((my + my2) - (enemy + enemy2));
		}
		
		count++;
		
		switch types {
			case 1: y++;
			case 2: x++;
			case 3: y--;
			case 4: x--;
		}
	}
	return (my - enemy);
}

func drawHorizontalLine(img draw.Image, sx, ex, y int, c image.RGBAColor){
	if sx > ex {
		t := ex;
		ex = sx;
		sx = t;
	}
	for sx < ex {
		set(img, sx, y, c);
		sx += 1;
	}
}

func set(img draw.Image, x, y int, c image.RGBAColor){
	if x >= 0 && y >= 0 && x < img.Width() && y < img.Height() {
		img.Set(x, y, c);
	}
}

// http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
func drawCircle(img draw.Image, cx int, cy int, r int, c image.RGBAColor, fc image.RGBAColor){
	x := r;
	y := 0;
	error := -r;
	for x >= y {
		var t int;
		t = cy + y;
		if fc.A != uint8(0) { drawHorizontalLine(img, cx - x, cx + x, t, fc); }
		if c.A != uint8(0) {
		set(img, cx + x, t, c);
		set(img, cx - x, t, c);
		}
		t = cy - y;
		if fc.A != uint8(0) { drawHorizontalLine(img, cx - x, cx + x, t, fc); }
		if c.A != uint8(0) {
			set(img, cx + x, t, c);
			set(img, cx - x, t, c);
		}
		t = cy + x;
		if fc.A != uint8(0) { drawHorizontalLine(img, cx - y, cx + y, t, fc); }
		if c.A != uint8(0) {
			set(img, cx + y, t, c);
			set(img, cx - y, t, c);
		}
		t = cy - x;
		if fc.A != uint8(0) { drawHorizontalLine(img, cx - y, cx + y, t, fc); }
		if c.A != uint8(0) {
			img.Set(cx + y, t, c);
			img.Set(cx - y, t, c);
		}
		error += y;
		y += 1;
		error += y;
		if error >= 0 {
			x -= 1;
			error -= x;
			error -= x
		}
	}
}

func (game *Games) print_board(){
	var point int8 = 0;
	line_x := 0;
	line_y := 0;
	siro := 0;
	kuro := 0;
	r := 14;
	black := image.RGBAColor {   0,   0,   0, 255};
	white := image.RGBAColor { 255, 255, 255, 255};
	green := image.RGBAColor {   0, 255,   0, 255};
	width := game.img.Width();
	height := game.img.Height();

	if (width < GREEN_SIZE) || (height < GREEN_SIZE) {
		println("error");
		return;
	}
	// 緑に塗る
	for y := 0; y < 248; y++ {
		if line_y == y {
			line_y += 31;
		} else {
			line_x = 0;
			for x := 0; x < 248; x++ {
				if line_x == x {
					line_x += 31;
				} else {
					game.img.Set(x, y, green);
				}
			}
		}
	}
	// 丸を書く
	for y := 0; y < MASU; y++ {
		for x := 0; x < MASU; x++ {
			point = game.board.data[y][x];
			if point == SIRO {
				drawCircle(game.img, (x * 30) + (x + 1) + 15, (y * 30) + (y + 1) + 15, r, image.RGBAColor { A: 0 }, white);
				siro++;
			} else if point == KURO {
				drawCircle(game.img, (x * 30) + (x + 1) + 15, (y * 30) + (y + 1) + 15, r, image.RGBAColor { A: 0 }, black);
				kuro++;
			}
		}
	}
}