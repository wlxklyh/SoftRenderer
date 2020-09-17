# SoftRenderer
![](https://badgen.net/badge/language/Java/green)![](https://badgen.net/badge/language/C++/green)![](https://badgen.net/badge/language/CSharp/green)
![](https://badgen.net/badge/Platform/Android/cyan) ![](https://badgen.net/badge/Platform/iOS/cyan) ![](https://badgen.net/badge/Platform/Unity/cyan) ![](https://badgen.net/badge/Platform/Unreal/cyan)

Android、iOS、Unity、Unreal都可以运行 
---

|平台|语言|效果|工程路径|
|-|-|-|-|
|![](https://badgen.net/badge/Platform/Android/cyan) |![](https://badgen.net/badge/language/Java/green) |https://github.com/wlxklyh/SoftRenderer/tree/master/Ado|![](Img/2020-09-17-12-49-22.png)|
|![](https://badgen.net/badge/Platform/iOS/cyan) |![](https://badgen.net/badge/language/C++/green) |https://github.com/wlxklyh/SoftRenderer/tree/master/iOS|![](Img/2020-09-17-12-49-30.png)|
|![](https://badgen.net/badge/Platform/Unity/cyan) |![](https://badgen.net/badge/language/CSharp/green) |https://github.com/wlxklyh/SoftRenderer/tree/master/Unity|![](Img/2020-09-17-12-49-37.png)|
|![](https://badgen.net/badge/Platform/Unreal/cyan) |![](https://badgen.net/badge/language/C++/green) |https://github.com/wlxklyh/SoftRenderer/tree/master/Unreal|![](Img/2020-09-17-12-49-41.png)|

## 前言
程序员的三大浪漫，编译原理、图形学、操作系统，所以我也有一种情节，想去学图形学。然后我拿起图形学的书，看完了第一章 似懂非懂，然后书又沾灰了。过了一段时间工作上遇到渲染管线相关的问题，然后又开始去网上看博客，看完之后也是似懂非懂，你是否也有跟我一样的经历了？Opengl的接口背后是什么呢？渲染管线怎么用程序表达。于是我找到一个skywind3000的软渲染器，然后自己再实现一遍，这样之后对渲染管线的理解更加深刻了。

通过代码来了解渲染管线 不调用图形库，ado、ios、unity、unreal都可以运行 

## 一、渲染管线

### 渲染管线流程图
```cpp
/*
 *  1、最简单的渲染流水线:
 *       分成CPU阶段和GPU阶段
 *      +--------------+     +-------------+
 *      |              |     |             |
 *      |     CPU      +----->     GPU     |
 *      |              |     |             |
 *      +--------------+     +-------------+
 *
 *  2、其中CPU阶段就是应用阶段  GPU阶段包括了几何阶段和光栅化阶段
 *      +--------------+     +-----------------+  +----------------+   +----------------+
 *      |              |     |                 |  |                |   |                |
 *      |   应用阶段    +----->     几何阶段      +-->      光栅化     +--->     像素处理     |
 *      |              |     |                 |  |                |   |                |
 *      +--------------+     +-----------------+  +----------------+   +----------------+
 *
 *  3、几何阶段：
 *      +--------------+     +-----------------+  +------------------+   +-------------+  +-------------+
 *      |              |     |                 |  |                  |   |             |  |             |
 *      |  顶点着色器    +----->  曲面细分着色器    +-->     几何着色器     +--->    裁剪     |-->    屏幕投射  |
 *      |              |     |                 |  |                  |   |             |  |             |
 *      +--------------+     +-----------------+  +------------------+   +-------------+  +-------------+
 *
 *  4、光栅化阶段：
 *      +--------------+     +--------------+  +------------------+
 *      |              |     |              |  |                  |
 *      |  三角形遍历    +----->  三角形设置    +-->     片元着色器    |
 *      |              |     |              |  |                  |
 *      +--------------+     +--------------+  +------------------+
 *
 *  5、像素处理阶段：
 *        深度测试ZTest
 *        颜色混合
 *      模板测试（模板缓冲）
 *
 ```
### 工程调用堆栈
```cpp
 *    【说明】：下面的代码根据上面的流水线来讲解和划分
 *    绘制调用堆栈：
 *        HScreenDevice::Draw
 *            HScreenDevice::ClearScreen                                    清屏
 *            HCube::Draw                                                    Cube绘制
 *                HCube::DrawBox                                            立方体绘制
 *                    HCube::DrawPlane                                    长方形绘制
 *                        HCube::DrawTriangle                                三角形绘制
 *
 *                            HCube::UpdateMVPMat()                            1、更新MVP矩阵                           -|
 *                            HCube::vert()                                    2、顶点着色器 之后就是裁剪空间坐标了           |
 *                                                                             3、曲面细分着色器 几何着色器【TODO】          |--->几何阶段
 *                            HCube::CheckTriangleInCVV()                      4、裁剪 检查在不在裁剪空间里面                |
 *                            HCube::CalTriangleScreenSpacePos()               5、屏幕投射                               -|
 *
 *                            HCube::InitTriangleInterpn()                     1、插值初始化 后面透视校正用               -|
 *                            Triangle::CalculateTrap() DrawTrap DrawScanline  2、三角形设置、三角形遍历 得到片元信息        |--->光栅化阶段
 *                            HCube::frag                                      3、片元着色器                            -|
 *
 *                            ZTest Zwrite
```
## 二、工程说明
### Ado
- 基本介绍：
用了ImageView来显示HScreenDevice的buffer，主要去看FirstFragment这个的代码。

- 如何打开工程：
用AndroudStudio打开

- 主代码：
具体看代码的注释：
https://github.com/wlxklyh/SoftRenderer/blob/master/Ado/app/src/main/java/com/wlxklyh/softrenderer/FirstFragment.java

### iOS
- 基本介绍：
用了UIImage来显示HScreenDevice的buffer，主要去看SoftRenderer.h这个的代码。

- 如何打开工程：
xcode打开工程

- 主代码：
具体看代码的注释：
https://github.com/wlxklyh/SoftRenderer/blob/master/iOS/SoftRenderer/SoftRenderer/SoftRenderer.h

### unity
- 基本介绍：
MainScene下面有个Canvas Canvas有个Image,MainCode.cs脚本每帧会调用软渲染器的Draw,然后把FrameBuffer数据取出来 调用Unity的Texture2D.SetPixel接口 然后用Image把这个Texture2d显示出来。

- 如何打开工程：
Unity打开工程 MainScene

- 主代码：
具体看代码的注释：
https://github.com/wlxklyh/SoftRenderer/blob/master/Unity/Assets/MainCode.cs

### Unreal
- 基本介绍：
UMainUICpp里面有个MainImage 每帧会把软渲染器的数据显示到MainImage中，除此之外，没使用Unreal的其他接口。

- 如何打开工程：
打开工程，打开MainUILevel关卡，然后运行就会如下显示

- 主代码
具体看代码的注释：
https://github.com/wlxklyh/SoftRenderer/blob/master/Unreal/Source/Graphic/SoftRenderer/ScreenDevice.h

## 三、TODO
后续要用C++跨平台。
- 三角形裁剪完善
- 几何着色器和片元着色器完善
- BRDF光照模型
- 模板测试


## 附
**欢迎加微信讨论（备注Gituhb）**
![](Img/2020-08-28-14-03-04.png)