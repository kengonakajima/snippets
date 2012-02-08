/* x.c : xscratch program by nuka 1996.2.2 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	"check.xbm"

int	fputs(const char *str, FILE *stream);

#define		PARTSNUM	256
#define		ONE		4096

enum	parts_status	{
	NONE,
	POINT,
	RAISED
};

typedef	struct	parttl	{
	struct parttl	*n, *p;
	int		stat, x, y, w, h;
	Pixmap		source, view;
	int		bx, by;
} PARTTL;

typedef	struct	eyetl	{
	int		x, y, scale;
} EYETL;

typedef	struct	inputtl	{
	int	x, y;
	int	button[3];
} INPUTTL;

Display		*display;
Window		root, cut, scratch;
Pixmap		rootmap, cutmap, scratchmap;
Colormap	cmap;
GC		gc, shadow_gc;
XEvent		event;
KeySym		key;
PARTTL		parts_buf[PARTSNUM];
PARTTL		*out_parts_hd;
PARTTL		*in_parts_hd;
EYETL		eye;
INPUTTL		input;

unsigned int	width, height, depth;

unsigned long	white, black, back;

#define		WINW	640
#define		WINH	512

#define		MINW	64
#define		MINH	64
#define		MINL	24


void	InitX(void)
{
	int	screen, dum;
	Pixmap	check;

	display = XOpenDisplay("");
	screen = DefaultScreen(display);
	cmap = DefaultColormap(display, screen);

	white = WhitePixel(display, screen);
	black = BlackPixel(display, screen);
	
	XGetGeometry(display, DefaultRootWindow(display),
		     &root, &dum, &dum, &width, &height, (unsigned int *)&dum, &depth);
	
	rootmap = XCreatePixmap(display, root, width, height, depth);
	cutmap = XCreatePixmap(display, root, width, height, depth);
	
	scratch = XCreateWindow(display, root, 0, 0, WINW, WINH, 0, depth,
			    InputOutput, CopyFromParent, 0, NULL);
	XSelectInput(display, scratch,
		     ExposureMask | KeyPressMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XMapRaised(display, scratch);
	scratchmap = XCreatePixmap(display, root, WINW, WINH, depth);
	
	gc = XCreateGC(display, root, 0, 0);
	XSetSubwindowMode(display, gc, IncludeInferiors);
	
	shadow_gc = XCreateGC(display, root, 0, 0);
	XSetFillStyle(display, shadow_gc, FillStippled);
	check = XCreateBitmapFromData(display, root, check_bits, check_width, check_height);
	XSetStipple(display, shadow_gc, check);

	XGrabButton(display, Button1, Mod1Mask, DefaultRootWindow(display), False,
		    PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
		    GrabModeAsync, GrabModeAsync, DefaultRootWindow(display), None);
}

void	EndX(void)
{
	XFlush(display);
	XDestroyWindow(display, scratch);
	XFreePixmap(display, rootmap);
	XFreePixmap(display, cutmap);
	XFreePixmap(display, scratchmap);
	XCloseDisplay(display);
}

unsigned long SetColor(unsigned short red, unsigned short green, unsigned short blue)
{
	XColor	color;
	
	color.red = red;	color.green = green;	color.blue = blue;
	if ( 0 == XAllocColor(display, cmap, &color) ) {
		fputs("Can't allocate color.\n", stderr);
		exit(1);
	}
	return color.pixel;
}

#define	CMAX16	(65536 / 16)

void	InitColor(void)
{
	back = SetColor(CMAX16 * 8, CMAX16 * 8, CMAX16 * 8);
}

void	CutMode(int x, int y)
{
	XSetWindowAttributes	atr;
	unsigned long		value = 0;

	XCopyArea(display, root, rootmap, gc, 0, 0, width, height, 0, 0);
	
	atr.override_redirect = True;
	value |= CWOverrideRedirect;

	cut = XCreateWindow(display, root, 0, 0, width, height, 0, depth,
			    InputOutput, CopyFromParent, value, &atr);
	XCopyArea(display, rootmap, cutmap, gc, 0, 0, width, height, 0, 0);
	XCopyArea(display, cutmap, cut, gc, 0, 0, width, height, 0, 0);
	XMapRaised(display, cut);
}

void	CutRectangle(int px, int py, int x, int y, int *sx, int *sy, int *w, int *h)
{
	if (x < px) {
		*sx = x;	*w = px - x + 1;
	} else {
		*sx = px;	*w = x - px + 1;
	}
	if (y < py) {
		*sy = y;	*h = py - y + 1;
	} else {
		*sy = py;	*h = y - py + 1;
	}
}

void	DrawShadow(Pixmap map, int sx, int sy, int w, int h, int s_width)
{
	XSetForeground(display, shadow_gc, black);
	XFillRectangle(display, map, shadow_gc,
		       sx + w - s_width, sy, s_width, h);
	XFillRectangle(display, map, shadow_gc,
		       sx, sy + h - s_width, w, s_width);
}

void	MaxArea(int *rx, int *ry, int *rw, int *rh, int x, int y, int w, int h, int xx, int yy, int ww, int hh)
{
	if (x < xx)	*rx = x;
	else		*rx = xx;
	if (y < yy)	*ry = y;
	else		*ry = yy;
	if (x + w < xx + ww)	*rw = xx + ww - *rx;
	else			*rw = x + w - *rx;
	if (y + h < yy + hh)	*rh = yy + hh - *ry;
	else			*rh = y + h - *ry;
}

void	CalcView(PARTTL *part, int *px, int *py, int *pw, int *ph)
{
	*px = (part->x - eye.x) * eye.scale / ONE + WINW / 2;
	*py = (part->y - eye.y) * eye.scale / ONE + WINH / 2;
	*pw = part->w * eye.scale / ONE;
	*ph = part->h * eye.scale / ONE;
}

void	CalcCoord(int px, int py, PARTTL *part)
{
	part->x = (px - WINW / 2) * ONE / eye.scale + eye.x;
	part->y = (py - WINH / 2) * ONE / eye.scale + eye.y;
}

void	AffinePart(PARTTL *part, int dw, int dh, int first)
{
	int	i;
	Pixmap	affine = XCreatePixmap(display, root, part->w, dh, depth);

	if (0 == first) XFreePixmap(display, part->view);
	part->view = XCreatePixmap(display, root, dw, dh, depth);
	for (i = 0; i < dh; ++i) {
		XCopyArea(display, part->source, affine, gc, 0, i * part->h / dh, part->w, 1, 0, i);
	}
	for (i = 0; i < dw; ++i) {
		XCopyArea(display, affine, part->view, gc, i * part->w / dw, 0, 1, dh, i, 0);
	}
	XFreePixmap(display, affine);
}

void	AffineParts(void)
{
	PARTTL	*part = in_parts_hd;
	int	px = 0, py = 0, pw = 0, ph = 0;

	while (NULL != part) {
		CalcView(part, &px, &py, &pw, &ph);
		AffinePart(part, pw, ph, 0);
		part = part->n;
	}
}

#define	PUTPART(x,y)	XCopyArea(display,part->view,scratchmap,gc,0,0,pw,ph,x,y);

#define	RUP	8
#define	RWAKU	5
#define	PWAKU	5

void	PutParts(void)
{
	PARTTL	*part = in_parts_hd;
	
	XSetForeground(display, gc, back);
	XFillRectangle(display, scratchmap, gc, 0, 0, WINW, WINH);
	if (NULL == part) return;
	while (NULL != part->n) part = part->n;
	while (NULL != part) {
		int	px = 0, py = 0, pw = 0, ph = 0;
		
		CalcView(part, &px, &py, &pw, &ph);
		XSetForeground(display, gc, black);
		switch (part->stat) {
		    case	RAISED:
			XFillRectangle(display, scratchmap, gc,
				       px - RWAKU - RUP, py - RWAKU - RUP,
				       pw + RWAKU * 2, ph + RWAKU * 2);
			DrawShadow(scratchmap, px + 8 - 1 - RWAKU, py + 8 - 1 - RWAKU,
				   pw + RWAKU * 2, ph + RWAKU * 2, 8 + RUP);
			PUTPART(px - RUP, py - RUP);
			break;
		    case 	POINT:
			XFillRectangle(display, scratchmap, gc, px - PWAKU, py - PWAKU,
				       pw + PWAKU * 2, ph + PWAKU * 2);
			DrawShadow(scratchmap, px + 8 - 1 - PWAKU, py + 8 - 1 - PWAKU,
				   pw + PWAKU * 2, ph + PWAKU * 2, 8);
			PUTPART(px, py);
			break;
		    default:
			XDrawRectangle(display, scratchmap, gc, px - 1, py - 1,
				       pw + 2, ph + 2);
			DrawShadow(scratchmap, px + 8 - 1, py + 8 - 1,
				   pw + 2, ph + 2, 8);
			PUTPART(px, py);
			break;
		}
		part = part->p;
	}
	XCopyArea(display, scratchmap, scratch, gc, 0, 0, WINW, WINH, 0, 0);
}

void	PointPart(int x, int y)
{
	PARTTL	*part = in_parts_hd;
	int	px = 0, py = 0, pw = 0, ph = 0;

	while (NULL != part) {
		CalcView(part, &px, &py, &pw, &ph);
		
		if (x < px || px + pw < x ||
		    y < py || py + ph < y) {
			part->stat = NONE;
			part = part->n;		continue;
		}
		part->stat = POINT;
		part->bx = x - px;		part->by = y - py;
		part = part->n;
		break;
	}
	while (NULL != part) {
		part->stat = NONE;
		part = part->n;
	}
}

void	RaisePart(void)
{
	PARTTL	*part = in_parts_hd;
	
	while (NULL != part) {
		if (POINT == part->stat) {
			if (NULL == part->p) {
				part->stat = RAISED;
			} else {
				(part->p)->n = part->n;
				if (NULL != part->n) (part->n)->p = part->p;
				part->p = NULL;
				part->n = in_parts_hd;
				in_parts_hd->p = part;
				in_parts_hd = part;
				part->stat = RAISED;
			}
		} else {
			part->stat = NONE;
		}
		part = part->n;
	}
}

int	CantCut(int w, int h)
{
	if ((w < MINW && h < MINH) || w < MINL || h < MINL) return 1;
	else return 0;
}

int	CompressMNEvent(XEvent *event)
{
	XEvent	ahead;

	while (XEventsQueued(event->xmotion.display, QueuedAfterReading) > 0) {
		XPeekEvent(event->xmotion.display, &ahead);
		if (MotionNotify != ahead.type) return 1;
		if (ahead.xmotion.window != event->xmotion.window) return 1;
		XNextEvent(event->xmotion.display, event);
	}
	return 0;
}


#define	SCROLL_SPDIV	8

void	MainLoop(void)
{
    int	px = 0, py = 0, sx = 9, sy = 9, w = 0, h = 0;
    int	osx, osy, ow, oh;
    
#define		INPUT_TEXT_SIZE		16
    PutParts();
    while (1) {
	char	string_length, input_text[INPUT_TEXT_SIZE];
	
	XFlush(display);
	if (input.button[2]) {
	    eye.x +=  (input.x - WINW / 2) / SCROLL_SPDIV;
	    eye.y +=  (input.y - WINH / 2) / SCROLL_SPDIV;
	    PutParts();
	    XSync(display, 0);
	    if (0 == XEventsQueued(display, QueuedAfterReading)) continue;
	}
	while (XEventsQueued(display, QueuedAfterReading) > 0) {
	    XNextEvent(display, &event);
	    switch (event.type) {
	      case	Expose:
		XCopyArea(display, scratchmap, scratch, gc,
			  0, 0, WINW, WINH, 0, 0);
		break;
	      case	MappingNotify:
		XRefreshKeyboardMapping((XMappingEvent *)&event);
		break;
	      case	KeyPress:
		string_length = XLookupString((XKeyEvent *)&event,
					      input_text, INPUT_TEXT_SIZE, &key, NULL);
		if (1 == string_length) {
		    switch (input_text[0]) {
		      case	'q':
			return;
		      case	'u':
			eye.scale += 256;
			AffineParts();
			break;
		      case	'd':
			if (eye.scale > 256) eye.scale -= 256;
			AffineParts();
			break;
		    }
		    PutParts();
		}
		break;
	      case	MotionNotify:
		if (CompressMNEvent(&event)) {
		    input.x = event.xmotion.x;
		    input.y = event.xmotion.y;
		    break;
		}
		if (scratch != event.xmotion.window) {
		    int	rx, ry, rw, rh;
		    
		    osx = sx;	osy = sy;
		    ow = w;		oh = h;
		    
		    XCopyArea(display, rootmap, cutmap, gc,
			      osx - 9, osy - 9, ow + 10, oh + 10,
			      osx - 9, osy - 9);
		    CutRectangle(px, py, event.xmotion.x, event.xmotion.y,
				 &sx, &sy, &w, &h);
		    XSetForeground(display, gc, black);
		    if (CantCut(w, h)) {
			XDrawRectangle(display, cutmap, gc,
				       sx - 1, sy - 1,
				       w + 1, h + 1);
			XCopyArea(display, rootmap, cutmap, gc,
				  sx, sy, w, h, sx, sy);
		    } else {
			XDrawRectangle(display, cutmap, gc,
				       sx - 9, sy - 9,
				       w + 1, h + 1);
			DrawShadow(cutmap, sx, sy, w, h, 8);
			XCopyArea(display, rootmap, cutmap, gc,
				  sx, sy, w, h, sx - 8, sy - 8);
		    }
		    MaxArea(&rx, &ry, &rw, &rh,
			    sx - 9, sy - 9, w + 10, h + 10,
			    osx - 9, osy - 9, ow + 10, oh + 10);
		    XCopyArea(display, cutmap, cut, gc,
			      rx, ry, rw, rh, rx, ry);
		}
		if (input.button[0] && NULL != in_parts_hd) {
		    if (RAISED == in_parts_hd->stat) {
			CalcCoord(event.xmotion.x - in_parts_hd->bx,
				  event.xmotion.y - in_parts_hd->by,
				  in_parts_hd);
			PutParts();
		    } else {
			PointPart(event.xmotion.x, event.xmotion.y);
			PutParts();
		    }
		} else {
		    PointPart(event.xmotion.x, event.xmotion.y);
		    PutParts();
		}
		break;
	      case	ButtonPress:
		input.button[event.xbutton.button - 1] = 1;
		if (1 == event.xbutton.button && scratch != event.xbutton.window) {
		    px = event.xbutton.x;		py = event.xbutton.y;
		    CutMode(px, py);
		    break;
		}
		if (1 == event.xbutton.button) {
		    RaisePart();
		    PutParts();
		    break;
		}
		break;
	      case	ButtonRelease:
		input.button[event.xbutton.button - 1] = 0;
		sx = sy = 9;	w = h = 0;
		
		if (1 == event.xbutton.button && scratch != event.xbutton.window) {
		    int	sx, sy, w, h;
		    PARTTL	*part;
		    
		    XDestroyWindow(display, cut);
		    CutRectangle(px, py, event.xmotion.x, event.xmotion.y,
				 &sx, &sy, &w, &h);
		    if (!CantCut(w, h)) {
			if (NULL == out_parts_hd) {
			    fputs("parts over flow.\n",
				  stderr);
			    exit(1);
			}
			part = out_parts_hd;
			out_parts_hd = out_parts_hd->n;
			
			part->p = NULL;
			part->n = in_parts_hd;
			if (NULL != in_parts_hd) {
			    in_parts_hd->p = part;
			}
			in_parts_hd = part;
			
			part->stat = NONE;
			part->x = eye.x + rand() % (WINW / 2) - WINW / 2;
			part->y = eye.y + rand() % (WINH / 2) - WINH / 2;
			part->w = w;	part->h = h;
			part->source = XCreatePixmap(display, root, w, h, depth);
			XCopyArea(display, rootmap,
				  part->source, gc,
				  sx, sy, w, h, 0, 0);
			{
			    int	px = 0, py = 0;
			    int	pw = 0, ph = 0;
			    ;
			    CalcView(part, &px, &py,
				     &pw, &ph);
			    AffinePart(part, pw, ph, 1);
			}
			PutParts();
		    }
		    break;
		}
		break;
	    }
	}
    }
}

void	InitScratch(void)
{
	int	i;

	out_parts_hd = parts_buf;
	for (i = 0; i < PARTSNUM - 1; ++i) {
		parts_buf[i].n = parts_buf + i + 1;
	}
	parts_buf[PARTSNUM - 1].n = NULL;
	in_parts_hd = NULL;

	eye.x = eye.y = 0;
	eye.scale = ONE;

	input.x = input.y = 0;
	for (i = 0; i < 3; ++i) input.button[i] = 0;
}

void	main(void)
{
	InitX();
	InitColor();
	InitScratch();
	MainLoop();
	EndX();
}
