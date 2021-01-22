package com.wlxklyh.softrenderer;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.graphics.Bitmap;

import java.util.ArrayList;
import java.util.List;


//软渲染器参考：https://github.com/skywind3000/mini3d
//mini3d笔记：https://zhuanlan.zhihu.com/p/74510058
//图形学流水线文章：https://positiveczp.github.io/%E7%BB%86%E8%AF%B4%E5%9B%BE%E5%BD%A2%E5%AD%A6%E6%B8%B2%E6%9F%93%E7%AE%A1%E7%BA%BF.pdf
//意图：
//1、增加了注释为了更好的理解  mini3d  写的很精简 看懂了之后对渲染流水线更加深刻了
//2、同是对图形学感兴趣的朋友看了可以更好的理解，刚开始看图形学 没有代码总感觉很虚。
//备注：
//C++代码大部分是值传递 没有使用指针 纯为了展示mini3d的过程

/*
 *  1、最简单的渲染流水线:
 *	   分成CPU阶段和GPU阶段
 *      +--------------+     +-------------+
 *      |              |     |             |
 *      |     CPU      +----->     GPU     |
 *      |              |     |             |
 *      +--------------+     +-------------+
 *
 *	2、其中CPU阶段就是Application 应用阶段  GPU阶段包括了几何阶段和光栅化阶段
 *      +--------------+     +-----------------+  +----------------+   +----------------+
 *      |              |     |                 |  |                |   |                |
 *      |   应用阶段    +----->     几何阶段     +-->      光栅化     +--->     像素处理    |
 *      |              |     |                 |  |                |   |                |
 *      +--------------+     +-----------------+  +----------------+   +----------------+
 *
 *  3、几何阶段：
 *		+--------------+     +-----------------+  +------------------+   +-------------+  +-------------+
 *      |              |     |                 |  |                  |   |             |  |             |
 *      |  顶点着色器   +----->   曲面细分着色器  +-->     几何着色器     +--->    裁剪     |-->    屏幕投射   |
 *      |              |     |                 |  |                  |   |             |  |             |
 *      +--------------+     +-----------------+  +------------------+   +-------------+  +-------------+
 *
 *  4、光栅化阶段：
 *		+--------------+     +--------------+  +------------------+
 *      |              |     |              |  |                  |
 *      |  三角形遍历   +----->  三角形设置    +-->   片元着色器      |
 *      |              |     |              |  |                  |
 *      +--------------+     +--------------+  +------------------+
 *
 *  5、像素处理阶段：
 *		深度测试ZTest
 *		颜色混合
 *      模板测试（模板缓冲）
 *
 *	【说明】：下面的代码根据上面的流水线来讲解和划分
 *	绘制调用堆栈：
 *		HScreenDevice::Draw
 *			HScreenDevice::ClearScreen									清屏
 *			HCube::Draw													Cube绘制
 *				HCube::DrawBox											立方体绘制
 *					HCube::DrawPlane									长方形绘制
 *						HCube::DrawTriangle								三角形绘制
 *
 *							HCube::UpdateMVPMat()							1、更新MVP矩阵					   -|
 *							HCube::vert()									2、顶点着色器 之后就是裁剪空间坐标了		|
 *																			3、曲面细分着色器 几何着色器【TODO】		|--->几何阶段
 *							HCube::CheckTriangleInCVV()						4、裁剪 检查在不在裁剪空间里面			|
 *							HCube::CalTriangleScreenSpacePos()				5、屏幕投射						   -|
 *
 *							HCube::InitTriangleInterpn()					1、插值初始化 后面透视校正用			   -|
 *							Triangle::CalculateTrap() DrawTrap DrawScanline 2、三角形设置、三角形遍历 得到片元信息		|--->光栅化阶段
 *							HCube::frag										3、片元着色器						   -|
 *
 *							ZTest Zwrite
 *
 *
 */




//===========================数学工具 Begin=============================

