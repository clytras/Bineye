// FullAlignStatic.cpp : implementation file
//

#include <afxcmn.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxtempl.h>		// Let manage CArray

#include ".\LinkCtrl.h"
#include ".\pwio.h"

//For those people who do not have the absolutely latest SDK header files
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)   // Taken from VC6 WINUSER.H
#endif // IDC_HAND


#define FAT_TEXTCOLOR			0xFF0000
#define FAT_BACKCOLOR			-1
#define FAT_HOVERTEXTCOLOR		0x0000FF
#define FAT_HOVERBACKCOLOR		-1
#define FAT_SELECTEDTEXTCOLOR	0x400040
#define FAT_SELECTEDBACKCOLOR	-1

#define FAT_TIMEREVENT		101

/////////////////////////////////////////////////////////////////////////////
// CLnkCtrl

CLnkCtrl::CLnkCtrl() :
m_nWidth(400),
m_nHeight(-1),
m_nLines(0),
m_iID(0),
m_nLastLineWidth(0),
m_nOutWordImgDist(-1),
m_strOutWord(_T("")),
m_strTag(_T("")),
m_bAlignLastLine(false),
m_bDrawBk(true),
m_pFn(NULL),
m_lParam(NULL),
m_pBmpLinkImg(NULL),
m_pHOverBmpLinkImg(NULL),
m_pSelectedBmpLinkImg(NULL),
m_bIsMouseIn(false),
m_bSelected(false),
m_bDisable(false),
m_bTimerSet(false),
m_bBreakWords(true),
m_bCheckHOverRgn(true),
m_bCheckHOverRgnIn(false),
m_bDrawShadow(false),
m_bFullAlign(false),
m_bUseLinkImgTransparentColor(true),
m_bForceSingleLineWidth(true),
m_bHOverFontUnderline(false),
m_crTextColor(FAT_TEXTCOLOR),
m_crBackColor(FAT_BACKCOLOR),
m_crHOverTextColor(FAT_HOVERTEXTCOLOR),
m_crHOverBackColor(FAT_HOVERBACKCOLOR),
m_crSelectedTextColor(FAT_SELECTEDTEXTCOLOR),
m_crSelectedBackColor(FAT_SELECTEDBACKCOLOR),
m_crLinkImgTransparentColor(-1),
m_crHOverLinkImgTransparentColor(-1),
m_crSelectedLinkImgTransparentColor(-1),
m_crShadowColor(0),
m_hBmpBkTransparent(NULL),
m_hBmpBkTransDC(NULL),
m_pBmpParentBg(NULL)
, m_bOnClickCheckMouse(false)
, m_iParentID(0)
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

	if(m_pHOverBmpLinkImg)
	{
		m_pHOverBmpLinkImg->DeleteObject();
		delete m_pHOverBmpLinkImg;
	}

	if(m_pSelectedBmpLinkImg)
	{
		m_pSelectedBmpLinkImg->DeleteObject();
		delete m_pSelectedBmpLinkImg;
	}

}

BEGIN_MESSAGE_MAP(CLnkCtrl, CStatic)
	//{{AFX_MSG_MAP(CLnkCtrl)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLnkCtrl message handlers

void CLnkCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC* pParentDC = GetParent()->GetDC();
	CRect rc;

	if(!m_bTimerSet)
	{
		SetTimer(FAT_TIMEREVENT, 10, NULL);

		m_hBmpBkTransparent = CreateCompatibleBitmap(pParentDC->m_hDC, m_nWidth, m_nHeight);
		m_hBmpBkTransDC = CreateCompatibleDC(pParentDC->m_hDC);

		HGDIOBJ hOldBmp = SelectObject(m_hBmpBkTransDC, (HGDIOBJ)m_hBmpBkTransparent);

		BitBlt(m_hBmpBkTransDC, 0, 0, m_nWidth, m_nHeight, pParentDC->m_hDC, 0, 0, SRCCOPY);

		SelectObject(m_hBmpBkTransDC, hOldBmp);
		DeleteDC(m_hBmpBkTransDC);

		m_bTimerSet = true;
	}

	DWORD dwStyle = GetStyle();
	if (!(dwStyle & SS_NOTIFY)) 
	{
		// Turn on notify flag to get mouse messages and STN_CLICKED.
		// Otherwise, I'll never get any mouse clicks!
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
	}

	if(m_crBackColor == -1)
	{
		CDC *pParentDC = GetParent()->GetDC();
		m_crBackColor = pParentDC->GetBkColor();
		GetParent()->ReleaseDC(pParentDC);
	}

	if(m_crHOverBackColor == -1)
	{
		CDC *pParentDC = GetParent()->GetDC();
		m_crHOverBackColor = pParentDC->GetBkColor();
		GetParent()->ReleaseDC(pParentDC);
	}

	if(m_nHeight == -1) m_nHeight = GetHeightOrPaint(&dc);

	GetClientRect(rc);

	if(rc.Width() != m_nWidth || rc.Height() != m_nHeight)
	{
		SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
		GetClientRect(rc);
	}

	dc.SetTextColor(m_bIsMouseIn ? m_crHOverTextColor : m_bSelected ? m_crSelectedTextColor : m_crTextColor);
	dc.SetBkColor(m_bIsMouseIn ? m_crHOverBackColor : m_bSelected ? m_crSelectedBackColor : m_crBackColor);

	dc.SetBkMode(TRANSPARENT);

	//if(m_bDrawBk)
	//{
		//dc.FillSolidRect(rc, RGB(255,255,255)); // dc.GetBkColor());

		//CDC *pParentDC = GetParent()->GetDC();
		//CRect wrc;

		//GetWindowRect(wrc);

		//GetParent()->ScreenToClient(wrc);


		////BitBlt(dc.m_hDC, 0, 0, wrc.Width(), wrc.Height(), m_hBmpBkTransDC, 0, 0, SRCCOPY);

		//GetParent()->ReleaseDC(pParentDC);


	CRect wrc;
	GetWindowRect(wrc);

	GetParent()->ScreenToClient(wrc);
	//GetParent()->RedrawWindow(wrc, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
	//GetParent()->InvalidateRect(wrc);

	//CDC* pParentDC = GetParent()->GetDC();

	//GetParent()->RedrawWindow(wrc, NULL, RDW_INVALIDATE|RDW_UPDATENOW);


	//dc.BitBlt(0, 0, wrc.Width(), wrc.Height(), pParentDC, wrc.left, wrc.top, SRCCOPY);

	m_hBmpBkTransDC = CreateCompatibleDC(dc.m_hDC);

	HGDIOBJ hOB = SelectObject(m_hBmpBkTransDC, m_hBmpBkTransparent);

	BitBlt(dc.m_hDC, 0, 0, wrc.Width(), wrc.Height(), m_hBmpBkTransDC, wrc.left, wrc.top, SRCCOPY);

	SelectObject(m_hBmpBkTransDC, hOB);

	DeleteDC(m_hBmpBkTransDC);



	//CFont* pFont = (CFont*)pDC->SelectObject(&m_Font);

	//pDC->SetBkMode(TRANSPARENT);
	//pDC->TextOut(0, 0, "Test xdsdfs");



	GetParent()->ReleaseDC(pParentDC);



	//}
	//else
		m_bDrawBk = TRUE;

	GetHeightOrPaint(&dc, TRUE);


}

CSize CLnkCtrl::GetControlSize() { return(CSize(GetWidth(), GetHeight())); }
int CLnkCtrl::GetWidth(BOOL bRecalc /*= FALSE*/)
{
	if(bRecalc)
	{
		CDC *pDC = GetDC();
		m_nHeight = GetHeightOrPaint(pDC);
		ReleaseDC(pDC);
	}

	return(m_nWidth);
}
void CLnkCtrl::SetWidth(int nWidth) { m_nWidth = nWidth; m_nHeight = -1; }
int CLnkCtrl::GetHeight()
{
	if(m_nHeight == -1)
	{
		CDC *pDC = GetDC();
		m_nHeight = GetHeightOrPaint(pDC);
		ReleaseDC(pDC);
	}

	return(m_nHeight);
}

