#pragma once
//#pragma comment( lib, "UxTheme.lib" )

#include <afxwin.h>
#include <math.h>
//#include <uxtheme.h>
#include "pwio.h"

typedef struct tagRGBAQUAD {
        BYTE    rgbaBlue;
        BYTE    rgbaGreen;
        BYTE    rgbaRed;
        BYTE    rgbaAlpha;
} RGBAQUAD;
typedef RGBAQUAD FAR* LPRGBAQUAD;

//#define RGBA(r, g, b, a)  ( (DWORD) ( ((BYTE) (r) | ((WORD) (g) << 8)) | (( (DWORD) (BYTE) (b) ) << 16) | (( (DWORD) (BYTE) (a) ) << 24) ) )
#define RGBA(r, g, b, a) ((DWORD) (RGB(r, g, b) | (((DWORD)(BYTE)(a)) << 24)) )

#define ALPHABYTE(sc, dc, sa) ((BYTE)(sc * (sa / 255.0) + dc * (1.0 - (sa / 255.0))))
#define GRV(dw) LOBYTE(LOWORD(dw))
#define GGV(dw) HIBYTE(LOWORD(dw))
#define GBV(dw) LOBYTE(HIWORD(dw))
#define GAV(dw) HIBYTE(HIWORD(dw))

#define BITS_XY(x, y, w, h) (((h - (y)) * w) - (w - (x)))


class CPWGraphics
{
public:
	enum BtnState
	{
		stateNormal,
		statePressed,
		stateHot,
		stateDefault,
		stateDisable
	};

public:
	typedef class CBtnStateEx
	{
	public:
		CBtnStateEx()
		: iState(CBtnStateEx::Normal) {}
	public:
		static int Normal;
		static int Pressed;
		static int Hot;
		static int Default;
		static int Disable;
		static int Focused;

	private:
		void SetState(int iStt, bool bKeepDef = true, bool bKeepDis = true, bool bKeepFocs = true)
		{
			int iKeep = 0;

			if(bKeepDef)  iKeep  = iState & CBtnStateEx::Default;
			if(bKeepDis)  iKeep |= iState & CBtnStateEx::Disable;
			if(bKeepFocs) iKeep |= iState & CBtnStateEx::Focused;

			iState = iStt | iKeep;
		}
		void BitState(bool bset, int iStt) { iState = (iState & ~iStt) | (bset ? iStt : 0); }

	public:
		bool IsState(int iStt) { return (iState & iStt) == iStt; }

		void SetNormal(void)			  { SetState(CBtnStateEx::Normal);  }
		void SetPressed(void)			  { SetState(CBtnStateEx::Pressed); }
		void SetHot(void)				  { SetState(CBtnStateEx::Hot);     }
		void SetDefault(bool bset = true) { BitState(bset, CBtnStateEx::Default); }
		void SetDisable(bool bset = true) { BitState(bset, CBtnStateEx::Disable); }
		void SetFocused(bool bset = true) { BitState(bset, CBtnStateEx::Focused); }

		void BitNormal(bool bset = true)  { BitState(bset, CBtnStateEx::Normal);  }
		void BitPressed(bool bset = true) { BitState(bset, CBtnStateEx::Pressed); }
		void BitHot(bool bset = true)     { BitState(bset, CBtnStateEx::Hot);     }
		void BitDefault(bool bset = true) { BitState(bset, CBtnStateEx::Default); }
		void BitDisable(bool bset = true) { BitState(bset, CBtnStateEx::Disable); }
		void BitFocused(bool bset = true) { BitState(bset, CBtnStateEx::Focused); }

		bool IsNormal(void)  { return IsState(CBtnStateEx::Normal);  }
		bool IsPressed(void) { return IsState(CBtnStateEx::Pressed); }
		bool IsHot(void)     { return IsState(CBtnStateEx::Hot);     }
		bool IsDefault(void) { return IsState(CBtnStateEx::Default); }
		bool IsDisable(void) { return IsState(CBtnStateEx::Disable); }
		bool IsFocused(void) { return IsState(CBtnStateEx::Focused); }

	public:
		int iState;
	} BtnStateEx ;

public:
	typedef class CAlignment
	{
	public:
		CAlignment(int iHorizontal = CAlignment::Center, int iVertical = CAlignment::Middle)
		: iHAlign(iHorizontal)
		, iVAlign(iVertical) {}
	public:
		static int Left;
		static int Right;
		static int Center;
		static int HorAbsolute;
		static int Top;
		static int Bottom;
		static int Middle;
		static int VerAbsolute;

	private:
		int iHAlign;
		int iVAlign;

	public:
		void SetHorizontalAlign(int iHorizontal) { iHAlign = iHorizontal; }
		int GetHorizontalAlign() { return iHAlign; }

		void SetVerticalAlign(int iVertical) { iVAlign = iVertical; }
		int GetVerticalAlign() { return iVAlign; }

		void SetLeft()   { iHAlign = CAlignment::Left; }
		void SetRight()  { iHAlign = CAlignment::Right; }
		void SetCenter() { iHAlign = CAlignment::Center; }
		void SetTop()    { iVAlign = CAlignment::Top; }
		void SetBottom() { iVAlign = CAlignment::Bottom; }
		void SetMiddle() { iVAlign = CAlignment::Middle; }
		void SetHorAbsolute() { iHAlign = CAlignment::HorAbsolute; }
		void SetVerAbsolute() { iVAlign = CAlignment::VerAbsolute; }

		bool IsLeft()    { return iHAlign == CAlignment::Left; }
		bool IsRight()   { return iHAlign == CAlignment::Right; }
		bool IsCenter()  { return iHAlign == CAlignment::Center; }
		bool IsTop()     { return iVAlign == CAlignment::Top; }
		bool IsBottom()  { return iVAlign == CAlignment::Bottom; }
		bool IsMiddle()  { return iVAlign == CAlignment::Middle; }
		bool IsHorAbsolute() { return iHAlign == CAlignment::HorAbsolute; }
		bool IsVerAbsolute() { return iVAlign == CAlignment::VerAbsolute; }

	} Alignment;

public:
	CPWGraphics(void);
	~CPWGraphics(void);
	//static BOOL IsThemeUsed(void);
	static void DrawLine(CWnd* pWnd, CDC* pDC, COLORREF cr, int x, int y, int cx = -1, int nSize = 1);
	static void DrawLineEx(CDC* pDC, COLORREF cr, int x1, int y1, int x2, int y2, int nSize = 1, int nPenStyle = PS_SOLID);
	static void GradientFillDC(CDC* pDC,int x,int y,int cx, int cy, COLORREF crFrom, COLORREF crTo, int iWayDraw = 1 /* (==1) = x way, (!=1) = y way */);
	static COLORREF Blend(COLORREF crFrom, COLORREF crTo, double percent);