class MathHelper
{
    public static void GetRGBAFromInt(int color,Integer  r,Integer g,Integer b,Integer a)
    {
        r = (Integer)(color / 255 / 255 / 255);
        g = (Integer)((color - r * 255 * 255 * 255)/(255 * 255));
        b = (Integer)((color % (255 * 255))/ 255);
        a = (Integer)(color % (255));
    }
    //插值函数   t为[0,1]之间
    public static float Interp(float x1, float x2, float t)
    {
        return x1 + (x2 - x1) * t;
    }
    //Clamp函数 Value
    public static int Clamp(int x, int min, int max)
    {
        return (x < min) ? min : ((x > max) ? max : x);
    }
    //获取单位矩阵
    public static HMatrix GetIdentityMat() {
        HMatrix matRet = new HMatrix();
        matRet.m[0][0] = matRet.m[1][1] = matRet.m[2][2] = matRet.m[3][3] = 1.0f;
        matRet.m[0][1] = matRet.m[0][2] = matRet.m[0][3] = 0.0f;
        matRet.m[1][0] = matRet.m[1][2] = matRet.m[1][3] = 0.0f;
        matRet.m[2][0] = matRet.m[2][1] = matRet.m[2][3] = 0.0f;
        matRet.m[3][0] = matRet.m[3][1] = matRet.m[3][2] = 0.0f;
        return matRet;
    }
    //获取0矩阵
    public static HMatrix GetZeroMat() {
        HMatrix matRet = new HMatrix();
        matRet.m[0][0] = matRet.m[0][1] = matRet.m[0][2] = matRet.m[0][3] = 0.0f;
        matRet.m[1][0] = matRet.m[1][1] = matRet.m[1][2] = matRet.m[1][3] = 0.0f;
        matRet.m[2][0] = matRet.m[2][1] = matRet.m[2][2] = matRet.m[2][3] = 0.0f;
        matRet.m[3][0] = matRet.m[3][1] = matRet.m[3][2] = matRet.m[3][3] = 0.0f;
        return matRet;
    }
    //获取平移矩阵
    public static HMatrix GetTranslateMat(float x, float y, float z) {
        HMatrix matRet = new HMatrix();
        matRet = GetIdentityMat();
        matRet.m[3][0] = x;
        matRet.m[3][1] = y;
        matRet.m[3][2] = z;
        return matRet;
    }
    //获取缩放矩阵
    public static HMatrix GetScaleMat(float x, float y, float z) {
        HMatrix matRet = new HMatrix();
        matRet = GetIdentityMat();
        matRet.m[0][0] = x;
        matRet.m[1][1] = y;
        matRet.m[2][2] = z;
        return matRet;
    }
    //获取旋转矩阵 X轴旋转
    // https://blog.csdn.net/csxiaoshui/article/details/65446125
    public static HMatrix GetRotateMatX(float x) {
        HMatrix matRet = GetIdentityMat();//单位矩阵
        float SinValue = (float)Math.sin(x);
        float CosValue = (float)Math.cos(x);

        matRet.m[0][0] = 1; matRet.m[1][0] = 0;			matRet.m[2][0] = 0;			matRet.m[3][0] = 0;
        matRet.m[0][1] = 0; matRet.m[1][1] = CosValue;	matRet.m[2][1] = -SinValue; matRet.m[3][1] = 0;
        matRet.m[0][2] = 0; matRet.m[1][2] = SinValue;	matRet.m[2][2] = CosValue;	matRet.m[3][2] = 0;
        matRet.m[0][3] = 0; matRet.m[1][3] = 0;			matRet.m[2][3] = 0;			matRet.m[3][3] = 1;

        return matRet;
    }
    //获取旋转矩阵 Y轴旋转
    // https://blog.csdn.net/csxiaoshui/article/details/65446125
    public static HMatrix GetRotateMatY(float y) {
        HMatrix matRet = GetIdentityMat();//单位矩阵
        float SinValue = (float)Math.sin(y);
        float CosValue = (float)Math.cos(y);

        matRet.m[0][0] = CosValue;	matRet.m[1][0] = 0; matRet.m[2][0] = SinValue;	matRet.m[3][0] = 0;
        matRet.m[0][1] = 0;			matRet.m[1][1] = 1;	matRet.m[2][1] = 0;			matRet.m[3][1] = 0;
        matRet.m[0][2] = -SinValue;	matRet.m[1][2] = 0;	matRet.m[2][2] = CosValue;	matRet.m[3][2] = 0;
        matRet.m[0][3] = 0;			matRet.m[1][3] = 0;	matRet.m[2][3] = 0;			matRet.m[3][3] = 1;

        return matRet;
    }
    //获取旋转矩阵 Z轴旋转
    // https://blog.csdn.net/csxiaoshui/article/details/65446125
    public static HMatrix GetRotateMatZ(float z) {
        HMatrix matRet = GetIdentityMat();//单位矩阵
        float SinValue = (float)Math.sin(z);
        float CosValue = (float)Math.cos(z);

        matRet.m[0][0] = CosValue; matRet.m[1][0] = -SinValue; matRet.m[2][0] = 0;	matRet.m[3][0] = 0;
        matRet.m[0][1] = SinValue; matRet.m[1][1] = CosValue;  matRet.m[2][1] = 0;	matRet.m[3][1] = 0;
        matRet.m[0][2] = 0;		   matRet.m[1][2] = 0;		   matRet.m[2][2] = 1;	matRet.m[3][2] = 0;
        matRet.m[0][3] = 0;		   matRet.m[1][3] = 0;		   matRet.m[2][3] = 0;	matRet.m[3][3] = 1;

        return matRet;
    }
    //获取旋转矩阵 XYZ轴旋转
    public static HMatrix GetRotateMat(float x, float y, float z)
    {
        //X Y Z矩阵相乘 这里是为了好理解 但是这样做效率有浪费 6次三角函数 2次矩阵乘法
        HMatrix matRet = GetRotateMatX(x).Mul(GetRotateMatY(y)).Mul(GetRotateMatZ(z));
        return matRet;
    }
    //获取LookAt矩阵
    //相机的位置 相机的看着那个位置（决定相机方向） 相机上方位置
    // see:https://zhuanlan.zhihu.com/p/66384929
    // Rx Ry Rz 0
    // Ux Uy Uz 0
    // Dx Dy Dz 0
    // 0  0  0  1 相机空间是左手系
    public static HMatrix GetLookAtMat(HVector camera, HVector at, HVector up)
    {
        HMatrix matRet = new HMatrix();
        HVector CameraXAxis, CameraYAxis, CameraZAxis;
        CameraZAxis = at.Sub(camera);
        CameraZAxis = CameraZAxis.Normalize();
        CameraYAxis = up.Normalize();
        CameraXAxis = CameraZAxis.CrossProduct(CameraYAxis);
        CameraXAxis = CameraXAxis.Normalize();

        matRet.m[0][0] = CameraXAxis.x;
        matRet.m[1][0] = CameraXAxis.y;
        matRet.m[2][0] = CameraXAxis.z;
        matRet.m[3][0] = -CameraXAxis.DotProduct(camera);

        matRet.m[0][1] = CameraYAxis.x;
        matRet.m[1][1] = CameraYAxis.y;
        matRet.m[2][1] = CameraYAxis.z;
        matRet.m[3][1] = -CameraYAxis.DotProduct(camera);

        matRet.m[0][2] = CameraZAxis.x;
        matRet.m[1][2] = CameraZAxis.y;
        matRet.m[2][2] = CameraZAxis.z;
        matRet.m[3][2] = -CameraZAxis.DotProduct(camera);

        matRet.m[0][3] = matRet.m[1][3] = matRet.m[2][3] = 0.0f;
        matRet.m[3][3] = 1.0f;
        return matRet;
    }
    //获取投影矩阵 乘以这个矩阵之后得到的是相机空间的坐标
    public static HMatrix GetPerspectiveMat(float fovy, float aspect, float zn, float zf)
    {
        float fax = 1.0f / (float)Math.tan(fovy * 0.5f);

        HMatrix matRet = GetZeroMat();
        matRet.m[0][0] = (float)(fax / aspect);
        matRet.m[1][1] = (float)(fax);
        matRet.m[2][2] = zf / (zf - zn);
        matRet.m[3][2] = -zn * zf / (zf - zn);
        matRet.m[2][3] = 1;
        return matRet;
    }
}
//===========================数学工具 End===============================


//形状接口
interface HShape
{
    void Draw();
};

