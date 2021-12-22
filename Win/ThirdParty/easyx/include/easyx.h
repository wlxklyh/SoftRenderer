/******************************************************
 * EasyX Library for C++ (Ver:20210719)
 * https://easyx.cn
 *
 * EasyX.h
 *		Provides the latest API.
 ******************************************************/

#pragma once

#ifndef WINVER
#define WINVER 0x0400			// Specifies that the minimum required platform is Windows 95 and Windows NT 4.0.
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500		// Specifies that the minimum required platform is Windows 2000.
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410	// Specifies that the minimum required platform is Windows 98.
#endif

#ifdef UNICODE
	#pragma comment(lib,"EasyXw.lib")
#else
	#pragma comment(lib,"EasyXa.lib")
#endif


#ifndef __cplusplus
#error EasyX is only for C++
#endif

#include <windows.h>
#include <tchar.h>

// EasyX Window Properties
#define EW_SHOWCONSOLE		1		// Show console window
#define EW_NOCLOSE			2		// Disable the close button
#define EW_NOMINIMIZE		4		// Disable the minimize button
#define EW_DBLCLKS			8		// Support double-click events


// ��ɫ
#define	BLACK			0
#define	BLUE			0xAA0000
#define	GREEN			0x00AA00
#define	CYAN			0xAAAA00
#define	RED				0x0000AA
#define	MAGENTA			0xAA00AA
#define	BROWN			0x0055AA
#define	LIGHTGRAY		0xAAAAAA
#define	DARKGRAY		0x555555
#define	LIGHTBLUE		0xFF5555
#define	LIGHTGREEN		0x55FF55
#define	LIGHTCYAN		0xFFFF55
#define	LIGHTRED		0x5555FF
#define	LIGHTMAGENTA	0xFF55FF
#define	YELLOW			0x55FFFF
#define	WHITE			0xFFFFFF

// ������ɫת����
#define BGR(color)	( (((color) & 0xFF) << 16) | ((color) & 0xFF00FF00) | (((color) & 0xFF0000) >> 16) )


class IMAGE;

// �����ߵ���ʽ
class LINESTYLE
{
public:
	LINESTYLE();
	LINESTYLE(const LINESTYLE &style);
	LINESTYLE& operator = (const LINESTYLE &style);			// ��ֵ��������غ���
	virtual ~LINESTYLE();

	DWORD	style;
	DWORD	thickness;
	DWORD	*puserstyle;
	DWORD	userstylecount;
};

class FILLSTYLE
{
public:
	FILLSTYLE();
	FILLSTYLE(const FILLSTYLE &style);
	FILLSTYLE& operator = (const FILLSTYLE &style);			// ��ֵ��������غ���
	virtual ~FILLSTYLE();

	int			style;			// �����ʽ
	long		hatch;			// ���ͼ����ʽ
	IMAGE*		ppattern;		// ���ͼ��
};

// ����ͼ�����
class IMAGE
{
public:
	int getwidth() const;	// ��ȡ����Ŀ��
	int getheight() const;	// ��ȡ����ĸ߶�

private:
	int width, height;		// ����Ŀ��
	HBITMAP m_hBmp;
	HDC m_hMemDC;
	float m_data[6];
	COLORREF	m_LineColor;		// ��ǰ������ɫ
	COLORREF	m_FillColor;		// ��ǰ�����ɫ
	COLORREF	m_TextColor;		// ��ǰ������ɫ
	COLORREF	m_BkColor;			// ��ǰ������ɫ
	DWORD*		m_pBuffer;			// ��ͼ�����ڴ�

	LINESTYLE	m_LineStyle;		// ������ʽ
	FILLSTYLE	m_FillStyle;		// �����ʽ

	virtual void SetDefault();						// ����ΪĬ��״̬

public:
	IMAGE(int _width = 0, int _height = 0);			// ����ͼ��
	IMAGE(const IMAGE &img);						// �������캯��
	IMAGE& operator = (const IMAGE &img);			// ��ֵ��������غ���
	virtual ~IMAGE();
	virtual void Resize(int _width, int _height);	// �����ߴ�
};



// ��ͼģʽ��غ���

HWND initgraph(int width, int height, int flag = NULL);	// ��ʼ��ͼ�λ���
void closegraph();										// �ر�ͼ�λ���

// ��ͼ��������

void cleardevice();					// ����
void setcliprgn(HRGN hrgn);			// ���õ�ǰ��ͼ�豸�Ĳü���
void clearcliprgn();				// ����ü�������Ļ����

