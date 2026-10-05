//
//  SceneDelegate.m
//  Window
//
//  Created by Mayur Tadekar on 05/03/26.
//

#import "SceneDelegate.h"
#import "GLESView.h"


@implementation SceneDelegate
{
    GLESView *view;
}


- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions {
    
    view = (GLESView *)[[[self window]rootViewController]view];
    if([view isKindOfClass:[GLESView class]])
    {
        [view startDisplayLink];
    }
}


- (void)sceneWillResignActive:(UIScene *)scene {
    if([view isKindOfClass:[GLESView class]])
    {
        [view stopDisplayLink];
    }
}


- (void)sceneDidBecomeActive:(UIScene *)scene {
    if([view isKindOfClass:[GLESView class]])
    {
        [view startDisplayLink];
    }
}


- (void)sceneWillEnterForeground:(UIScene *)scene {
    
}

- (void)sceneDidEnterBackground:(UIScene *)scene {
   
}


- (void)sceneDidDisconnect:(UIScene *)scene {
    if([view isKindOfClass:[GLESView class]])
    {
        [view stopDisplayLink];
    }
}

-(void)dealloc{
    [super dealloc];
}
@end
