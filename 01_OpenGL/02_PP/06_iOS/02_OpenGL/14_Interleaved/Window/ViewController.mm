#import "ViewController.h"
#import "GLESView.h"

@implementation ViewController
{
    GLESView *view;
}

-(void)viewDidLoad
{
    // code
    [super viewDidLoad];
}

-(void)didReceiveMemoryWarning
{
    // code
    [super didReceiveMemoryWarning];
}

-(void)dealloc
{
    // code
    [view release];
    
    [super dealloc];
}
@end
