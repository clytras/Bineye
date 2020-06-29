// FullAlignStatic.cpp : implementation file
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxtempl.h>		// Let manage CArray

#include ".\LinkCtrl.h"

//For those people who do not have the absolutely latest SDK header files
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)   // Taken from VC6 WINUSER.H
#endif // IDC_HAND


#define FAT_TEXTCOLOR		0xFF0000
#define FAT_BACKCOLOR		-1
#define FAT_HOVERTEXTCOLOR	0x0000FF
#define FAT_HOVERBACKCOLOR	-1

#define FAT_TIMEREVENT		101

/////////////////////////////////////////////////////////////////////////////
// CLnkCtrl

const int CLnkCtrl::SBCNoBorder = -1;

const int CLnkCtrl::SBSLeft   = 1;
const int CLnkCtrl::SBSRight  = 2;
const int CLnkCtrl::SBSTop    = 4;
const int CLnkCtrl::SBSBottom = 8;
const int CLnkCtrl::SBSAll    = SBSLeft | SBSRight |SBSTop | SBSBottom;


CLnkCtrl::CLnkCtrl() :
m_nWidth(400),
m_nHeight(-1),
m_nLines(0),
m_nAddY(0),
m_nLastLineWidth(0),
m_nOutWordImgDist(-1),
m_strOutWord(_T("")),
m_bAlignLastLine(false),
m_bDrawBk(true),
m_pFn(NULL),
m_lParam(NULL),
m_pBmpLinkImg(NULL),
m_bIsMouseIn(false),
m_bTimerSet(false),
m_bBreakWords(true),
m_bCheckHOverRgn(true),
m_bCheckHOverRgnIn(false),
m_bFullAlign(false),
m_bUseLinkImgTransparentColor(true),
m_bForceSingleLineWidth(true),
m_bHOverFontUnderline(false),
m_crTextColor(FAT_TEXTCOLOR),
m_crBackColor(FAT_BACKCOLOR),
m_crHOverTextColor(FAT_HOVERTEXTCOLOR),
m_crHOverBackColor(FAT_HOVERBACKCOLOR),
m_crLinkImgTransparentColor(-1),
m_nInBorderSize(0),
m_crBorderColor(SBCNoBorder),
m_iBorderSides(SBSLeft | SBSTop | SBSBottom),
m_bHasKeybordFocus(false),
m_bShowHand(true)
{
	CDC *pDC = GetDesktopWindow()->GetDC();
	m_nLogPixelsY = ::GetDeviceCaps(pDC->GetSafeHdc(), LOGPIXELSY);
	GetDesktopWindow()->ReleaseDC(pDC);

	BuildFont(TRUE);

	m_hHand = AfxGetApp()->LoadStandardCursor(IDC_HAND);
	m_hSelectedCursor = m_hArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

CLnkCtrl::~CLnkCtrl()
{
	m_Font.DeleteObject();
	m_rgnHOverCheck.DeleteObject();
	if(m_pBmpLinkImg)
	{
		m_pBmpLinkImg->DeleteObject();
		delete m_pBmpLinkImg;
	}
}

BEGIN_MESSAGE_MAP(CLnkCtrl, CStatic)
	//{{AFX_MSG_MAP(CLnkCtrl)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipsNotify)
	//}}AFX_MSG_MAP
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLnkCtrl message handlers

void CLnkCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rc;


	DWORD dwStyle = GetStyle();
	if (!(dwStyle & SS_NOTIFY)) 
	{
		// Turn on notify flag to get mouse messages and STN_CLICKED.
		// Otherwise, I'll never get any mouse clicks!
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
	}

	if(!m_bTimerSet)
	{
		SetTimer(FAT_TIMEREVENT, 10, NULL);
		m_bTimerSet = true;

		//m_strToolTip = "Test ToolTip3...";

		//m_tpLnkToolTip.Create(this->GetParent());

		//m_tpLnkToolTip.AddTool(this, m_strToolTip, CRect(0,0, 100, 20), 101);

		//m_tpLnkToolTip.Activate(TRUE);

		//m_tpLnkToolTip.Create(GetParent(), FALSE, 500, 2000);
		//m_tpLnkToolTip.Create(

		//m_tpLnkToolTip.AddTool(this, m_strToolTip);
		

		//this->EnableToolTips();

	}

	if(m_crBackColor == -1)
	{
		//CDC *pParentDC = GetParent()->GetDC();
		//m_crBackColor = pParentDC->GetBkColor();
		//GetParent()->ReleaseDC(pParentDC);
		m_crBackColor = GetSysColor(COLOR_BTNFACE);
	}

	if(m_crHOverBackColor == -1)
	{
		//CDC *pParentDC = GetParent()->GetDC();
		//m_crHOverBackColor = pParentDC->GetBkColor();
		//GetParent()->ReleaseDC(pParentDC);
		m_crHOverBackColor = GetSysColor(COLOR_BTNFACE);
	}

	if(m_nHeight == -1) m_nHeight = GetHeightOrPaint(&dc);

	GetClientRect(rc);

	if(rc.Width() != m_nWidth || rc.Height() != m_nHeight)
	{
		SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
		GetClientRect(rc);
	}

	dc.SetTextColor(m_bIsMouseIn ? m_crHOverTextColor : m_crTextColor);
	dc.SetBkColor(m_bIsMouseIn ? m_crHOverBackColor : m_crBackColor);

	if(m_bDrawBk)
	{
		dc.FillSolidRect(rc, dc.GetBkColor());
		if(m_crBorderColor != SBCNoBorder /*&& m_bIsMouseIn*/)
		{
			DrawBorder(&dc);
		}
	}
	else
		m_bDrawBk = TRUE;

	if(GetFocus() && GetFocus()->m_hWnd == m_hWnd && m_bDrawBk)
	{
		CRect rcFocus(rc);
		//rcFocus.DeflateRect(1, 3);

		rcFocus.SetRect(rc.left, rc.top, rc.right+5, rc.bottom+5);

		//dc.DrawFocusRect(rcFocus);
	}

	GetHeightOrPaint(&dc, TRUE);

	//if(m_bDrawBk) DrawFocus(true);
}

