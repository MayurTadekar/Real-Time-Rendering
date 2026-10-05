//
//  main.mm
//  Window
//
//  Created by Mayur Tadekar on 24/04/26.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[])
{
    // Code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString *delegateClassName = NSStringFromClass([AppDelegate class]);
    int result = UIApplicationMain(argc, argv, nil, delegateClassName);
    
    [pool release];
    
    return(result);
}
