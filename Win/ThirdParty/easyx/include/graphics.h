/******************************************************
 * EasyX Library for C++ (Ver:20210719)
 * https://easyx.cn
 *
 * graphics.h
 *		Based on easyx.h and retaining several old API.
 ******************************************************/

#pragma once

#include <easyx.h>

// 兼容 initgraph 绘图模式的宏定义（无实际意义）
#define DETECT	0
#define VGA		0
#define	VGALO	0
#define VGAMED	0
#define VGAHI	0
#define CGA		0
#define	CGAC0	0
#define	CGAC1	0
#define CGAC2	0
#define CGAC3	0
#define CGAHI	0
#define EGA		0
#define EGALO	0
#define	EGAHI	0

// 绘图窗口初始化参数（已废弃，请使用新的宏定义）
#define SHOWCONSOLE		1		// 创建图形窗口时，保留控制台的显示
#define NOCLOSE			2		// 没有关闭功能
#define NOMINIMIZE		4		// 没有最小化功能

// BGI 格式的初始化图形设备，默认 640 x 480
HWND initgraph(int* gdriver, int* gmode, char* path);

void bar(int left, int top, int right, int bottom);		// 画无边框填充矩形
void bar3d(int left, int top, int right, int bottom, int depth, bool topflag);	// 画有边框三维填充矩形

void drawpoly(int numpoints, const int *polypoints);	// 画多边形
void fillpoly(int numpoints, const int *polypoints);	// 画填充的多边形

int getmaxx();					// 获取最大的宽度值
int getmaxy();					// 获取最大的高度值

COLORREF getcolor();			// 获取当前绘图前景色
void setcolor(COLORREF color);	// 设置当前绘图前景色

void setwritemode(int mode);	// 设置前景的二元光栅操作模式




///////////////////////////////////////////////////////////////////////////////////
// 以下函数仅为兼容早期 EasyX 的接口，不建议使用。请使用相关新函数替换。
//

#if _MSC_VER > 1200
	#define _EASYX_DEPRECATE					__declspec(deprecated("This function is deprecated."))
	#define _EASYX_DEPRECATE_WITHNEW(_NewFunc)	__declspec(deprecated("This function is deprecated. Instead, use this new function: " #_NewFunc ". See https://docs.easyx.cn/" #_NewFunc " for details."))
	#define _EASYX_DEPRECATE_OVERLOAD(_Func)	__declspec(deprecated("This overload is deprecated. See https://docs.easyx.cn/" #_Func " for details."))
#else
	#define _EASYX_DEPRECATE
	#define _EASYX_DEPRECATE_WITHNEW(_NewFunc)
	#define _EASYX_DEPRECATE_OVERLOAD(_Func)
#endif

