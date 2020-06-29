// ButtonExt.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Bineye.h"
#include ".\ButtonExt.h"


// CButtonExt

CButtonExt::CButtonExt() :
m_bInit(FALSE),
m_bRoundRect(FALSE),
m_nRoundX(7),
m_nRoundY(7),
m_colrefTextColor(0xFFFFFF),
m_enmState(CPWGraphics::stateNormal),
m_pBmpBtnImg(NULL)
{
	m_objWndRegion.DeleteObject();
	m_objWndDrawRegion.DeleteObject();
	m_objWndFocusRegion.DeleteObject();

	m_objFont.CreateStockObject(DEFAULT_GUI_FONT);
	m_objFont.SetFontFace(_T("Tahoma"));
	m_objFont.SetFontCharSet(GREEK_CHARSET);
	//m_colrefBkColor = GetSysColor(
}

CButtonExt::~CButtonExt()
{
	if(m_pBmpBtnImg) 
	{
		m_pBmpBtnImg->DeleteObject();
		delete m_pBmpBtnImg;
	}
}


BEGIN_MESSAGE_MAP(CButtonExt, CButton)
	//{{AFX_MSG_MAP(CButtonExt)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonExt message handlers

void CButtonExt::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	
	BITMAP bmp;
	CDC memDC;
	CString strText;
	CRect brc;
	int nImgX,
		nImgY;

	if(!m_bInit) InitCtrl();

	GetWindowText(strText);
	GetClientRect(brc);

	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);

	// -----

	//DrawPlastic(lpDrawItemStruct->hDC);

	if(!IsWindowEnabled()) m_enmState = CPWGraphics::stateDisable;

	lpDrawItemStruct->rcItem.right--;
	lpDrawItemStruct->rcItem.bottom--;

	CPWGraphics::DrawPlastic(lpDrawItemStruct->hDC, 
							 m_enmState, 
							 &lpDrawItemStruct->rcItem,
							 0x00D8E9EC,
							 -1, 
							 0xC0C0);

	if(GetFocus() && GetFocus()->m_hWnd == m_hWnd)
	{
		CRect rcFocus(lpDrawItemStruct->rcItem);
		rcFocus.DeflateRect(1, 3);
		dc.DrawFocusRect(rcFocus);
	}

	dc.SetBkMode(TRANSPARENT);

	m_objFont.AttachDC(&dc);


	if(m_pBmpBtnImg)
	{
		m_pBmpBtnImg->GetObject(sizeof(bmp), &bmp);
		nImgX = brc.Width() / 2 - (bmp.bmWidth + dc.GetTextExtent(strText).cx + 5) / 2;
		nImgY = brc.Height() / 2 - bmp.bmHeight / 2;

		memDC.CreateCompatibleDC(&dc);
		HGDIOBJ hOldBmp = memDC.SelectObject((HGDIOBJ)m_pBmpBtnImg->m_hObject);

		TransparentBlt(dc.m_hDC, nImgX, nImgY, bmp.bmWidth, bmp.bmHeight,
					   memDC.m_hDC, 0, 0, bmp.bmWidth, bmp.bmHeight, m_colrefBtnImgTransparentColor);

		memDC.SelectObject(hOldBmp);
		memDC.DeleteDC();

		nImgX += bmp.bmWidth + 5;
	} else {
		nImgX = brc.Width() / 2 - dc.GetTextExtent(strText).cx / 2;
	}

	nImgY = brc.Height() / 2 - dc.GetTextExtent(strText).cy / 2;

	//brc.SetRect(nImgX, nImgY, nImgX + dc.GetTextExtent(strText).cx, nImgY + dc.GetTextExtent(strText).cy);

	brc.SetRect(nImgX, brc.top, nImgX + dc.GetTextExtent(strText).cx, brc.bottom);

	dc.DrawText(strText, brc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	m_objFont.DetachDC();

	dc.Detach();


	return;



	// -----





	//dc.SetBkColor(0xFFC0C0);
	//dc.SetTextColor(RGB(255,255,255));

	dc.SetBkColor(m_colrefBkColor);
	dc.SetTextColor(m_colrefTextColor);

	////dc.FillSolidRect(brc, RGB(0,0,255));

	CBrush bkbrush;

	//bkbrush.CreateSolidBrush(0xFFC0C0);

	bkbrush.CreateSolidBrush(m_colrefBkColor);


	dc.FillRgn(&m_objWndDrawRegion, &bkbrush);

	bkbrush.DeleteObject();
	bkbrush.CreateSolidBrush(0x000000);


	dc.FrameRgn(&m_objWndDrawRegion, &bkbrush, 1, 1);

	bkbrush.DeleteObject();


	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		CRect frc;

		//GetClientRect(frc);
		//frc.InflateRect(-1, -1);
		//dc.DrawFocusRect(frc);

		bkbrush.CreateSolidBrush(RGB(255,255,0));

		dc.FrameRgn(&m_objWndFocusRegion, &bkbrush, 1, 1);

		bkbrush.DeleteObject();


	}

	dc.DrawText(strText, brc, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

	dc.Detach();
}

