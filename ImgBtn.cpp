// ImgBtn.cpp : implementation file
//

#include "stdafx.h"
#include "Bineye.h"
#include "Bineyedlg.h"
#include "ImgBtn.h"
#include ".\inc\PWGraphics.h"


// CImgBtn

IMPLEMENT_DYNAMIC(CImgBtn, CButton)

CImgBtn::CImgBtn() :
m_enmState(CPWGraphics::stateNormal),
m_bUseBgColor(false),
m_crBgColor((COLORREF)0),
m_bIsHOver(false)
{
	for(int i = 0; i < IBIMAGESTYPESCOUNT; i++)
	{
		m_pBmpImages[i] = NULL;
		m_ptImages[i].SetPoint(0, 0);
	}

	//m_pBmpImages[IBImgTypes::Normal] = new CBitmap();
	//m_pBmpImages[IBImgTypes::Normal]->LoadBitmap(IDB_TSTBTN);
}

CImgBtn::~CImgBtn()
{
	for(int i = 0; i < IBIMAGESTYPESCOUNT; i++)
	{
		if(m_pBmpImages[i])
		{
			m_pBmpImages[i]->DeleteObject();
			delete m_pBmpImages[i];
		}
	}
}


BEGIN_MESSAGE_MAP(CImgBtn, CButton)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CImgBtn message handlers


void CImgBtn::LoadBitmaps(UINT nIDBmpNormal,
						  UINT nIDBmpHot /*= 0*/,
						  UINT nIDBmpPressed /*= 0*/,
						  UINT nIDBmpDisabled /*= 0*/,
						  UINT nIDBmpFocused /*= 0*/)
{
	UINT uiBmps[] = { nIDBmpNormal,
					  nIDBmpHot,
					  nIDBmpPressed,
					  nIDBmpDisabled,
					  nIDBmpFocused };

	for(int i = 0; i < IBIMAGESTYPESCOUNT; i++)
	{
		if(uiBmps[i]) {
			m_pBmpImages[i] = new CBitmap();
			m_pBmpImages[i]->LoadBitmap(uiBmps[i]);
		} else if(m_pBmpImages[i]) {
			m_pBmpImages[i]->DeleteObject();
			delete m_pBmpImages[i];
		}
	}

	if(m_pBmpImages[IBImgTypes::Normal] && ::IsWindow(m_hWnd))
	{
		BITMAP bmp;
		m_pBmpImages[IBImgTypes::Normal]->GetBitmap(&bmp);

		SetWindowPos(NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
	}

	/*IBImages.pNormal = new CBitmap();
	IBImages.pNormal->LoadBitmap(nIDBmpNormal);

	if(nIDBmpHot) {
		IBImages.pHot = new CBitmap();
		IBImages.pHot->LoadBitmap(nIDBmpHot);
	} else if(IBImages.pHot) {
		IBImages.pHot->DeleteObject();
		delete IBImages.pHot;
	}

	if(nIDBmpPressed) {
		IBImages.pPressed = new CBitmap();
		IBImages.pPressed->LoadBitmap(nIDBmpPressed);
	} else if(IBImages.pPressed) {
		IBImages.pPressed->DeleteObject();
		delete IBImages.pPressed;
	}

	if(nIDBmpDisabled) {
		IBImages.pDisabled = new CBitmap();
		IBImages.pDisabled->LoadBitmap(nIDBmpDisabled);
	} else if(IBImages.pDisabled) {
		IBImages.pDisabled->DeleteObject();
		delete IBImages.pDisabled;
	}

	if(nIDBmpFocused) {
		IBImages.pFocused = new CBitmap();
		IBImages.pFocused->LoadBitmap(nIDBmpFocused);
	} else if(IBImages.pFocused) {
		IBImages.pFocused->DeleteObject();
		delete IBImages.pFocused;
	}*/
}

void CImgBtn::SetBitmapXY(int iImgType, LPPOINT lpPt)
{
	m_ptImages[iImgType].SetPoint(lpPt->x, lpPt->y);
}

BOOL CImgBtn::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	cs.dwExStyle |= WS_EX_TRANSPARENT;
	cs.style |= BS_OWNERDRAW;

	if(m_pBmpImages[IBImgTypes::Normal])
	{
		BITMAP bmp;
		m_pBmpImages[IBImgTypes::Normal]->GetBitmap(&bmp);

		cs.cx = bmp.bmWidth;
		cs.cy = bmp.bmHeight;
	}

	return CButton::PreCreateWindow(cs);
}