CSize CLnkCtrl::GetControlSize() { return(CSize(GetWidth(), GetHeight())); }
int CLnkCtrl::GetWidth() { return(m_nWidth); }
void CLnkCtrl::SetWidth(int nWidth) { m_nWidth = nWidth + m_nInBorderSize * 2; m_nHeight = -1; }
int CLnkCtrl::GetHeight()
{
	if(m_nHeight == -1)
	{
		CDC *pDC = GetDC();
		m_nHeight = GetHeightOrPaint(pDC);
		ReleaseDC(pDC);
		DrawFocus(true);
	}

	return(m_nHeight);
}

int CLnkCtrl::GetHeightOrPaint(CDC *pDC, BOOL bPaint /*= FALSE*/)
{
	BITMAP bmp;
	CFont *pFont;
	CPoint txtPos(m_nInBorderSize, m_nInBorderSize);
	CArray<CPoint, CPoint> ptRgnPoints;
	CString strText(_T("")),
			strLine(_T("")),
			strWord(_T("")),
			strSpc(_T("")),
			strTmp(_T(""));
	bool bIsWordDone = false,
		 bIsTextDone = false;
	//static int nAddY = 0;
	int i, j,
		nCharBegin,
		nCharLen,
		nSpcAdd = 0,
		nSpcCount = 0,
		nSpcWidth = 0,
		nSpcPos = 0,
		nCharHeight = 0,
		nTmpHeight = 0,
		nTmpWidth = m_nInBorderSize * 2,
		nWordWidth,
		nCalc,
		nSpaceWidth,
		nMinusWidth,
		nOutWordWidth = 0,
		nProcWidth = m_nWidth;

	GetWindowText(strText);

	if(m_bIsMouseIn && (m_bFontUnderline != m_bHOverFontUnderline))
		pFont = (CFont*)pDC->SelectObject(&m_FontHOver);
	else
		pFont = (CFont*)pDC->SelectObject(&m_Font);

	//if(strText.IsEmpty() && bPaint)
	//	return(0);
	//else if(strText.IsEmpty())
	//	strText = _T("A");

	m_nLineHeight = nCharHeight = pDC->GetOutputTextExtent(CString(_T("ABCDEFGhijklmnop"))).cy;
	nSpaceWidth = pDC->GetOutputTextExtent(_T(" ")).cx;
	nMinusWidth = pDC->GetOutputTextExtent(_T("-")).cx;

	if(!bPaint)
		m_nAddY = m_nInBorderSize;
	else
		txtPos.y += m_nAddY;

	if(m_bCheckHOverRgnIn && !m_bFullAlign)
		ptRgnPoints.Add(CPoint(txtPos.x, txtPos.y));

	if(m_pBmpLinkImg)
	{
		m_pBmpLinkImg->GetObject(sizeof(bmp), &bmp);

		txtPos.x = nOutWordWidth = bmp.bmWidth + (m_nOutWordImgDist != -1 ? m_nOutWordImgDist : nSpaceWidth) + m_nInBorderSize;
		if(bPaint)
		{
			CDC bmpDC;
			bmpDC.CreateCompatibleDC(pDC);
			CBitmap *oldBmp = bmpDC.SelectObject(m_pBmpLinkImg);

			if(m_bUseLinkImgTransparentColor)
				TransparentBlt(pDC->m_hDC, m_nInBorderSize, m_nInBorderSize, bmp.bmWidth, bmp.bmHeight,
							   bmpDC.m_hDC, 0, 0, bmp.bmWidth, bmp.bmHeight, m_crLinkImgTransparentColor);
			else
				pDC->BitBlt(m_nInBorderSize, m_nInBorderSize, bmp.bmWidth, bmp.bmHeight, &bmpDC, 0, 0, SRCCOPY);

			bmpDC.SelectObject(oldBmp);
			bmpDC.DeleteDC();
		}
	}
	else if(!m_strOutWord.IsEmpty())
	{
		txtPos.x = nOutWordWidth = pDC->GetOutputTextExtent(m_strOutWord).cx + (m_nOutWordImgDist != -1 ? m_nOutWordImgDist : nSpaceWidth) + m_nInBorderSize;

		if(bPaint)
		{
			pDC->DrawText(m_strOutWord, CRect(txtPos.x, txtPos.y, nOutWordWidth, m_nLineHeight), DT_LEFT);

			nCalc = txtPos.x - nOutWordWidth;
			strSpc = _T(' ');
			while(pDC->GetOutputTextExtent(strSpc).cx < nCalc) strSpc += _T(' ');
			strSpc += _T(' ');
			pDC->DrawText(strSpc, CRect(nOutWordWidth - nSpaceWidth, 0, nOutWordWidth, m_nLineHeight), DT_LEFT);
			strTmp.Empty();
		}
	}

	nProcWidth = m_nWidth - nOutWordWidth;
	m_nLines = 0;
	m_nLastLineWidth = nProcWidth;

	for(i = 0; i < strText.GetLength(); i++)
	{
		bIsTextDone = i + 1 == strText.GetLength();
		if(bIsTextDone)
		{
			strWord += strText.GetAt(i);
			nSpcCount++;
			if(strLine.GetLength() == 0)
			{
				strLine = strWord;
				strWord.Empty();
			}
		}

		if(strText.GetAt(i) == _T(' ') || bIsTextDone)
		{
			nTmpWidth = pDC->GetOutputTextExtent(strLine).cx + 
						pDC->GetOutputTextExtent(strWord).cx;

			if(((nTmpWidth > nProcWidth) ||
			    (bIsTextDone && m_bAlignLastLine)) &&
			   !(bIsTextDone && m_nLines <= 1 && nTmpWidth <= nProcWidth))
			{
				if(bIsTextDone && m_bAlignLastLine)
				{
					if(nTmpWidth < nProcWidth)
					{
						strLine += strWord;
						strWord.Empty();
					}
				}

				if(nTmpWidth > nProcWidth && m_bBreakWords && m_bFullAlign)
				{
					nCalc = pDC->GetOutputTextExtent(strLine).cx;

					if(((nProcWidth - nCalc) / (nSpcCount - 1) >= nSpaceWidth) &&
					   strWord.GetLength() > 5)
					{
						j = 1;

						while(pDC->GetOutputTextExtent(strWord.Left(j)).cx +
							  nMinusWidth <= nProcWidth - nCalc) j++;

						if(j > 2)
						{
							if((strWord.GetLength() - j) <= 1) j--;

							j--;
							strLine += strWord.Left(j);
							strLine += _T("-");
							strWord = strWord.Right(strWord.GetLength() - j);
						}
					}
				}

				if(strLine.GetAt(strLine.GetLength() - 1) == _T(' '))
					strLine = strLine.Left(strLine.GetLength() - 1);

				if(bPaint)
				{
					txtPos.x = nOutWordWidth;

					if(m_bFullAlign)
					{
						nSpcCount--;

						nCalc = (nProcWidth - (pDC->GetOutputTextExtent(strLine).cx - nSpaceWidth * nSpcCount));
						nSpcWidth = nCalc / nSpcCount;
						nSpcAdd += int(nCalc % nSpcCount) - 1;

						strTmp.Empty();
						nCharBegin = nCharLen = 0;

						for(j = 0; j < strLine.GetLength(); j++)
						{
							bIsWordDone = j + 1 == strLine.GetLength();

							if(strLine.GetAt(j) == _T(' ') || bIsWordDone)
							{
								if(bIsWordDone)	nCharLen++;
								if(nCharLen > 0)
								{
									strTmp = strLine.Mid(nCharBegin, nCharLen);
									nWordWidth = pDC->GetOutputTextExtent(strTmp).cx;
									nCalc = nSpcPos = txtPos.x + nWordWidth;
									if(bIsWordDone)	txtPos.x = m_nWidth - nWordWidth;
									pDC->DrawText(strTmp, CRect(txtPos.x, txtPos.y, txtPos.x + nWordWidth, txtPos.y + m_nLineHeight), DT_LEFT);
									txtPos.x += nWordWidth + nSpcWidth;
									if(nSpcAdd > 0)
									{
										txtPos.x++;
										nSpcAdd--;
									}

									if(!bIsWordDone)
									{
										nCalc = txtPos.x - nCalc;
										strSpc = _T(' ');
										while(pDC->GetOutputTextExtent(strSpc).cx < nCalc) strSpc += _T(' ');
										strSpc += _T(' ');
										pDC->DrawText(strSpc, CRect(nSpcPos, txtPos.y, nSpcPos + pDC->GetOutputTextExtent(strSpc).cx, txtPos.y + m_nLineHeight), DT_LEFT);
										strTmp.Empty();
									}
								}
								nCharBegin = j + 1;
								nCharLen = 0;
							}
							else
								nCharLen++;
						}
					}
					else
						pDC->DrawText(strLine, CRect(txtPos.x, txtPos.y, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);

					txtPos.x = nOutWordWidth;
					txtPos.y += nCharHeight;
				}

				if(m_bCheckHOverRgnIn && !m_bFullAlign)
				{
					nCalc = pDC->GetOutputTextExtent(strLine).cx + nOutWordWidth;
					ptRgnPoints.Add(CPoint(nCalc, txtPos.y - nCharHeight));
					ptRgnPoints.Add(CPoint(nCalc, txtPos.y));
				}

				m_nLines++;
				nTmpHeight += nCharHeight;

				if(bIsTextDone)
				{
					if(!strWord.IsEmpty())
					{
						if(bPaint)
							pDC->DrawText(strWord, CRect(txtPos.x, txtPos.y, txtPos.x + pDC->GetOutputTextExtent(strWord).cx, txtPos.y + m_nLineHeight), DT_LEFT);

						if(m_bCheckHOverRgnIn && !m_bFullAlign)
						{
							nCalc = pDC->GetOutputTextExtent(strWord).cx + nOutWordWidth;
							ptRgnPoints.Add(CPoint(nCalc, txtPos.y));
							ptRgnPoints.Add(CPoint(nCalc, txtPos.y + nCharHeight));
						}

						m_nLines++;
						nTmpHeight += nCharHeight;
						m_nLastLineWidth = nOutWordWidth + pDC->GetOutputTextExtent(strWord).cx;
					}
				}
				else
				{
					nSpcCount = 1;
					strWord += strText.GetAt(i);
					strLine = strWord;
				}
			}
			else if(bIsTextDone)
			{
				if(bPaint)
				{
					strLine += strWord;
					pDC->DrawText(strLine, CRect(txtPos.x, txtPos.y, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
				}

				if(m_bCheckHOverRgnIn && !m_bFullAlign)
				{
					nCalc = pDC->GetOutputTextExtent(strLine).cx + nOutWordWidth;
					ptRgnPoints.Add(CPoint(nCalc, txtPos.y));
					ptRgnPoints.Add(CPoint(nCalc, txtPos.y + nCharHeight));
				}

				m_nLines++;
				nTmpHeight += nCharHeight;
				m_nLastLineWidth = nOutWordWidth + pDC->GetOutputTextExtent(strLine).cx;
			}
			else
			{
				nSpcCount++;
				strWord += strText.GetAt(i);
				strLine += strWord;
			}

			strWord.Empty();
		}
		else if(strText.GetAt(i) == _T('\n'))
		{
			if(bPaint)
			{
				strLine += strWord;
				pDC->DrawText(strLine, CRect(txtPos.x, txtPos.y, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
				txtPos.y += nCharHeight;
				txtPos.x = nOutWordWidth + m_nInBorderSize;
			}

			if(m_bCheckHOverRgnIn && !m_bFullAlign)
			{
				nCalc = pDC->GetOutputTextExtent(strLine).cx + nOutWordWidth;
				ptRgnPoints.Add(CPoint(nCalc, txtPos.y - nCharHeight));
				ptRgnPoints.Add(CPoint(nCalc, txtPos.y));
			}

			m_nLines++;
			nTmpHeight += nCharHeight;

			nSpcCount = 0;
			strLine.Empty();
			strWord.Empty();
		}
		else if(strText.GetAt(i) != _T('\r'))
			strWord += strText.GetAt(i);
	}

	if(m_nLines == 1 && !m_bForceSingleLineWidth)
		m_nWidth = pDC->GetOutputTextExtent(strText).cx + nOutWordWidth + m_nInBorderSize;

	if(m_pBmpLinkImg && nTmpHeight < bmp.bmHeight)
	{
		if(!bPaint) m_nAddY = (bmp.bmHeight - nTmpHeight) / 2;
		nTmpHeight = bmp.bmHeight;
	}

	nTmpHeight += m_nInBorderSize * 2;

	m_bCheckHOverRgnIn = m_bCheckHOverRgn && m_nLastLineWidth != m_nWidth && m_nLines > 1;

	if(m_bCheckHOverRgnIn)
	{
		m_rgnHOverCheck.DeleteObject();

		if(m_bFullAlign)
		{
			ptRgnPoints.Add(CPoint(m_nInBorderSize, m_nInBorderSize));
			ptRgnPoints.Add(CPoint(m_nWidth, m_nInBorderSize));
			ptRgnPoints.Add(CPoint(m_nWidth, nTmpHeight - m_nLineHeight));
			ptRgnPoints.Add(CPoint(m_nLastLineWidth, nTmpHeight - m_nLineHeight));
			ptRgnPoints.Add(CPoint(m_nLastLineWidth, nTmpHeight));
			ptRgnPoints.Add(CPoint(nOutWordWidth, nTmpHeight));
		}
		else
			ptRgnPoints.Add(CPoint(nOutWordWidth, nTmpHeight));

		if(nOutWordWidth > 0)
		{
			ptRgnPoints.Add(CPoint(nOutWordWidth, m_pBmpLinkImg ? bmp.bmHeight : m_nLineHeight));
			ptRgnPoints.Add(CPoint(m_nInBorderSize, m_pBmpLinkImg ? bmp.bmHeight : m_nLineHeight));
		}

		m_rgnHOverCheck.CreatePolygonRgn((LPPOINT)ptRgnPoints.GetData(), ptRgnPoints.GetSize(), ALTERNATE);
	}

	pDC->SelectObject(pFont);

	return(nTmpHeight);
}

void CLnkCtrl::SetAlignLastLine(BOOL bAlignLastLine) { m_bAlignLastLine = bAlignLastLine; }
BOOL CLnkCtrl::GetAlignLastLine() const { return(m_bAlignLastLine); }

BOOL CLnkCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_pFn && m_bIsMouseIn)
	{
		::SetCursor(m_hSelectedCursor);
		return(true);
	}
	else
		return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

CString CLnkCtrl::GetOutWord() const { return(m_strOutWord); }
void CLnkCtrl::SetOutWord(LPCTSTR lpszOutWord)
{
	m_strOutWord = lpszOutWord;
	m_nHeight = -1;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

COLORREF CLnkCtrl::GetTextColor() const { return(m_crTextColor); }
void CLnkCtrl::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor = crTextColor;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

COLORREF CLnkCtrl::GetBackColor() const { return(m_crBackColor); }
void CLnkCtrl::SetBackColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

COLORREF CLnkCtrl::GetHOverTextColor() const { return(m_crHOverTextColor); }
void CLnkCtrl::SetHOverTextColor(COLORREF crHOverTextColor)
{
	m_crHOverTextColor = crHOverTextColor;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

COLORREF CLnkCtrl::GetHOverBackColor() const { return(m_crHOverBackColor); }
void CLnkCtrl::SetHOverBackColor(COLORREF crHOverBackColor)
{
	m_crHOverBackColor = crHOverBackColor;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

void CLnkCtrl::SetLParam(LPARAM lParam) { m_lParam = lParam; }
LPARAM CLnkCtrl::GetLParam() const { return(m_lParam); }

void CLnkCtrl::SetCallbackFunction(PFASHYPERLINKFN pFn) { m_pFn = pFn; }
//PFASHYPERLINKFN CLnkCtrl::GetCallbackFunction() const { return(m_pFn); }

CString CLnkCtrl::GetFontFace() const { return(m_strFontFace); }
void CLnkCtrl::SetFontFace(LPCTSTR lpszFontFace)
{
	m_strFontFace = lpszFontFace;
	BuildFont();
	if(IsWindow(m_hWnd)) UpdateWindow();
}

int CLnkCtrl::GetFontPointSize() const { return(m_nFontPointSize); }
void CLnkCtrl::SetFontPointSize(int nPointSize)
{
	m_nFontPointSize = nPointSize;
	BuildFont();
	if(IsWindow(m_hWnd)) UpdateWindow();
}

BOOL CLnkCtrl::GetFontBold() const { return(m_bFontBold); }
void CLnkCtrl::SetFontBold(BOOL bFontBold)
{
	m_bFontBold = bFontBold;
	BuildFont();
	if(IsWindow(m_hWnd)) UpdateWindow();
}

BOOL CLnkCtrl::GetFontUnderline() const { return(m_bFontUnderline); }
void CLnkCtrl::SetFontUnderline(BOOL bFontUnderline)
{
	m_bFontUnderline = bFontUnderline;
	BuildFont();
	if(IsWindow(m_hWnd)) UpdateWindow();
}

void CLnkCtrl::BuildFont(BOOL bInit /*= FALSE*/)
{
	LOGFONT lf;

	m_nHeight = -1;

	if(bInit)
	{
		m_Font.DeleteObject();
		m_Font.CreateStockObject(DEFAULT_GUI_FONT);
		m_Font.GetLogFont(&lf);
		m_strFontFace = (LPCTSTR)&lf.lfFaceName;
		m_nFontPointSize = -int((lf.lfHeight * 72) / m_nLogPixelsY);
		m_bFontBold = lf.lfWeight == FW_BOLD;
		m_bFontUnderline = lf.lfUnderline;
	}
	else
	{
		m_Font.GetLogFont(&lf);
		m_Font.DeleteObject();
		_tcscpy((LPTSTR)&lf.lfFaceName, m_strFontFace);
		lf.lfHeight = -MulDiv(m_nFontPointSize, m_nLogPixelsY, 72);
		lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
		lf.lfUnderline = m_bFontUnderline;
		m_Font.CreateFontIndirect(&lf);
	}

	if(m_bFontUnderline != m_bHOverFontUnderline)
		lf.lfUnderline = m_bHOverFontUnderline;

	m_FontHOver.DeleteObject();
	m_FontHOver.CreateFontIndirect(&lf);
}

void CLnkCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_pFn && m_bIsMouseIn) m_pFn(this, m_lParam);
	CStatic::OnLButtonUp(nFlags, point);
	SetFocus();
}

void CLnkCtrl::OnTimer(UINT nIDEvent) 
{
	Highlight();
	CStatic::OnTimer(nIDEvent);
}

void CLnkCtrl::Highlight()
{
	POINT pt;
	BOOL bIsUp;

	if(!m_pFn) return;

	GetCursorPos(&pt);
	bIsUp = WindowFromPoint(pt)->m_hWnd == m_hWnd;

	if(bIsUp && m_bCheckHOverRgnIn)
	{
		ScreenToClient(&pt);
		bIsUp = m_rgnHOverCheck.PtInRegion(pt);
	}

	if(bIsUp)
	{
		if(!m_bIsMouseIn)
		{
			if(m_bShowHand) ::SetCursor(m_hSelectedCursor = m_hHand);
			m_bDrawBk = m_crBackColor != m_crHOverBackColor;
			m_bIsMouseIn = true;
			Invalidate();
		}
	}
	else if(m_bIsMouseIn)
	{
		if(m_bShowHand) ::SetCursor(m_hSelectedCursor = m_hArrow);
		m_bDrawBk = m_crBackColor != m_crHOverBackColor;
		m_bIsMouseIn = false;
		Invalidate();
	}
}

int CLnkCtrl::GetLines() { return(m_nLines); }
void CLnkCtrl::SetBreakWords(BOOL bBreakWords) { m_bBreakWords = bBreakWords; }
BOOL CLnkCtrl::GetBreakWords() { return(m_bBreakWords); }
void CLnkCtrl::SetCheckHOverRng(BOOL bCheckHOverRgn) { m_bCheckHOverRgn = bCheckHOverRgn; }
BOOL CLnkCtrl::GetCheckHOverRng() { return(m_bCheckHOverRgn); }
void CLnkCtrl::ResetHeight() { m_nHeight = -1; }
BOOL CLnkCtrl::GetFullAlign() { return(m_bFullAlign); }
void CLnkCtrl::SetFullAlign(BOOL bSetFullAlign) { m_bFullAlign = bSetFullAlign; }

CBitmap* CLnkCtrl::GetLinkImg() { return(m_pBmpLinkImg); }
void CLnkCtrl::SetResLinkImg(UINT nBmpID, COLORREF crTransparentColor /*= -1*/)
{
	if(!nBmpID) return;

	if(m_pBmpLinkImg)
		m_pBmpLinkImg->DeleteObject();
	else
		m_pBmpLinkImg = new CBitmap();

	m_pBmpLinkImg->LoadBitmap(nBmpID);

	if(crTransparentColor == -1)
	{
		CDC dc;
		dc.CreateCompatibleDC(NULL);
		CBitmap *oldBmp = dc.SelectObject(m_pBmpLinkImg);
		m_crLinkImgTransparentColor = dc.GetPixel(0, 0);
		dc.SelectObject(oldBmp);
		dc.DeleteDC();
	}
	else
		m_crLinkImgTransparentColor = crTransparentColor;
}

void CLnkCtrl::DeleteLinkImg()
{
	if(m_pBmpLinkImg)
	{
		m_pBmpLinkImg->DeleteObject();
		delete m_pBmpLinkImg;
		m_pBmpLinkImg = NULL;
	}
}

void CLnkCtrl::SetForceSingleLineWidth(BOOL bForceSingleLineWidth) { m_bForceSingleLineWidth = bForceSingleLineWidth; }
BOOL CLnkCtrl::GetForceSingleLineWidth() { return(m_bForceSingleLineWidth); }
BOOL CLnkCtrl::GetUseLinkImgTransparentColor() { return(m_bUseLinkImgTransparentColor); }
void CLnkCtrl::SetUseLinkImgTransparentColor(BOOL bUseLinkImgTransparentColor) { m_bUseLinkImgTransparentColor = bUseLinkImgTransparentColor; }

BOOL CLnkCtrl::GetHOverFontUnderline() { return(m_bHOverFontUnderline); }
void CLnkCtrl::SetHOverFontUnderline(BOOL bHOverFontUnderline)
{
	m_bHOverFontUnderline = bHOverFontUnderline;
	BuildFont();
	if(IsWindow(m_hWnd)) UpdateWindow();
}

int CLnkCtrl::GetOutWordImgDist() { return(m_nOutWordImgDist); }
void CLnkCtrl::SetOutWordImgDist(int nOutWordImgDist) { m_nOutWordImgDist = nOutWordImgDist; }

BOOL CLnkCtrl::OnToolTipsNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult)
{

    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT*)pTTTStruct;
    UINT nID = pTTTStruct->idFrom;

	TRACE("TTN_NEEDTEXT");

    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        /*nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
            return(TRUE);
        }*/
		pTTT->lpszText = m_strToolTip.GetBuffer(m_strToolTip.GetLength() + 1);
		pTTT->hinst = NULL;
		return(TRUE);
    }


    return(FALSE);

}

BOOL CLnkCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	//m_tpLnkToolTip.RelayEvent(pMsg);
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CLnkCtrl::SetInBorderSize(int nInBorderSize)
{
	m_nInBorderSize = nInBorderSize;
}

int CLnkCtrl::GetInBorderSize(void) { return m_nInBorderSize; }

void CLnkCtrl::SetBorderColor(COLORREF crBorderColor)
{
	m_crBorderColor = crBorderColor;
}

void CLnkCtrl::DrawBorder(CDC* pDC)
{
	if(m_iBorderSides & SBSTop)
		CPWGraphics::DrawLineEx(pDC, m_crBorderColor, 0, 0, m_nWidth, 0);

	if(m_iBorderSides & SBSRight)
		CPWGraphics::DrawLineEx(pDC, m_crBorderColor, m_nWidth - 1, 0, m_nWidth - 1, m_nHeight - 1);

	if(m_iBorderSides & SBSBottom)
		CPWGraphics::DrawLineEx(pDC, m_crBorderColor, m_nWidth - 1, m_nHeight - 1, 0, m_nHeight - 1);

	if(m_iBorderSides & SBSLeft)
		CPWGraphics::DrawLineEx(pDC, m_crBorderColor, 0, m_nHeight - 1, 0, 0);
}

void CLnkCtrl::SetBorderSides(bool left, bool right, bool top, bool bottom)
{
	m_iBorderSides = SBSAll;

	if(left) m_iBorderSides |= SBSLeft;
	if(right) m_iBorderSides |= SBSRight;
	if(top) m_iBorderSides |= SBSTop;
	if(bottom) m_iBorderSides |= SBSBottom;
}

void CLnkCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CStatic::OnKillFocus(pNewWnd);

	m_bHasKeybordFocus = false;
	DrawFocus();
}

void CLnkCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CStatic::OnSetFocus(pOldWnd);

	m_bHasKeybordFocus = true;
	DrawFocus();
}

void CLnkCtrl::DrawFocus(bool bAfterPaint /*= false*/)
{
	CRect rc;

	GetClientRect(rc);
	//rc.DeflateRect(2, 2);
	rc.InflateRect(1, 1);

	CDC *pDC = GetDC();

	if(bAfterPaint && m_bHasKeybordFocus)
		pDC->DrawFocusRect(rc);
	else if(!bAfterPaint)
		pDC->DrawFocusRect(rc);

	ReleaseDC(pDC);
}

void CLnkCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if(nChar == 13 || nChar == 10 || nChar == 32)
		if(m_pFn) m_pFn(this, m_lParam);

	CStatic::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CLnkCtrl::GetShowHandCursor() { return(m_bShowHand); }
void CLnkCtrl::SetShowHandCursor(BOOL m_bSet) { m_bShowHand = m_bSet; }
