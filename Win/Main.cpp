#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include "ScreenDevice.h"
#include "graphics.h"

int main() {
    //（0）准备一个图片文件 输出到这个图片文件查看渲染结果
    std::ofstream out;
    out.open("OutputPic.ppm");

    //（1）宽高

    int PicW = 640;
    int PicH = 960;
    HScreenDevice::GetInstance()->Init(PicW,PicH);
    HScreenDevice::GetInstance()->shape = new HCube();

    //（2）这个是ppm图片格式 后面渲染的结果用这个来显示
    out << "P3\n" << PicW << " " << PicH << "\n255\n";

    //（3）绘制 然后把绘制结果输出到图片文件里面
    initgraph(PicW, PicH);	// 创建绘图窗口
    HScreenDevice::GetInstance()->Draw();
    for (int i = 0; i < PicH; i++)
    {
        for (int j = 0; j < PicW; j++)
        {
            int ir = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 0];;
            int ig = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 1];;
            int ib = HScreenDevice::GetInstance()->FrameBuff[(i * HScreenDevice::GetInstance()->ScreenWidth + j) * 4 + 2];;
            out << ir << " " << ig << " " << ib << "\n";

            putpixel(j, i, (((int)ir) << 0) + (((int)ig) << 8) + (((int)ib) <<16));
        }
    }


    //DebugDrawSocView();
    

    out.close();

    //（4）打开结果图片 （分平台）
#ifdef _WIN32
    system(" start .\\OutputPic.ppm");
#elif __APPLE__

#include "TargetConditionals.h"

#if TARGET_IPHONE_SIMULATOR
    // iOS Simulator
#elif TARGET_OS_IPHONE
    // iOS device
#elif TARGET_OS_MAC
    // Other kinds of Mac OS
    FILE *pp = popen("open OutputPic.ppm", "r");
#else
#   error "Unknown Apple platform"
#endif
#elif __ANDROID__
    // android
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

    while (!_kbhit())
    {
    }
    _getch();
    closegraph();			

    return 0;
}