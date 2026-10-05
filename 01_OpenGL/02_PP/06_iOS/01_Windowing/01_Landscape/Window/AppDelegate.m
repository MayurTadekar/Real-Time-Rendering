//
//  AppDelegate.m
//  Window
//
//  Created by Mayur Tadekar on 24/04/26.
//

#import "AppDelegate.h"

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Code
    // Override point for customization after application launch.
    return(YES);
}

- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options
{
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    
    // Code
    
    return([[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role]);
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions
{
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
    
    // Code
}

-(void)dealloc
{
    // Code
    [super dealloc];
}


@end