//纹理  这里没有去读文件 直接在代码里面赋值了
class HTexture
{
    public HTexture()
    {
        //初始化纹理
        TextureW = 256;
        TextureH = 256;
        int i, j;
        for (j = 0; j < TextureH; j++) {
            for (i = 0; i < TextureW; i++) {
                int x = i / 32, y = j / 32;
                Texture[j][i] = (int)((((x + y) % 2) > 0) ? 0xffffffff : 0x3fbcefff);
            }
        }
    }

    public int TextureW, TextureH;
    public int Texture[][] = new int [256][256];

    //纹理采样：参考https://gameinstitute.qq.com/community/detail/115739
    //1、这里可以实现：最近点采样、Bilinear 4个点取均值、Trilinear加了MipMap这个维度
    public int RreadTexture(float u, float v) {
        int x, y;
        u = u * TextureW;
        v = v * TextureH;
        x = (int)(u + 0.5f);
        y = (int)(v + 0.5f);
        x = MathHelper.Clamp(x, 0, TextureW - 1);
        y = MathHelper.Clamp(y, 0, TextureH - 1);
        return Texture[y][x];
    }
};
//渲染设备
class  HScreenDevice
{
    private static final String TAG = "SoftRenderer";
    //============单例 Begin============
    public static HScreenDevice DeviceInstance;
    public static HScreenDevice GetInstance()
    {
        if (DeviceInstance == null)
        {
            DeviceInstance = new HScreenDevice();
        }
        return DeviceInstance;
    }
    //============单例 End============
    //渲染的形状
    public HShape shape;
    public HScreenDevice()
    {
        shape = null;
    }
    //屏幕分辨率宽
    public int ScreenWidth;
    //屏幕分辨率高
    public int ScreenHeight;
    //屏幕缓冲
    public List<Integer> FrameBuff = new ArrayList<Integer>();
    //深度缓冲 绘制过程ZTest ZWrite
    public List<Float>  DepthBuff = new ArrayList<Float>();
    public void Init(int width, int height)
    {
        //1、屏幕像素分辨率
        ScreenWidth = width;
        ScreenHeight = height;

        //2、屏幕缓冲和深度缓冲
        InitScreen();
    }
    // 清理屏幕
    public void InitScreen()
    {
        Log.i(TAG,"InitScreen");
        FrameBuff.clear();
        DepthBuff.clear();
        for (int i=0; i < ScreenWidth * ScreenHeight; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                FrameBuff.add((Integer)255);
            }
            DepthBuff.add(0.0f);
        }
    }

    // 清理屏幕
    public void ClearScreen()
    {
        Log.i(TAG,"ClearScreen");

        for (int i=0; i < ScreenWidth * ScreenHeight; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                FrameBuff.set(i * 4 + j,(Integer)255);
            }
            DepthBuff.set(i,0.0f);
        }
    }


    //=====================================================================
    // 主绘制函数
    //=====================================================================
    public void Draw()
    {
        //1、清理屏幕缓冲
        ClearScreen();
        //2、绘制一个图形
        if (shape!=null)
        {
            shape.Draw();
        }
    }
    //Test 测试把帧缓冲改成红色 测试下是否显示成红色
    public void InitTestRed()
    {
        for (int i = 0; i < ScreenHeight; i++)
        {
            for (int j = 0; j < ScreenWidth; j++)
            {
                //R
                Integer RByte = 255;
                FrameBuff.set((i * ScreenWidth + j) * 4, RByte);
                //G
                Integer GByte = 0;
                FrameBuff.set((i * ScreenWidth + j) * 4 + 1, GByte);
                //B
                Integer BByte = 0;
                FrameBuff.set((i * ScreenWidth + j) * 4 + 2, BByte);
                //A
                Integer AByte = 0;
                FrameBuff.set((i * ScreenWidth + j) * 4 + 3, AByte);
            }
        }
    }
};

//===========================几何阶段 Begin=============================
//H只是个前缀就跟OpenGL的GL一样
class HMatrix
{
    public HMatrix()
    {
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
        m[0][1] = m[0][2] = m[0][3] = 0.0f;
        m[1][0] = m[1][2] = m[1][3] = 0.0f;
        m[2][0] = m[2][1] = m[2][3] = 0.0f;
        m[3][0] = m[3][1] = m[3][2] = 0.0f;
    }
    /*  数据图示
     *  m[0,0],m[1,0],m[2,0],m[3,0],
     *  m[0,1],m[1,1],m[2,1],m[3,1],
     *  m[0,2],m[1,2],m[2,2],m[3,2],
     *  m[0,3],m[1,3],m[2,4],m[3,3],
     */
    public float m[][] = new float[4][4];
    //矩阵加法
    public HMatrix Add(HMatrix mat)
    {
        HMatrix matRet = new HMatrix();
        int i, j;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                matRet.m[i][j] = m[i][j] + mat.m[i][j];
            }
        }
        return matRet;
    }
    //矩阵减法
    public HMatrix Sub(HMatrix mat)
    {
        HMatrix matRet = new HMatrix();
        int i, j;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                matRet.m[i][j] = m[i][j] - mat.m[i][j];
            }
        }
        return matRet;
    }
    //矩阵乘法
    public HMatrix Mul(HMatrix mat)
    {
        HMatrix matRet = new HMatrix();
        int i, j;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                matRet.m[i][j] = (m[i][0] * mat.m[0][j]) +
                    (m[i][1] * mat.m[1][j]) +
                    (m[i][2] * mat.m[2][j]) +
                    (m[i][3] * mat.m[3][j]);
            }
        }
        return matRet;
    }
    //矩阵缩放
    public HMatrix Scale(float f)
    {
        HMatrix matRet = new HMatrix();
        int i, j;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                matRet.m[i][j] = m[i][j] * f;
            }
        }
        return matRet;
    }

    public boolean equals(HMatrix mat1,HMatrix mat2)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (mat1.m[i][j] != mat2.m[i][j])
                {
                    return false;
                }
            }
        }
        return true;
    }
};

//向量或者是点都是这个类便是
class HVector
{
    public HVector()
    {
        w = 1;
    }
    public HVector(float xp, float yp, float zp, float wp)
    {
        x = xp;
        y = yp;
        z = zp;
        w = wp;
    }


    //表示向量时w无用
    public float x, y, z, w;

    //向量长度
    public float Length() { return (float)Math.sqrt(x * x + y * y + z * z);}

