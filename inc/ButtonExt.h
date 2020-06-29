#ifndef BUTTONEXT_H
#define BUTTONEXT_H

#include <afxwin.h>
#include "FontExt.h"	// Added by ClassView
#include "PWGraphics.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonExt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CButtonExt window

class CButtonExt : public CButton
{
// Construction
public:
	CButtonExt();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonExt)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	//enum BtnExtState
	//{
	//	stateNormal,
	//	statePressed,
	//	stateHot,
	//	stateDefault,
	//	stateDisable
	//};
	CFontExt* GetFontExt();
	int GetRoundRectY();
	int GetRoundRectX();
	void SetRoundRectXY(int nRndXY);
	void SetRoundRectY(int nRndY);
	void SetRoundRectX(int nRndX);
	void SetFocusColor(COLORREF colrefFocusColor);
	COLORREF GetFocusColor();
	void SetBorderColor(COLORREF colrefBorderColor);
	COLORREF GetBorderColor();
	void SetTextColor(COLORREF colrefTextColor);
	COLORREF GetTextColor();
	void SetBkColor(COLORREF colrefBkColor);
	COLORREF GetBkColor();
	BOOL GetRoundRect();
	void SetRoundRect(BOOL bRoundRect, int nRndX = -1, int nRndY = -1);
	virtual ~CButtonExt();

	// Generated message map functions
protected:
	int m_nRoundY;
	int m_nRoundX;
	CPWGraphics::BtnState m_enmState;
	COLORREF m_colrefFocusColor;
	BOOL m_bRoundRect;
	COLORREF m_colrefBorderColor;
	COLORREF m_colrefTextColor;
	COLORREF m_colrefBkColor;
	COLORREF m_colrefBtnImgTransparentColor;
	CFontExt m_objFont;
	//{{AFX_MSG(CButtonExt)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CRgn m_objWndDrawRegion;
	CRgn m_objWndRegion;
	CRgn m_objWndFocusRegion;
	void InitCtrl();
	BOOL m_bInit;
	BOOL m_bIsHOver;
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
private:
	void DoTME(DWORD dwFlags = TME_HOVER | TME_LEAVE);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void DrawPlastic(HDC hDC);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
private:
	CBitmap *m_pBmpBtnImg;
public:
	void SetBtnImg(UINT nResImgID, COLORREF colrefTransparentColor);
protected:
	virtual void PreSubclassWindow();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // BUTTONEXT_H
