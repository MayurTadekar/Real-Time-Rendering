#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyView : NSView
@end

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];

    [pool release];
    return 0;
}

@implementation AppDelegate
{
    NSWindow *window;
    MyView   *view;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSRect winRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);

    window = [[NSWindow alloc] initWithContentRect:winRect
                                        styleMask:NSWindowStyleMaskTitled |
                                                  NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable |
                                                  NSWindowStyleMaskResizable
                                          backing:NSBackingStoreBuffered
                                            defer:NO];

    [window setTitle:@"MST : macOS Window"];
    [window center];

    view = [[MyView alloc] initWithFrame:winRect];
    [window setContentView:view];

    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)windowWillClose:(NSNotification *)notification
{
    [NSApp terminate:self];
}

-(void)dealloc
{
    [view release];
    [window release];
    [super dealloc];
}

@end

@implementation MyView
{
    NSString *text;
}

-(id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        text = [@"Press any key or click mouse" copy];

        [self setWantsLayer:YES];
        [[self layer] setBackgroundColor:[[NSColor blackColor] CGColor]];
    }
    return self;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)drawRect:(NSRect)dirtyRect
{
    [[NSColor blackColor] setFill];
    NSRectFill(dirtyRect);

    NSFont *font = [NSFont fontWithName:@"Helvetica" size:32.0];
    NSColor *color = [NSColor greenColor];

    NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
                           font, NSFontAttributeName,
                           color, NSForegroundColorAttributeName,
                           nil];

    NSSize textSize = [text sizeWithAttributes:attrs];

    NSPoint point;
    point.x = (dirtyRect.size.width  - textSize.width)  * 0.5;
    point.y = (dirtyRect.size.height - textSize.height) * 0.5;

    [text drawAtPoint:point withAttributes:attrs];
}

-(void)keyDown:(NSEvent *)event
{
    unichar key = [[event characters] characterAtIndex:0];

    [text release];
    text = [[NSString stringWithFormat:@"Key pressed : %c", key] copy];

    [self setNeedsDisplay:YES];

    if(key == 27)   // ESC
    {
        [NSApp terminate:self];
    }
}

-(void)mouseDown:(NSEvent *)event
{
    [text release];
    text = [@"Left Mouse Button Clicked" copy];

    [self setNeedsDisplay:YES];
}

-(void)dealloc
{
    [text release];
    [super dealloc];
}

@end
