// FontExt.cpp: implementation of the CFontExt class.
//
//////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "FontExt.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFontExt::CFontExt() :
m_pAttachedDC(NULL),
m_pAttachedDC_oldFont(NULL),
m_nCharSet(GREEK_CHARSET)
{

}

CFontExt::~CFontExt()
{
	DetachDC();
}

int CFontExt::GetDesktopLogPixelsY()
{
	int nLogPixelsY;
	HDC hdc = ::GetDC(GetDesktopWindow());
	nLogPixelsY = ::GetDeviceCaps(hdc, LOGPIXELSY);
	::ReleaseDC(GetDesktopWindow(), hdc);

	return(nLogPixelsY);
}

LONG CFontExt::PointSize2FontHeight(int nPointSize)
{
	return(-MulDiv(nPointSize, GetDesktopLogPixelsY(), 72));
}

int CFontExt::FontHeight2PointSize(LONG nFontHeight)
{
	return(-int((nFontHeight * 72) / GetDesktopLogPixelsY()));
}

void CFontExt::ReBuildFont()
{
	ASSERT(m_hObject);

	LOGFONT lf;

	GetLogFont(&lf);

	_tcscpy((LPTSTR)&lf.lfFaceName, m_strFontFace);
	lf.lfHeight = PointSize2FontHeight(m_nFontPointsize);
	lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = m_bFontItalic;
	lf.lfUnderline = m_bFontUnderline;
	lf.lfCharSet = m_nCharSet;
	lf.lfQuality = ANTIALIASED_QUALITY;

	DeleteObject();
	CreateFontIndirect(&lf);
	ReAttachDC();
}

CFontExt &CFontExt::operator=(HFONT hOther)
{
	if(!hOther)
		return(*this);

	LOGFONT lf;

	::GetObject(hOther, sizeof(lf), &lf);

	DeleteObject();
	CreateFontIndirect(&lf);

	return(*this);
}

void CFontExt::ReadVars()
{
	ASSERT(m_hObject);

	LOGFONT lf;

	GetLogFont(&lf);

	m_strFontFace = lf.lfFaceName;
	m_nFontPointsize = FontHeight2PointSize(lf.lfHeight);
	m_bFontBold = lf.lfWeight == FW_BOLD;
	m_bFontItalic = lf.lfItalic;
	m_bFontUnderline = lf.lfUnderline;
	m_nCharSet = lf.lfCharSet;
}

void CFontExt::AttachDC(CDC *pdc)
{
	DetachDC();

	m_pAttachedDC = pdc;
	m_pAttachedDC_oldFont = m_pAttachedDC->SelectObject(this);
}

void CFontExt::DetachDC()
{
	if(!m_pAttachedDC || !m_pAttachedDC_oldFont ||
	   !m_pAttachedDC->m_hDC || !m_pAttachedDC_oldFont->m_hObject)
		return;

	m_pAttachedDC->SelectObject(m_pAttachedDC_oldFont);

	m_pAttachedDC_oldFont = NULL;
	m_pAttachedDC = NULL;
}

void CFontExt::ReAttachDC()
{
	if(!m_pAttachedDC || !m_pAttachedDC_oldFont ||
	   !m_pAttachedDC->m_hDC || !m_pAttachedDC_oldFont->m_hObject)
		return;

	m_pAttachedDC->SelectObject(this);
}

CString CFontExt::GetFontFase() //const
{
	ReadVars();
	return(m_strFontFace);
}

void CFontExt::SetFontFace(LPCTSTR szFontFace) { SetFontFace(CString(szFontFace)); }
void CFontExt::SetFontFace(CString strFontFace)
{
	ReadVars();
	m_strFontFace = strFontFace;
	ReBuildFont();
}

int CFontExt::GetFontPointSize()
{
	ReadVars();
	return(m_nFontPointsize);
}

void CFontExt::SetFontPointSize(int nFontPointSize)
{
	ReadVars();
	m_nFontPointsize = nFontPointSize;
	ReBuildFont();
}

BOOL CFontExt::GetFontBold()
{
	ReadVars();
	return(m_bFontBold);
}

void CFontExt::SetFontBold(BOOL bFontBold)
{
	ReadVars();
	m_bFontBold = bFontBold;
	ReBuildFont();
}

BOOL CFontExt::GetFontItalic()
{
	ReadVars();
	return(m_bFontItalic);
}

void CFontExt::SetFontItalic(BOOL bFontItalic)
{
	ReadVars();
	m_bFontItalic = bFontItalic;
	ReBuildFont();
}

BOOL CFontExt::GetFontUnderline()
{
	ReadVars();
	return(m_bFontUnderline);
}

void CFontExt::SetFontUnderline(BOOL bFontUnderline)
{
	ReadVars();
	m_bFontUnderline = bFontUnderline;
	ReBuildFont();
}

int CFontExt::GetFontHeight()
{
	ASSERT(m_hObject);

	CDC dc;

	dc.CreateCompatibleDC(NULL);

	CFont *oldFont = dc.SelectObject(this);

	CSize fontsize = dc.GetOutputTextExtent(CString(_T("ABCDFGabcdfg")));

	dc.SelectObject(oldFont);
	dc.DeleteDC();

	return(fontsize.cy);
}

int CFontExt::GetTextWidth(LPCTSTR lpszText)
{
	ASSERT(m_hObject);

	CDC dc;

	dc.CreateCompatibleDC(NULL);

	CFont *oldFont = dc.SelectObject(this);

	CSize fontsize = dc.GetOutputTextExtent(CString(lpszText));

	dc.SelectObject(oldFont);
	dc.DeleteDC();

	return(fontsize.cx);
}

CSize CFontExt::GetRealTextExtent(CString strText, CDC* pDC /*= NULL*/)
{
	CDC dc;
	BOOL bDeleteDC = FALSE;
	CFont *oldFont;

	if(pDC)
		dc.Attach(pDC->GetSafeHdc());
	else
	{
		dc.CreateCompatibleDC(NULL);
		oldFont = dc.SelectObject(this);
		bDeleteDC = TRUE;
	}

	int i = 0, j = 0;
	int nTxtWidth = 0,
		nTxtHeight = 0;

	while(true)
	{
		j = strText.Find(_T('\n'), i);

		nTxtHeight += dc.GetOutputTextExtent(CString(_T("AbCdEfGh"))).cy;

		if(j == -1)
		{
			nTxtWidth = __max(nTxtWidth, dc.GetOutputTextExtent(strText.Mid(i)).cx);
			break;
		}
		else
		{
			nTxtWidth = __max(nTxtWidth, dc.GetOutputTextExtent(strText.Mid(i, j - i)).cx);
			i = j + 1;
		}
	};

	if(bDeleteDC)
		dc.DeleteDC();
	else
		dc.Detach();


	return CSize(nTxtWidth, nTxtHeight);
}

int CFontExt::GetFontCharSet(void)
{
	ReadVars();
	return(m_nCharSet);
}

void CFontExt::SetFontCharSet(int nCharSet)
{
	ReadVars();
	m_nCharSet = nCharSet;
	ReBuildFont();
}