    //向量加法
    public HVector Add(HVector vec)
    {
        HVector vecRet = new HVector();
        vecRet.w = 1;
        vecRet.x = x + vec.x;
        vecRet.y = y + vec.y;
        vecRet.z = z + vec.z;
        return vecRet;
    }

    //向量减法
    public HVector Sub(HVector vec)
    {
        HVector vecRet = new HVector();
        vecRet.w = 1;
        vecRet.x = x - vec.x;
        vecRet.y = y - vec.y;
        vecRet.z = z - vec.z;
        return vecRet;
    }

    //向量点乘 返回是值 A x B = |A||B|Cos
    public float DotProduct(HVector vec)
    {
        return  x * vec.x + y * vec.y + z * vec.z;
    }

    //向量叉乘 返回向量 右手螺旋决定方向
    public HVector CrossProduct(HVector vec)
    {
        HVector vecRet = new HVector();
        float m1, m2, m3;
        m1 = y * vec.z - z * vec.y;
        m2 = z * vec.x - x * vec.z;
        m3 = x * vec.y - y * vec.x;
        vecRet.x = m1;
        vecRet.y = m2;
        vecRet.z = m3;
        vecRet.w = 1.0f;
        return vecRet;
    }

    //向量插值
    public HVector InterpVec(HVector vec, float t)
    {
        HVector vecRet = new HVector();
        vecRet.x = MathHelper.Interp(x, vec.x, t);
        vecRet.y = MathHelper.Interp(y, vec.y, t);
        vecRet.z = MathHelper.Interp(z, vec.z, t);
        vecRet.w = 1.0f;
        return vecRet;
    }

    //向量归一
    public HVector Normalize()
    {
        HVector vecRet = new HVector();
        float len = Length();
        if (len != 0.0f)
        {
            vecRet.x = x / len;
            vecRet.y = y / len;
            vecRet.z = z / len;
        }
        return vecRet;
    }

    //向量乘矩阵
    public HVector MulMat(HMatrix mat)
    {
        HVector vec = new HVector();
        float X = x, Y = y, Z = z, W = w;
        vec.x = X * mat.m[0][0] + Y * mat.m[1][0] + Z * mat.m[2][0] + W * mat.m[3][0];
        vec.y = X * mat.m[0][1] + Y * mat.m[1][1] + Z * mat.m[2][1] + W * mat.m[3][1];
        vec.z = X * mat.m[0][2] + Y * mat.m[1][2] + Z * mat.m[2][2] + W * mat.m[3][2];
        vec.w = X * mat.m[0][3] + Y * mat.m[1][3] + Z * mat.m[2][3] + W * mat.m[3][3];
        return vec;
    }

    //检查齐次裁剪坐标 cvv canonical view volume
    public boolean CheckInCVV()
    {
        int check = 0;
        if (z < 0.0f) check |= 1;
        if (z > w) check |= 2;
        if (x < -w) check |= 4;
        if (x > w) check |= 8;
        if (y < -w) check |= 16;
        if (y > w) check |= 32;
        return check == 0;
    }

};


//描述一个模型的空间坐标转换的过程的类
class HTransform
{
    public HTransform()
    {
        Init();
    }

    //Unity 里面就是 Position Rotation Scale来构建这个矩阵 后面这里可以拆分下更好理解
    // ModelMatrix，就是将模型坐标变换到WorldMatrix的Matrix，WorldMatrix = Mt * Mr * Ms  ModleMatrix =  Mt * Mr * Ms
    public HMatrix ModleMat;
    // 世界坐标转到视锥体 转成相机坐标 View矩阵做转换
    public HMatrix ViewMat;
    // 投影矩阵 视锥体坐标乘以这个投影矩阵 就得到屏幕坐标
    public HMatrix ProjectionMat;
    //MVP 矩阵就是 ModleMat *  ViewMat * ProjectionMat
    public HMatrix MVPMat;


    // 更新投影矩阵
    public void UpdateMVPMat()
    {
        MVPMat = ModleMat.Mul(ViewMat).Mul(ProjectionMat);
    }

    public void Init()
    {
        int ScreenWidth = HScreenDevice.GetInstance().ScreenWidth;
        int ScreenHeight = HScreenDevice.GetInstance().ScreenHeight;
        ModleMat = MathHelper.GetIdentityMat();
        HVector camera = new HVector(5, 0, 0, 1);
        HVector at = new HVector(0, 0, 0, 1);
        HVector up = new HVector(0, 1, 0, 1);
        ViewMat = MathHelper.GetLookAtMat(
                camera,
                at,
                up
        );
        // fov = 90度 0.5pai
        ProjectionMat = MathHelper.GetPerspectiveMat(3.1415926f * 0.5f, (float)ScreenWidth / (float)ScreenHeight, 1.0f, 500.0f);
        UpdateMVPMat();
    }

    //坐标转换到屏幕坐标
    public HVector MulMVPMat(HVector Origin)
    {
        return Origin.MulMat(MVPMat);
    }

    //归一化 且屏幕坐标
    //宽->
    //高↓
    public HVector HomogenizeToScreenCoord(HVector Origin)
    {
        int ScreenWidth = HScreenDevice.GetInstance().ScreenWidth;
        int ScreenHeight = HScreenDevice.GetInstance().ScreenHeight;
        float rhw = 1.0f / Origin.w;
        HVector vecRet = new HVector();
        vecRet.x = (Origin.x * rhw + 1.0f) * ScreenWidth * 0.5f;
        vecRet.y = (1.0f - Origin.y * rhw) * ScreenHeight * 0.5f;
        vecRet.z = Origin.z * rhw;
        vecRet.w = 1.0f;
        return vecRet;
    }
};
//===========================几何阶段 End=============================



//===========================光栅化阶段 Begin=============================
//颜色 RGBA
class HColor
{
    public float r, g, b, a;
};

//纹理坐标 uc
class HTexcoord
{
    public float u, v;
};

//顶点信息
class HVertex
{
    public HVertex()
    {

    }

    public HVertex Copy()
    {
        HVertex HVertexRet = new HVertex();

        HVertexRet.pos.x = pos.x;
        HVertexRet.pos.y = pos.y;
        HVertexRet.pos.z = pos.z;
        HVertexRet.pos.w = pos.w;
        HVertexRet.uv.u = uv.u;
        HVertexRet.uv.v = uv.v;
        HVertexRet.color.r = color.r;
        HVertexRet.color.g = color.g;
        HVertexRet.color.b = color.b;
        HVertexRet.color.a = color.a;
        HVertexRet.rhw = rhw;

        return HVertexRet;
    }

