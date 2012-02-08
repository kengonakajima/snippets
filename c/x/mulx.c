#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <float.h>

#define RAND() (( rand() >> 4 ) & 32767 )

struct general {
	
	Display *d;
	Window rootw,rr;
	Window draww;
	int sc;
	GC gc;
	GC erasegc;
	XEvent ev;
	KeySym key;
	XSizeHints hint;
	unsigned long fg,bg;
	XFontStruct *fnt[8];
	int rootORwin;

	unsigned long valuemask;
	XSetWindowAttributes attr;
	
	Pixmap pixmap;
	
	int x,y,width,height,border,depth;

	float xrate,yrate;			/* ベースのサイズが、512x512だから、比 */
	Visual *visual;
	int displayok;

	Pixmap clearpixmap;
	
} gene[10] ;

XPoint pt[5000];
XSegment sg[5000];


void main( int argc,char **argv)
{
	int i,j;
	int ndisplay;
	int ntimes;
	int npoints;	
	char text[8];
	fd_set readfds;
	struct timeval tv;
	char buf[256];
	char buf2[256];
	int nfds;
	
	if(argc == 4){
		ndisplay = atoi(argv[1]);
		ntimes =  atoi(argv[2]);
		npoints = atoi(argv[3]);
		
	} else {
		printf("bad parameter.\nUse as: a.out <displayno> <ntimes> <npoints>\n");
		exit(-1);
		
	}

	
	tv.tv_sec = 0;	
	tv.tv_usec = 10000;		/* 割りこみの設定*/
	

	
	gene[0].d = XOpenDisplay("murasaki:0"); 


	printf("OpenDisplay ok.\n");
	
	for(i = 0 ; i <= ndisplay ; i++){
		if( gene[i].d == NULL){
			printf("can't open a display.\n");
			exit(-1);
		}
	}

	for(i = 0 ; i <= ndisplay ; i++){
	
		
		gene[i].rootw = DefaultRootWindow(gene[i].d);
		gene[i].sc = DefaultScreen(gene[i].d);
		

		XGetGeometry(gene[i].d,gene[i].rootw,
			     &gene[i].rr,&gene[i].x,&gene[i].y,
			     &gene[i].width,&gene[i].height,
			     &gene[i].border,
			     &gene[i].depth);
	/* Pixel color */
	
		gene[i].bg = BlackPixel(gene[i].d,gene[i].sc);
		gene[i].fg = WhitePixel(gene[i].d,gene[i].sc);
		
	
	/* GCを作る。 */
		gene[i].gc = XCreateGC(gene[i].d,gene[i].rootw, 0 , 0 );
		gene[i].erasegc = XCreateGC(gene[i].d,gene[i].rootw, 0,0);
		
		
		XSetBackground(gene[i].d, gene[i].gc, gene[i].bg);
		XSetForeground(gene[i].d, gene[i].gc, gene[i].fg);
		XSetBackground(gene[i].d, gene[i].erasegc, gene[i].fg);
		XSetForeground(gene[i].d, gene[i].erasegc, gene[i].bg);
		
		
	/* 線の幅を0にしておくと描画が速いそうだ */
		XSetLineAttributes(gene[i].d,gene[i].gc,0,LineSolid,CapNotLast,JoinMiter);

		/* デフォルトのウィンドウの位置 */

		gene[i].hint.x = 700;
		gene[i].hint.y = 500;
		gene[i].hint.width = 512;
		gene[i].hint.height = 512;
		gene[i].hint.flags = PPosition | PSize;

		gene[i].valuemask = 0;
		gene[i].attr.override_redirect = True; 
		gene[i].valuemask |= CWOverrideRedirect;
		
		/* ウィンドウ作る ->イベント選ぶー> マップする、 この流れ。*/	

		gene[i].visual = DefaultVisual(gene[i].d, gene[i].sc);
		gene[i].draww = XCreateWindow(gene[i].d,gene[i].rootw,
					      0,0,gene[i].width,gene[i].height,
					      0,gene[i].depth,
					      InputOutput,gene[i].visual,
					      gene[i].valuemask,
					      &gene[i].attr);
		
					      
					      
		XSetStandardProperties(gene[i].d, gene[i].draww, "shoot", "shoot",None,argv,argc,&gene[i].hint);
		XMapRaised(gene[i].d,gene[i].draww);

		/* pixmapを作る。 */
		XGetGeometry(gene[i].d,gene[i].draww,
			     &gene[i].rr,&gene[i].x,&gene[i].y,
			     &gene[i].width,&gene[i].height,
			     &gene[i].border,&gene[i].depth);
		printf("%d %d %d %d\n",gene[i].width ,gene[i].height,gene[i].border , gene[i].depth);
		
		gene[i].pixmap = XCreatePixmap(gene[i].d,gene[i].draww,
					       gene[i].width,
					       gene[i].height,gene[i].depth
					       );
		
	/* 取れるイベントを制限する。 */
		XSelectInput(gene[i].d,gene[i].draww, ButtonPressMask | KeyPressMask | ExposureMask );

	}

	/* 	それぞれのディスプレイに、同じことを表示する。 */

	for(j = 0 ; j <= ntimes ; j++){	
		if(  ( j  & 31) == 0 )	fprintf(stderr,"j:%d",j);
			
		for(i = 0 ; i < npoints ;i++){
		
			sg[i].x1 = (short) ( RAND() % (short)gene[0].width);
			sg[i].y1 = (short) ( RAND() % (short)gene[0].height);
			sg[i].x2 = (short) ( RAND() % (short)gene[0].width);
			sg[i].y2 = (short) ( RAND() % (short)gene[0].height);
			
			
		}
		for(i = npoints ; i < npoints * 3 ; i++){
			sg[i].x1 = (short) ( RAND() % gene[0].width);
			sg[i].y1 = (short) ( RAND() % gene[0].height);
			sg[i].x2 = sg[i].x1+5;
			sg[i].y2 = sg[i].y1+7;
		}
	

		/* 規定の時間、待つ。 */

		/* select を使うための準備 */
			
		FD_ZERO(&readfds);
		FD_SET(0,&readfds);
		nfds  = select(1,&readfds,NULL,NULL,&tv);
		if(nfds ){
			if( FD_ISSET(0,&readfds) ){
				if( j = read(0,buf,sizeof(buf)) ){
					buf[j] = '\0';
				} else {
					buf[0] = '\0';
				}
			}
		} else {
			buf[0] = '\0';
		}
		
		for(i = 0  ; i <= ndisplay ; i++){



#if 0
			XDrawLines( gene[i].d,gene[i].pixmap,gene[i].gc,pt,npoints,CoordModeOrigin);
			XCopyArea(gene[i].d,gene[i].pixmap,gene[i].draww,
				  gene[i].gc,
				  0,0,gene[i].width,gene[i].height,0,0);
			
			XFlush(gene[i].d);
			XDrawLines( gene[i].d,gene[i].pixmap,gene[i].erasegc,pt,npoints,CoordModeOrigin);
#endif
			#if 0
			XDrawLines( gene[i].d,gene[i].draww,gene[i].gc,pt,npoints,CoordModeOrigin);
			XFlush(gene[i].d);
			XDrawLines( gene[i].d,gene[i].draww,gene[i].erasegc,pt,npoints,CoordModeOrigin);
			#endif

			#if 1
			XDrawSegments(gene[i].d,gene[i].draww,gene[i].gc,sg,npoints*3);
			XFlush(gene[i].d); 
			XDrawSegments(gene[i].d,gene[i].draww,gene[i].erasegc,sg,npoints*3);
			#endif


			/* 入力。 */
			
			if(	XCheckWindowEvent(
					  gene[i].d,gene[i].draww,
					  ButtonPressMask | KeyPressMask | ExposureMask ,
					  &gene[i].ev
				)
			){
			
			switch(gene[i].ev.type){
				case Expose : {
					XFillRectangle(gene[i].d,gene[i].draww,gene[i].erasegc,0,0,
						       gene[i].width,gene[i].height); 
					break;
				}
				case MappingNotify :{	/* この行は必ず必要らしい。 */
					XRefreshKeyboardMapping( &gene[i].ev);
					break;
				}
				case ButtonPress : {
					XDrawRectangle(gene[i].d,gene[i].draww,gene[i].gc,
						       50,50,400,400);
					fprintf(stderr,"ButtonPress.");
					for(i = 0 ; i <= ndisplay ; i++){
						XCloseDisplay(gene[i].d);
					}
					exit(-1);
					break;
				}
				case KeyPress : {
					i = XLookupString( &gene[i].ev,text,sizeof(text),
							  &gene[i].key,0);
					break;
				}
				default : {
					break;
				}
			}
			}	/* user input switch */

		}
	}

	for(i = 0 ; i  <= ndisplay  ; i ++){

		XCloseDisplay(gene[i].d);
	}
		


}