void getlinestyle(LINESTYLE* pstyle);						// ��ȡ��ǰ������ʽ
void setlinestyle(const LINESTYLE* pstyle);					// ���õ�ǰ������ʽ
void setlinestyle(int style, int thickness = 1, const DWORD *puserstyle = NULL, DWORD userstylecount = 0);	// ���õ�ǰ������ʽ
void getfillstyle(FILLSTYLE* pstyle);						// ��ȡ��ǰ�����ʽ
void setfillstyle(const FILLSTYLE* pstyle);					// ���õ�ǰ�����ʽ
void setfillstyle(int style, long hatch = NULL, IMAGE* ppattern = NULL);		// ���õ�ǰ�����ʽ
void setfillstyle(BYTE* ppattern8x8);						// ���õ�ǰ�����ʽ

void setorigin(int x, int y);							// ��������ԭ��
void getaspectratio(float *pxasp, float *pyasp);		// ��ȡ��ǰ��������
void setaspectratio(float xasp, float yasp);			// ���õ�ǰ��������

int  getrop2();						// ��ȡǰ���Ķ�Ԫ��դ����ģʽ
void setrop2(int mode);				// ����ǰ���Ķ�Ԫ��դ����ģʽ
int  getpolyfillmode();				// ��ȡ��������ģʽ
void setpolyfillmode(int mode);		// ���ö�������ģʽ

void graphdefaults();				// �������л�ͼ����ΪĬ��ֵ

COLORREF getlinecolor();			// ��ȡ��ǰ������ɫ
void setlinecolor(COLORREF color);	// ���õ�ǰ������ɫ
COLORREF gettextcolor();			// ��ȡ��ǰ������ɫ
void settextcolor(COLORREF color);	// ���õ�ǰ������ɫ
COLORREF getfillcolor();			// ��ȡ��ǰ�����ɫ
void setfillcolor(COLORREF color);	// ���õ�ǰ�����ɫ
COLORREF getbkcolor();				// ��ȡ��ǰ��ͼ����ɫ
void setbkcolor(COLORREF color);	// ���õ�ǰ��ͼ����ɫ
int  getbkmode();					// ��ȡ�������ģʽ
void setbkmode(int mode);			// ���ñ������ģʽ

// ��ɫģ��ת������
COLORREF RGBtoGRAY(COLORREF rgb);
void RGBtoHSL(COLORREF rgb, float *H, float *S, float *L);
void RGBtoHSV(COLORREF rgb, float *H, float *S, float *V);
COLORREF HSLtoRGB(float H, float S, float L);
COLORREF HSVtoRGB(float H, float S, float V);


// ��ͼ����

COLORREF getpixel(int x, int y);				// ��ȡ�����ɫ
void putpixel(int x, int y, COLORREF color);	// ����

void line(int x1, int y1, int x2, int y2);		// ����

void rectangle	   (int left, int top, int right, int bottom);	// ������
void fillrectangle (int left, int top, int right, int bottom);	// ��������(�б߿�)
void solidrectangle(int left, int top, int right, int bottom);	// ��������(�ޱ߿�)
void clearrectangle(int left, int top, int right, int bottom);	// ��վ�������

void circle		(int x, int y, int radius);		// ��Բ
void fillcircle (int x, int y, int radius);		// �����Բ(�б߿�)
void solidcircle(int x, int y, int radius);		// �����Բ(�ޱ߿�)
void clearcircle(int x, int y, int radius);		// ���Բ������

void ellipse	 (int left, int top, int right, int bottom);	// ����Բ
void fillellipse (int left, int top, int right, int bottom);	// �������Բ(�б߿�)
void solidellipse(int left, int top, int right, int bottom);	// �������Բ(�ޱ߿�)
void clearellipse(int left, int top, int right, int bottom);	// �����Բ������

void roundrect	   (int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight);		// ��Բ�Ǿ���
void fillroundrect (int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight);		// �����Բ�Ǿ���(�б߿�)
void solidroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight);		// �����Բ�Ǿ���(�ޱ߿�)
void clearroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight);		// ���Բ�Ǿ�������

void arc	 (int left, int top, int right, int bottom, double stangle, double endangle);	// ����Բ��(��ʼ�ǶȺ���ֹ�Ƕ�Ϊ������)
void pie	 (int left, int top, int right, int bottom, double stangle, double endangle);	// ����Բ����(��ʼ�ǶȺ���ֹ�Ƕ�Ϊ������)
void fillpie (int left, int top, int right, int bottom, double stangle, double endangle);	// �������Բ����(�б߿�)
void solidpie(int left, int top, int right, int bottom, double stangle, double endangle);	// �������Բ����(�ޱ߿�)
void clearpie(int left, int top, int right, int bottom, double stangle, double endangle);	// �����Բ��������

void polyline	 (const POINT *points, int num);								// ��������������
void polygon	 (const POINT *points, int num);								// �������
void fillpolygon (const POINT *points, int num);								// �����Ķ����(�б߿�)
void solidpolygon(const POINT *points, int num);								// �����Ķ����(�ޱ߿�)
void clearpolygon(const POINT *points, int num);								// ��ն��������

void polybezier(const POINT *points, int num);									// ������������
void floodfill(int x, int y, COLORREF color, int filltype = FLOODFILLBORDER);	// �������



// ������غ���
void outtextxy(int x, int y, LPCTSTR str);	// ��ָ��λ������ַ���
void outtextxy(int x, int y, TCHAR c);		// ��ָ��λ������ַ�
int textwidth(LPCTSTR str);					// ��ȡ�ַ���ռ�õ����ؿ�
int textwidth(TCHAR c);						// ��ȡ�ַ�ռ�õ����ؿ�
int textheight(LPCTSTR str);				// ��ȡ�ַ���ռ�õ����ظ�
int textheight(TCHAR c);					// ��ȡ�ַ�ռ�õ����ظ�
int drawtext(LPCTSTR str, RECT* pRect, UINT uFormat);	// ��ָ����������ָ����ʽ����ַ���
int drawtext(TCHAR c, RECT* pRect, UINT uFormat);		// ��ָ����������ָ����ʽ����ַ�

// ���õ�ǰ������ʽ(�������)
//		nHeight: �ַ���ƽ���߶ȣ�
//		nWidth: �ַ���ƽ�����(0 ��ʾ����Ӧ)��
//		lpszFace: �������ƣ�
//		nEscapement: �ַ�������д�Ƕ�(��λ 0.1 ��)��
//		nOrientation: ÿ���ַ�����д�Ƕ�(��λ 0.1 ��)��
//		nWeight: �ַ��ıʻ���ϸ(0 ��ʾĬ�ϴ�ϸ)��
//		bItalic: �Ƿ�б�壻
//		bUnderline: �Ƿ��»��ߣ�
//		bStrikeOut: �Ƿ�ɾ���ߣ�
//		fbCharSet: ָ���ַ�����
//		fbOutPrecision: ָ�����ֵ�������ȣ�
//		fbClipPrecision: ָ�����ֵļ�����ȣ�
//		fbQuality: ָ�����ֵ����������
//		fbPitchAndFamily: ָ���Գ��淽ʽ�������������ϵ�С�
void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace);
void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily);
void settextstyle(const LOGFONT *font);	// ���õ�ǰ������ʽ
void gettextstyle(LOGFONT *font);		// ��ȡ��ǰ������ʽ



// ͼ�������
void loadimage(IMAGE *pDstImg, LPCTSTR pImgFile, int nWidth = 0, int nHeight = 0, bool bResize = false);					// ��ͼƬ�ļ���ȡͼ��(bmp/gif/jpg/png/tif/emf/wmf/ico)
void loadimage(IMAGE *pDstImg, LPCTSTR pResType, LPCTSTR pResName, int nWidth = 0, int nHeight = 0, bool bResize = false);	// ����Դ�ļ���ȡͼ��(bmp/gif/jpg/png/tif/emf/wmf/ico)
void saveimage(LPCTSTR pImgFile, IMAGE* pImg = NULL);																		// ����ͼ��(bmp/gif/jpg/png/tif)
void getimage(IMAGE *pDstImg, int srcX, int srcY, int srcWidth, int srcHeight);												// �ӵ�ǰ��ͼ�豸��ȡͼ��
void putimage(int dstX, int dstY, const IMAGE *pSrcImg, DWORD dwRop = SRCCOPY);												// ����ͼ����Ļ
void putimage(int dstX, int dstY, int dstWidth, int dstHeight, const IMAGE *pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);		// ����ͼ����Ļ(ָ�����)
void rotateimage(IMAGE *dstimg, IMAGE *srcimg, double radian, COLORREF bkcolor = BLACK, bool autosize = false, bool highquality = true);// ��תͼ��
void Resize(IMAGE* pImg, int width, int height);	// ������ͼ�豸�Ĵ�С
DWORD* GetImageBuffer(IMAGE* pImg = NULL);			// ��ȡ��ͼ�豸���Դ�ָ��
IMAGE* GetWorkingImage();							// ��ȡ��ǰ��ͼ�豸
void SetWorkingImage(IMAGE* pImg = NULL);			// ���õ�ǰ��ͼ�豸
HDC GetImageHDC(IMAGE* pImg = NULL);				// ��ȡ��ͼ�豸���(HDC)