    public HVertex(float x,float y,float z,float w,float u,float v,float r,float g,float b,float rhwp)
    {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        pos.w = w;
        uv.u = u;
        uv.v = v;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = 1;
        rhw = rhwp;
    }

    public HVector pos = new HVector();
    public HTexcoord uv = new HTexcoord();
    public HColor color = new HColor();
    public float rhw;

    //屏幕坐标的三角形插值的时候要初始化 rhw 做透视校正用 真正取值的时候乘以w
    public void Initrhw()
    {
        rhw = 1.0f / pos.w;

        uv.u *= rhw;
        uv.v *= rhw;

        color.r *= rhw;
        color.g *= rhw;
        color.b *= rhw;
        color.a *= rhw;
    }

    //插值屏幕坐标的顶点信息
    public HVertex InterpVertex(HVertex vertex, float t)
    {
        HVertex HVertexRet = new HVertex();
        HVertexRet.pos = pos.InterpVec(vertex.pos, t);
        HVertexRet.uv.u = MathHelper.Interp(uv.u, vertex.uv.u, t);
        HVertexRet.uv.v = MathHelper.Interp(uv.v, vertex.uv.v, t);
        HVertexRet.color.r = MathHelper.Interp(color.r, vertex.color.r, t);
        HVertexRet.color.g = MathHelper.Interp(color.g, vertex.color.g, t);
        HVertexRet.color.b = MathHelper.Interp(color.b, vertex.color.b, t);
        HVertexRet.rhw = MathHelper.Interp(rhw, vertex.rhw, t);
        return HVertexRet;
    }

    // Step 1/d 的步伐
    public HVertex Step(HVertex vertex, float d)
    {
        HVertex HVertexRet = new HVertex();
        if (d == 0.0f)
        {
            return HVertexRet;
        }

        float inv = 1.0f / d;
        HVertexRet.pos.x = (vertex.pos.x - pos.x) * inv;
        HVertexRet.pos.y = (vertex.pos.y - pos.y) * inv;
        HVertexRet.pos.z = (vertex.pos.z - pos.z) * inv;
        HVertexRet.pos.w = (vertex.pos.w - pos.w) * inv;
        HVertexRet.uv.u = (vertex.uv.u - uv.u) * inv;
        HVertexRet.uv.v = (vertex.uv.v - uv.v) * inv;
        HVertexRet.color.r = (vertex.color.r - color.r) * inv;
        HVertexRet.color.g = (vertex.color.g - color.g) * inv;
        HVertexRet.color.b = (vertex.color.b - color.b) * inv;
        HVertexRet.rhw = (vertex.rhw - rhw) * inv;

        return HVertexRet;
    }

    //顶点加法
    public HVertex Add(HVertex vertex)
    {
        HVertex HVertexRet = new HVertex();
        HVertexRet.pos.x = pos.x + vertex.pos.x;
        HVertexRet.pos.y = pos.y + vertex.pos.y;
        HVertexRet.pos.z = pos.z + vertex.pos.z;
        HVertexRet.pos.w = pos.w + vertex.pos.w;
        HVertexRet.rhw = rhw + vertex.rhw;
        HVertexRet.uv.u = uv.u + vertex.uv.u;
        HVertexRet.uv.v = uv.v + vertex.uv.v;
        HVertexRet.color.r = color.r + vertex.color.r;
        HVertexRet.color.g = color.g + vertex.color.g;
        HVertexRet.color.b = color.b + vertex.color.b;

        return HVertexRet;
    }
};

//边 线段
class HEdge
{
    public HVertex v1, v2;
    public HVertex v;//临时变量
};

//梯形 有个理论是 所有的三角形都可以拆分成一个平底三角形和平顶三角形
class HTrapezoid
{
    public float top, bottom;
    public HEdge left = new HEdge();
    public HEdge right = new HEdge();
    //根据Y坐标 计算出左右两条边纵坐标等于Y的 顶点
    public void CalculateLRVertex(float y)
    {
        float s1 = left.v2.pos.y - left.v1.pos.y;
        float s2 = right.v2.pos.y - right.v1.pos.y;
        float t1 = (y - left.v1.pos.y) / s1;
        float t2 = (y - right.v1.pos.y) / s2;
        left.v = left.v1.InterpVertex(left.v2, t1);
        right.v = right.v1.InterpVertex(right.v2, t1);
    }
    public void EdgeInterp(float y) {
        float s1 = left.v2.pos.y - left.v1.pos.y;
        float s2 = right.v2.pos.y - right.v1.pos.y;
        float t1 = (y - left.v1.pos.y) / s1;
        float t2 = (y - right.v1.pos.y) / s2;

        /*
         *  根据y值左边两个点left v1 v2 插值得到left v  同理right
         *
         *       /--------\
         *      /          \
         *     y------------y
         *    /              \
         *   /----------------\
         *
         */

        //根据Y坐标 得到左右两边的点
        left.v = left.v1.InterpVertex(left.v2, t1);
        right.v = right.v1.InterpVertex(right.v2, t2);
    }



};

//光栅化的时候 三角形遍历的时候去生成图元的过程 用扫描线
class HScanline
{
    public HVertex v, step;
    public float x, y, width;
};

//屏幕三角形
class HTriangle
{
    public HTriangle() {}
    //三个顶点的数据
    public HVertex p1InObjectSpace, p2InObjectSpace, p3InObjectSpace;

    //中间忽略了世界坐标、相机坐标 直接乘MVP就得到ClipSpace坐标

    //三个顶点坐标转化的裁剪空间的坐标
    public HVertex p1InClipSpace, p2InClipSpace, p3InClipSpace;