int CLnkCtrl::GetHeightOrPaint(CDC *pDC, BOOL bPaint /*= FALSE*/)
{
	BITMAP bmp;
	CFont *pFont;
	CPoint txtPos(0, 0);
	CArray<CPoint, CPoint> ptRgnPoints;
	COLORREF crsave;
	CString strText(_T("")),
			strLine(_T("")),
			strWord(_T("")),
			strSpc(_T("")),
			strTmp(_T(""));
	bool bIsWordDone = false,
		 bIsTextDone = false;
	int i, j,
		nCharBegin,
		nCharLen,
		nSpcAdd = 0,
		nSpcCount = 0,
		nSpcWidth = 0,
		nSpcPos = 0,
		nCharHeight = 0,
		nTmpHeight = 0,
		nTmpWidth = 0,
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

	memset(&bmp, 0, sizeof(bmp));

	/*if(strText.IsEmpty() && bPaint)
		return(0);
	else if(strText.IsEmpty())
		strText = _T("A");*/

	m_nLineHeight = nCharHeight = pDC->GetOutputTextExtent(CString(_T("ABCDEFGhijklmnop"))).cy;
	nSpaceWidth = pDC->GetOutputTextExtent(_T(" ")).cx;
	nMinusWidth = pDC->GetOutputTextExtent(_T("-")).cx;

	if(m_bCheckHOverRgnIn && !m_bFullAlign)
		ptRgnPoints.Add(CPoint(0, 0));

	CBitmap *pBmpUse = m_bIsMouseIn && m_pHOverBmpLinkImg ? m_pHOverBmpLinkImg : m_bSelected ? m_pSelectedBmpLinkImg : m_pBmpLinkImg;
	COLORREF crBmpTransparent = m_bIsMouseIn && m_pHOverBmpLinkImg ? m_crHOverLinkImgTransparentColor :
																	m_bSelected ? m_crSelectedLinkImgTransparentColor :
																	 m_crLinkImgTransparentColor;

	if(pBmpUse)
	{
		pBmpUse->GetObject(sizeof(bmp), &bmp);

		txtPos.x = nOutWordWidth = bmp.bmWidth + (m_nOutWordImgDist != -1 ? m_nOutWordImgDist : nSpaceWidth);
		if(bPaint)
		{
			CDC bmpDC;
			bmpDC.CreateCompatibleDC(pDC);
			CBitmap *oldBmp = bmpDC.SelectObject(pBmpUse);

			if(m_bUseLinkImgTransparentColor)
				TransparentBlt(pDC->m_hDC, 0, 0, bmp.bmWidth, bmp.bmHeight,
							   bmpDC.m_hDC, 0, 0, bmp.bmWidth, bmp.bmHeight, crBmpTransparent);
			else
				pDC->BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &bmpDC, 0, 0, SRCCOPY);

			bmpDC.SelectObject(oldBmp);
			bmpDC.DeleteDC();
		}
	}
	else if(!m_strOutWord.IsEmpty())
	{
		txtPos.x = nOutWordWidth = pDC->GetOutputTextExtent(m_strOutWord).cx + (m_nOutWordImgDist != -1 ? m_nOutWordImgDist : nSpaceWidth);

		if(bPaint)
		{
			if(m_bDrawShadow)
			{
				crsave = pDC->GetTextColor();
				pDC->SetTextColor(m_crShadowColor);
				pDC->DrawText(m_strOutWord, CRect(txtPos.x+1, txtPos.y+1, nOutWordWidth, m_nLineHeight), DT_LEFT);
				pDC->SetTextColor(crsave);
			}
			pDC->DrawText(m_strOutWord, CRect(txtPos.x, txtPos.y, nOutWordWidth, m_nLineHeight), DT_LEFT);

			nCalc = txtPos.x - nOutWordWidth;
			strSpc = _T(' ');
			while(pDC->GetOutputTextExtent(strSpc).cx < nCalc) strSpc += _T(' ');
			strSpc += _T(' ');
			pDC->DrawText(strSpc, CRect(nOutWordWidth - nSpaceWidth, 0, nOutWordWidth, m_nLineHeight), DT_LEFT);
			strTmp.Empty();
		}
	}


	if(bPaint && m_nLines == 1 && pDC->GetOutputTextExtent(strText).cy < bmp.bmHeight)
	{
		txtPos.y = bmp.bmHeight / 2 - pDC->GetOutputTextExtent(strText).cy / 2;
	}

	nProcWidth = m_nWidth - nOutWordWidth;
	m_nLines = 0;
	m_nLastLineWidth = nProcWidth;

	if(!strText.IsEmpty()) {
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

										if(m_bDrawShadow)
										{
											crsave = pDC->GetTextColor();
											pDC->SetTextColor(m_crShadowColor);
											pDC->DrawText(strTmp, CRect(txtPos.x+1, txtPos.y+1, txtPos.x + nWordWidth, txtPos.y + m_nLineHeight), DT_LEFT);
											pDC->SetTextColor(crsave);
										}

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


											if(m_bDrawShadow)
											{
												crsave = pDC->GetTextColor();
												pDC->SetTextColor(m_crShadowColor);
												pDC->DrawText(strSpc, CRect(nSpcPos, txtPos.y+1, nSpcPos + pDC->GetOutputTextExtent(strSpc).cx, txtPos.y + m_nLineHeight + 1), DT_LEFT);
												pDC->SetTextColor(crsave);
											}

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
						{
							if(m_bDrawShadow)
							{
								crsave = pDC->GetTextColor();
								pDC->SetTextColor(m_crShadowColor);
								pDC->DrawText(strLine, CRect(txtPos.x+1, txtPos.y+1, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
								pDC->SetTextColor(crsave);
							}
							pDC->DrawText(strLine, CRect(txtPos.x, txtPos.y, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
						}

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

						if(m_bDrawShadow)
						{
							crsave = pDC->GetTextColor();
							pDC->SetTextColor(m_crShadowColor);
							pDC->DrawText(strLine, CRect(txtPos.x+1, txtPos.y+1, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
							pDC->SetTextColor(crsave);
						}
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
					if(m_bDrawShadow)
					{
						crsave = pDC->GetTextColor();
						pDC->SetTextColor(m_crShadowColor);
						pDC->DrawText(strLine, CRect(txtPos.x+1, txtPos.y+1, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
						pDC->SetTextColor(crsave);
					}
					pDC->DrawText(strLine, CRect(txtPos.x, txtPos.y, txtPos.x + pDC->GetOutputTextExtent(strLine).cx, txtPos.y + m_nLineHeight), DT_LEFT);
					txtPos.y += nCharHeight;
					txtPos.x = nOutWordWidth;
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
	}


	if(strText.IsEmpty())
		m_nWidth = bmp.bmWidth;
	else if(m_nLines == 1 && !m_bForceSingleLineWidth)
		m_nWidth = pDC->GetOutputTextExtent(strText).cx + nOutWordWidth;

	if(m_pBmpLinkImg && nTmpHeight < bmp.bmHeight)
		nTmpHeight = bmp.bmHeight;

	m_bCheckHOverRgnIn = m_bCheckHOverRgn && m_nLastLineWidth != m_nWidth && m_nLines > 1;

	if(m_bCheckHOverRgnIn)
	{
		m_rgnHOverCheck.DeleteObject();

		if(m_bFullAlign)
		{
			ptRgnPoints.Add(CPoint(0, 0));
			ptRgnPoints.Add(CPoint(m_nWidth, 0));
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
			ptRgnPoints.Add(CPoint(0, m_pBmpLinkImg ? bmp.bmHeight : m_nLineHeight));
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

COLORREF CLnkCtrl::GetSelectedTextColor() const { return(m_crSelectedTextColor); }
void CLnkCtrl::SetSelectedTextColor(COLORREF crSelectedTextColor)
{
	m_crSelectedTextColor = crSelectedTextColor;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

COLORREF CLnkCtrl::GetSelectedBackColor() const { return(m_crSelectedBackColor); }
void CLnkCtrl::SetSelectedBackColor(COLORREF crSelectedBackColor)
{
	m_crSelectedBackColor = crSelectedBackColor;
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
		_tcscpy((char*)&lf.lfFaceName, m_strFontFace);
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

	if(m_bDisable && !m_bIsMouseIn)
		return;
	else if(m_bDisable)
	{
		bIsUp = false;
		m_bIsMouseIn = true;
	}

	if(bIsUp)
	{
		if(!m_bIsMouseIn)
		{
			::SetCursor(m_hSelectedCursor = m_hHand);
			m_bDrawBk = m_crBackColor != m_crHOverBackColor;
			m_bIsMouseIn = true;
			Invalidate();
		}
	}
	else if(m_bIsMouseIn)
	{
		::SetCursor(m_hSelectedCursor = m_hArrow);
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

CBitmap* CLnkCtrl::GetHOverLinkImg() { return(m_pHOverBmpLinkImg); }
void CLnkCtrl::SetHOverResLinkImg(UINT nBmpID, COLORREF crTransparentColor /*= -1*/)
{
	if(!nBmpID) return;

	if(m_pHOverBmpLinkImg)
		m_pHOverBmpLinkImg->DeleteObject();
	else
		m_pHOverBmpLinkImg = new CBitmap();

	m_pHOverBmpLinkImg->LoadBitmap(nBmpID);

	if(crTransparentColor == -1)
	{
		CDC dc;
		dc.CreateCompatibleDC(NULL);
		CBitmap *oldBmp = dc.SelectObject(m_pHOverBmpLinkImg);
		m_crHOverLinkImgTransparentColor = dc.GetPixel(0, 0);
		dc.SelectObject(oldBmp);
		dc.DeleteDC();
	}
	else
		m_crHOverLinkImgTransparentColor = crTransparentColor;
}

CBitmap* CLnkCtrl::GetSelectedLinkImg() { return(m_pSelectedBmpLinkImg); }
void CLnkCtrl::SetSelectedResLinkImg(UINT nBmpID, COLORREF crTransparentColor /*= -1*/)
{
	if(!nBmpID) return;

	if(m_pSelectedBmpLinkImg)
		m_pSelectedBmpLinkImg->DeleteObject();
	else
		m_pSelectedBmpLinkImg = new CBitmap();

	m_pSelectedBmpLinkImg->LoadBitmap(nBmpID);

	if(crTransparentColor == -1)
	{
		CDC dc;
		dc.CreateCompatibleDC(NULL);
		CBitmap *oldBmp = dc.SelectObject(m_pSelectedBmpLinkImg);
		m_crSelectedLinkImgTransparentColor = dc.GetPixel(0, 0);
		dc.SelectObject(oldBmp);
		dc.DeleteDC();
	}
	else
		m_crSelectedLinkImgTransparentColor = crTransparentColor;
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

void CLnkCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CStatic::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	CRect wrc;

	GetClientRect(wrc);

	CDC* pDC = GetDC();

	//pDC->DrawFocusRect(wrc);

	ReleaseDC(pDC);
}

void CLnkCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CStatic::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	CRect wrc;

	GetClientRect(wrc);

	CDC* pDC = GetDC();

	//pDC->DrawFocusRect(wrc);

	ReleaseDC(pDC);

}

BOOL CLnkCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	/*static bool bIsInv;


	if(bIsInv)
	{
		bIsInv = false;
		//return true;
	}

	return true;

	CRect wrc;
	GetWindowRect(wrc);

	GetParent()->ScreenToClient(wrc);
	//GetParent()->RedrawWindow(wrc, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
	//GetParent()->InvalidateRect(wrc);

	CDC* pParentDC = GetParent()->GetDC();

	GetParent()->RedrawWindow(wrc, NULL, RDW_INVALIDATE|RDW_UPDATENOW);


	pDC->BitBlt(0, 0, wrc.Width(), wrc.Height(), pParentDC, wrc.left, wrc.top, SRCCOPY);

	//CFont* pFont = (CFont*)pDC->SelectObject(&m_Font);

	//pDC->SetBkMode(TRANSPARENT);
	//pDC->TextOut(0, 0, "Test xdsdfs");



	GetParent()->ReleaseDC(pParentDC);

	bIsInv = true;*/

	if(m_pBmpParentBg)
	{
		CRect wrc;
		
		GetWindowRect(wrc);
		GetParent()->ScreenToClient(wrc);

		CDC memDC;
		BITMAP bmp;

		m_pBmpParentBg->GetObject(sizeof(bmp), &bmp);

		memDC.CreateCompatibleDC(pDC);

		HGDIOBJ hOldBmp = memDC.SelectObject((HGDIOBJ)m_pBmpParentBg->m_hObject);

		pDC->BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &memDC, wrc.left, wrc.top, SRCCOPY);

		memDC.SelectObject(hOldBmp);
		memDC.DeleteDC();
	}

	return true; //CStatic::OnEraseBkgnd(pDC);
}

BOOL CLnkCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	//cs.lpszClass = AfxRegisterWndClass(0, 0, (HBRUSH)GetStockObject(WHITE_BRUSH), 0);

	return CStatic::PreCreateWindow(cs);
}

void CLnkCtrl::SetParentBgBmp(CBitmap* pBmpParentBg)
{
	m_pBmpParentBg = pBmpParentBg;
}

BOOL CLnkCtrl::GetDisable(void) const {	return m_bDisable; }
void CLnkCtrl::SetDisable(BOOL bDisable /*= TRUE*/) { m_bDisable = bDisable; }

BOOL CLnkCtrl::GetSelected(void) const { return m_bSelected; }
void CLnkCtrl::SetSelected(BOOL bSelected /*= TRUE*/)
{
	m_bSelected = bSelected;
	/*if(IsWindow(m_hWnd))*/ this->Invalidate(); // UpdateWindow();
}

BOOL CLnkCtrl::GetDrawShadow(void) { return m_bDrawShadow; }
void CLnkCtrl::SetDrawShadow(bool bDrawShadow)
{
	m_bDrawShadow = bDrawShadow;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

COLORREF CLnkCtrl::GetShadowColor(void) { return m_crShadowColor; }
void CLnkCtrl::SetShadowColor(COLORREF crShadowColor)
{
	m_crShadowColor = crShadowColor;
	if(IsWindow(m_hWnd)) UpdateWindow();
}

void CLnkCtrl::SetTag(LPCTSTR lpszTag) { m_strTag = lpszTag; }
LPCTSTR CLnkCtrl::GetTag(void) { return (LPCTSTR)m_strTag; }

void CLnkCtrl::SetID(int iID) { m_iID = iID; }
int CLnkCtrl::GetID(void) { return m_iID; }

int CLnkCtrl::GetParentID(void) { return m_iParentID; }

void CLnkCtrl::SetParentID(int iParentID) { m_iParentID = iParentID; }

CString CLnkCtrl::GetText(void)
{
	CString strText;
	GetWindowText(strText);
	return strText;
}

void CLnkCtrl::SetFileIcon(LPCTSTR lpszFile)
{
	SHFILEINFO sfi;
	BITMAP bmp;
	ICONINFO ii;
	HIMAGELIST hImgLst;
	CImageList *pImglst;

	if(CPWIO::FileExist(lpszFile))
	{
		hImgLst = (HIMAGELIST)SHGetFileInfoA((LPCSTR)lpszFile, 0L, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON);
		pImglst = CImageList::FromHandle(hImgLst);

		//{
			if(GetIconInfo(sfi.hIcon, &ii))
			{
				GetObject(ii.hbmColor, sizeof(bmp), &bmp);

				if(m_pBmpLinkImg)
					m_pBmpLinkImg->DeleteObject();
				else
					m_pBmpLinkImg = new CBitmap();

				CDC *pDeskDC = GetDesktopWindow()->GetDC();
				CDC dcSrc, dcDest;

				dcSrc.CreateCompatibleDC(NULL);
				dcSrc.SelectObject((HGDIOBJ)ii.hbmColor);

				m_pBmpLinkImg->CreateCompatibleBitmap(&dcSrc, 16 /*bmp.bmWidth*/, 16 /*bmp.bmHeight*/);


				dcDest.CreateCompatibleDC(NULL);
				CBitmap *oldBmp = dcDest.SelectObject(m_pBmpLinkImg);
				CBitmap maskBmp;
				maskBmp.Attach((HGDIOBJ)ii.hbmMask);

				CBrush br(0xFF00FF);

				CBrush *oldBr = dcDest.SelectObject(&br);

				dcDest.PatBlt(0, 0, 16, 16, PATCOPY);
				//dcDest.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dcSrc, 0, 0, SRCCOPY);
				//dcDest.MaskBlt(0, 0, 16, 16, &dcSrc, 0, 0, maskBmp, 0, 0, SRCCOPY);
				//dcDest.DrawIcon(0, 0, sfi.hIcon);

				pImglst->Draw(&dcDest, sfi.iIcon, CPoint(0, 0), ILD_NORMAL /*TRANSPARENT*/);
				//pImglst->DrawEx(&dcDest, sfi.iIcon, CPoint(0, 0), CSize(16, 16), CLR_DEFAULT, CLR_DEFAULT, ILD_IMAGE /*TRANSPARENT*/);
				

				m_crLinkImgTransparentColor = dcDest.GetPixel(0, 0);


				dcSrc.DeleteDC();

				dcDest.SelectObject(oldBr);
				dcDest.SelectObject(oldBmp);
				dcDest.DeleteDC();

				GetDesktopWindow()->ReleaseDC(pDeskDC);
				maskBmp.Detach();
			}

			DestroyIcon(sfi.hIcon);
		//}
	}
}
