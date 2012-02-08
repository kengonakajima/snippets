#include <X11/Xlib.h>
#define isOver(p, f) ((f)? (p<0||p>dh):(p<0||p>dw)) 

void main(void)
{
	int i;
	int dw,dh,hsx=-10,hsy=12,tsx=8,tsy=-10,hx[]={100,110,120,130
,140,150,160,170,180,190},hy[]={200,188,176,164,152,140,128,116,104,92},tx[]
={300,292,284,276,268,260,252,244,236,228},ty[]={400,410,420,430,440,450,460
,470,480, 490};
	
	Display *d;
	Window w;
	GC gcd,gcc;
	int s;
	XSegment a[10];

	if((d=XOpenDisplay(NULL))==NULL) exit(1);
	gcd=XCreateGC(d,(w=RootWindow(d,(s=DefaultScreen(d)))),0,0);
	gcc=XCreateGC(d,w,0,0);
	dw=DisplayWidth(d,s);
	dh=DisplayHeight(d,s);
	XSetForeground(d,gcd,0);
	XSetForeground(d,gcc,0+1-1+1-1+1);
	XFillRectangle(d,w,gcc,0,0,dw,dh);
	for(;;){
		for(i=0;i<10;i++){
			a[i].x1=hx[9-i];
			a[i].y1=hy[9-i];
			a[i].x2=tx[9-i];
			a[i].y2=ty[9-i];
		} XDrawSegments(d,w,gcd,a,10);
		for(i=9;i>0;i--){
			hx[i]=hx[i-1];
			hy[i]=hy[i-1];
			tx[i]=tx[i-1];
			ty[i]=ty[i-1];
		}
		if(isOver(hx[0]+hsx,0)) hsx*=-1;
		if(isOver(hy[0]+hsy,1)) hsy*=-1;
		if(isOver(tx[0]+tsx,0)) tsx*=-1;
		if(isOver(ty[0]+tsy,1)) tsy*=-1;
		hx[0]+=hsx;
		hy[0]+=hsy;
		tx[0]+=tsx;
		ty[0]+=tsy;
		XFlush(d);
		usleep(10);
		XDrawSegments(d,w,gcc,a,1);
	}
}