    //三个顶点坐标转化的屏幕空间的坐标
    public HVertex p1InScreenSpace, p2InScreenSpace, p3InScreenSpace;
    public int CalculateTrap(List<HTrapezoid> trapezoid)
    {
        HVertex p;
        float k, x;

        //顶点排序
        if (p1InScreenSpace.pos.y > p2InScreenSpace.pos.y)
        {
            p = p1InScreenSpace;
            p1InScreenSpace = p2InScreenSpace;
            p2InScreenSpace = p;
        }

        if (p1InScreenSpace.pos.y > p3InScreenSpace.pos.y)
        {
            p = p1InScreenSpace;
            p1InScreenSpace = p3InScreenSpace;
            p3InScreenSpace = p;
        }

        if (p2InScreenSpace.pos.y > p3InScreenSpace.pos.y)
        {
            p = p2InScreenSpace;
            p2InScreenSpace = p3InScreenSpace;
            p3InScreenSpace = p;
        }
        if (p1InScreenSpace.pos.y == p2InScreenSpace.pos.y && p1InScreenSpace.pos.y == p3InScreenSpace.pos.y)
            return 0;
        if (p1InScreenSpace.pos.x == p2InScreenSpace.pos.x && p1InScreenSpace.pos.x == p3InScreenSpace.pos.x)
            return 0;

        if (p1InScreenSpace.pos.y == p2InScreenSpace.pos.y) {	// triangle down
            if (p1InScreenSpace.pos.x > p2InScreenSpace.pos.x)
            {
                p = p1InScreenSpace;
                p1InScreenSpace = p2InScreenSpace;
                p2InScreenSpace = p;
            }
            trapezoid.get(0).top = p1InScreenSpace.pos.y;
            trapezoid.get(0).bottom = p3InScreenSpace.pos.y;
            trapezoid.get(0).left.v1 = p1InScreenSpace;
            trapezoid.get(0).left.v2 = p3InScreenSpace;
            trapezoid.get(0).right.v1 = p2InScreenSpace;
            trapezoid.get(0).right.v2 = p3InScreenSpace;
            return (trapezoid.get(0).top < trapezoid.get(0).bottom) ? 1 : 0;
        }

        if (p2InScreenSpace.pos.y == p3InScreenSpace.pos.y) {	// triangle up
            if (p2InScreenSpace.pos.x > p3InScreenSpace.pos.x)
            {
                p = p2InScreenSpace;
                p2InScreenSpace = p3InScreenSpace;
                p3InScreenSpace = p;
            }
            trapezoid.get(0).top = p1InScreenSpace.pos.y;
            trapezoid.get(0).bottom = p3InScreenSpace.pos.y;
            trapezoid.get(0).left.v1 = p1InScreenSpace;
            trapezoid.get(0).left.v2 = p2InScreenSpace;
            trapezoid.get(0).right.v1 = p1InScreenSpace;
            trapezoid.get(0).right.v2 = p3InScreenSpace;
            return (trapezoid.get(0).top < trapezoid.get(0).bottom) ? 1 : 0;
        }

        trapezoid.get(0).top = p1InScreenSpace.pos.y;
        trapezoid.get(0).bottom = p2InScreenSpace.pos.y;
        trapezoid.get(1).top = p2InScreenSpace.pos.y;
        trapezoid.get(1).bottom = p3InScreenSpace.pos.y;

        k = (p3InScreenSpace.pos.y - p1InScreenSpace.pos.y) / (p2InScreenSpace.pos.y - p1InScreenSpace.pos.y);
        x = p1InScreenSpace.pos.x + (p2InScreenSpace.pos.x - p1InScreenSpace.pos.x) * k;

        if (x <= p3InScreenSpace.pos.x) {		// triangle left
            trapezoid.get(0).left.v1 = p1InScreenSpace;
            trapezoid.get(0).left.v2 = p2InScreenSpace;
            trapezoid.get(0).right.v1 = p1InScreenSpace;
            trapezoid.get(0).right.v2 = p3InScreenSpace;
            trapezoid.get(1).left.v1 = p2InScreenSpace;
            trapezoid.get(1).left.v2 = p3InScreenSpace;
            trapezoid.get(1).right.v1 = p1InScreenSpace;
            trapezoid.get(1).right.v2 = p3InScreenSpace;
        }
        else {					// triangle right
            trapezoid.get(0).left.v1 = p1InScreenSpace;
            trapezoid.get(0).left.v2 = p3InScreenSpace;
            trapezoid.get(0).right.v1 = p1InScreenSpace;
            trapezoid.get(0).right.v2 = p2InScreenSpace;
            trapezoid.get(1).left.v1 = p1InScreenSpace;
            trapezoid.get(1).left.v2 = p3InScreenSpace;
            trapezoid.get(1).right.v1 = p2InScreenSpace;
            trapezoid.get(1).right.v2 = p3InScreenSpace;
        }
        return 2;
    }
};

class a2v
{
    public a2v(HVector posPara)
    {
        pos = posPara;
    }
    public HVector pos = new HVector();		//模型坐标
    public HVector normal = new HVector();		//法线坐标
    public HTexcoord uv = new HTexcoord();		//uv坐标
    public HColor color = new HColor();		//顶点颜色
};

class v2f
{
    public HVector pos = new HVector();		//模型坐标
    public HTexcoord uv = new HTexcoord();		//uv坐标
};

//立方体
class HCube implements HShape {
    public HCube() {
        mesh.add(new HVertex(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f, 1f));
        mesh.add(new HVertex(1, -1, 1, 1, 0, 1, 0.2f, 1.0f, 0.2f, 1));
        mesh.add(new HVertex(1, 1, 1, 1, 1, 1, 0.2f, 0.2f, 1.0f, 1));
        mesh.add(new HVertex(-1, 1, 1, 1, 1, 0, 1.0f, 0.2f, 1.0f, 1));
        mesh.add(new HVertex(-1, -1, -1, 1, 0, 0, 1.0f, 1.0f, 0.2f, 1));
        mesh.add(new HVertex(1, -1, -1, 1, 0, 1, 0.2f, 1.0f, 1.0f, 1));
        mesh.add(new HVertex(1, 1, -1, 1, 1, 1, 1.0f, 0.3f, 0.3f, 1));
        mesh.add(new HVertex(-1, 1, -1, 1, 1, 0, 0.2f, 1.0f, 0.3f, 1));
    }

    //坐标变换
    public HTransform Transform = new HTransform();
    //纹理
    public HTexture Texture = new HTexture();
    //mesh
    //8个顶点  前面4个顶点是正方体的前面  后面4个顶点是正方体的后面
    public List<HVertex> mesh = new ArrayList<HVertex>();


