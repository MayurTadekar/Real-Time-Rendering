//
//  MyView.h
//  Window
//
//  Created by Mayur Tadekar on 25/04/26.
//

#import <UIKit/UIKit.h>

@interface GLESView : UIView <UIGestureRecognizerDelegate>
-(void)startDisplayLink;
-(void)stopDisplayLink;
@end