void CImgBtn::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	//TRACE(_T("SCL = %d\n"), SetClassLong(m_hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH)));

	//this->GetStyle(

	ModifyStyle(0, BS_OWNERDRAW);
	ModifyStyleEx(0, WS_EX_TRANSPARENT);

	CButton::PreSubclassWindow();
}

void CImgBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  Add your code to draw the specified item

	CDC dc;
	//RGBAQUAD *lpBits;
	//BITMAP bmp;
	//BITMAPINFO bi;
	CRect wrc;

	//RGBQUAD *lpMemBits;
	//RGBAQUAD *lpLblBits;


	GetWindowRect(wrc);
	GetParent()->ScreenToClient(wrc);
	//GetClientRect(wrc);

	dc.Attach(lpDrawItemStruct->hDC);

	//lpDrawItemStruct->rcItem



	CDC memDC;
	//CBitmap memBmp;
	HBITMAP hMemBmp;
	BITMAP bmpImg, bmpMem;
	BITMAPINFO bi;
	RGBQUAD *lpMemBits;
	RGBAQUAD *lpImgBits;
	CBitmap *pImg;

	//CBrush brColor(BEValue.GetRGB());
	//COLORREF crBorder = 0x999999;
	//CPen pnBorder(PS_SOLID, 1, crBorder);

	//CRect rcRGB (x, y,      x + 84, y + 20);
	//CRect rcRGBA(x, y + 19, x + 84, y + 39);
	CRect rcMem;

	//m_rcBigColors.UnionRect(rcRGB, rcRGBA);

	//CRect *prcClr[] = { &rcRGB, &rcRGBA };
	//CBitmap *pbmLbls[] = { &m_pBitmaps->m_bmLblRGB, &m_pBitmaps->m_bmLblRGBA };

	//HGDIOBJ hOldPen = (HGDIOBJ)pDC->SelectObject(pnBorder);
	//HGDIOBJ hOldBrush = (HGDIOBJ)pDC->SelectObject(GetStockObject(NULL_BRUSH));

	//pDC->Rectangle(rcRGB);
	//pDC->Rectangle(rcRGBA);

	//rcRGB.DeflateRect(1, 1);
	//rcRGBA.DeflateRect(1, 1);

	bool isFocus = GetFocus() && GetFocus()->m_hWnd == m_hWnd;

	pImg = (m_enmState == CPWGraphics::stateHot || isFocus) ? m_pBmpImages[IBImgTypes::Hot]
												 : m_pBmpImages[IBImgTypes::Normal];

	memDC.CreateCompatibleDC(&dc);
	
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biWidth = wrc.Width();
	bi.bmiHeader.biHeight = wrc.Height();
	bi.bmiHeader.biSizeImage = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight * 4;

	hMemBmp = CreateDIBSection(dc.GetSafeHdc(),
							   &bi,
							   DIB_RGB_COLORS,
							   (void**)&lpMemBits, NULL, NULL);

	GetObject(hMemBmp, sizeof(bmpMem), &bmpMem);

	HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(hMemBmp);





	//CDC *pDC = GetParent()->GetDC();

	if(!m_bUseBgColor)
	{
		CBineyeDlg *pPar = (CBineyeDlg*)GetParent();
		CDC bgDC;
		bgDC.CreateCompatibleDC(&dc);
		HGDIOBJ hOldBgBmp = bgDC.SelectObject( pPar->GetBgBmp() );

		//memDC.BitBlt(0, 0, wrc.Width()+5, wrc.Height()+0, &bgDC /*pDC*/ , wrc.left + (pPar->m_bDisplayLogo ? pPar->m_bmpLogo.bmWidth * 0: 0), wrc.top + 3, SRCCOPY);
		memDC.BitBlt(0, 0, wrc.Width(), wrc.Height(), &bgDC /*pDC*/ , wrc.left + (pPar->m_bDisplayLogo ? -pPar->m_bmpLogo.bmWidth : 0), wrc.top, SRCCOPY);

		bgDC.SelectObject(hOldBgBmp);
		bgDC.DeleteDC();
	}
	else
	{
		dc.FillSolidRect(wrc, m_crBgColor);
	}

	//HGDIOBJ hOldPen = (HGDIOBJ)dc.SelectObject(::GetStockObject(WHITE_PEN));

	//dc.MoveTo(0, 0);
	//dc.LineTo(10, 10);

	//dc.SelectObject(hOldPen);

	//GetParent()->ReleaseDC(pDC);



	//HGDIOBJ hOldMemPen = (HGDIOBJ)memDC.SelectObject(GetStockObject(NULL_PEN));
	//HGDIOBJ hOldMemBrush = (HGDIOBJ)memDC.GetCurrentBrush();

