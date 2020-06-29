// PWControls.cpp: implementation of the CPWControls class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "ZKR_BioPresentation.h"
#include "PWControls.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPWControls::CPWControls()
{
}

CPWControls::~CPWControls()
{
}

CBitmap* CPWControls::BmpChangeColor(HBITMAP hBitmap, COLORREF crFrom, COLORREF crTo)
{
	CDC dcBmp,
		dcMask;
	CBitmap bmBmp,
			bmMask;
	CBrush br;
	BITMAP bmp;

	if(!hBitmap) return false;

	bmBmp.Attach(hBitmap);
	bmBmp.GetObject(sizeof(bmp), &bmp);

	bmMask.CreateBitmap(bmp.bmWidth, bmp.bmHeight, 1, 1, NULL);

	dcBmp.CreateCompatibleDC(NULL);
	dcMask.CreateCompatibleDC(NULL);
	br.CreateSolidBrush(crTo);

	CBitmap *bmpBmpOld = dcBmp.SelectObject(&bmBmp);
	CBitmap *bmpMaskOld = dcMask.SelectObject(&bmMask);
	CBrush *brOld = dcBmp.SelectObject(&br);

	if(crFrom = -1)	crFrom = dcBmp.GetPixel(0, 0);

	dcBmp.SetBkColor(crFrom);
	dcMask.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dcBmp, 0, 0, SRCCOPY);
	dcBmp.SetBkColor(0xFFFFFF);
	dcBmp.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dcMask, 0, 0, 0xE20746); //00E20746h = DSPDxax

	dcBmp.SelectObject(bmpBmpOld);
	dcBmp.SelectObject(brOld);
	dcMask.SelectObject(bmpMaskOld);
	dcBmp.DeleteDC();
	dcMask.DeleteDC();
	br.DeleteObject();
	bmMask.DeleteObject();

	bmBmp.Detach();

	return((CBitmap*)hBitmap);
}

BOOL CPWControls::DrawWndRoundRect(CWnd *pWnd, 
								   CDC *pParenrDC, 
								   int nRoundX, 
								   int nRoundY, 
								   COLORREF colrefBkDraw,
								   BOOL bDrawBorder /*= FALSE*/,
								   int nBorderSize /*= 1*/,
								   COLORREF colrefBorderColor /*= 0x000000*/)
{
	CRect wrc;
	CRgn rgn;
	CBrush objBkBrush;

	pWnd->GetWindowRect(wrc);
	pWnd->GetParent()->ScreenToClient(wrc);
	wrc.InflateRect(nRoundX / 2, nRoundY / 2);
	rgn.CreateRoundRectRgn(wrc.left, wrc.top, wrc.right, wrc.bottom, nRoundX, nRoundY);
	objBkBrush.CreateSolidBrush(colrefBkDraw);
	pParenrDC->FillRgn(&rgn, &objBkBrush);

	if(bDrawBorder)
	{
		objBkBrush.DeleteObject();
		objBkBrush.CreateSolidBrush(colrefBorderColor);
		pParenrDC->FrameRgn(&rgn, &objBkBrush, nBorderSize, nBorderSize);
	}

	rgn.DeleteObject();
	objBkBrush.DeleteObject();
	return(true);
}

int CPWControls::TopHeight(CWnd *pWnd)
{
	CRect rc;

	pWnd->GetWindowRect(rc);

	if(IsWindow(*pWnd->GetParent()))
		pWnd->GetParent()->ScreenToClient(rc);
	
	return(rc.bottom);
}

int CPWControls::LeftWidth(CWnd *pWnd)
{
	CRect rc;

	pWnd->GetWindowRect(rc);

	if(IsWindow(*pWnd->GetParent()))
		pWnd->GetParent()->ScreenToClient(rc);
	
	return(rc.right);
}

const int CPWControls::bbfBitmapText(0x1);
const int CPWControls::bbfTextBitmap(0x2);

const int CPWControls::bbfHorAlignLeft(0x4);
const int CPWControls::bbfHorAlignCenter(0x8);
const int CPWControls::bbfHorAlignRight(0x10);

const int CPWControls::bbfVertAlignTop(0x20);
const int CPWControls::bbfVertAlignCenter(0x40);
const int CPWControls::bbfVertAlignBottom(0x80);