    public void Draw() {
        DrawBox();
    }

    // 根据左右两边的端点，初始化计算出扫描线的起点和步长
    HScanline GetScanline(HTrapezoid trap, int y) {
        HScanline scanlineRet = new HScanline();
        // 左右两点的 宽度
        float width = trap.right.v.pos.x - trap.left.v.pos.x;
        // 起点X 坐标
        scanlineRet.x = (int) (trap.left.v.pos.x + 0.5f);
        // 宽度
        scanlineRet.width = (int) (trap.right.v.pos.x + 0.5f) - scanlineRet.x;
        // y坐标
        scanlineRet.y = y;
        // 扫描起点
        scanlineRet.v = trap.left.v.Copy();
        //
        if (trap.left.v.pos.x >= trap.right.v.pos.x) scanlineRet.width = 0;
        //计算步伐
        scanlineRet.step = trap.left.v.Step(trap.right.v, width);
        return scanlineRet;
    }


    void DrawScanline(HScanline scanline) {
        HScreenDevice ScreenDevice = HScreenDevice.GetInstance();
        List<Integer> framebuffer = ScreenDevice.FrameBuff;
        List<Float> zbuffer = ScreenDevice.DepthBuff;

        int x = (int) scanline.x;
        int y = (int) scanline.y;
        int scanlineWidth = (int) scanline.width;
        int ScreenWidth = ScreenDevice.ScreenWidth;
        int ScreenHeight = ScreenDevice.ScreenHeight;

        for (; scanlineWidth > 0; x++, scanlineWidth--) {
            if (x >= 0 && x < ScreenWidth) {
                float rhw = scanline.v.rhw;
                if (rhw >= zbuffer.get(x + y * ScreenWidth)) {
                    float w = 1.0f / rhw;
                    zbuffer.set(x + y * ScreenWidth, rhw);

                    float u = scanline.v.uv.u * w;
                    float v = scanline.v.uv.v * w;
                    v2f fragIn = new v2f();
                    fragIn.uv.u = u;
                    fragIn.uv.v = v;
                    int color = frag(fragIn);//片元着色器
                    Integer r = (Integer) 0x0, g = (Integer) 0x0, b = (Integer) 0x0, a = (Integer) 0x0;
                    r = Color.red(color);
                    g = Color.green(color);
                    b = Color.blue(color);
                    a = Color.alpha(color);

                    framebuffer.set((x + y * ScreenWidth) * 4, r);
                    framebuffer.set((x + y * ScreenWidth) * 4 + 1, g);
                    framebuffer.set((x + y * ScreenWidth) * 4 + 2, b);
                    framebuffer.set((x + y * ScreenWidth) * 4 + 3, a);
                }
            }
            scanline.v = scanline.v.Add(scanline.step);
            if (x >= ScreenWidth) break;
        }
    }

    // 主渲染函数
    void DrawTrap(HTrapezoid trap) {
        //HScanline scanline;
        int j, top, bottom;
        top = (int) (trap.top + 0.5f);
        bottom = (int) (trap.bottom + 0.5f);
        for (j = top; j < bottom; j++) {
            if (j >= 0 && j < HScreenDevice.GetInstance().ScreenHeight) {//todo
                //插值得到梯形的腰的两个点
                trap.EdgeInterp((float) j + 0.5f);
                //初始化扫描线
                HScanline scanline = GetScanline(trap, j);
                //绘制扫描线
                DrawScanline(scanline);
            }
        }
    }

    //更新MVP矩阵
    void UpdateMVPMat() {
        HMatrix mat = MathHelper.GetRotateMat(0, 0.8f, 0.8f);
        Transform.ModleMat = mat;
        Transform.UpdateMVPMat();
    }

    //计算三角形三个顶点的裁剪空间的坐标
    void InitTriangleClipSpacePos(HTriangle Triangle) {
        //三角形的模型坐标乘以MVP矩阵 得到投影坐标（相机空间）
        Triangle.p1InClipSpace = Triangle.p1InObjectSpace.Copy();
        Triangle.p2InClipSpace = Triangle.p2InObjectSpace.Copy();
        Triangle.p3InClipSpace = Triangle.p3InObjectSpace.Copy();


    }

    //计算三角形三个顶点的裁剪空间的坐标
    void CalTriangleScreenSpacePos(HTriangle Triangle) {
        //顶点的其他数据
        Triangle.p1InScreenSpace = Triangle.p1InObjectSpace.Copy();
        Triangle.p2InScreenSpace = Triangle.p2InObjectSpace.Copy();
        Triangle.p3InScreenSpace = Triangle.p3InObjectSpace.Copy();
        //归一化然后乘宽高
        Triangle.p1InScreenSpace.pos = Transform.HomogenizeToScreenCoord(Triangle.p1InClipSpace.pos);
        Triangle.p2InScreenSpace.pos = Transform.HomogenizeToScreenCoord(Triangle.p2InClipSpace.pos);
        Triangle.p3InScreenSpace.pos = Transform.HomogenizeToScreenCoord(Triangle.p3InClipSpace.pos);
        //保存Z信息
        Triangle.p1InScreenSpace.pos.w = Triangle.p1InClipSpace.pos.w;
        Triangle.p2InScreenSpace.pos.w = Triangle.p2InClipSpace.pos.w;
        Triangle.p3InScreenSpace.pos.w = Triangle.p3InClipSpace.pos.w;
    }


    //检查三角形是否在裁剪
    boolean CheckTriangleInCVV(HTriangle Triangle) {
        if (Triangle.p1InClipSpace.pos.CheckInCVV() == false) return false;
        if (Triangle.p2InClipSpace.pos.CheckInCVV() == false) return false;
        if (Triangle.p3InClipSpace.pos.CheckInCVV() == false) return false;
        return true;
    }

    //深度Z初始化顶点插值信息
    void InitTriangleInterpn(HTriangle Triangle) {
        Triangle.p1InScreenSpace.Initrhw();
        Triangle.p2InScreenSpace.Initrhw();
        Triangle.p3InScreenSpace.Initrhw();
    }

    //简单的顶点着色器
    v2f vert(a2v v) {
        v2f output = new v2f();
        output.pos = Transform.MulMVPMat(v.pos);
        return output;
    }