//	for(i = 0; i < 2; i++)
//	{
//		if(i == 0)
//		{
//			memDC.FillRect(rcMem, &brColor);
//		} else
//		{
//			CPWGraphics::DrawTransparentBg(&memDC, rcMem);
//			CPWGraphics::FillAlphaRect(&memDC, rcMem, BEValue.GetRGBA());
//		}

		//pbmLbls[i]->GetBitmap(&bmpLbl);

		/*m_pBmpImages[IBImgTypes::Normal]*/ pImg->GetBitmap(&bmpImg);

		ZeroMemory(&bi, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biPlanes = bmpImg.bmPlanes;
		bi.bmiHeader.biBitCount = bmpImg.bmBitsPixel;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biWidth = bmpImg.bmWidth;
		bi.bmiHeader.biHeight = bmpImg.bmHeight;
		bi.bmiHeader.biSizeImage = bmpImg.bmWidthBytes * bmpImg.bmHeight;

		lpImgBits = (RGBAQUAD*)GlobalAlloc(GPTR, bmpImg.bmWidthBytes * bmpImg.bmHeight);
		GetDIBits(dc.GetSafeHdc(),
			*pImg /*m_pBmpImages[IBImgTypes::Normal]*/,
				  0,
				  bmpImg.bmHeight,
				  lpImgBits,
				  &bi,
				  DIB_RGB_COLORS);

		int x, y, imgpos, mempos;

		for(x = 0; x < bmpImg.bmWidth; x++)
		{
			//continue;
			for(y = 0; y < bmpImg.bmHeight; y++)
			{
				imgpos = BITS_XY(x, y, bmpImg.bmWidth, bmpImg.bmHeight);
				mempos = BITS_XY(x, y, bmpMem.bmWidth, bmpMem.bmHeight);

				if(lpImgBits[imgpos].rgbaAlpha > 0)
				{
					lpMemBits[mempos].rgbRed   = ALPHABYTE(lpImgBits[imgpos].rgbaRed, lpMemBits[mempos].rgbRed, lpImgBits[imgpos].rgbaAlpha);
					lpMemBits[mempos].rgbGreen = ALPHABYTE(lpImgBits[imgpos].rgbaGreen, lpMemBits[mempos].rgbGreen, lpImgBits[imgpos].rgbaAlpha);
					lpMemBits[mempos].rgbBlue  = ALPHABYTE(lpImgBits[imgpos].rgbaBlue, lpMemBits[mempos].rgbBlue, lpImgBits[imgpos].rgbaAlpha);
				}
			}
		}


	/*if(GetFocus() && GetFocus()->m_hWnd == m_hWnd)
	{
		CRect rcFocus(lpDrawItemStruct->rcItem);
		rcFocus.DeflateRect(4, 4);
		memDC.DrawFocusRect(rcFocus);
	}*/

		dc.BitBlt(0, 0, bmpImg.bmWidth, bmpImg.bmHeight,
					&memDC, 0, 0, SRCCOPY);

		GlobalFree(lpImgBits);
	//}

	memDC.SelectObject(hOldMemBmp);
	//memDC.SelectObject(hOldMemPen);
	//memDC.SelectObject(hOldMemBrush);

	memDC.DeleteDC();
	DeleteObject((HGDIOBJ)hMemBmp);

	
	//pDC->SelectObject(hOldPen);
	//pDC->SelectObject(hOldBrush);

	//if(bReleaseDC) ReleaseDC(pDC);
//}



//----------------------------------------------------------



	dc.Detach();
	return;


/*
	CBitmap *pCurBmp = dc.GetCurrentBitmap();

	pCurBmp->GetBitmap(&bmp);

	// load CircleMask bits
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bmp.bmWidth;
	bi.bmiHeader.biHeight = bmp.bmHeight;
	bi.bmiHeader.biPlanes = bmp.bmPlanes;
	bi.bmiHeader.biBitCount = bmp.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;

	lpBits = (RGBAQUAD*)GlobalAlloc(GPTR, bmp.bmWidthBytes * bmp.bmHeight);

	GetDIBits(dc.GetSafeHdc(),
			  *pCurBmp,
			  0,
			  bmp.bmHeight,
			  (LPVOID)lpBits,
			  &bi,
			  DIB_RGB_COLORS);

	int x, y, pos;

	for(x = wrc.left; x <= wrc.right; x++)
	{
		for(y = wrc.top; y <= wrc.bottom; y++)
		{
			pos = BITS_XY(x, y, bmp.bmWidth, bmp.bmHeight);

			//TRACE(_T("abgr(x, y) = 0x%08X\n"), (DWORD*)&lpBits[pos]);

			lpBits[pos].rgbaRed = 0xFF;
			lpBits[pos].rgbaBlue = 0xFF;
			lpBits[pos].rgbaGreen = 0xFF;
		}
	}

	SetDIBits(dc.GetSafeHdc(),
			  *pCurBmp,
			  0,
			  bmp.bmHeight,
			  (LPVOID)lpBits,
			  &bi,
			  DIB_RGB_COLORS);

	GlobalFree(lpBits);

	dc.Detach();*/
}