const int CPWControls::bbfAlignCenter(bbfHorAlignCenter | bbfVertAlignCenter);

const int CPWControls::bbfUseBmpID(0x100);
const int CPWControls::bbfUseBgID(0x200);
const int CPWControls::bbfUseBackColor(0x400);
const int CPWControls::bbfUseForeColor(0x800);
const int CPWControls::bbfUseMarginBorder(0x1000);
const int CPWControls::bbfUseBmpTextMargin(0x2000);
					 
const int CPWControls::bbfUseColors(bbfUseBackColor | bbfUseForeColor);
const int CPWControls::bbfUseMargins(bbfUseMarginBorder | bbfUseBmpTextMargin);
const int CPWControls::bbfUseResources(bbfUseBmpID | bbfUseBgID);

const int CPWControls::bbfBgHorAlignLeft(0x4000);
const int CPWControls::bbfBgHorAlignCenter(0x8000);
const int CPWControls::bbfBgHorAlignRight(0x10000);

const int CPWControls::bbfBgVertAlignTop(0x20000);
const int CPWControls::bbfBgVertAlignCenter(0x40000);
const int CPWControls::bbfBgVertAlignBottom(0x80000);

const int CPWControls::bbfBgAlignCenter(bbfBgHorAlignCenter | bbfBgVertAlignCenter);
const int CPWControls::bbfBgAlignStretch(0x100000);
const int CPWControls::bbfBgAlignTiled(0x200000);

const int CPWControls::bbfTransparentBmp(0x400000);
const int CPWControls::bbfTransparentBg(0x800000);
const int CPWControls::bbfTransparentResources(bbfTransparentBmp | bbfTransparentBg);

const int CPWControls::bbfSetBmp(0x1000000);