#define rgn_xy 6

void CButtonExt::InitCtrl()
{
	CRect crc;

	DoTME();
	GetClientRect(crc);

	m_objWndRegion.DeleteObject();
	m_objWndDrawRegion.DeleteObject();
	m_objWndFocusRegion.DeleteObject();

	m_objWndRegion.CreateRoundRectRgn(crc.left, crc.top, crc.right, crc.bottom, m_nRoundX, m_nRoundY);
	m_objWndDrawRegion.CreateRoundRectRgn(crc.left, crc.top, crc.right, crc.bottom, m_nRoundX, m_nRoundY);

	crc.InflateRect(-1, -1);
	m_objWndFocusRegion.CreateRoundRectRgn(crc.left, crc.top, crc.right, crc.bottom, m_nRoundX, m_nRoundY);

	SetWindowRgn(m_objWndRegion, TRUE);

	m_bInit = TRUE;
	m_bIsHOver = FALSE;
}

BOOL CButtonExt::GetRoundRect() { return(m_bRoundRect); }
void CButtonExt::SetRoundRect(BOOL bRoundRect, int nRndX /*= -1*/, int nRndY /*= -1*/)
{
	m_bRoundRect = bRoundRect;

	if(nRndX != -1) m_nRoundX = nRndX;
	if(nRndY != -1) m_nRoundY = nRndY;
}

COLORREF CButtonExt::GetBkColor() { return(m_colrefBkColor); }
void CButtonExt::SetBkColor(COLORREF colrefBkColor) { m_colrefBkColor = colrefBkColor; }

COLORREF CButtonExt::GetTextColor() { return(m_colrefTextColor); }
void CButtonExt::SetTextColor(COLORREF colrefTextColor) { m_colrefTextColor = colrefTextColor; }

COLORREF CButtonExt::GetBorderColor() { return(m_colrefBorderColor); }
void CButtonExt::SetBorderColor(COLORREF colrefBorderColor) { m_colrefBorderColor = colrefBorderColor; }

COLORREF CButtonExt::GetFocusColor() { return(m_colrefFocusColor); }
void CButtonExt::SetFocusColor(COLORREF colrefFocusColor) { m_colrefFocusColor = colrefFocusColor; }

void CButtonExt::SetRoundRectX(int nRndX) { m_nRoundX = nRndX; }
void CButtonExt::SetRoundRectY(int nRndY) { m_nRoundY = nRndY; }
void CButtonExt::SetRoundRectXY(int nRndXY) { m_nRoundX = m_nRoundY = nRndXY; }
int CButtonExt::GetRoundRectX() { return(m_nRoundX); }
int CButtonExt::GetRoundRectY() { return(m_nRoundY); }

CFontExt* CButtonExt::GetFontExt() { return(&m_objFont); }

BOOL CButtonExt::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	if(message == WM_MOUSEHOVER)
	{
		if(!m_bIsHOver)
		{
			m_enmState = CPWGraphics::stateHot;
			SetBkColor(RGB(0x0,0x0,0xFF));
			RedrawWindow();
			m_bIsHOver = TRUE;

		}
	}
	else if(message == WM_MOUSELEAVE)
	{
		m_enmState = CPWGraphics::stateNormal;
		SetBkColor(0xFFC0C0);
		RedrawWindow();
		m_bIsHOver = FALSE;
	}
	else if(message == WM_LBUTTONDOWN)
	{
		m_enmState = CPWGraphics::statePressed;
		SetBkColor(RGB(0xff,0x00,0x00));
		RedrawWindow();
	}
	else if(message == WM_LBUTTONUP)
	{
		m_enmState = CPWGraphics::stateHot;
		SetBkColor(RGB(0x0,0x0,0xFF));
		RedrawWindow();
	}
	else if(message == WM_MOUSEMOVE)
		DoTME();

	return CButton::OnWndMsg(message, wParam, lParam, pResult);
}

