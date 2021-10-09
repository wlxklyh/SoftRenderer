// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <math.h>
//����Ⱦ���ο���https://github.com/skywind3000/mini3d  
//mini3d�ʼǣ�https://zhuanlan.zhihu.com/p/74510058
//ͼ��ѧ��ˮ�����£�https://positiveczp.github.io/%E7%BB%86%E8%AF%B4%E5%9B%BE%E5%BD%A2%E5%AD%A6%E6%B8%B2%E6%9F%93%E7%AE%A1%E7%BA%BF.pdf
//��ͼ��
//1��������ע��Ϊ�˸��õ����  mini3d  д�ĺܾ��� ������֮�����Ⱦ��ˮ�߸��������
//2��ͬ�Ƕ�ͼ��ѧ����Ȥ�����ѿ��˿��Ը��õ���⣬�տ�ʼ��ͼ��ѧ û�д����ܸо����顣
//��ע��
//C++����󲿷���ֵ���� û��ʹ��ָ�� ��Ϊ��չʾmini3d�Ĺ��� 

/*
 *  1����򵥵���Ⱦ��ˮ��:
 *	   �ֳ�CPU�׶κ�GPU�׶�
 *      +--------------+     +-------------+
 *      |              |     |             |
 *      |     CPU      +----->     GPU     |
 *      |              |     |             |
 *      +--------------+     +-------------+
 *
 *	2������CPU�׶ξ���Application Ӧ�ý׶�  GPU�׶ΰ����˼��ν׶κ͹�դ���׶�
 *      +--------------+     +-----------------+  +----------------+   +----------------+
 *      |              |     |                 |  |                |   |                |
 *      |   Ӧ�ý׶�   +----->     ���ν׶�    +-->      ��դ��    +--->     ���ش���   |
 *      |              |     |                 |  |                |   |                |
 *      +--------------+     +-----------------+  +----------------+   +----------------+
 *
 *  3�����ν׶Σ�
 *		+--------------+     +-----------------+  +------------------+   +-------------+  +-------------+
 *      |              |     |                 |  |                  |   |             |  |             |
 *      |  ������ɫ��  +-----> ����ϸ����ɫ��  +-->   ������ɫ��     +--->    �ü�     |-->  ��ĻͶ��   |
 *      |              |     |                 |  |                  |   |             |  |             |
 *      +--------------+     +-----------------+  +------------------+   +-------------+  +-------------+
 *
 *  4����դ���׶Σ�
 *		+--------------+     +--------------+  +------------------+
 *      |              |     |              |  |                  |
 *      |  �����α���  +----->  ����������  +-->   ƬԪ��ɫ��     |
 *      |              |     |              |  |                  |
 *      +--------------+     +--------------+  +------------------+
 *
 *  5�����ش���׶Σ�
 *		��Ȳ���ZTest
 *		��ɫ���
 *      ģ����ԣ�ģ�建�壩
 *
 *	��˵����������Ĵ�������������ˮ��������ͻ���
 *	���Ƶ��ö�ջ��
 *		HScreenDevice::Draw					
 *			HScreenDevice::ClearScreen									����
 *			HCube::Draw													Cube����
 *				HCube::DrawBox											���������
 *					HCube::DrawPlane									�����λ���
 *						HCube::DrawTriangle								�����λ���
 *						
 *							HCube::UpdateMVPMat()							1������MVP����							   -|
 *							HCube::vert()									2��������ɫ�� ֮����ǲü��ռ�������		|
 *																			3������ϸ����ɫ�� ������ɫ����TODO��		|--->���ν׶�
 *							HCube::CheckTriangleInCVV()						4���ü� ����ڲ��ڲü��ռ�����				|
 *							HCube::CalTriangleScreenSpacePos()				5����ĻͶ��								   -|
 *							
 *							HCube::InitTriangleInterpn()					1����ֵ��ʼ�� ����͸��У����			   -|
 *							Triangle::CalculateTrap() DrawTrap DrawScanline 2�����������á������α��� �õ�ƬԪ��Ϣ		|--->��դ���׶�
 *							HCube::frag										3��ƬԪ��ɫ��							   -|
 *							
 *							ZTest Zwrite
 *						
 *						
 */						




//===========================��ѧ���� Begin=============================
//��ֵ����   tΪ[0,1]֮�� 
template<typename T>
T Interp(T x1, T x2, T t) { return x1 + (x2 - x1) * t; }

//Clamp���� Value
template<typename T>
T Clamp(T x, T min, T max) { return (x < min) ? min : ((x > max) ? max : x); }
//===========================��ѧ���� End===============================

//��״����
class HShape
{
public:

	virtual void Draw() = 0;
};

//����  ����û��ȥ���ļ� ֱ���ڴ������渳ֵ��
class HTexture
{
public:
	HTexture()
	{
		//��ʼ������
		TextureW = 256;
		TextureH = 256;
		int i, j;
		for (j = 0; j < TextureH; j++) {
			for (i = 0; i < TextureW; i++) {
				int x = i / 32, y = j / 32;
				Texture[j][i] = ((x + y) & 1) ? 0xffffffff : 0x3fbcefff;
			}
		}
	}