	static BOOL BlitReplaceBitmapColor(HBITMAP hbm, COLORREF rgbOld, COLORREF rgbNew, HDC hdcBm = NULL);

	static CBitmap* CPWGraphics::BmpChangeColor(CBitmap* pBmp, COLORREF crFrom, COLORREF crTo);
	//static CBitmap* BmpChangeColor(HBITMAP hBitmap, COLORREF crFrom, COLORREF crTo);
	static CString ColRef2HTML(COLORREF colrefColor);
	static COLORREF OffsetColor(COLORREF colrefColor, int iOffset);
	static void DrawLine(HDC hDC, int X1, int Y1, int X2, int Y2, COLORREF colrefColor);
	static void DrawVGradient(HDC hDC, COLORREF colrefEndColor, COLORREF colrefStartColor, int x, int y, int x2, int y2);
	static void CPWGraphics::DrawPlasticRect(HDC hDC, LPRECT prc, COLORREF color);
	static void DrawPlastic(HDC hDC, CPWGraphics::BtnState state, LPRECT prc, COLORREF colBk = -1, COLORREF colHiLi = -1,COLORREF colHot = -1, BOOL bBkOnly = FALSE);
	static void DrawPlasticBorder(HDC hDC, LPRECT prc, COLORREF col = -1);
	static UINT GABS(const int& nmbr);
	static COLORREF DifColor(COLORREF color, UINT nDist);
	static COLORREF OrColor(COLORREF colFore, COLORREF colBK, UINT nForePercent);
	static COLORREF DrawGlassRect(CDC *pDC, LPRECT prc, COLORREF col, int iReflectAmount = 32);

	enum CCType
	{
		cc_mono,
		cc_bit3,
		cc_full
	};

	static COLORREF CrossColor(COLORREF color, CPWGraphics::CCType itype = cc_full);
	static void GlowText(CDC *pDC, LPRECT lprc, COLORREF colText, int nBlendPercent = 40, COLORREF colBlend = 0xffffffff);
	static void DrawTransparentBg(CDC *pDC, LPRECT lprc, int iDotSize = 4, COLORREF colDark = 0xB4B4B4, COLORREF colLight = 0xFFFFFF);
	static void FillAlphaRect(CDC *pDC, LPRECT lprc, COLORREF crAlphaColor);
	static BYTE GetAValue(DWORD rgba) { return HIBYTE(HIWORD(rgba)); }

	// Wu antializing functions -----------------------------------------------------
	static void DrawPixel(CDC *pDC, short x, short y, short color);
	static void DrawWuCirlce (CDC * pDC, int x, int y, int r);
	static void DrawWuLine(CDC *pDC, short X0, short Y0, short X1, short Y1, short BaseColor, short NumLevels, unsigned short IntensityBits);
	static void DrawWuLine(CDC *pDC, int X0, int Y0, int X1, int Y1, COLORREF clrLine);
	static BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB);
	static HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression = BI_RGB /*, CPalette* pPal*/ );
};