    //简单的片元着色器
    int frag(v2f f) {
        int color = Texture.RreadTexture(f.uv.u, f.uv.v);
        return color;
    }


    //画三角形 传入的
    void DrawTriangle(HTriangle Triangle) {
        //1、更新立方体的MVP矩阵
        UpdateMVPMat();

        //2.1、初始化裁剪空间坐标
        InitTriangleClipSpacePos(Triangle);

        //2.2、 顶点着色器
        Triangle.p1InClipSpace.pos = vert(new a2v(Triangle.p1InObjectSpace.pos)).pos;
        Triangle.p2InClipSpace.pos = vert(new a2v(Triangle.p2InObjectSpace.pos)).pos;
        Triangle.p3InClipSpace.pos = vert(new a2v(Triangle.p3InObjectSpace.pos)).pos;
        //曲面细分着色器【TODO】
        //几何着色器【TODO】

        //3、用裁剪空间裁剪三角形（这里比较暴力  一个点不在CVV则剔除）
        if (CheckTriangleInCVV(Triangle) == false) return;

        //4、归一化然后计算得到屏幕坐标
        CalTriangleScreenSpacePos(Triangle);

        //5、插值初始化
        InitTriangleInterpn(Triangle);

        //6、屏幕坐标的三角形拆分三角形为0-2个梯形，并且返回可用梯形数量
        List<HTrapezoid> traps = new ArrayList<HTrapezoid>();
        traps.add(new HTrapezoid());
        traps.add(new HTrapezoid());
        int n = Triangle.CalculateTrap(traps);

        //7、梯形扫描 绘制梯形
        if (n >= 1) DrawTrap(traps.get(0));
        if (n >= 2) DrawTrap(traps.get(1));
    }

    //画面
    void DrawPlane(int p1_index, int p2_index, int p3_index, int p4_index) {
        HVertex p1 = mesh.get(p1_index);
        HVertex p2 = mesh.get(p2_index);
        HVertex p3 = mesh.get(p3_index);
        HVertex p4 = mesh.get(p4_index);

        //纹理绘制到这个面上面
        p1.uv.u = 0;
        p1.uv.v = 0;

        p2.uv.u = 0;
        p2.uv.v = 1;

        p3.uv.u = 1;
        p3.uv.v = 1;

        p4.uv.u = 1;
        p4.uv.v = 0;
        HTriangle T1 = new HTriangle();
        T1.p1InObjectSpace = p1;
        T1.p2InObjectSpace = p2;
        T1.p3InObjectSpace = p3;
        DrawTriangle(T1);


        HTriangle T2 = new HTriangle();
        T2.p1InObjectSpace = p3;
        T2.p2InObjectSpace = p4;
        T2.p3InObjectSpace = p1;
        DrawTriangle(T2);
    }

    //画立方体
    void DrawBox() {
        DrawPlane(0, 1, 2, 3);//前面
        DrawPlane(7, 6, 5, 4);//后面
        DrawPlane(0, 4, 5, 1);//下面
        DrawPlane(1, 5, 6, 2);//右面
        DrawPlane(2, 6, 7, 3);//上面
        DrawPlane(3, 7, 4, 0);//左面
    }


};
//===========================光栅化阶段 End=============================

//=====实例代码
//1、初始化加一个立方体 HScreenDevice::GetInstance()->shape = new HCube();
//2、绘制				HScreenDevice::GetInstance()->Draw();



public class FirstFragment extends Fragment {
    private final static String TAG = "SoftRenderer";
    private ImageView imgView;
    private long frameInternalTime = (int)(1000.0f / 60.0f);
    private long lastFrameTime = 0;
    private boolean softRendererHasInited = false;
    private HandlerThread rendererThread = new HandlerThread("SoftRenderer");
    private Handler rendererHandler;
    private Handler mainLooperHandler;
    private Runnable FBOFlushToScreenRun = new Runnable() {
        @Override
        public void run() {
            flushToScreen();
        }
    };


    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_first, container, false);
    }

    Looper mainLooper = null;
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        Log.i(TAG,"onViewCreated");
        initSoftRenderer(view);
    }

    private void initSoftRenderer(@NonNull View view) {
        Log.i(TAG,"initSoftRenderer");
        //线程初始化
        mainLooperHandler = new Handler(Looper.getMainLooper());
        rendererThread.start();
        rendererHandler = new Handler(rendererThread.getLooper());
        rendererHandler.post(new Runnable() {
            @Override
            public void run() {
                while(true) {
                    long nowTime = SystemClock.currentThreadTimeMillis();
                    if (nowTime - lastFrameTime > frameInternalTime) {
                        if (softRendererHasInited) {
                            HScreenDevice.GetInstance().Draw();
                            mainLooperHandler.post(FBOFlushToScreenRun);
                        }
                    }
                }
            }
        });


        //初始化充当屏幕的imgView
        imgView = view.findViewById(R.id.imageView);

        //获取设备宽高 然后初始化
        int width = 512;
        int height = 512;
        HScreenDevice.GetInstance().Init(width,height);
        HScreenDevice.GetInstance().shape = new HCube();
        softRendererHasInited = true;
    }

    private void flushToScreen() {
        Log.i(TAG,"flushToScreen");
        int width = 512;
        int height = 512;

        Bitmap bitmap = Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
        for (int i = 0; i < HScreenDevice.GetInstance().ScreenHeight; i++)
        {
            for (int j = 0; j < HScreenDevice.GetInstance().ScreenWidth; j++) {
                Integer RByte = HScreenDevice.GetInstance()
                        .FrameBuff.get((i * HScreenDevice.GetInstance().ScreenWidth + j) * 4 + 0);
                Integer GByte = HScreenDevice.GetInstance()
                        .FrameBuff.get((i * HScreenDevice.GetInstance().ScreenWidth + j) * 4 + 1);
                Integer BByte = HScreenDevice.GetInstance()
                        .FrameBuff.get((i * HScreenDevice.GetInstance().ScreenWidth + j) * 4 + 2);
                if (RByte == null) {
                    Log.e(TAG,"RByte == null");
                } else {
                    int color = Color.argb(255,
                            RByte,
                            GByte,
                            BByte
                    );
                    bitmap.setPixel(i,j,color);
                }
            }
        }
        imgView.setImageBitmap(bitmap);
    }
}