// ��������

int	getwidth();			// ��ȡ��ͼ�����
int	getheight();		// ��ȡ��ͼ���߶�

void BeginBatchDraw();	// ��ʼ��������
void FlushBatchDraw();	// ִ��δ��ɵĻ�������
void FlushBatchDraw(int left, int top, int right, int bottom);	// ִ��ָ��������δ��ɵĻ�������
void EndBatchDraw();	// �����������ƣ���ִ��δ��ɵĻ�������
void EndBatchDraw(int left, int top, int right, int bottom);	// �����������ƣ���ִ��ָ��������δ��ɵĻ�������

HWND GetHWnd();								// ��ȡ��ͼ���ھ��(HWND)
TCHAR* GetEasyXVer();						// ��ȡ EasyX ��ǰ�汾

// ��ȡ�û�����
bool InputBox(LPTSTR pString, int nMaxCount, LPCTSTR pPrompt = NULL, LPCTSTR pTitle = NULL, LPCTSTR pDefault = NULL, int width = 0, int height = 0, bool bOnlyOK = true);



// Message
//
//	Category	Type				Description
//
//	EM_MOUSE	WM_MOUSEMOVE		The mouse moves.
//				WM_MOUSEWHEEL		The mouse wheel is rotated.
//				WM_LBUTTONDOWN		The left mouse button is pressed.
//				WM_LBUTTONUP		The left mouse button is released.
//				WM_LBUTTONDBLCLK	The left mouse button is double-clicked.
//				WM_MBUTTONDOWN		The middle mouse button is pressed.
//				WM_MBUTTONUP		The middle mouse button is released.
//				WM_MBUTTONDBLCLK	The middle mouse button is double-clicked.
//				WM_RBUTTONDOWN		The right mouse button is pressed.
//				WM_RBUTTONUP		The right mouse button is released.
//				WM_RBUTTONDBLCLK	The right mouse button is double-clicked.
//
//	EM_KEY		WM_KEYDOWN			A key is pressed.
//				WM_KEYUP			A key is released.
//
//	EM_CHAR		WM_CHAR
//
//	EM_WINDOW	WM_ACTIVATE			The window is activated or deactivated.
//				WM_MOVE				The window has been moved.
//				WM_SIZE				The size of window window has changed.

// Message Category
#define EM_MOUSE	1
#define EM_KEY		2
#define EM_CHAR		4
#define EM_WINDOW	8

// Message Structure
struct EASYXMSG
{
	USHORT message;					// The message identifier.
	union
	{
		// Data of the mouse message
		struct
		{
			bool ctrl		:1;		// Indicates whether the CTRL key is down.
			bool shift		:1;		// Indicates whether the SHIFT key is down.
			bool lbutton	:1;		// Indicates whether the left mouse button is down.
			bool mbutton	:1;		// Indicates whether the middle mouse button is down.
			bool rbutton	:1;		// Indicates whether the right mouse button is down.
			short x;				// The x-coordinate of the cursor.
			short y;				// The y-coordinate of the cursor.
			short wheel;			// The distance the wheel is rotated, expressed in multiples or divisions of 120.
		};

		// Data of the key message
		struct
		{
			BYTE vkcode;			// The virtual-key code of the key.
			BYTE scancode;			// The scan code of the key. The value depends on the OEM.
			bool extended	:1;		// Indicates whether the key is an extended key, such as a function key or a key on the numeric keypad. The value is true if the key is an extended key; otherwise, it is false.
			bool prevdown	:1;		// Indicates whether the key is previously up or down.
		};

		// Data of the char message
		TCHAR ch;

		// Data of the window message
		struct
		{
			WPARAM wParam;
			LPARAM lParam;
		};
	};
};

// Message Function
EASYXMSG getmessage(BYTE filter = -1);										// Get a messages until a message is available for retrieval.
void getmessage(EASYXMSG *msg, BYTE filter = -1);							// Get a messages until a message is available for retrieval.
bool peekmessage(EASYXMSG *msg, BYTE filter = -1, bool removemsg = true);	// Get a message if any exist, otherwise return false.
void flushmessage(BYTE filter = -1);										// Flush the message buffer.