HBITMAP CPWControls::ButtonBitmap(CButton *button,
					 UINT nIDSmallBmp,
					 UINT nIDBg,
					 COLORREF crBack,
					 COLORREF crFore,
					 COLORREF crTansparent,
					 CFont *fnt,
					 int iBorderMargin,
					 int iBmpTextMargin,
					 UINT uFlags)
{
	CBitmap *oldbmp, 
			*oldbmp2, 
			*oldbmp3,
			 crbmp,
			 inbmp, 
			 bgbmp;
	CBrush   brback, 
		    *brold;
	CRect    rcbtn;
	CString  btntxt;
	CDC		 fdc, 
			*bdc = button->GetDC(), 
			 indc,
			 bgdc;
	BOOL	 bCrFnt = false;
	BITMAP	 bmp;
	HBITMAP	 hBmpRet;


	if(!(uFlags & bbfUseBackColor))
		crBack = GetSysColor(COLOR_BTNFACE);

	if(!(uFlags & bbfUseForeColor))
		crFore = GetSysColor(COLOR_BTNTEXT);

	if(!(uFlags & bbfUseMarginBorder))
		iBorderMargin = 2;

	if(!(uFlags & bbfUseBmpTextMargin))
		iBmpTextMargin = 4;

	if(!fnt)
	{
		//bCrFnt = true;
		//fnt = new CFont();
		//fnt->CreateStockObject(DEFAULT_GUI_FONT);

		fnt = button->GetFont();
	}

	button->GetWindowText(btntxt);
	button->GetWindowRect(&rcbtn);
	rcbtn.right -= 2;
	rcbtn.bottom -= 2;

	fdc.CreateCompatibleDC(bdc);

	if(uFlags & bbfUseBmpID)
	{
		indc.CreateCompatibleDC(bdc);
		inbmp.LoadBitmap(nIDSmallBmp);
	}

	if(uFlags & bbfUseBgID)
	{
		bgdc.CreateCompatibleDC(bdc);
		bgbmp.LoadBitmap(nIDBg);
	}
	
	hBmpRet = CreateCompatibleBitmap(bdc->m_hDC, rcbtn.Width(), rcbtn.Height());
	
	crbmp.Attach((HGDIOBJ)hBmpRet);

	button->ReleaseDC(bdc);

	oldbmp = fdc.SelectObject(&crbmp);

	if(uFlags & bbfUseBmpID)
		oldbmp2 = indc.SelectObject(&inbmp);

	if(uFlags & bbfUseBgID)
		oldbmp3 = bgdc.SelectObject(&bgbmp);

	brback.CreateSolidBrush(crBack);
	brold = fdc.SelectObject(&brback);

	fdc.SetBkColor(crBack);
	fdc.SetTextColor(crFore);
	fdc.SetBkMode(TRANSPARENT);

	fdc.FillSolidRect(0, 0, rcbtn.Width(), rcbtn.Height(), crBack);

	CPoint textpos, bgpos, bmppos;

	if(uFlags & bbfUseBgID)
	{
		if(uFlags & bbfTransparentBg)
			BmpChangeColor((HBITMAP)bgbmp.m_hObject, -1, crBack);

		bgbmp.GetObject(sizeof(bmp), &bmp);

		if(uFlags & bbfBgAlignTiled)
		{
			int iBgDrawWidth = 0;
			int iBgDwawHeight;

			while(iBgDrawWidth < rcbtn.Width())
			{
				iBgDwawHeight = 0;
				while(iBgDwawHeight < rcbtn.Height())
				{
					fdc.BitBlt(iBgDrawWidth, 
							   iBgDwawHeight, 
							   (rcbtn.Width() - iBgDrawWidth) < bmp.bmWidth ? rcbtn.Width() - iBgDrawWidth : bmp.bmWidth,
							   (rcbtn.Height() - iBgDwawHeight) < bmp.bmHeight ? rcbtn.Height() - iBgDwawHeight : bmp.bmHeight,
							   &bgdc,
							   0,
							   0,
							   SRCCOPY);

					iBgDwawHeight += bmp.bmHeight;
				}

				iBgDrawWidth += bmp.bmWidth;
			}
		}
		else if(uFlags & bbfBgAlignStretch)
		{
			fdc.SetStretchBltMode(HALFTONE);
			fdc.StretchBlt(0, 
						   0, 
						   rcbtn.Width(), 
						   rcbtn.Height(), 
						   &bgdc, 
						   0, 
						   0, 
						   bmp.bmWidth, 
						   bmp.bmHeight, 
						   SRCCOPY);
		}
		else
		{
			if(uFlags & bbfBgHorAlignLeft)
				bgpos.x = 0;
			else if(uFlags & bbfBgHorAlignRight)
				bgpos.x = rcbtn.Width() - bmp.bmWidth;
			else /* bbfBgHorAlignCenter */
				bgpos.x = rcbtn.Width() / 2 - bmp.bmWidth / 2;

			if(uFlags & bbfBgVertAlignTop)
				bgpos.y = 0;
			else if(uFlags & bbfBgVertAlignBottom)
				bgpos.y = rcbtn.Height() - bmp.bmHeight;
			else /* bbfBgVertAlignCenter */
				bgpos.y = rcbtn.Height() / 2 - bmp.bmHeight / 2;

			fdc.BitBlt(bgpos.x, bgpos.y, bmp.bmWidth, bmp.bmHeight, &bgdc, 0, 0, SRCCOPY);
		}
	}

	CFont *oldfnt = fdc.SelectObject(fnt);

	CSize stxt = btntxt.IsEmpty() ? CSize(0, 0) : fdc.GetTextExtent(btntxt);

	if(uFlags & bbfUseBmpID)
	{
		inbmp.GetObject(sizeof(bmp), &bmp);
		if(uFlags & bbfBitmapText)
		{
			if(uFlags & bbfHorAlignLeft)
				bmppos.x = iBorderMargin;
			else if(uFlags & bbfHorAlignRight)
				bmppos.x = rcbtn.Width() - bmp.bmWidth - iBmpTextMargin - stxt.cx - iBorderMargin;
			else /* bbfHorAlignCenter */
				bmppos.x = rcbtn.Width() / 2 - (bmp.bmWidth + iBmpTextMargin + stxt.cx) / 2;

			textpos.x = bmppos.x + bmp.bmWidth + iBmpTextMargin;
		}
		else /* bbfTextBitmap */
		{ 
			if(uFlags & bbfHorAlignLeft)
				textpos.x = iBorderMargin;
			else if(uFlags & bbfHorAlignRight)
				textpos.x = rcbtn.Width() - bmp.bmWidth - iBmpTextMargin - stxt.cx - iBorderMargin;
			else /* bbfHorAlignCenter */
				textpos.x = rcbtn.Width() / 2 - (bmp.bmWidth + iBmpTextMargin + stxt.cx) / 2;

			bmppos.x = textpos.x + stxt.cx + iBmpTextMargin;
		}

		if(bmp.bmHeight > stxt.cy)
		{
			if(uFlags & bbfVertAlignTop)
				bmppos.y = iBorderMargin;
			else if(uFlags & bbfVertAlignBottom)
				bmppos.y = rcbtn.Height() - bmp.bmHeight - iBorderMargin;
			else /* bbfVertAlignCenter */
				bmppos.y = rcbtn.Height() / 2 - bmp.bmHeight / 2;

			textpos.y = bmppos.y + bmp.bmHeight / 2 - stxt.cy / 2;
		}
		else
		{
			if(uFlags & bbfVertAlignTop)
				textpos.y = iBorderMargin;
			else if(uFlags & bbfVertAlignBottom)
				textpos.y = rcbtn.Height() - stxt.cy - iBorderMargin;
			else /* bbfVertAlignCenter */
				textpos.y = rcbtn.Height() / 2 - stxt.cy / 2;

			bmppos.y = textpos.y + stxt.cy / 2 - bmp.bmHeight / 2;
		}

		if(uFlags & bbfTransparentBmp)
			TransparentBlt(fdc.m_hDC, bmppos.x, bmppos.y, bmp.bmWidth, bmp.bmHeight,
			indc.m_hDC, 0, 0, bmp.bmWidth, bmp.bmHeight, crTansparent == -1 ? indc.GetPixel(0, 0) : crTansparent);
		else
			fdc.BitBlt(bmppos.x, bmppos.y, bmp.bmWidth, bmp.bmHeight, &indc, 0, 0, SRCCOPY);
	}
	else if(!btntxt.IsEmpty())
	{
		if(uFlags & bbfHorAlignLeft)
			textpos.x = iBorderMargin;
		else if(uFlags & bbfHorAlignRight)
			textpos.x = rcbtn.Width() - stxt.cx - iBorderMargin;
		else /* bbfHorAlignCenter */
			textpos.x = rcbtn.Width() / 2 - stxt.cx / 2;

		if(uFlags & bbfVertAlignTop)
			textpos.y = iBorderMargin;
		else if(uFlags & bbfVertAlignBottom)
			textpos.y = rcbtn.Height() - stxt.cy - iBorderMargin;
		else /* bbfVertAlignCenter */
			textpos.y = rcbtn.Height() / 2 - stxt.cy / 2;
	}

	if(!btntxt.IsEmpty()) fdc.TextOut(textpos.x, textpos.y, btntxt);

	fdc.SelectObject(oldfnt);
	fdc.SelectObject(oldbmp);
	fdc.SelectObject(brold);
	fdc.DeleteDC();
	brback.DeleteObject();

	if(bCrFnt) delete fnt;

	if(uFlags & bbfUseBmpID)
	{
		indc.SelectObject(oldbmp2);
		indc.DeleteDC();
		inbmp.DeleteObject();
	}

	if(uFlags & bbfUseBgID)
	{
		bgdc.SelectObject(oldbmp3);
		bgdc.DeleteDC();
		bgbmp.DeleteObject();
	}

	if(uFlags & bbfSetBmp)
	{
		HBITMAP oldsetbmp = button->SetBitmap(hBmpRet);

		if(oldsetbmp)
			DeleteObject(oldsetbmp);
	}

	crbmp.Detach();

	return hBmpRet;
}

CPoint CPWControls::GetClientWndPos(CWnd* pWnd)
{
	CRect wrc;

	pWnd->GetWindowRect(wrc);
	if(pWnd->GetParent())
		pWnd->GetParent()->ScreenToClient(wrc);

	return CPoint(wrc.left, wrc.top);
}

CSize CPWControls::GetWndSize(CWnd* pWnd)
{
	CRect wrc;

	pWnd->GetWindowRect(wrc);
	return CSize(wrc.Width(), wrc.Height());
}
