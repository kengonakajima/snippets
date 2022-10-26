// SDL -lm -liconv -lobjc  -Wl,-framework -Wl,CoreAudio -Wl,-framework -Wl,AudioToolbox -Wl,-weak_framework -Wl,CoreHaptics -Wl,-weak_framework -Wl,GameController -Wl,-framework -Wl,ForceFeedback -Wl,-framework -Wl,CoreVideo -Wl,-framework -Wl,Cocoa -Wl,-framework -Wl,Carbon -Wl,-framework -Wl,IOKit -Wl,-weak_framework -Wl,QuartzCore -Wl,-weak_framework -Wl,Metal   -install_name  /usr/local/lib/libSDL2-2.0.0.dylib -compatibility_version 2401 -current_version 2401.1 -Wl,-single_module


#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

int main(int argc, char** argv)
{
  NSLog(@"Testing");

  NSRect frame = NSMakeRect(0, 0, 200, 200);
  NSWindow* window  = [[[NSWindow alloc] initWithContentRect:frame
                        styleMask:NSBorderlessWindowMask
                        backing:NSBackingStoreBuffered
                        defer:NO] autorelease];
  [window setBackgroundColor:[NSColor blueColor]];
  [window makeKeyAndOrderFront:NSApp];

}


