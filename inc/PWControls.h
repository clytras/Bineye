// PWControls.h: interface for the CPWControls class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PWCONTROLS_H
#define PWCONTROLS_H

// for mfc support
#include <afxwin.h>
#include <afxext.h>
#include <afxdtctl.h>
#include <afxcmn.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPWControls  
{
public:
	static const int bbfBitmapText,
					 bbfTextBitmap,

					 bbfHorAlignLeft,
					 bbfHorAlignCenter,
					 bbfHorAlignRight,

					 bbfVertAlignTop,
					 bbfVertAlignCenter,
					 bbfVertAlignBottom,

					 bbfAlignCenter,

					 bbfUseBmpID,
					 bbfUseBgID,
					 bbfUseBackColor,
					 bbfUseForeColor,
					 bbfUseMarginBorder,
					 bbfUseBmpTextMargin,
					 
					 bbfUseColors,
					 bbfUseMargins,
					 bbfUseResources,

					 bbfBgHorAlignLeft,
					 bbfBgHorAlignCenter,
					 bbfBgHorAlignRight,

					 bbfBgVertAlignTop,
					 bbfBgVertAlignCenter,
					 bbfBgVertAlignBottom,

					 bbfBgAlignCenter,
					 bbfBgAlignStretch,
					 bbfBgAlignTiled,

					 bbfTransparentBmp,
					 bbfTransparentBg,
					 bbfTransparentResources,

					 bbfSetBmp;


/*
#define BBF_BITMAP_TEXT				0x00000001
#define BBF_TEXT_BITMAP				0x00000002

#define BFF_HALIGN_LEFT				0x00000004
#define BFF_HALIGN_CENTER			0x00000008
#define BFF_HALIGN_RIGHT			0x00000010

#define BFF_VALIGN_TOP				0x00000020
#define BFF_VALIGN_CENTER			0x00000040
#define BFF_VALIGN_BOTTOM			0x00000080

#define BFF_ALIGN_CENTER			BFF_HALIGN_CENTER | BFF_VALIGN_CENTER

#define BFF_USE_BMPID				0x00000100
#define BFF_USE_BGID				0x00000200
#define BFF_USE_COLOR_BACK			0x00000400
#define BFF_USE_COLOR_FORE			0x00000800
#define BFF_USE_BORDER_MARGIN		0x00001000
#define BFF_USE_BMPTEXT_MARGIN		0x00002000

#define BFF_USE_COLORS				BFF_USE_COLOR_BACK | BFF_USE_COLOR_FORE
#define BFF_USE_MARGINS				BFF_USE_BORDER_MARGIN | BFF_USE_BMPTEXT_MARGIN
#define BFF_USE_RESOURCES			BFF_USE_BMPID | BFF_USE_BGID

#define BFF_BG_HALIGN_LEFT			0x00004000
#define BFF_BG_HALIGN_CENTER		0x00008000
#define BFF_BG_HALIGN_RIGHT			0x00010000

#define BFF_BG_VALIGN_TOP			0x00020000
#define BFF_BG_VALIGN_CENTER		0x00040000
#define BFF_BG_VALIGN_BOTTOM		0x00080000

#define BFF_BG_ALIGN_CENTER			BFF_BG_HALIGN_CENTER | BFF_BG_VALIGN_CENTER
#define BFF_BG_ALIGN_STRETCH		0x00100000
#define BFF_BG_ALIGN_TILED			0x00200000

#define BFF_TRANSPARENT_BMP			0x00400000
#define BFF_TRANSPARENT_BG			0x00800000
#define BFF_TRANSPARENT_RESOURCES	BFF_TRANSPARENT_BMP | BFF_TRANSPARENT_BG

#define BFF_SET_BITMAP				0x01000000
*/

	static HBITMAP ButtonBitmap(CButton *button,
					 UINT nIDSmallBmp,
					 UINT nIDBg,
					 COLORREF crBack,
					 COLORREF cdFore,
					 COLORREF crTansparent,
					 CFont *fnt,
					 int iBorderMargin,
					 int iBmpTextMargin,
					 UINT uFlags);

	static CBitmap* BmpChangeColor(HBITMAP hBitmap, COLORREF crFrom, COLORREF crTo);
	static int LeftWidth(CWnd *pWnd);
	static int TopHeight(CWnd *pWnd);
	static BOOL DrawWndRoundRect(CWnd *pWnd,
								 CDC *pParenrDC,
								 int nRoundX,
								 int nRoundY,
								 COLORREF colrefBkDraw,
								 BOOL bDrawBorder = FALSE,
								 int nBorderSize = 1,
								 COLORREF colrefBorderColor = 0x000000);
	CPWControls();
	virtual ~CPWControls();

	static CPoint GetClientWndPos(CWnd* pWnd);
	static CSize GetWndSize(CWnd* pWnd);
};

#endif // PWCONTROLS_H
