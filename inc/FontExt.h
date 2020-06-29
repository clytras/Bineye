// FontExt.h: interface for the CFontExt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FONTEXT_H
#define FONTEXT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxwin.h"

class CFontExt : public CFont  
{
public:
	int GetFontHeight();
	void SetFontUnderline(BOOL bFontUnderline = TRUE);
	BOOL GetFontUnderline();
	void SetFontItalic(BOOL bFontItalic = TRUE);
	BOOL GetFontItalic();
	void SetFontBold(BOOL bFontBold = TRUE);
	BOOL GetFontBold();
	void SetFontPointSize(int nFontPointSize);
	int GetFontPointSize();
	void SetFontFace(CString strFontFace);
	void SetFontFace(LPCTSTR szFontFace);
	CString GetFontFase() /*const*/;
	void ReAttachDC();
	void DetachDC();
	void AttachDC(CDC *pdc);
	CFontExt &operator=(HFONT hOther);
	static int FontHeight2PointSize(LONG nFontHeight);
	static LONG PointSize2FontHeight(int nPointSize);
	static int GetDesktopLogPixelsY();
	CFontExt();
	virtual ~CFontExt();

private:
	void ReadVars();
	void ReBuildFont();
protected:
	CFont* m_pAttachedDC_oldFont;
	CDC* m_pAttachedDC;
	BOOL m_bFontUnderline;
	BOOL m_bFontItalic;
	BOOL m_bFontBold;
	int m_nFontPointsize;
	int m_nCharSet;
	CString m_strFontFace;
public:
	int GetTextWidth(LPCTSTR lpszText);
	CSize GetRealTextExtent(CString strText, CDC* pDC = NULL);
	int GetFontCharSet(void);
	void SetFontCharSet(int nCharSet);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // FONTEXT_H
