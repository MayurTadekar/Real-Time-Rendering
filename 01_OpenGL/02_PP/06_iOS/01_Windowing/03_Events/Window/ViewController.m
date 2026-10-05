//
//  ViewController.m
//  Window
//
//  Created by Mayur Tadekar on 24/04/26.
//

#import "ViewController.h"

#import "MyView.h"

@implementation ViewController
{
    MyView *view;
}

 -(void)loadView
{
    // Dont call to super loadView if you want to create our own ViewController.
    
    // Code
    view = [[MyView alloc] initWithFrame:CGRectZero];
    
    [self setView:view];
}

- (void)viewDidLoad
{
    // Code
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

-(void)didReceiveMemoryWarning
{
    // Code
    [super didReceiveMemoryWarning];
    
}

-(void)dealloc
{
    // Code
    [super dealloc];
    
    [view release];
}

@end
