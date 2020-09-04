//
//  ViewController.m
//  SoftRenderer
//
//  Created by wlxk on 2020/9/4.
//  Copyright © 2020 hawklin. All rights reserved.
//

#import "ViewController.h"

@interface ViewController (){
    
}

@property UIImageView * ContentView;
@end

@implementation ViewController

@synthesize ContentView;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSLog(@"View Did Load");
    self.view.backgroundColor = [UIColor greenColor];
    // Do any additional setup after loading the view.
    self.ContentView = [[UIImageView alloc] initWithFrame:CGRectMake(100, 100, 200, 200)];
    self.ContentView.backgroundColor = [UIColor redColor];
    [self.view addSubview:self.ContentView];
    
}


@end