	int TextureW, TextureH;
	int Texture[256][256];

	//����������ο�https://gameinstitute.qq.com/community/detail/115739
	//1���������ʵ�֣�����������Bilinear 4����ȡ��ֵ��Trilinear����MipMap���ά��
	int RreadTexture(float u, float v) {
		int x, y;
		u = u * TextureW;
		v = v * TextureH;
		x = (int)(u + 0.5f);
		y = (int)(v + 0.5f);
		x = Clamp(x, 0, TextureW - 1);
		y = Clamp(y, 0, TextureH - 1);
		return Texture[y][x];
	}
};
//��Ⱦ�豸
class  HScreenDevice
{
public:
	//============���� Begin============
	static HScreenDevice *DeviceInstance;
	static HScreenDevice *GetInstance()
	{
		if (DeviceInstance == NULL)
		{
			DeviceInstance = new HScreenDevice();
		}
		return DeviceInstance;
	}
	//============���� End============

	//��Ⱦ����״
	HShape *shape;
	HScreenDevice()
	{
		shape = NULL;
	};
	~HScreenDevice()
	{
	};
	//��Ļ�ֱ��ʿ�
	int ScreenWidth;
	//��Ļ�ֱ��ʸ�
	int ScreenHeight;
	//��Ļ����
	unsigned char* FrameBuff;
	//��Ȼ��� ���ƹ���ZTest ZWrite
	float* DepthBuff;


	void Init(int width, int height)
	{
		//1����Ļ���طֱ���
		ScreenWidth = width;
		ScreenHeight = height;


		//2����Ļ�������Ȼ��� 
		FrameBuff = (unsigned char*)malloc(ScreenWidth * ScreenHeight * 4);
		DepthBuff = (float*)malloc(ScreenWidth * ScreenHeight * 4);
	}

	// ������Ļ
	void ClearScreen()
	{
		for (int i = 0; i < ScreenHeight; i++)
		{
			for (int j = 0; j < ScreenWidth; j++)
			{
				//R
				FrameBuff[(i * ScreenWidth + j) * 4] = 0x0;
				//G
				FrameBuff[(i * ScreenWidth + j) * 4 + 1] = 0x0;
				//B
				FrameBuff[(i * ScreenWidth + j) * 4 + 2] = 0x0;
				//A
				FrameBuff[(i * ScreenWidth + j) * 4 + 3] = 0x0;
				//Z buffer ��0
				DepthBuff[i * ScreenWidth + j] = 0.0f;
			}
		}
	}

	//=====================================================================
	// �����ƺ���
	//=====================================================================
	void Draw()
	{
		//1��������Ļ����
		ClearScreen();
		//2������һ��ͼ��
		if (shape)
		{
			shape->Draw();
		}
	}



	//Test ���԰�֡����ĳɺ�ɫ �������Ƿ���ʾ�ɺ�ɫ
	void InitTestRed()
	{
		for (int i = 0; i < ScreenHeight; i++)
		{
			for (int j = 0; j < ScreenWidth; j++)
			{
				//R
				FrameBuff[(i * ScreenWidth + j) * 4] = 0xFF;
				//G
				FrameBuff[(i * ScreenWidth + j) * 4 + 1] = 0x0;
				//B
				FrameBuff[(i * ScreenWidth + j) * 4 + 2] = 0x0;
				//A
				FrameBuff[(i * ScreenWidth + j) * 4 + 3] = 0xFF;
			}
		}
	}
};
HScreenDevice* HScreenDevice::DeviceInstance = NULL;

