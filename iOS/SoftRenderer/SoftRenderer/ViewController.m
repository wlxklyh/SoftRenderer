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
- (UIImage*)convertBufferToUIImage:(void*)rawImagePixels width:(int)width height:(int)height;

@property UIImageView * ContentView;
@end

@implementation ViewController

@synthesize ContentView;

- (UIImage*)convertBufferToUIImage:(void*)rawImagePixels width:(int)width height:(int)height
{
    int totalBytesForImage = width * height * 4;
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, rawImagePixels, totalBytesForImage, dataProviderReleaseCallback);
    CGColorSpaceRef defaultRGBColorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImageFromBytes = CGImageCreate(width, height, 8, 32, 4 * width, defaultRGBColorSpace, kCGBitmapByteOrderDefault | kCGImageAlphaLast, dataProvider, NULL, NO, kCGRenderingIntentDefault);
    CGDataProviderRelease(dataProvider);
    CGColorSpaceRelease(defaultRGBColorSpace);
    UIImage *image = [UIImage imageWithCGImage:cgImageFromBytes];
    return image;

}


- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    self.view.backgroundColor = [UIColor blueColor];
    
    self.ContentView = [[UIImageView alloc] initWithFrame:CGRectMake(100, 100, 200, 200)];
    
    char* rawImagePixels = malloc(200*200*4);
    for(int i=0;i<200;i++)
    {
        for (int j=0; j<200; j++) {
            rawImagePixels[(i*200+j)*4+0] = 0;
            rawImagePixels[(i*200+j)*4+1] = 255;
            rawImagePixels[(i*200+j)*4+2] = 0;
            rawImagePixels[(i*200+j)*4+3] = 255;
        }
    }
    UIImage* image = [self convertBufferToUIImage: rawImagePixels width : 200 height :200];
    self.ContentView.image = image;
    
    //free(rawImagePixels);
    [self.view addSubview:self.ContentView];
}



void dataProviderReleaseCallback (void *info, const void *data, size_t size)
{
    free((void *)data);
}



@end