// 设置当前字体样式(该函数已不再使用，请使用 settextstyle 代替)
//		nHeight: 字符的平均高度；
//		nWidth: 字符的平均宽度(0 表示自适应)；
//		lpszFace: 字体名称；
//		nEscapement: 字符串的书写角度(单位 0.1 度)；
//		nOrientation: 每个字符的书写角度(单位 0.1 度)；
//		nWeight: 字符的笔画粗细(0 表示默认粗细)；
//		bItalic: 是否斜体；
//		bUnderline: 是否下划线；
//		bStrikeOut: 是否删除线；
//		fbCharSet: 指定字符集；
//		fbOutPrecision: 指定文字的输出精度；
//		fbClipPrecision: 指定文字的剪辑精度；
//		fbQuality: 指定文字的输出质量；
//		fbPitchAndFamily: 指定以常规方式描述字体的字体系列。
_EASYX_DEPRECATE_WITHNEW(settextstyle) void setfont(int nHeight, int nWidth, LPCTSTR lpszFace);
_EASYX_DEPRECATE_WITHNEW(settextstyle) void setfont(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
_EASYX_DEPRECATE_WITHNEW(settextstyle) void setfont(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily);
_EASYX_DEPRECATE_WITHNEW(settextstyle) void setfont(const LOGFONT *font);	// 设置当前字体样式
_EASYX_DEPRECATE_WITHNEW(gettextstyle) void getfont(LOGFONT *font);			// 获取当前字体样式

// 以下填充样式不再使用，新的填充样式请参见帮助文件
#define	NULL_FILL			BS_NULL
#define	EMPTY_FILL			BS_NULL
#define	SOLID_FILL			BS_SOLID
// 普通一组
#define	BDIAGONAL_FILL		BS_HATCHED, HS_BDIAGONAL					// /// 填充 (对应 BGI 的 LTSLASH_FILL)
#define CROSS_FILL			BS_HATCHED, HS_CROSS						// +++ 填充
#define DIAGCROSS_FILL		BS_HATCHED, HS_DIAGCROSS					// xxx 填充 (heavy cross hatch fill) (对应 BGI 的 XHTACH_FILL)
#define DOT_FILL			(BYTE*)"\x80\x00\x08\x00\x80\x00\x08\x00"	// xxx 填充 (对应 BGI 的 WIDE_DOT_FILL)	
#define FDIAGONAL_FILL		BS_HATCHED, HS_FDIAGONAL					// \\\ 填充
#define HORIZONTAL_FILL		BS_HATCHED, HS_HORIZONTAL					// === 填充
#define VERTICAL_FILL		BS_HATCHED, HS_VERTICAL						// ||| 填充
// 密集一组
#define BDIAGONAL2_FILL		(BYTE*)"\x44\x88\x11\x22\x44\x88\x11\x22"
#define CROSS2_FILL			(BYTE*)"\xff\x11\x11\x11\xff\x11\x11\x11"	// (对应 BGI 的 fill HATCH_FILL)
#define DIAGCROSS2_FILL		(BYTE*)"\x55\x88\x55\x22\x55\x88\x55\x22"
#define DOT2_FILL			(BYTE*)"\x88\x00\x22\x00\x88\x00\x22\x00"	// (对应 BGI 的 CLOSE_DOT_FILL)
#define FDIAGONAL2_FILL		(BYTE*)"\x22\x11\x88\x44\x22\x11\x88\x44"
#define HORIZONTAL2_FILL	(BYTE*)"\x00\x00\xff\x00\x00\x00\xff\x00"
#define VERTICAL2_FILL		(BYTE*)"\x11\x11\x11\x11\x11\x11\x11\x11"
// 粗线一组
#define BDIAGONAL3_FILL		(BYTE*)"\xe0\xc1\x83\x07\x0e\x1c\x38\x70"	// (对应 BGI 的 SLASH_FILL)
#define CROSS3_FILL			(BYTE*)"\x30\x30\x30\x30\x30\x30\xff\xff"
#define DIAGCROSS3_FILL		(BYTE*)"\xc7\x83\xc7\xee\x7c\x38\x7c\xee"
#define DOT3_FILL			(BYTE*)"\xc0\xc0\x0c\x0c\xc0\xc0\x0c\x0c"
#define FDIAGONAL3_FILL		(BYTE*)"\x07\x83\xc1\xe0\x70\x38\x1c\x0e"
#define HORIZONTAL3_FILL	(BYTE*)"\xff\xff\x00\x00\xff\xff\x00\x00"	// (对应 BGI 的 LINE_FILL)	
#define VERTICAL3_FILL		(BYTE*)"\x33\x33\x33\x33\x33\x33\x33\x33"
// 其它
#define INTERLEAVE_FILL		(BYTE*)"\xcc\x33\xcc\x33\xcc\x33\xcc\x33"	// (对应 BGI 的 INTERLEAVE_FILL)

// 关于“当前点”的函数
_EASYX_DEPRECATE	int	getx();								// 获取当前 x 坐标
_EASYX_DEPRECATE	int	gety();								// 获取当前 y 坐标
_EASYX_DEPRECATE	void moveto(int x, int y);				// 移动当前点(绝对坐标)
_EASYX_DEPRECATE	void moverel(int dx, int dy);			// 移动当前点(相对坐标)
_EASYX_DEPRECATE	void lineto(int x, int y);				// 画线(至绝对坐标)
_EASYX_DEPRECATE	void linerel(int dx, int dy);			// 画线(至相对坐标)
_EASYX_DEPRECATE	void outtext(LPCTSTR str);				// 在当前位置输出字符串
_EASYX_DEPRECATE	void outtext(TCHAR c);					// 在当前位置输出字符

// 关于“鼠标操作”的函数
// 鼠标消息
// 支持如下消息：
//		WM_MOUSEMOVE		鼠标移动
//		WM_MOUSEWHEEL		鼠标滚轮拨动
//		WM_LBUTTONDOWN		左键按下
//		WM_LBUTTONUP		左键弹起
//		WM_LBUTTONDBLCLK	左键双击
//		WM_MBUTTONDOWN		中键按下
//		WM_MBUTTONUP		中键弹起
//		WM_MBUTTONDBLCLK	中键双击
//		WM_RBUTTONDOWN		右键按下
//		WM_RBUTTONUP		右键弹起
//		WM_RBUTTONDBLCLK	右键双击
struct MOUSEMSG
{
	UINT uMsg;				// 当前鼠标消息
	bool mkCtrl		:1;		// Ctrl 键是否按下
	bool mkShift	:1;		// Shift 键是否按下
	bool mkLButton	:1;		// 鼠标左键是否按下
	bool mkMButton	:1;		// 鼠标中键是否按下
	bool mkRButton	:1;		// 鼠标右键是否按下
	short x;				// 当前鼠标 x 坐标
	short y;				// 当前鼠标 y 坐标
	short wheel;			// 鼠标滚轮滚动值 (120 的倍数)
};
_EASYX_DEPRECATE							bool MouseHit();			// 检查是否存在鼠标消息
_EASYX_DEPRECATE_WITHNEW(getmessage)		MOUSEMSG GetMouseMsg();		// 获取一个鼠标消息。如果没有，就等待
_EASYX_DEPRECATE_WITHNEW(peekmessage)		bool PeekMouseMsg(MOUSEMSG *pMsg, bool bRemoveMsg = true);	// 获取一个鼠标消息，并立即返回
_EASYX_DEPRECATE_WITHNEW(flushmessage)		void FlushMouseMsgBuffer();	// 清空鼠标消息
