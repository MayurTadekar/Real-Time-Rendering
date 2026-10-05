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

void WriteLog(NSString *message)
{
    // Path to .app bundle
    NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
    
    // Directory where .app is stored
    NSString *appParentDir = [bundlePath stringByDeletingLastPathComponent];
    
    // Final log file path
    NSString *logFilePath = [appParentDir stringByAppendingPathComponent:@"mst_log.txt"];
    
    NSString *finalMessage = [NSString stringWithFormat:@"%@ : %@\n",
                              [NSDate date], message];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:logFilePath])
    {
        [finalMessage writeToFile:logFilePath
                       atomically:YES
                         encoding:NSUTF8StringEncoding
                            error:nil];
    }
    else
    {
        NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:logFilePath];
        [fileHandle seekToEndOfFile];
        [fileHandle writeData:[finalMessage dataUsingEncoding:NSUTF8StringEncoding]];
        [fileHandle closeFile];
    }
}

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
    
    WriteLog(@"Application Did Finish Launching");
}

-(void)applicationWillTerminate:(NSNotification *)notification
{
    // Code
    WriteLog(@"Application Will Terminate");
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
    
    if(self)
    {
        [[self window] setContentView:self];
        
        text = @"Hello, World!!!";
        
        [self setWantsLayer:YES];
        NSColor *backgroundColor = [NSColor blackColor];
        struct CGColor *bgColor = [backgroundColor CGColor];
        [[self layer]setBackgroundColor: bgColor];
        
        WriteLog(@"MyView initWithFrame called");
    }
    
    return(self);
}

-(void)drawRect:(NSRect)dirtyRect
{
    // Code
    NSFont *textFont = [NSFont fontWithName:@"Helvetica"
                                       size:32];
    NSColor *textColor = [NSColor colorWithDeviceRed:0.0
                                              green:1.0
                                               blue:0.0
                                              alpha:1.0];
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:textFont, NSFontAttributeName,
                                     textColor, NSForegroundColorAttributeName,
                                     nil];
    NSSize textSize = [text sizeWithAttributes:dictionary];

    NSPoint point;
    point.x = (dirtyRect.size.width / 2) - (textSize.width / 2);
    point.y = (dirtyRect.size.height / 2) - (textSize.height / 2) + 12;
    
    [text drawAtPoint:point
       withAttributes:dictionary];
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
            text = @"'f' or 'F' key is pressed";
            [[self window] toggleFullScreen:self];
            break;
            
        default:
            break;
    }
    
    WriteLog([NSString stringWithFormat:@"Key Pressed: %d", key]);
}

-(void)mouseDown:(NSEvent *)event
{
    // Code
    text = @"Left Mouse Button is clicked";

    WriteLog(@"Mouse Left Button Clicked");

    [self setNeedsDisplay:YES];
}

-(void)dealloc
{
    // Code
    [super dealloc];
}

@end
