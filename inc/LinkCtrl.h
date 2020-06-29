#ifndef LINKCTRL_H
#define LINKCTRL_H

#pragma once

#include "afxcmn.h"
#include "PWGraphics.h"
#include "ToolTipEx.h"

typedef void (__cdecl* PFASHYPERLINKFN)( CWnd* , LPARAM );	// Callback function

class CLnkCtrl : public CStatic
{
// Construction
public:
	CLnkCtrl();

// Attributes
public:
	/*struct iLC_Link
	{
		CString strText;
		CString strOutWord;
		LPARAM lParam;
		UINT nResImgID;

		iLC_Link::iLC_Link() :
		strText(_T("")),
		strOutWord(_T("")),
		lParam(NULL),
		nResImgID(0) {}
	};*/

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLnkCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetOutWordImgDist(int nOutWordImgDist);
	int GetOutWordImgDist();
	void SetHOverFontUnderline(BOOL bHOverFontUnderline = TRUE);
	BOOL GetHOverFontUnderline();
	void SetUseLinkImgTransparentColor(BOOL bUseLinkImgTransparentColor = TRUE);
	BOOL GetUseLinkImgTransparentColor();
	BOOL GetForceSingleLineWidth();
	void SetForceSingleLineWidth(BOOL bForceSingleLineWidth = TRUE);
	void DeleteLinkImg();
	void SetResLinkImg(UINT nBmpID, COLORREF crTransparentColor = -1);
	CBitmap* GetLinkImg();
	void SetFullAlign(BOOL bSetFullAlign = TRUE);
	BOOL GetFullAlign();
	void ResetHeight();
	BOOL GetCheckHOverRng();
	void SetCheckHOverRng(BOOL bCheckHOverRng = TRUE);
	BOOL GetBreakWords();
	void SetBreakWords(BOOL bBreakWords);
	int GetLines();
	CSize GetControlSize();
	BOOL GetFontUnderline() const;
	void SetFontUnderline(BOOL bFontUnderline = TRUE);
	BOOL GetFontBold() const;
	void SetFontBold(BOOL bFontBold = TRUE);
	int GetFontPointSize() const;
	void SetFontPointSize(int nPointSize);
	CString GetFontFace() const;
	void SetFontFace(LPCTSTR lpszFontFace);
	//PFASHYPERLINKFN GetCallbackFunction();
	void SetCallbackFunction(PFASHYPERLINKFN pFn);
	LPARAM GetLParam() const;
	void SetLParam(LPARAM lParam);
	COLORREF GetHOverBackColor() const;
	void SetHOverBackColor(COLORREF crHOverBackColor);
	COLORREF GetHOverTextColor() const;
	void SetHOverTextColor(COLORREF crHOverTextColor);
	COLORREF GetBackColor() const;
	void SetBackColor(COLORREF crBackColor);
	COLORREF GetTextColor() const;
	void SetTextColor(COLORREF crTextColor);
	CString GetOutWord() const;
	void SetOutWord(LPCTSTR lpszOutWord);
	BOOL GetAlignLastLine() const;
	void SetAlignLastLine(BOOL bAlignLastLine = TRUE);
	int GetHeight();
	void SetWidth(int nWidth);
	int GetWidth();
	static const int SBCNoBorder;
	static const int SBSLeft;
	static const int SBSRight;
	static const int SBSTop;
	static const int SBSBottom;
	static const int SBSAll;

	virtual ~CLnkCtrl();

	// Generated message map functions
protected:
	BOOL m_bShowHand;
	int m_nInBorderSize;
	COLORREF m_crBorderColor;
	int m_iBorderSides;
	bool m_bHasKeybordFocus;
	int m_nOutWordImgDist;
	BOOL m_bHOverFontUnderline;
	BOOL m_bUseLinkImgTransparentColor;
	COLORREF m_crLinkImgTransparentColor;
	BOOL m_bForceSingleLineWidth;
	BOOL m_bFullAlign;
	BOOL m_bCheckHOverRgn;
	BOOL m_bBreakWords;
	BOOL m_bFontUnderline;
	BOOL m_bFontBold;
	int m_nFontPointSize;
	CString m_strFontFace;
	CBitmap *m_pBmpLinkImg;
	BOOL m_bIsMouseIn;
	LPARAM m_lParam;
	PFASHYPERLINKFN m_pFn;
	COLORREF m_crHOverBackColor;
	COLORREF m_crHOverTextColor;
	COLORREF m_crBackColor;
	CString m_strOutWord;
	BOOL m_bAlignLastLine;
	CFont m_Font;
	CFont m_FontHOver;
	int m_nLineHeight;
	int m_nLines;
	int m_nHeight;
	int m_nWidth;
	COLORREF m_crTextColor;
	//{{AFX_MSG(CLnkCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnToolTipsNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CString m_strToolTip;
	CToolTipEx m_tpLnkToolTip;
	int m_nLastLineWidth;
	int m_nAddY;
	CRgn m_rgnHOverCheck;
	BOOL m_bCheckHOverRgnIn;
	BOOL m_bTimerSet;
	void Highlight();
	int m_nLogPixelsY;
	void BuildFont(BOOL bInit = FALSE);
	BOOL m_bDrawBk;
	HCURSOR m_hHand;
	HCURSOR m_hArrow;
	HCURSOR m_hSelectedCursor;
	int GetHeightOrPaint(CDC *pDC, BOOL bPaint = FALSE);
	void DrawFocus(bool bAfterPaint = false);
	void DrawBorder(CDC* pDC);
public:
	void SetShowHandCursor(BOOL m_bSet);
	BOOL GetShowHandCursor();
	void SetInBorderSize(int nInBorderSize);
	int GetInBorderSize(void);
	void SetBorderColor(COLORREF crBorderColor);
	void SetBorderSides(bool left, bool right, bool top, bool bottom);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // LINKCTRL_H
