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
    float RotateValue;
    NSTimer *MainThreadSchduledTimer;
}
- (UIImage*)convertBufferToUIImage:(void*)rawImagePixels width:(int)width height:(int)height;

@property UIImageView * ContentView;
@property float RotateValue;
@property UIImage* ImageContent;
@end

@implementation ViewController

@synthesize ContentView;
@synthesize RotateValue;
@synthesize ImageContent;

std::vector<HCube*> shapeCubeVec;

char* rawImagePixels = nullptr;
int width = 400;
int height = 400;

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



- (void)OnFrameUpdate {
    //旋转立方体
    self.RotateValue = self.RotateValue + 0.01;
    for (int i=0; i<shapeCubeVec.size(); i++) {
        shapeCubeVec[i]->Transform.ModleMat = GetRotateMat(0, self.RotateValue, self.RotateValue);
    }
    //绘制
    HScreenDevice::GetInstance()->Draw();
    //得到FBO
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
    
    //更新UI
    ImageContent = [self convertBufferToUIImage: rawImagePixels width : width height :height];
    self.ContentView.image = ImageContent;
    
}


- (void)viewDidLoad {
    NSLog(@"========   viewDidLoad   =======\n");
    [super viewDidLoad];

    self.RotateValue = 0;
    self.view.backgroundColor = [UIColor blueColor];
    
    //软渲染器初始化
    HScreenDevice::GetInstance()->Init(width,height);

    HCube * shapeCube = new HCube();
    shapeCube->Transform.ModleMat = GetRotateMat(0, self.RotateValue, self.RotateValue);
    shapeCubeVec.push_back(shapeCube);
    HScreenDevice::GetInstance()->shapeVec.push_back(shapeCube);
    

    
    rawImagePixels = new char[width*height*4];
    
    //添加View
    self.ContentView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    [self.view addSubview:self.ContentView];
    
    //渲染帧
    MainThreadSchduledTimer = [NSTimer scheduledTimerWithTimeInterval:0.016 target:self selector:@selector(OnFrameUpdate) userInfo:nil repeats:YES];

}


- (void)viewDidUnload {
    NSLog(@"========   释放： dealloc   =======\n");
    if(MainThreadSchduledTimer)
    {
        [MainThreadSchduledTimer invalidate];
        MainThreadSchduledTimer = nil;
    }

    for(int i=0;i<shapeCubeVec.size();i++)
    {
        free(shapeCubeVec[i]);
    }

    if(rawImagePixels)
    {
        free(rawImagePixels);
        rawImagePixels = nullptr;
    }
}


void dataProviderReleaseCallback (void *info, const void *data, size_t size)
{
    free((void *)data);
}



@end