//===========================���ν׶� Begin=============================
//Hֻ�Ǹ�ǰ׺�͸�OpenGL��GLһ��
class HMatrix
{
public:
	HMatrix()
	{
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
		m[0][1] = m[0][2] = m[0][3] = 0.0f;
		m[1][0] = m[1][2] = m[1][3] = 0.0f;
		m[2][0] = m[2][1] = m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2] = 0.0f;
	}
	/*  ����ͼʾ
	 *  m[0][0],m[1][0],m[2][0],m[3][0],
	 *  m[0][1],m[1][1],m[2][1],m[3][1],
	 *  m[0][2],m[1][2],m[2][2],m[3][2],
	 *  m[0][3],m[1][3],m[2][4],m[3][3],
	 */
	float m[4][4];
	//����ӷ�
	HMatrix Add(const HMatrix& mat)
	{
		HMatrix matRet;
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
	//�������
	HMatrix Sub(const HMatrix& mat)
	{
		HMatrix matRet;
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
	//����˷�
	HMatrix Mul(const HMatrix& mat)
	{
		HMatrix matRet;
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
	//��������
	HMatrix Scale(float f)
	{
		HMatrix matRet;
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

	bool operator==(const HMatrix& mat)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (m[i][j] != mat.m[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}

	bool operator!=(const HMatrix& mat)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (m[i][j] != mat.m[i][j])
				{
					return true;
				}
			}
		}
		return false;
	}

	//���䵥Ԫ����
	bool UnitTest()
	{
		HMatrix mat1, mat2, mat3;
		mat1.m[0][0] = 0; mat1.m[1][0] = 0; mat1.m[2][0] = 0; mat1.m[3][0] = 0;
		mat1.m[0][1] = 0; mat1.m[1][1] = 0; mat1.m[2][1] = 0; mat1.m[3][1] = 0;
		mat1.m[0][2] = 0; mat1.m[1][2] = 0; mat1.m[2][2] = 0; mat1.m[3][2] = 0;
		mat1.m[0][3] = 0; mat1.m[1][3] = 0; mat1.m[2][3] = 0; mat1.m[3][3] = 0;

		if (mat1.Add(mat2) != mat3)
		{
			return false;
		}
		return true;
	}
};

//���������ǵ㶼����������
class HVector
{
public:
	HVector()
	{
		w = 1;
	}
	HVector(float xp, float yp, float zp, float wp) : x(xp), y(yp), z(zp), w(wp)
	{

	}


	//��ʾ����ʱw����
	float x, y, z, w;

	//��������
	float Length() { return sqrt(x * x + y * y + z * z); }

	//�����ӷ�
	HVector Add(const HVector& vec)
	{
		HVector vecRet;
		vecRet.w = 1;
		vecRet.x = x + vec.x;
		vecRet.y = y + vec.y;
		vecRet.z = z + vec.z;
		return vecRet;
	}

	//��������
	HVector Sub(const HVector& vec)
	{
		HVector vecRet;
		vecRet.w = 1;
		vecRet.x = x - vec.x;
		vecRet.y = y - vec.y;
		vecRet.z = z - vec.z;
		return vecRet;
	}

	//������� ������ֵ A x B = |A||B|Cos 
	float DotProduct(const HVector& vec)
	{
		return  x * vec.x + y * vec.y + z * vec.z;
	}

	//������� �������� ����������������
	HVector CrossProduct(const HVector& vec)
	{
		HVector vecRet;
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

	//������ֵ
	HVector InterpVec(const HVector& vec, float t)
	{
		HVector vecRet;
		vecRet.x = Interp(x, vec.x, t);
		vecRet.y = Interp(y, vec.y, t);
		vecRet.z = Interp(z, vec.z, t);
		vecRet.w = 1.0;
		return vecRet;
	}

	//������һ
	HVector Normalize()
	{
		HVector vecRet;
		float len = Length();
		if (len != 0.0f)
		{
			vecRet.x = x / len;
			vecRet.y = y / len;
			vecRet.z = z / len;
		}
		return vecRet;
	}

	//�����˾���
	HVector MulMat(const HMatrix& mat)
	{
		HVector vec;
		float X = x, Y = y, Z = z, W = w;
		vec.x = X * mat.m[0][0] + Y * mat.m[1][0] + Z * mat.m[2][0] + W * mat.m[3][0];
		vec.y = X * mat.m[0][1] + Y * mat.m[1][1] + Z * mat.m[2][1] + W * mat.m[3][1];
		vec.z = X * mat.m[0][2] + Y * mat.m[1][2] + Z * mat.m[2][2] + W * mat.m[3][2];
		vec.w = X * mat.m[0][3] + Y * mat.m[1][3] + Z * mat.m[2][3] + W * mat.m[3][3];
		return vec;
	}

	//�����βü����� cvv canonical view volume
	bool CheckInCVV()
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

//��ȡ��λ����
static HMatrix GetIdentityMat() {
	HMatrix matRet;
	matRet.m[0][0] = matRet.m[1][1] = matRet.m[2][2] = matRet.m[3][3] = 1.0f;
	matRet.m[0][1] = matRet.m[0][2] = matRet.m[0][3] = 0.0f;
	matRet.m[1][0] = matRet.m[1][2] = matRet.m[1][3] = 0.0f;
	matRet.m[2][0] = matRet.m[2][1] = matRet.m[2][3] = 0.0f;
	matRet.m[3][0] = matRet.m[3][1] = matRet.m[3][2] = 0.0f;
	return matRet;
}

//��ȡ0����
static HMatrix GetZeroMat() {
	HMatrix matRet;
	matRet.m[0][0] = matRet.m[0][1] = matRet.m[0][2] = matRet.m[0][3] = 0.0f;
	matRet.m[1][0] = matRet.m[1][1] = matRet.m[1][2] = matRet.m[1][3] = 0.0f;
	matRet.m[2][0] = matRet.m[2][1] = matRet.m[2][2] = matRet.m[2][3] = 0.0f;
	matRet.m[3][0] = matRet.m[3][1] = matRet.m[3][2] = matRet.m[3][3] = 0.0f;
	return matRet;
}

//��ȡƽ�ƾ���
static HMatrix GetTranslateMat(float x, float y, float z) {
	HMatrix matRet;
	matRet = GetIdentityMat();
	matRet.m[3][0] = x;
	matRet.m[3][1] = y;
	matRet.m[3][2] = z;
	return matRet;
}

//��ȡ���ž���
static HMatrix GetScaleMat(float x, float y, float z) {
	HMatrix matRet;
	matRet = GetIdentityMat();
	matRet.m[0][0] = x;
	matRet.m[1][1] = y;
	matRet.m[2][2] = z;
	return matRet;
}

//��ȡ��ת���� X����ת
// https://blog.csdn.net/csxiaoshui/article/details/65446125
static HMatrix GetRotateMatX(float x) {
	HMatrix matRet = GetIdentityMat();//��λ����
	float SinValue = (float)sin(x);
	float CosValue = (float)cos(x);

	matRet.m[0][0] = 1; matRet.m[1][0] = 0;			matRet.m[2][0] = 0;			matRet.m[3][0] = 0;
	matRet.m[0][1] = 0; matRet.m[1][1] = CosValue;	matRet.m[2][1] = -SinValue; matRet.m[3][1] = 0;
	matRet.m[0][2] = 0; matRet.m[1][2] = SinValue;	matRet.m[2][2] = CosValue;	matRet.m[3][2] = 0;
	matRet.m[0][3] = 0; matRet.m[1][3] = 0;			matRet.m[2][3] = 0;			matRet.m[3][3] = 1;

	return matRet;
}

//��ȡ��ת���� Y����ת
// https://blog.csdn.net/csxiaoshui/article/details/65446125
static HMatrix GetRotateMatY(float y) {
	HMatrix matRet = GetIdentityMat();//��λ����
	float SinValue = (float)sin(y);
	float CosValue = (float)cos(y);

	matRet.m[0][0] = CosValue;	matRet.m[1][0] = 0; matRet.m[2][0] = SinValue;	matRet.m[3][0] = 0;
	matRet.m[0][1] = 0;			matRet.m[1][1] = 1;	matRet.m[2][1] = 0;			matRet.m[3][1] = 0;
	matRet.m[0][2] = -SinValue;	matRet.m[1][2] = 0;	matRet.m[2][2] = CosValue;	matRet.m[3][2] = 0;
	matRet.m[0][3] = 0;			matRet.m[1][3] = 0;	matRet.m[2][3] = 0;			matRet.m[3][3] = 1;

	return matRet;
}

//��ȡ��ת���� Z����ת
// https://blog.csdn.net/csxiaoshui/article/details/65446125
static HMatrix GetRotateMatZ(float z) {
	HMatrix matRet = GetIdentityMat();//��λ����
	float SinValue = (float)sin(z);
	float CosValue = (float)cos(z);

	matRet.m[0][0] = CosValue; matRet.m[1][0] = -SinValue; matRet.m[2][0] = 0;	matRet.m[3][0] = 0;
	matRet.m[0][1] = SinValue; matRet.m[1][1] = CosValue;  matRet.m[2][1] = 0;	matRet.m[3][1] = 0;
	matRet.m[0][2] = 0;		   matRet.m[1][2] = 0;		   matRet.m[2][2] = 1;	matRet.m[3][2] = 0;
	matRet.m[0][3] = 0;		   matRet.m[1][3] = 0;		   matRet.m[2][3] = 0;	matRet.m[3][3] = 1;

	return matRet;
}

//��ȡ��ת���� XYZ����ת
static HMatrix GetRotateMat(float x, float y, float z)
{
	//X Y Z������� ������Ϊ�˺���� ����������Ч�����˷� 6�����Ǻ��� 2�ξ���˷�
	HMatrix matRet = GetRotateMatX(x).Mul(GetRotateMatY(y)).Mul(GetRotateMatZ(z));
	return matRet;
}

//��ȡLookAt����
//�����λ�� ����Ŀ����Ǹ�λ�ã������������ ����Ϸ�λ��
// see:https://zhuanlan.zhihu.com/p/66384929
// Rx Ry Rz 0
// Ux Uy Uz 0
// Dx Dy Dz 0
// 0  0  0  1 ����ռ�������ϵ 
static HMatrix GetLookAtMat(HVector& camera, HVector& at, HVector& up)
{
	HMatrix matRet;
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

//��ȡͶӰ���� �����������֮��õ���������ռ������
static HMatrix GetPerspectiveMat(float fovy, float aspect, float zn, float zf)
{
	float fax = 1.0f / (float)tan(fovy * 0.5f);

	HMatrix matRet = GetZeroMat();
	matRet.m[0][0] = (float)(fax / aspect);
	matRet.m[1][1] = (float)(fax);
	matRet.m[2][2] = zf / (zf - zn);
	matRet.m[3][2] = -zn * zf / (zf - zn);
	matRet.m[2][3] = 1;
	return matRet;
}

//����һ��ģ�͵Ŀռ�����ת���Ĺ��̵���
class HTransform
{
public:
	HTransform()
	{
		Init();
	} 

	//Unity ������� Position Rotation Scale������������� ����������Բ���¸������
	// ModelMatrix�����ǽ�ģ������任��WorldMatrix��Matrix��WorldMatrix = Mt * Mr * Ms  ModleMatrix =  Mt * Mr * Ms
	HMatrix ModleMat;
	// ��������ת����׶�� ת��������� View������ת��
	HMatrix ViewMat;
	// ͶӰ���� ��׶������������ͶӰ���� �͵õ���Ļ����
	HMatrix ProjectionMat;
	//MVP ������� ModleMat *  ViewMat * ProjectionMat
	HMatrix MVPMat;


	// ����ͶӰ����
	void UpdateMVPMat()
	{
		MVPMat = ModleMat.Mul(ViewMat).Mul(ProjectionMat);
	}

	void Init()
	{
		int ScreenWidth = HScreenDevice::GetInstance()->ScreenWidth;
		int ScreenHeight = HScreenDevice::GetInstance()->ScreenHeight;
		ModleMat = GetIdentityMat();
		HVector camera(5, 0, 0, 1);
		HVector at(0, 0, 0, 1);
		HVector up(0, 1, 0, 1);
		ViewMat = GetLookAtMat(
			camera,
			at,
			up
		);
		// fov = 90�� 0.5pai
		ProjectionMat = GetPerspectiveMat(3.1415926f * 0.5f, (float)ScreenWidth / (float)ScreenHeight, 1.0f, 500.0f);
		UpdateMVPMat();
	}

	//����ת������Ļ����
	HVector MulMVPMat(HVector& Origin)
	{
		return Origin.MulMat(MVPMat);
	}

	//��һ�� ����Ļ����
	//��->
	//�ߡ�
	HVector HomogenizeToScreenCoord(HVector& Origin)
	{
		int ScreenWidth = HScreenDevice::GetInstance()->ScreenWidth;
		int ScreenHeight = HScreenDevice::GetInstance()->ScreenHeight;
		float rhw = 1.0f / Origin.w;
		HVector vecRet;
		vecRet.x = (Origin.x * rhw + 1.0f) * ScreenWidth * 0.5f;
		vecRet.y = (1.0f - Origin.y * rhw) * ScreenHeight * 0.5f;
		vecRet.z = Origin.z * rhw;
		vecRet.w = 1.0f;
		return vecRet;
	}
};
//===========================���ν׶� End=============================



//===========================��դ���׶� Begin=============================
//��ɫ RGBA
class HColor
{
public:
	float r, g, b, a;
};

//�������� uc
class HTexcoord
{
public:
	float u, v;
};

//������Ϣ
class HVertex
{
public:
	HVector pos;
	HTexcoord uv;
	HColor color;
	float rhw;

	//��Ļ����������β�ֵ��ʱ��Ҫ��ʼ�� rhw ��͸��У���� ����ȡֵ��ʱ�����w 
	void Initrhw()
	{
		rhw = 1.0f / pos.w;

		uv.u *= rhw;
		uv.v *= rhw;

		color.r *= rhw;
		color.g *= rhw;
		color.b *= rhw;
		color.a *= rhw;
	}

	//��ֵ��Ļ����Ķ�����Ϣ
	HVertex InterpVertex(HVertex vertex, float t)
	{
		HVertex HVertexRet;
		HVertexRet.pos = pos.InterpVec(vertex.pos, t);
		HVertexRet.uv.u = Interp(uv.u, vertex.uv.u, t);
		HVertexRet.uv.v = Interp(uv.v, vertex.uv.v, t);
		HVertexRet.color.r = Interp(color.r, vertex.color.r, t);
		HVertexRet.color.g = Interp(color.g, vertex.color.g, t);
		HVertexRet.color.b = Interp(color.b, vertex.color.b, t);
		HVertexRet.rhw = Interp(rhw, vertex.rhw, t);
		return HVertexRet;
	}

	// Step 1/d �Ĳ��� 
	HVertex Step(HVertex vertex, float d)
	{
		HVertex HVertexRet;
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

	//����ӷ�
	HVertex Add(HVertex vertex)
	{
		HVertex HVertexRet;
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

//�� �߶�
class HEdge
{
public:
	HVertex v1, v2;
	HVertex v;//��ʱ����
};

//���� �и������� ���е������ζ����Բ�ֳ�һ��ƽ�������κ�ƽ��������
class HTrapezoid
{
public:
	float top, bottom;
	HEdge left, right;
	//����Y���� ������������������������Y�� ����
	void CalculateLRVertex(float y)
	{
		float s1 = left.v2.pos.y - left.v1.pos.y;
		float s2 = right.v2.pos.y - right.v1.pos.y;
		float t1 = (y - left.v1.pos.y) / s1;
		float t2 = (y - right.v1.pos.y) / s2;
		left.v = left.v1.InterpVertex(left.v2, t1);
		right.v = right.v1.InterpVertex(right.v2, t1);
	}
	void EdgeInterp(float y) {
		float s1 = left.v2.pos.y - left.v1.pos.y;
		float s2 = right.v2.pos.y - right.v1.pos.y;
		float t1 = (y - left.v1.pos.y) / s1;
		float t2 = (y - right.v1.pos.y) / s2;

		/*
		 *  ����yֵ���������left v1 v2 ��ֵ�õ�left v  ͬ��right
		 *  
		 *       /--------\
		 *      /          \
		 *     y------------y
		 *    /              \
		 *   /----------------\
		 *   
		 */

		 //����Y���� �õ��������ߵĵ�
		left.v = left.v1.InterpVertex(left.v2, t1);
		right.v = right.v1.InterpVertex(right.v2, t2);
	}



};

//��դ����ʱ�� �����α�����ʱ��ȥ����ͼԪ�Ĺ��� ��ɨ����
class HScanline
{
public:
	HVertex v, step;
	float x, y, width;
};

//��Ļ������
class HTriangle
{
public:
	HTriangle() {}
	//�������������
	HVertex p1InObjectSpace, p2InObjectSpace, p3InObjectSpace;

	//�м�������������ꡢ������� ֱ�ӳ�MVP�͵õ�ClipSpace����

	//������������ת���Ĳü��ռ������
	HVertex p1InClipSpace, p2InClipSpace, p3InClipSpace;

	//������������ת������Ļ�ռ������
	HVertex p1InScreenSpace, p2InScreenSpace, p3InScreenSpace;
	int CalculateTrap(HTrapezoid* trapezoid)
	{
		HVertex p;
		float k, x;

		//��������
		if (p1InScreenSpace.pos.y > p2InScreenSpace.pos.y) p = p1InScreenSpace, p1InScreenSpace = p2InScreenSpace, p2InScreenSpace = p;
		if (p1InScreenSpace.pos.y > p3InScreenSpace.pos.y) p = p1InScreenSpace, p1InScreenSpace = p3InScreenSpace, p3InScreenSpace = p;
		if (p2InScreenSpace.pos.y > p3InScreenSpace.pos.y) p = p2InScreenSpace, p2InScreenSpace = p3InScreenSpace, p3InScreenSpace = p;
		if (p1InScreenSpace.pos.y == p2InScreenSpace.pos.y && p1InScreenSpace.pos.y == p3InScreenSpace.pos.y) return 0;
		if (p1InScreenSpace.pos.x == p2InScreenSpace.pos.x && p1InScreenSpace.pos.x == p3InScreenSpace.pos.x) return 0;

		if (p1InScreenSpace.pos.y == p2InScreenSpace.pos.y) {	// triangle down
			if (p1InScreenSpace.pos.x > p2InScreenSpace.pos.x) p = p1InScreenSpace, p1InScreenSpace = p2InScreenSpace, p2InScreenSpace = p;
			trapezoid[0].top = p1InScreenSpace.pos.y;
			trapezoid[0].bottom = p3InScreenSpace.pos.y;
			trapezoid[0].left.v1 = p1InScreenSpace;
			trapezoid[0].left.v2 = p3InScreenSpace;
			trapezoid[0].right.v1 = p2InScreenSpace;
			trapezoid[0].right.v2 = p3InScreenSpace;
			return (trapezoid[0].top < trapezoid[0].bottom) ? 1 : 0;
		}

		if (p2InScreenSpace.pos.y == p3InScreenSpace.pos.y) {	// triangle up
			if (p2InScreenSpace.pos.x > p3InScreenSpace.pos.x) p = p2InScreenSpace, p2InScreenSpace = p3InScreenSpace, p3InScreenSpace = p;
			trapezoid[0].top = p1InScreenSpace.pos.y;
			trapezoid[0].bottom = p3InScreenSpace.pos.y;
			trapezoid[0].left.v1 = p1InScreenSpace;
			trapezoid[0].left.v2 = p2InScreenSpace;
			trapezoid[0].right.v1 = p1InScreenSpace;
			trapezoid[0].right.v2 = p3InScreenSpace;
			return (trapezoid[0].top < trapezoid[0].bottom) ? 1 : 0;
		}

		trapezoid[0].top = p1InScreenSpace.pos.y;
		trapezoid[0].bottom = p2InScreenSpace.pos.y;
		trapezoid[1].top = p2InScreenSpace.pos.y;
		trapezoid[1].bottom = p3InScreenSpace.pos.y;

		k = (p3InScreenSpace.pos.y - p1InScreenSpace.pos.y) / (p2InScreenSpace.pos.y - p1InScreenSpace.pos.y);
		x = p1InScreenSpace.pos.x + (p2InScreenSpace.pos.x - p1InScreenSpace.pos.x) * k;

		if (x <= p3InScreenSpace.pos.x) {		// triangle left
			trapezoid[0].left.v1 = p1InScreenSpace;
			trapezoid[0].left.v2 = p2InScreenSpace;
			trapezoid[0].right.v1 = p1InScreenSpace;
			trapezoid[0].right.v2 = p3InScreenSpace;
			trapezoid[1].left.v1 = p2InScreenSpace;
			trapezoid[1].left.v2 = p3InScreenSpace;
			trapezoid[1].right.v1 = p1InScreenSpace;
			trapezoid[1].right.v2 = p3InScreenSpace;
		}
		else {					// triangle right
			trapezoid[0].left.v1 = p1InScreenSpace;
			trapezoid[0].left.v2 = p3InScreenSpace;
			trapezoid[0].right.v1 = p1InScreenSpace;
			trapezoid[0].right.v2 = p2InScreenSpace;
			trapezoid[1].left.v1 = p1InScreenSpace;
			trapezoid[1].left.v2 = p3InScreenSpace;
			trapezoid[1].right.v1 = p2InScreenSpace;
			trapezoid[1].right.v2 = p3InScreenSpace;
		}
		return 2;
	}
};

struct a2v
{
	a2v(HVector posPara)
	{
		pos = posPara;
	}
	HVector pos;		//ģ������
	HVector normal;		//��������
	HTexcoord uv;		//uv����
	HColor color;		//������ɫ
};

struct v2f
{
	HVector pos;		//ģ������
	HTexcoord uv;		//uv����
};

//������
class HCube :public HShape
{
public:
	HCube() {};
	//����任
	HTransform Transform;
	//����
	HTexture Texture;
	//mesh
	//8������  ǰ��4���������������ǰ��  ����4��������������ĺ���
	HVertex mesh[8] = {
		{ { -1, -1,  1, 1 }, { 0, 0 }, { 1.0f, 0.2f, 0.2f }, 1 },
		{ {  1, -1,  1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 0.2f }, 1 },
		{ {  1,  1,  1, 1 }, { 1, 1 }, { 0.2f, 0.2f, 1.0f }, 1 },
		{ { -1,  1,  1, 1 }, { 1, 0 }, { 1.0f, 0.2f, 1.0f }, 1 },
		{ { -1, -1, -1, 1 }, { 0, 0 }, { 1.0f, 1.0f, 0.2f }, 1 },
		{ {  1, -1, -1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 1.0f }, 1 },
		{ {  1,  1, -1, 1 }, { 1, 1 }, { 1.0f, 0.3f, 0.3f }, 1 },
		{ { -1,  1, -1, 1 }, { 1, 0 }, { 0.2f, 1.0f, 0.3f }, 1 },
	};
	// �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ���
	HScanline GetScanline(HTrapezoid trap, int y) {
		HScanline scanlineRet;
		// ��������� ���
		float width = trap.right.v.pos.x - trap.left.v.pos.x;
		// ���X ����
		scanlineRet.x = (int)(trap.left.v.pos.x + 0.5f);
		// ���
		scanlineRet.width = (int)(trap.right.v.pos.x + 0.5f) - scanlineRet.x;
		// y����
		scanlineRet.y = y;
		// ɨ�����
		scanlineRet.v = trap.left.v;
		// 
		if (trap.left.v.pos.x >= trap.right.v.pos.x) scanlineRet.width = 0;
		//���㲽��
		scanlineRet.step = trap.left.v.Step(trap.right.v, width);
		return scanlineRet;
	}


	void DrawScanline(HScanline scanline)
	{
		HScreenDevice* ScreenDevice = HScreenDevice::GetInstance();
		uint32* framebuffer = (uint32*)ScreenDevice->FrameBuff;
		float *zbuffer = ScreenDevice->DepthBuff;

		int x = scanline.x;
		int y = scanline.y;
		int scanlineWidth = scanline.width;
		int ScreenWidth = ScreenDevice->ScreenWidth;
		int ScreenHeight = ScreenDevice->ScreenHeight;

		for (; scanlineWidth > 0; x++, scanlineWidth--) {
			if (x >= 0 && x < ScreenWidth) {
				float rhw = scanline.v.rhw;
				if (rhw >= zbuffer[x + y * ScreenWidth]) {
					float w = 1.0f / rhw;
					zbuffer[x + y * ScreenWidth] = rhw;

					float u = scanline.v.uv.u * w;
					float v = scanline.v.uv.v * w;
					v2f fragIn;
					fragIn.uv.u = u;
					fragIn.uv.v = v;
					uint32 color = frag(fragIn);//ƬԪ��ɫ��
					
					framebuffer[x + y * ScreenWidth] = color;
				}
			}
			scanline.v = scanline.v.Add(scanline.step);
			if (x >= ScreenWidth) break;
		}
	}
	// ����Ⱦ����
	void DrawTrap(HTrapezoid trap) {
		//HScanline scanline;
		int j, top, bottom;
		top = (int)(trap.top + 0.5f);
		bottom = (int)(trap.bottom + 0.5f);
		for (j = top; j < bottom; j++) {
			if (j >= 0 && j < 1024) {//todo
				//��ֵ�õ����ε�����������
				trap.EdgeInterp((float)j + 0.5f);
				//��ʼ��ɨ����
				HScanline scanline = GetScanline(trap, j);
				//����ɨ����
				DrawScanline(scanline);
			}
		}
	}

	//����MVP����
	void UpdateMVPMat()
	{
		HMatrix mat = GetRotateMat(0, 0.8, 0.8);
		Transform.ModleMat = mat;
		Transform.UpdateMVPMat();
	}

	//������������������Ĳü��ռ������
	void InitTriangleClipSpacePos(HTriangle &Triangle)
	{
		//�����ε�ģ���������MVP���� �õ�ͶӰ���꣨����ռ䣩
		Triangle.p1InClipSpace = Triangle.p1InObjectSpace;
		Triangle.p2InClipSpace = Triangle.p2InObjectSpace;
		Triangle.p3InClipSpace = Triangle.p3InObjectSpace;


	}

	//������������������Ĳü��ռ������
	void CalTriangleScreenSpacePos(HTriangle &Triangle)
	{
		//�������������
		Triangle.p1InScreenSpace = Triangle.p1InObjectSpace;
		Triangle.p2InScreenSpace = Triangle.p2InObjectSpace;
		Triangle.p3InScreenSpace = Triangle.p3InObjectSpace;
		//��һ��Ȼ��˿��
		Triangle.p1InScreenSpace.pos = Transform.HomogenizeToScreenCoord(Triangle.p1InClipSpace.pos);
		Triangle.p2InScreenSpace.pos = Transform.HomogenizeToScreenCoord(Triangle.p2InClipSpace.pos);
		Triangle.p3InScreenSpace.pos = Transform.HomogenizeToScreenCoord(Triangle.p3InClipSpace.pos);
		//����Z��Ϣ
		Triangle.p1InScreenSpace.pos.w = Triangle.p1InClipSpace.pos.w;
		Triangle.p2InScreenSpace.pos.w = Triangle.p2InClipSpace.pos.w;
		Triangle.p3InScreenSpace.pos.w = Triangle.p3InClipSpace.pos.w;
	}


	//����������Ƿ��ڲü�
	bool CheckTriangleInCVV(HTriangle &Triangle)
	{
		if (Triangle.p1InClipSpace.pos.CheckInCVV() == false)return false;
		if (Triangle.p2InClipSpace.pos.CheckInCVV() == false)return false;
		if (Triangle.p3InClipSpace.pos.CheckInCVV() == false)return false;
		return true;
	}

	//���Z��ʼ�������ֵ��Ϣ
	void InitTriangleInterpn(HTriangle &Triangle)
	{
		Triangle.p1InScreenSpace.Initrhw();
		Triangle.p2InScreenSpace.Initrhw();
		Triangle.p3InScreenSpace.Initrhw();
	}

	//�򵥵Ķ�����ɫ��
	v2f vert(a2v v)
	{
		v2f output;
		output.pos = Transform.MulMVPMat(v.pos);
		return output;
	}

	//�򵥵�ƬԪ��ɫ��
	uint32 frag(v2f f)
	{
		uint32 color = Texture.RreadTexture(f.uv.u, f.uv.v);
		return color;
	}
		

	//�������� �����
	void DrawTriangle(HTriangle Triangle)
	{
		//1�������������MVP����
		UpdateMVPMat();
	
		//2.1����ʼ���ü��ռ�����
		InitTriangleClipSpacePos(Triangle);

		//2.2�� ������ɫ��
		Triangle.p1InClipSpace.pos = vert(a2v(Triangle.p1InObjectSpace.pos)).pos;
		Triangle.p2InClipSpace.pos = vert(a2v(Triangle.p2InObjectSpace.pos)).pos;
		Triangle.p3InClipSpace.pos = vert(a2v(Triangle.p3InObjectSpace.pos)).pos;
		//����ϸ����ɫ����TODO��
		//������ɫ����TODO��

		//3���òü��ռ�ü������Σ�����Ƚϱ���  һ���㲻��CVV���޳���
		if (CheckTriangleInCVV(Triangle) == false)return;

		//4����һ��Ȼ�����õ���Ļ����
		CalTriangleScreenSpacePos(Triangle);

		//5����ֵ��ʼ��
		InitTriangleInterpn(Triangle);

		//6����Ļ����������β��������Ϊ0-2�����Σ����ҷ��ؿ�����������
		HTrapezoid traps[2];
		int n = Triangle.CalculateTrap(traps);

		//7������ɨ�� ��������
		if (n >= 1) DrawTrap(traps[0]);
		if (n >= 2) DrawTrap(traps[1]);
	}

	//����
	void DrawPlane(int p1_index, int p2_index, int p3_index, int p4_index)
	{
		HVertex p1 = mesh[p1_index];
		HVertex p2 = mesh[p2_index];
		HVertex p3 = mesh[p3_index];
		HVertex p4 = mesh[p4_index];

		//������Ƶ����������
		p1.uv.u = 0;
		p1.uv.v = 0;

		p2.uv.u = 0;
		p2.uv.v = 1;

		p3.uv.u = 1;
		p3.uv.v = 1;

		p4.uv.u = 1;
		p4.uv.v = 0;
		HTriangle T1;
		T1.p1InObjectSpace = p1;
		T1.p2InObjectSpace = p2;
		T1.p3InObjectSpace = p3;

		HTriangle T2;
		T2.p1InObjectSpace = p3;
		T2.p2InObjectSpace = p4;
		T2.p3InObjectSpace = p1;
		DrawTriangle(T1);
		DrawTriangle(T2);
	}

	//��������
	void DrawBox()
	{
		DrawPlane(0, 1, 2, 3);//ǰ��
		DrawPlane(7, 6, 5, 4);//����
		DrawPlane(0, 4, 5, 1);//����
		DrawPlane(1, 5, 6, 2);//����
		DrawPlane(2, 6, 7, 3);//����
		DrawPlane(3, 7, 4, 0);//����
	}

	void Draw()
	{
		DrawBox();
	}

};
//===========================��դ���׶� End=============================

//=====ʵ������
//1����ʼ����һ�������� HScreenDevice::GetInstance()->shape = new HCube();
//2������				HScreenDevice::GetInstance()->Draw();