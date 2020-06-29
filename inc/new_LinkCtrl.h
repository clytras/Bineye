#ifndef LINKCTRL_H
#define LINKCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkCtrl.h : header file
//

typedef void (__cdecl* PFASHYPERLINKFN)( CWnd* , LPARAM );	// Callback function

/////////////////////////////////////////////////////////////////////////////
// CLnkCtrl window

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
	void SetHOverResLinkImg(UINT nBmpID, COLORREF crTransparentColor = -1);
	CBitmap* GetHOverLinkImg();

	void SetSelectedResLinkImg(UINT nBmpID, COLORREF crTransparentColor = -1);
	CBitmap* GetSelectedLinkImg();

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

	COLORREF GetSelectedTextColor() const;
	void SetSelectedTextColor(COLORREF crSelectedTextColor);
	COLORREF GetSelectedBackColor() const;
	void SetSelectedBackColor(COLORREF crSelectedBackColor);

	CString GetOutWord() const;
	void SetOutWord(LPCTSTR lpszOutWord);
	BOOL GetAlignLastLine() const;
	void SetAlignLastLine(BOOL bAlignLastLine = TRUE);
	int GetHeight();
	void SetWidth(int nWidth);
	int GetWidth(BOOL bRecalc = FALSE);
	virtual ~CLnkCtrl();

	// Generated message map functions
protected:
	int m_iID;
	int m_nOutWordImgDist;
	BOOL m_bHOverFontUnderline;
	BOOL m_bUseLinkImgTransparentColor;
	COLORREF m_crLinkImgTransparentColor;
	COLORREF m_crHOverLinkImgTransparentColor;
	COLORREF m_crSelectedLinkImgTransparentColor;
	BOOL m_bForceSingleLineWidth;
	BOOL m_bFullAlign;
	BOOL m_bSelected;
	BOOL m_bDisable;
	BOOL m_bCheckHOverRgn;
	BOOL m_bBreakWords;
	BOOL m_bFontUnderline;
	BOOL m_bFontBold;
	BOOL m_bDrawShadow;
	int m_nFontPointSize;
	CString m_strFontFace;
	CBitmap *m_pBmpLinkImg;
	CBitmap *m_pHOverBmpLinkImg;
	CBitmap *m_pSelectedBmpLinkImg;
	BOOL m_bIsMouseIn;
	LPARAM m_lParam;
	PFASHYPERLINKFN m_pFn;
	COLORREF m_crHOverBackColor;
	COLORREF m_crHOverTextColor;
	COLORREF m_crBackColor;
	COLORREF m_crSelectedTextColor;
	COLORREF m_crSelectedBackColor;
	COLORREF m_crShadowColor;
	CString m_strOutWord;
	CString m_strTag;
	BOOL m_bAlignLastLine;
	CFont m_Font;
	CFont m_FontHOver;
	int m_nLineHeight;
	int m_nLines;
	int m_nHeight;
	int m_nWidth;
	COLORREF m_crTextColor;
	HBITMAP m_hBmpBkTransparent;
	HDC m_hBmpBkTransDC;
	//{{AFX_MSG(CLnkCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_nLastLineWidth;
	CRgn m_rgnHOverCheck;
	BOOL m_bCheckHOverRgnIn;
	BOOL m_bTimerSet;
	int m_nLogPixelsY;
	void BuildFont(BOOL bInit = FALSE);
	BOOL m_bDrawBk;
	HCURSOR m_hHand;
	HCURSOR m_hArrow;
	HCURSOR m_hSelectedCursor;
	int GetHeightOrPaint(CDC *pDC, BOOL bPaint = FALSE);
public:
	void Highlight();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
private:
	CBitmap* m_pBmpParentBg;
public:
	void SetParentBgBmp(CBitmap* pBmpParentBg);
	void SetDisable(BOOL bDisable = TRUE);
	BOOL GetDisable(void) const;
	void SetSelected(BOOL bSelected = TRUE);
	BOOL GetSelected(void) const;
private:
	bool m_bOnClickCheckMouse;
public:
	void SetDrawShadow(bool bDrawShadow);
	BOOL GetDrawShadow(void);
	void SetShadowColor(COLORREF crShadowColor);
	COLORREF GetShadowColor(void);
	void SetTag(LPCTSTR lpszTag);
	LPCTSTR GetTag(void);
	void SetID(int iID);
	int GetID(void);
protected:
	int m_iParentID;

public:
	int GetParentID(void);
	void SetParentID(int iParentID);
	CString GetText(void);
	void SetFileIcon(LPCTSTR lpszFile);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // LINKCTRL_H