void CButtonExt::DoTME(DWORD dwFlags /*= TME_HOVER | TME_LEAVE*/)
{
	TRACKMOUSEEVENT tme;

	tme.cbSize = sizeof(tme);
	tme.dwFlags = dwFlags;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = 10;

	TrackMouseEvent(&tme);
}

BOOL CButtonExt::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return true; //CButton::OnEraseBkgnd(pDC);
}

void CButtonExt::DrawPlastic(HDC hDC)
{
	CRect wrc;
	int cx, cy;

	GetWindowRect(wrc);

	cx = wrc.Width();
	cy = wrc.Height();

	m_colrefBkColor = 0x00D8E9EC;

	COLORREF colrefTmpColor = m_colrefBkColor;

	if(!IsWindowEnabled()) m_enmState = CPWGraphics::stateDisable;

	switch(m_enmState)
	{
		case CPWGraphics::stateNormal:
		case CPWGraphics::stateDefault:
			colrefTmpColor = CPWGraphics::OffsetColor(colrefTmpColor, 0x8);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor,  0xF), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0xF), 
					1, 1, cx - 2, cy - 3);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor,  0x15), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x15), 
					1, 2, 2, cy - 5);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x5), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x20), 
					cx - 2, 2, cx - 3, cy - 5);

			colrefTmpColor = CPWGraphics::OffsetColor(colrefTmpColor, -0x60);

			CPWGraphics::DrawLine(hDC, 2, 0, cx - 2, 0, 0xff); //colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 0, 2, 0, cy - 2, 0xff); //colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 2, cy - 1, cx - 2, cy - 1, 0xff); //colrefTmpColor);
			CPWGraphics::DrawLine(hDC, cx - 1, 1, cx - 1, cy - 1, 0xff); //colrefTmpColor);

			SetPixel(hDC, 1, 1, colrefTmpColor);
			SetPixel(hDC, 1, cy - 2, colrefTmpColor);
			SetPixel(hDC, cx - 2, 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 2, colrefTmpColor);

			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, -0x15);

			SetPixel(hDC, 1, 0, colrefTmpColor);
			SetPixel(hDC, cx - 2, 0, colrefTmpColor);

			SetPixel(hDC, 0, 1, colrefTmpColor);
			SetPixel(hDC, 0, cy - 2, colrefTmpColor);

			SetPixel(hDC, 1, cy - 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 1, colrefTmpColor);

			SetPixel(hDC, cx - 1, 1, colrefTmpColor);
			SetPixel(hDC, cx - 1, cy - 2, colrefTmpColor);

			CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(m_colrefBkColor, 0x15));
			CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(m_colrefBkColor, -0x25));

			/*
			If iState = stateDefaulted Or m_bFocused Then
                tmpcolor = IIf(m_bUseCustomColors, m_lHighlightColor, GetSysColor(COLOR_HIGHLIGHT))
                APILine 2, 1, UserControl.ScaleWidth - 2, 1, OffsetColor(tmpcolor, &H15)
                APILine 1, 2, UserControl.ScaleWidth - 1, 2, OffsetColor(tmpcolor, &H15)
                APILine 1, UserControl.ScaleHeight - 3, UserControl.ScaleWidth - 1, UserControl.ScaleHeight - 3, OffsetColor(tmpcolor, -&H5)
                APILine 2, UserControl.ScaleHeight - 2, UserControl.ScaleWidth - 2, UserControl.ScaleHeight - 2, OffsetColor(tmpcolor, -&H15)
            End If
			*/

			//if(m_enmState == stateDefault /*|| m_bFocused*/)
			//{
				colrefTmpColor = GetSysColor(COLOR_HIGHLIGHT);
				CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(colrefTmpColor, 0x15));
				CPWGraphics::DrawLine(hDC, 1, 2, cx - 1, 2, CPWGraphics::OffsetColor(colrefTmpColor, 0x15));
				CPWGraphics::DrawLine(hDC, 1, cy - 3, cx - 1, cy - 3, CPWGraphics::OffsetColor(colrefTmpColor, -0x5));
				CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(colrefTmpColor, -0x15));
			//}
			break;
		case CPWGraphics::statePressed:
			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, -0x9);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor, -0xF), 
					colrefTmpColor, 
					1, 1, cx - 2, cy - 2);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x15), 
					CPWGraphics::OffsetColor(colrefTmpColor,  0x15), 
					cx - 2, 2, cx - 3, cy - 5);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x20), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x5), 
					1, 2, 2, cy - 5);

			colrefTmpColor = CPWGraphics::OffsetColor(colrefTmpColor, -0x60);

			CPWGraphics::DrawLine(hDC, 2, 0, cx - 2, 0, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 0, 2, 0, cy - 2, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 2, cy - 1, cx - 2, cy - 1, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, cx - 1, 1, cx - 1, cy - 1, colrefTmpColor);

			SetPixel(hDC, 1, 1, colrefTmpColor);
			SetPixel(hDC, 1, cy - 2, colrefTmpColor);
			SetPixel(hDC, cx - 2, 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 2, colrefTmpColor);

			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, -0x15);

			SetPixel(hDC, 1, 0, colrefTmpColor);
			SetPixel(hDC, cx - 2, 0, colrefTmpColor);

			SetPixel(hDC, 0, 1, colrefTmpColor);
			SetPixel(hDC, 0, cy - 2, colrefTmpColor);

			SetPixel(hDC, 1, cy - 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 1, colrefTmpColor);

			SetPixel(hDC, cx - 1, 1, colrefTmpColor);
			SetPixel(hDC, cx - 1, cy - 2, colrefTmpColor);

			CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(m_colrefBkColor, 0x8));
			CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(m_colrefBkColor, -0x15));
			break;
		case CPWGraphics::stateHot:
			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, 0x18);
			::SetBkColor(hDC, colrefTmpColor);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor,  0x10), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x10), 
					1, 1, cx - 2, cy - 3);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor,  0x15), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x15), 
					1, 2, 2, cy - 5);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x5), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x20), 
					cx - 2, 2, cx - 3, cy - 5);

			colrefTmpColor = CPWGraphics::OffsetColor(colrefTmpColor, -0x60);

			CPWGraphics::DrawLine(hDC, 2, 0, cx - 2, 0, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 0, 2, 0, cy - 2, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 2, cy - 1, cx - 2, cy - 1, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, cx - 1, 1, cx - 1, cy - 1, colrefTmpColor);

			SetPixel(hDC, 1, 1, colrefTmpColor);
			SetPixel(hDC, 1, cy - 2, colrefTmpColor);
			SetPixel(hDC, cx - 2, 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 2, colrefTmpColor);

			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, -0x15);

			SetPixel(hDC, 1, 0, colrefTmpColor);
			SetPixel(hDC, cx - 2, 0, colrefTmpColor);

			SetPixel(hDC, 0, 1, colrefTmpColor);
			SetPixel(hDC, 0, cy - 2, colrefTmpColor);

			SetPixel(hDC, 1, cy - 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 1, colrefTmpColor);

			SetPixel(hDC, cx - 1, 1, colrefTmpColor);
			SetPixel(hDC, cx - 1, cy - 2, colrefTmpColor);

			CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(m_colrefBkColor, 0x15));
			CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(m_colrefBkColor, -0x10));

			colrefTmpColor = GetSysColor(COLOR_BTNFACE);

			CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(m_colrefBkColor, 0x15));
			CPWGraphics::DrawLine(hDC, 1, 2, cx - 1, 2, CPWGraphics::OffsetColor(m_colrefBkColor, 0x15));
			CPWGraphics::DrawLine(hDC, 1, cy - 3, cx - 1, cy - 3, CPWGraphics::OffsetColor(m_colrefBkColor, -0x5));
			CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(m_colrefBkColor, -0x15));



				colrefTmpColor = 0xC0C0; //COLOR_HOTLIGHT);
				CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(colrefTmpColor, 0x15));
				CPWGraphics::DrawLine(hDC, 1, 2, cx - 1, 2, CPWGraphics::OffsetColor(colrefTmpColor, 0x15));
				CPWGraphics::DrawLine(hDC, 1, cy - 3, cx - 1, cy - 3, CPWGraphics::OffsetColor(colrefTmpColor, -0x5));
				CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(colrefTmpColor, -0x15));




			break;
		case CPWGraphics::stateDisable:
			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, 0x12);
			::SetBkColor(hDC, colrefTmpColor);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor,  0xF), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0xF), 
					1, 1, cx - 2, cy - 3);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor,  0x15), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x15), 
					1, 2, 2, cy - 5);

			CPWGraphics::DrawVGradient(hDC, 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x5), 
					CPWGraphics::OffsetColor(colrefTmpColor, -0x20), 
					cx - 2, 2, cx - 3, cy - 5);

			colrefTmpColor = CPWGraphics::OffsetColor(colrefTmpColor, -0x60);

			CPWGraphics::DrawLine(hDC, 2, 0, cx - 2, 0, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 0, 2, 0, cy - 2, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, 2, cy - 1, cx - 2, cy - 1, colrefTmpColor);
			CPWGraphics::DrawLine(hDC, cx - 1, 1, cx - 1, cy - 1, colrefTmpColor);

			SetPixel(hDC, 1, 1, colrefTmpColor);
			SetPixel(hDC, 1, cy - 2, colrefTmpColor);
			SetPixel(hDC, cx - 2, 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 2, colrefTmpColor);

			colrefTmpColor = CPWGraphics::OffsetColor(m_colrefBkColor, 0x5);

			SetPixel(hDC, 1, 0, colrefTmpColor);
			SetPixel(hDC, cx - 2, 0, colrefTmpColor);

			SetPixel(hDC, 0, 1, colrefTmpColor);
			SetPixel(hDC, 0, cy - 2, colrefTmpColor);

			SetPixel(hDC, 1, cy - 1, colrefTmpColor);
			SetPixel(hDC, cx - 2, cy - 1, colrefTmpColor);

			SetPixel(hDC, cx - 1, 1, colrefTmpColor);
			SetPixel(hDC, cx - 1, cy - 2, colrefTmpColor);

			CPWGraphics::DrawLine(hDC, 2, 1, cx - 2, 1, CPWGraphics::OffsetColor(m_colrefBkColor, 0x15));
			CPWGraphics::DrawLine(hDC, 2, cy - 2, cx - 2, cy - 2, CPWGraphics::OffsetColor(m_colrefBkColor, -0x25));

            //'If iState = stateDefaulted Or m_bFocused Then
            //'    tmpcolor = IIf(m_bUseCustomColors, m_lHighlightColor, GetSysColor(COLOR_HIGHLIGHT))
            //'    APILine 2, 1, UserControl.ScaleWidth - 2, 1, OffsetColor(tmpcolor, &H15)
            //'    APILine 1, 2, UserControl.ScaleWidth - 1, 2, OffsetColor(tmpcolor, &H15)
            //'    APILine 1, UserControl.ScaleHeight - 3, UserControl.ScaleWidth - 1, UserControl.ScaleHeight - 3, OffsetColor(tmpcolor, -&H5)
            //'    APILine 2, UserControl.ScaleHeight - 2, UserControl.ScaleWidth - 2, UserControl.ScaleHeight - 2, OffsetColor(tmpcolor, -&H15)
            //'End If

			break;
	}


}

BOOL CButtonExt::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CButton::PreCreateWindow(cs);
}

void CButtonExt::SetBtnImg(UINT nResImgID, COLORREF colrefTransparentColor)
{
	if(m_pBmpBtnImg)
		m_pBmpBtnImg->DeleteObject();
	else
		m_pBmpBtnImg = new CBitmap();

	m_pBmpBtnImg->LoadBitmap(nResImgID);
	m_colrefBtnImgTransparentColor = colrefTransparentColor;
}

void CButtonExt::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	DWORD gcl = GetClassLong(m_hWnd, GCL_STYLE);

	gcl &= ~CS_PARENTDC;


	SetClassLong(m_hWnd, GCL_STYLE, gcl);

	CButton::PreSubclassWindow();
}
