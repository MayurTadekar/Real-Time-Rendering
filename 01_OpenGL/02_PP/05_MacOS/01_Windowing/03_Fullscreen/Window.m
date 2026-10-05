#import <Foundation/Foundation.h>   // like stdio.h
#import <Cocoa/Cocoa.h>     // like windows.h

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyView : NSView
@end
/*
        
        obj.add(10, 20)
 
        [ obj add: 10, 20];
 
    Date *dptr = new Date();
 
    Date *dptr = [[Date alloc] init];
 
 
 */

int main(int argc, char *argv[])
{
    // Code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSApp = [NSApplication sharedApplication];
    
    [NSApp setDelegate: [[AppDelegate alloc] init]];
    
    [NSApp run];
    
    [pool release];
    
    return(0);
}

@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyView  *view;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    // Code
    NSRect winRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    window = [[NSWindow alloc]initWithContentRect: winRect
                                        styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                          backing:NSBackingStoreBuffered
                                            defer:NO];
    
    [window setTitle: @"MST: macOS Window"];
    [window center];
    
    view = [[MyView alloc] initWithFrame:winRect];
    
    [window setContentView:view];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)notification
{
    // Code
}

-(void)windowWillClose:(NSNotification *)notification
{
    // Code
    [NSApp terminate: self];
}

-(void)dealloc
{
    // Code
    [view release];
    
    [window release];
    
    [super dealloc];
}
@end

@implementation MyView
{
    @private
    NSString *text;
}


-(id)initWithFrame:(NSRect)frame
{
    // Code
    self = [super initWithFrame:frame];
    
    return(self);
}

-(void)drawRect:(NSRect)dirtyRect
{
    // Code
}

-(BOOL)acceptsFirstResponder
{
    // Code
    [[self window]makeFirstResponder:self];
    
    return(YES);
}

-(void)keyDown:(NSEvent*)event
{
    // Code
    int key = (int)[[event characters] characterAtIndex:0];
    
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate:self];
            break;
            
        case 'f':
        case 'F':
            [[self window] toggleFullScreen:self];
            break;
            
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)event
{
    // Code
}

-(void)dealloc
{
    // Code
    [super dealloc];
}

@end
