# SoftRenderer
![](https://badgen.net/badge/language/Java/green)![](https://badgen.net/badge/language/C++/green)![](https://badgen.net/badge/language/CSharp/green)
![](https://badgen.net/badge/Platform/Android/cyan) ![](https://badgen.net/badge/Platform/iOS/cyan) ![](https://badgen.net/badge/Platform/Unity/cyan) ![](https://badgen.net/badge/Platform/Unreal/cyan)



## 前言
程序员的三大浪漫，编译原理、图形学、操作系统，所以我也有一种情节，想去学图形学。然后我拿起图形学的书，看完了第一章 似懂非懂，然后书又沾灰了。过了一段时间工作上遇到渲染管线相关的问题，然后又开始去网上看博客，看完之后也是似懂非懂，你是否也有跟我一样的经历了？然后看到了一篇文



## 一、Unreal工程
### 1、使用了Unreal的什么接口？
UMainUICpp里面有个MainImage 每帧会把软渲染器的数据显示到MainImage中，除此之外，没使用Unreal的其他接口。

### 2、使用
打开工程，打开MainUILevel关卡，然后运行就会如下显示
![](Img/2020-06-22-09-08-07.png)

### 3、文档
具体看代码的注释：
https://github.com/wlxklyh/SoftRenderer/blob/master/Unreal/Source/Graphic/SoftRenderer/ScreenDevice.h

部分注释的截图
![](Img/2020-06-22-09-12-06.png)


## 二、Unity工程

### 1、使用了Unity的什么接口？
MainScene下面有个Canvas Canvas有个Image,MainCode.cs脚本每帧会调用软渲染器的Draw,然后把FrameBuffer数据取出来 调用Unity的Texture2D.SetPixel接口 然后用Image把这个Texture2d显示出来。

### 2、使用
用Unity打开工程，然后打开MainScene 然后点击运行
![](Img/![](Img/UnityDemo.gif).png)


### 3、文档
具体看代码的注释：https://github.com/wlxklyh/SoftRenderer/blob/master/Unity/Assets/MainCode.cs

![](Img/2020-06-22-20-30-25.png)
## 三、android工程
此工程给Android Programmer或者Java Programmer 想了解图形学、渲染管线的coder 阅读。
下图是渲染一个正方体的效果
![](img/../Img/2020-09-07-15-07-21.png)
工程位置：https://github.com/wlxklyh/SoftRenderer/tree/master/Ado
主代码和注释：https://github.com/wlxklyh/SoftRenderer/blob/master/Ado/app/src/main/java/com/wlxklyh/softrenderer/FirstFragment.java
## 四、iOS工程
![](Img/2020-09-16-20-43-34.png)
## 五、TODO
后续要用C++跨平台。
- 三角形裁剪完善
- 几何着色器和片元着色器完善
- BRDF光照模型
- 模板测试


## 附
**欢迎加微信讨论（备注Gituhb）**
![](Img/2020-08-28-14-03-04.png)