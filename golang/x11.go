package main

import (
    "os";
    "fmt";
    "image";
    "draw";
    "draw/x11";
    "rand";
    "time"
)

func drawHorizontalLine(img draw.Image, sx int, ex int, y int, c image.RGBAColor) {
    if sx > ex {
        t := ex;
        ex = sx;
        sx = t
    }
    for sx < ex {
        set(img, sx, y, c);
        sx += 1
    }
}

func set(img draw.Image, x int, y int, c image.RGBAColor)
{
    if x >= 0 && y >= 0 && x < img.Width() && y < img.Height() {
        img.Set(x, y, c)
    }
}

// http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
func drawCircle(img draw.Image, cx int, cy int, r int, c image.RGBAColor, fc image.RGBAColor) {
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

func main() {
    sync := make(<-chan bool);
    ctx, e := x11.NewWindow();
    if e != nil {
        fmt.Fprintf(os.Stderr, "%s\n", e.String());
        return
    }
    scr := ctx.Screen();
    go func(){
        for {
            x := rand.Int() % scr.Width();
            y := rand.Int() % scr.Height();
            r := rand.Int() % 120;
            c := image.RGBAColor { uint8(rand.Int()), uint8(rand.Int()), uint8(rand.Int()), 255 };
            drawCircle(scr, x, y, r, image.RGBAColor { A: 0 }, c);
            ctx.FlushImage();
            time.Sleep(100000000);
        }
    }();
    <-sync
}