HBRUSH CImgBtn::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  Change any attributes of the DC here


	pDC->SetBkMode(TRANSPARENT);

	// TODO:  Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

void CImgBtn::DoTME(DWORD dwFlags /*= TME_HOVER | TME_LEAVE*/)
{
	TRACKMOUSEEVENT tme;

	tme.cbSize = sizeof(tme);
	tme.dwFlags = dwFlags;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = 10;

	TrackMouseEvent(&tme);
}
BOOL CImgBtn::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	if(message == WM_MOUSEHOVER)
	{
		if(!m_bIsHOver)
		{
			m_enmState = CPWGraphics::stateHot;
			//SetBkColor(RGB(0x0,0x0,0xFF));
			RedrawWindow();
			m_bIsHOver = TRUE;

		}
	}
	else if(message == WM_MOUSELEAVE)
	{
		m_enmState = CPWGraphics::stateNormal;
		//SetBkColor(0xFFC0C0);
		RedrawWindow();
		m_bIsHOver = FALSE;
	}
	else if(message == WM_LBUTTONDOWN)
	{
		m_enmState = CPWGraphics::statePressed;
		//SetBkColor(RGB(0xff,0x00,0x00));
		RedrawWindow();
	}
	else if(message == WM_LBUTTONUP)
	{
		m_enmState = CPWGraphics::stateHot;
		//SetBkColor(RGB(0x0,0x0,0xFF));
		RedrawWindow();
	}
	else if(message == WM_MOUSEMOVE)
		DoTME();

	return CButton::OnWndMsg(message, wParam, lParam, pResult);
}
