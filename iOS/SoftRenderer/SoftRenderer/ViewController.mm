//
//  ViewController.m
//  SoftRenderer
//
//  Created by wlxk on 2020/9/4.
//  Copyright © 2020 hawklin. All rights reserved.
//

#import "ViewController.h"
#include "SoftRenderer.h"
@interface ViewController (){
    
}
- (UIImage*)convertBufferToUIImage:(void*)rawImagePixels width:(int)width height:(int)height;

@property UIImageView * ContentView;
@end

@implementation ViewController

@synthesize ContentView;

HCube* shapeCube;

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
    int width = 400;
    int height = 400;
    HScreenDevice::GetInstance()->Init(width,height);
    shapeCube = new HCube();
    HMatrix mat1 = GetRotateMat(0, 0.8, 0.8);
    mat1.m[3][0] = 2;
    mat1.m[2][0] = 0;
    mat1.m[1][0] = 0;
    shapeCube->Transform.ModleMat = mat1;

    HScreenDevice::GetInstance()->shapeVec.push_back(shapeCube);
    HScreenDevice::GetInstance()->Draw();
    
    self.ContentView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    
    char* rawImagePixels = new char[width*height*4];
    for (int i = 0; i < HScreenDevice::GetInstance()->ScreenHeight; i++)
    {
        for (int j = 0; j < HScreenDevice::GetInstance()->ScreenWidth; j++)
        {
            rawImagePixels[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 0] = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 0];
            rawImagePixels[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 1] = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 1];
            rawImagePixels[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 2] = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 2];
            rawImagePixels[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 3] = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 3];
        }
    }
    
    

    UIImage* image = [self convertBufferToUIImage: rawImagePixels width : width height :height];
    self.ContentView.image = image;
    
    //free(rawImagePixels);
    [self.view addSubview:self.ContentView];
}


- (void)dealloc {
    NSLog(@"========   释放： dealloc   =======\n");
    HScreenDevice::GetInstance()->shapeVec.clear();
    if(shapeCube)
    {
        free(shapeCube);
    }
}


void dataProviderReleaseCallback (void *info, const void *data, size_t size)
{
    free((void *)data);
}



@end
