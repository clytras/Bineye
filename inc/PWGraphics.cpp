
#include ".\pwgraphics.h"


int	CPWGraphics::CBtnStateEx::Normal(0x1);
int	CPWGraphics::CBtnStateEx::Pressed(0x2);
int	CPWGraphics::CBtnStateEx::Hot(0x4);
int	CPWGraphics::CBtnStateEx::Default(0x8);
int	CPWGraphics::CBtnStateEx::Disable(0x10);
int	CPWGraphics::CBtnStateEx::Focused(0x20);

int CPWGraphics::CAlignment::Left(1);
int CPWGraphics::CAlignment::Right(2);
int CPWGraphics::CAlignment::Center(3);
int CPWGraphics::CAlignment::HorAbsolute(4);
int CPWGraphics::CAlignment::Top(1);
int CPWGraphics::CAlignment::Bottom(2);
int CPWGraphics::CAlignment::Middle(3);
int CPWGraphics::CAlignment::VerAbsolute(4);


CPWGraphics::CPWGraphics(void)
{
}

CPWGraphics::~CPWGraphics(void)
{
}

//BOOL CPWGraphics::IsThemeUsed(void)
//{
//	BOOL bRet;
//
//	HRSRC hRes = FindResourceEx(AfxGetApp()->m_hInstance, 
//						RT_MANIFEST, 
//						MAKEINTRESOURCE(CREATEPROCESS_MANIFEST_RESOURCE_ID), 
//						MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
//
//	CString strManifestFile = CPWIO::GetModuleFilePath() + _T(".manifest");
//
//	bRet = IsAppThemed() &&
//		   IsThemeActive() &&
//		   (hRes != NULL || CPWIO::FileExist(strManifestFile));
//
//	return(bRet);

//}

void CPWGraphics::DrawLine(CWnd *pWnd, 
						   CDC *pDC, 
						   COLORREF cr, 
						   int x, 
						   int y, 
						   int cx /*= -1*/, 
						   int nSize /*= 1*/)
{
	CRect rc;
	CPen pen;

	if(cx == -1)
	{
		pWnd->GetClientRect(rc);
		cx = rc.Width();
	}

	pen.CreatePen(PS_SOLID, nSize, cr);
	CPen *oldPen = (CPen*)pDC->SelectObject(pen);

	pDC->MoveTo(x, y);
	pDC->LineTo(x + cx, y);
}

void CPWGraphics::DrawLineEx(CDC* pDC, 
							 COLORREF cr, 
							 int x1, 
							 int y1, 
							 int x2, 
							 int y2, 
							 int nSize /*= 1*/,
							 int nPenStyle /*= PS_SOLID*/)
{
	CPen pen;

	pen.CreatePen(PS_SOLID, nSize, cr);
	pDC->SelectObject(pen);

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);
}

COLORREF CPWGraphics::Blend(COLORREF crFrom, COLORREF crTo, double percent)
{
	int r, g, b;
	double p = double(percent) / 100.0;

	r = int(p * GetRValue(crTo) + (1 - p) * GetRValue(crFrom));
	g = int(p * GetGValue(crTo) + (1 - p) * GetGValue(crFrom));
	b = int(p * GetBValue(crTo) + (1 - p) * GetBValue(crFrom));

	return RGB(r, g, b);
}

UINT CPWGraphics::GABS(const int& nmbr)
{
	return (nmbr>0)?nmbr:-nmbr;
}

COLORREF CPWGraphics::DifColor(COLORREF color, UINT nDist)
{
	return RGB(
				GABS(GetRValue(color)-nDist),
				GABS(GetGValue(color)-nDist),
				GABS(GetBValue(color)-nDist)
				);
}

COLORREF CPWGraphics::OrColor(COLORREF colFore, COLORREF colBK, UINT nForePercent)
{
	return RGB (
	( GetRValue(colBK)*100 + ( nForePercent*(GetRValue(colFore)-GetRValue(colBK)) ) ) /100,
	( GetGValue(colBK)*100 + ( nForePercent*(GetGValue(colFore)-GetGValue(colBK)) ) ) /100,
	( GetBValue(colBK)*100 + ( nForePercent*(GetBValue(colFore)-GetBValue(colBK)) ) ) /100 );
}

void CPWGraphics::GradientFillDC(CDC* pDC,
								 int x,
								 int y,
								 int cx, 
								 int cy, 
								 COLORREF crFrom, 
								 COLORREF crTo, 
								 int iWayDraw /* = 1 /* (==1) = x way, (!=1) = y way */)
{
	int i, iStartPos, iEndPos, iStStartPos, iStEndPos, iSize;
	CPen pn, *spn;
	CWnd *dw = CWnd::GetDesktopWindow();
	BOOL bCrFnt = false;

	iWayDraw = iWayDraw == 1 ? 1 : 0;

	iStartPos   = iWayDraw ? x : y;
	iEndPos     = iWayDraw ? x + cx : y + cy;
	iStStartPos = iWayDraw ? y : x;
	iStEndPos   = iWayDraw ? y + cy : x + cx;
	iSize       = iWayDraw ? cx : cy;

	spn = pDC->GetCurrentPen();

	for(i = iStartPos; i <= iEndPos; i++)
	{
		pn.CreatePen(PS_SOLID, 1, Blend(crFrom, crTo, (i - iStartPos) * 100 / iSize));
		pDC->SelectObject(pn);
		if(iWayDraw)
		{
			pDC->MoveTo(i, iStStartPos);
			pDC->LineTo(i, iStEndPos);
		}
		else
		{
			pDC->MoveTo(iStStartPos, i);
			pDC->LineTo(iStEndPos, i);
		}
		pn.DeleteObject();
	}
		
	//pDC->SelectObject(spn);
	//pDC->SelectObject(oldbmp);
}


BOOL CPWGraphics::BlitReplaceBitmapColor(HBITMAP hbm, COLORREF rgbOld, COLORREF rgbNew, HDC hdcBm /*= NULL*/)
{

	BOOL bDone = FALSE;
	HDC hdcBmSel, hdcMaskBmSel;
	HBITMAP hbmMask;
	HGDIOBJ hbmDcOrig, hbmMaskDcOrig;
	COLORREF rgbDcOrigBack, rgbDcOrigFore;

	hdcBmSel = hdcBm;
	hdcMaskBmSel = NULL;
	hbmMask = NULL;
	hbmDcOrig = hbmMaskDcOrig = NULL;
	rgbDcOrigBack, rgbDcOrigFore = MAXDWORD;


	if(hdcBmSel == NULL)
	{
		hdcBmSel = ::CreateCompatibleDC(NULL);

		hbmDcOrig = ::SelectObject(hdcBmSel, hbm);
	}

	// Prepare monochrome mask bitmap from the given 'hbm' so that its
	// pixels with 'rgbOld' color will be white pixels of the mask

	BITMAP bm;
	::GetObject(hbm, sizeof(BITMAP), &bm);
	hdcMaskBmSel = ::CreateCompatibleDC(NULL);
	hbmMask = ::CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	hbmMaskDcOrig = ::SelectObject(hdcMaskBmSel, hbmMask);

	rgbDcOrigBack = ::SetBkColor(hdcBmSel, rgbOld);
	::BitBlt(hdcMaskBmSel, 0, 0, bm.bmWidth, bm.bmHeight, hdcBmSel, 0, 0, SRCCOPY);

	::SetBkColor(hdcBmSel, (rgbNew ^ rgbOld));
	rgbDcOrigFore = ::SetTextColor(hdcBmSel, 0x00000000);
	::BitBlt(hdcBmSel, 0, 0, bm.bmWidth, bm.bmHeight, hdcMaskBmSel, 0, 0, SRCINVERT);
	bDone = TRUE;


	if(hdcBm == NULL)
	{
		if(hdcBmSel != NULL)
		{
			if(hbmDcOrig != NULL) ::SelectObject(hdcBmSel, hbmDcOrig);
			::DeleteDC(hdcBmSel);
		}
	}
	else
	{
		if(rgbDcOrigBack != MAXDWORD) ::SetBkColor(hdcBm, rgbDcOrigBack);
		if(rgbDcOrigFore != MAXDWORD) ::SetTextColor(hdcBm, rgbDcOrigFore);
	}

	if(hdcMaskBmSel != NULL)
	{
		if(hbmMaskDcOrig != NULL) ::SelectObject(hdcMaskBmSel, hbmMaskDcOrig);
		::DeleteDC(hdcMaskBmSel);
	
		if(hbmMask != NULL) ::DeleteObject(hbmMask);
	}

	return bDone;
}

CBitmap* CPWGraphics::BmpChangeColor(CBitmap* pBmp, COLORREF crFrom, COLORREF crTo)
{
	CDC dcBmp,
		dcMask;
	CBitmap /*bmBmp,*/
			bmMask;
	CBrush br;
	BITMAP bmp;

	//if(!hBitmap) return false;

	//bmBmp.Attach(hBitmap);
	//bmBmp.GetObject(sizeof(bmp), &bmp);
	pBmp->GetObject(sizeof(bmp), &bmp);

	bmMask.CreateBitmap(bmp.bmWidth, bmp.bmHeight, 1, 1, NULL);

	dcBmp.CreateCompatibleDC(NULL);
	dcMask.CreateCompatibleDC(NULL);
	br.CreateSolidBrush(crTo);

	CBitmap *bmpBmpOld = dcBmp.SelectObject(pBmp /*&bmBmp*/);
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

	//bmBmp.Detach();

	return pBmp; //((CBitmap*)hBitmap);
}


CString CPWGraphics::ColRef2HTML(COLORREF colrefColor)
{
	CString strRet;

	strRet.Format(_T("#%.2x%.2x%.2x"),
		GetRValue(colrefColor),
		GetGValue(colrefColor),
		GetBValue(colrefColor));

	return strRet;
}

COLORREF CPWGraphics::OffsetColor(COLORREF colrefColor, int iOffset)
{
	int colRed, colGreen, colBlue,
	    colR,   colG,     colB;

	colR = (colrefColor & 0xFF);
	colG = ((colrefColor & 0xFF00) / 0x100);
	colB = ((colrefColor & 0xFF0000) / 0x10000);

	colRed   = COLORREF(iOffset + colR);
	colGreen = COLORREF(iOffset + colG);
	colBlue  = COLORREF(iOffset + colB);

	if(colRed > 0xFF)   colRed = 0xFF;
	if(colRed < 0)      colRed = 0;
	if(colGreen > 0xFF) colGreen = 0xFF;
	if(colGreen < 0)    colGreen = 0;
	if(colBlue > 0xFF)  colBlue = 0xFF;
	if(colBlue < 0)     colBlue = 0;

	return COLORREF(RGB(colRed, colGreen, colBlue));
}

void CPWGraphics::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, COLORREF colrefColor)
{
	HPEN hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, colrefColor);
	hOldPen = (HPEN)SelectObject(hDC, (HGDIOBJ)hPen);
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
	SelectObject(hDC, (HGDIOBJ)hOldPen);
	DeleteObject(hPen);
}

void CPWGraphics::DrawVGradient(HDC hDC, COLORREF colrefEndColor, COLORREF colrefStartColor, int x, int y, int x2, int y2)
{
	float dR, dG, dB;
	float sR, sG, sB;
	float eR, eG, eB;
	int i, ys;

	sR = float( colrefStartColor & 0xFF);
	sG = float((colrefStartColor / 0x100) & 0xFF);
	sB = float((colrefStartColor & 0xFF0000) / 0x10000); // + float((colrefStartColor & 0xFF0000) % 0x10000);
	eR = float( colrefEndColor & 0xFF);
	eG = float((colrefEndColor / 0x100) & 0xFF);
	eB = float((colrefEndColor & 0xFF0000) / 0x10000); // + float((colrefEndColor & 0xFF0000) % 0x10000);
	dR = float((sR - eR) / y2); // + float((sR - eR) % y2);
	dG = float((sG - eG) / y2); // + float((sG - eG) % y2);
	dB = float((sB - eB) / y2); // + float((sB - eB) % y2);

	for(i = y /*0*/; i <= y2; i++)
	{
		CPWGraphics::DrawLine(hDC, x, /*y +*/ i, x2, /*y +*/ i, RGB(int(eR + (i * dR)), int(eG + (i * dG)), int(eB + (i * dB))));
	}
}

void CPWGraphics::DrawPlasticRect(HDC hDC, 
								  LPRECT prc,
								  COLORREF color)
{
	int l = prc->left,
		r = prc->right,
		t = prc->top,
		b = prc->bottom;

	COLORREF colTmp = color;

	colTmp = OffsetColor(colTmp, 0x8);

	DrawVGradient(hDC, 
			OffsetColor(colTmp,  0xF), 
			OffsetColor(colTmp, -0xF), 
			l /*1*/, t /*1*/, r /*cx - 2*/, b-1 /*cy - 3*/);


	DrawVGradient(hDC, 
			OffsetColor(colTmp,  0x15), 
			OffsetColor(colTmp, -0x15), 
			l /*1*/, t+1 /*2*/, l+1 /*2*/, b-4 /*cy - 5*/);

	DrawVGradient(hDC, 
			OffsetColor(colTmp, -0x5), 
			OffsetColor(colTmp, -0x20), 
			r-1 /*cx - 2*/, t+1 /*2*/, r-2 /*cx - 3*/, b-2 /*3*/ /*cy - 5*/);

	colTmp = OffsetColor(colTmp, -0x60);


	//DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
	//DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
	//DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
	//DrawLine(hDC, r-1 /*cx - 1*/, t+1 /*1*/, r-1 /*cx - 1*/, b-1 /*cy - 1*/, colTmp);


	//SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
	//SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
	//SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
	//SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

	//colTmp = OffsetColor(color, -0x15);

	//SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
	//SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

	//SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
	//SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

	//SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
	//SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

	//SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
	//SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);

	//DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(color, 0x15));
	//DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(color, -0x25));

}


void CPWGraphics::DrawPlastic(HDC hDC, 
							  CPWGraphics::BtnState state, 
							  LPRECT prc, 
							  COLORREF colBk /*= -1*/, 
							  COLORREF colHiLi /*= -1*/,
							  COLORREF colHot /*= -1*/,
							  BOOL bBkOnly /*= FALSE*/)
{
	int l = prc->left,
		r = prc->right,
		t = prc->top,
		b = prc->bottom;

	if(colBk == -1) colBk = GetSysColor(COLOR_3DFACE);
	if(colHiLi == -1) colHiLi = GetSysColor(COLOR_HIGHLIGHT);
	if(colHot == -1) colHot = GetSysColor(COLOR_HOTLIGHT);

	//m_colrefBkColor = 0x00D8E9EC;

	COLORREF colTmp = colBk;

	//if(!IsWindowEnabled()) m_enmState = stateDisable;

	switch(state)
	{
		case stateNormal:
		case stateDefault:
			colTmp = OffsetColor(colTmp, 0x8);

			DrawVGradient(hDC, 
					OffsetColor(colTmp,  0xF), 
					OffsetColor(colTmp, -0xF), 
					l+1 /*1*/, t+1 /*1*/, r-2 /*cx - 2*/, b-3 /*cy - 3*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp,  0x15), 
					OffsetColor(colTmp, -0x15), 
					l+1 /*1*/, t+2 /*2*/, l+2 /*2*/, b-5 /*cy - 5*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp, -0x5), 
					OffsetColor(colTmp, -0x20), 
					r-2 /*cx - 2*/, t+2 /*2*/, r-3 /*cx - 3*/, b-3 /*cy - 5*/);

			colTmp = OffsetColor(colTmp, -0x60);


			DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
			DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
			DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
			DrawLine(hDC, r-1 /*cx - 1*/, t+1 /*1*/, r-1 /*cx - 1*/, b-1 /*cy - 1*/, colTmp);


			SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

			colTmp = OffsetColor(colBk, -0x15);

			SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

			SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);

			DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colBk, 0x15));
			DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colBk, -0x25));

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

			if(!bBkOnly)
			{
				colTmp = colHiLi; //GetSysColor(COLOR_HIGHLIGHT);
				DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colTmp, 0x15));
				DrawLine(hDC, l+1 /*1*/, t+2 /*2*/, r-1 /*cx - 1*/, t+2 /*2*/, OffsetColor(colTmp, 0x15));
				DrawLine(hDC, l+1 /*1*/, b-3 /*cy - 3*/, r-1 /*cx - 1*/, b-3 /*cy - 3*/, OffsetColor(colTmp, -0x5));
				DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colTmp, -0x15));
			}

			//}
			break;
		case statePressed:
			colTmp = OffsetColor(colBk, -0x9);

			DrawVGradient(hDC, 
					OffsetColor(colTmp, -0xF), 
					colTmp, 
					l+1 /*1*/, t+1 /*1*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp, -0x15), 
					OffsetColor(colTmp,  0x15), 
					r-2 /*cx - 2*/, t+2 /*2*/, r-3 /*cx - 3*/, b-5 /*cy - 5*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp, -0x20), 
					OffsetColor(colTmp, -0x5), 
					l+1 /*1*/, t+2 /*2*/, l+2 /*2*/, b-3 /*cy - 5*/);

			colTmp = OffsetColor(colTmp, -0x60);

			DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
			DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
			DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
			DrawLine(hDC, r-1 /*cx - 1*/, t+1 /*1*/, r-1 /*cx - 1*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

			colTmp = OffsetColor(colBk, -0x15);

			SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

			SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);

			DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colBk, 0x8));
			DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colBk, -0x15));
			break;
		case stateHot:
			colTmp = OffsetColor(colBk, 0x18);
			SetBkColor(hDC, colTmp);

			DrawVGradient(hDC, 
					OffsetColor(colTmp,  0x10), 
					OffsetColor(colTmp, -0x10), 
					l+1 /*1*/, t+1 /*1*/, r-2 /*cx - 2*/, b-3 /*cy - 3*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp,  0x15), 
					OffsetColor(colTmp, -0x15), 
					l+1 /*1*/, t+2 /*2*/, l+2 /*2*/, b-5 /*cy - 5*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp, -0x5), 
					OffsetColor(colTmp, -0x20), 
					r-2 /*cx - 2*/, t+2 /*2*/, r-3 /*cx - 3*/, b-3 /*cy - 5*/);

			colTmp = OffsetColor(colTmp, -0x60);

			DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
			DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
			DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
			DrawLine(hDC, r-1 /*cx - 1*/, t+1 /*1*/, r-1 /*cx - 1*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

			colTmp = OffsetColor(colBk, -0x15);

			SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

			SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);

			DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colBk, 0x15));
			DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colBk, -0x10));

			//colTmp = colBk; //GetSysColor(COLOR_BTNFACE);

			//DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colBk, 0x15));
			//DrawLine(hDC, l+1 /*1*/, t+2 /*2*/, r-1 /*cx - 1*/, t+2 /*2*/, OffsetColor(colBk, 0x15));
			//DrawLine(hDC, l+1 /*1*/, b-3 /*cy - 3*/, r-1 /*cx - 1*/, b-3 /*cy - 3*/, OffsetColor(colBk, -0x5));
			//DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colBk, -0x15));

			if(!bBkOnly)
			{
				colTmp = colHot; //0xC0C0; //COLOR_HOTLIGHT);
				DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colTmp, 0x15));
				DrawLine(hDC, l+1 /*1*/, t+2 /*2*/, r-1 /*cx - 1*/, t+2 /*2*/, OffsetColor(colTmp, 0x15));
				DrawLine(hDC, l+1 /*1*/, b-3 /*cy - 3*/, r-1 /*cx - 1*/, b-3 /*cy - 3*/, OffsetColor(colTmp, -0x5));
				DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colTmp, -0x15));
			}

			break;
		case stateDisable:
			colTmp = OffsetColor(colBk, 0x12);
			SetBkColor(hDC, colTmp);

			DrawVGradient(hDC, 
					OffsetColor(colTmp,  0xF), 
					OffsetColor(colTmp, -0xF), 
					l+1 /*1*/, t+1 /*1*/, r-2 /*cx - 2*/, b-3 /*cy - 3*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp,  0x15), 
					OffsetColor(colTmp, -0x15), 
					l+1 /*1*/, t+2 /*2*/, l+2 /*2*/, b-5 /*cy - 5*/);

			DrawVGradient(hDC, 
					OffsetColor(colTmp, -0x5), 
					OffsetColor(colTmp, -0x20), 
					r-2 /*cx - 2*/, t+2 /*2*/, r-3 /*cx - 3*/, b-3 /*cy - 5*/);

			colTmp = OffsetColor(colTmp, -0x60);

			DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
			DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
			DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
			DrawLine(hDC, r-1 /*cx - 1*/, t+1 /*1*/, r-1 /*cx - 1*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

			colTmp = OffsetColor(colBk, 0x5);

			SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

			SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

			SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
			SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

			SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
			SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);

			DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, OffsetColor(colBk, 0x15));
			DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, OffsetColor(colBk, -0x25));

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

void CPWGraphics::DrawPlasticBorder(HDC hDC, LPRECT prc, COLORREF col /*= -1*/)
{

	if(col == -1) col = GetSysColor(COLOR_3DFACE);

	int l = prc->left,
		r = prc->right,
		t = prc->top,
		b = prc->bottom;

	COLORREF colTmp = col;

	colTmp = OffsetColor(colTmp, -0x58);

	DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
	DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
	DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
	DrawLine(hDC, r-1 /*cx - 1*/, t+1 /*1*/, r-1 /*cx - 1*/, b-1 /*cy - 1*/, colTmp);

	SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
	SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

	colTmp = OffsetColor(col, -0x15);

	SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
	SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

	SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

	SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
	SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

	SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);
}

#define REFLECT_AMOUNT	32 // Decrease to make WHITE shines more

COLORREF CPWGraphics::DrawGlassRect(CDC *pDC, LPRECT prc, COLORREF col, int iReflectAmount /*= 32*/)
{
	COLORREF colEnd = DifColor(col, 86);
	COLORREF colorG;
	int r=0, g=0, b=0;
	int r1, g1, b1;
	int r2, g2, b2;
	int nWidth = prc->right - prc->left;
	int nHeight = prc->bottom - prc->top;

	r1 = GetRValue(colEnd);
	g1 = GetGValue(colEnd);
	b1 = GetBValue(colEnd);

	r2 = GetRValue(col);
	g2 = GetGValue(col);
	b2 = GetBValue(col);

	for(int xy = 0; xy <= nHeight; xy++)
	{
		r = r1 + (xy * (r2-r1) / nHeight);
		g = g1 + (xy * (g2-g1) / nHeight);
		b = b1 + (xy * (b2-b1) / nHeight);

		if(xy == nHeight) break;

		if (xy < nHeight / 2) {
			colorG = OrColor(RGB(r, g, b), RGB(255, 255, 255), (iReflectAmount /*REFLECT_AMOUNT*/ + 5 * xy) > 100 ?
				100 : (iReflectAmount /*REFLECT_AMOUNT*/ + 5 * xy));
		} else  {
			colorG = RGB(r, g, b);
		}

		pDC->FillSolidRect(prc->left, prc->top+xy, nWidth, 1, colorG);
	}

	return RGB(r, g, b);
}

COLORREF CPWGraphics::CrossColor(COLORREF color, CPWGraphics::CCType itype /*= CCType::cc_full*/)
{
	int z = 0;
	BYTE r, g, b;

	r = GetRValue(color);
	g = GetGValue(color);
	b = GetBValue(color);

	if(itype == CCType::cc_full) return RGB(~r, ~g, ~b);

	if(r > 0x7F) { r = 0; z++; } else r = 0xFF;
	if(g > 0x7F) { g = 0; z++; } else g = 0xFF;
	if(b > 0x7F) { b = 0; z++; } else b = 0xFF;

	if(itype == CCType::cc_mono)
	{
		if(z >= 2)
		{
			r = 0x00;
			g = 0x00;
			b = 0x00;
		}
		else
		{
			r = 0xFF;
			g = 0xFF;
			b = 0xFF;
		}
	}

	return RGB(r, g, b);
}

void CPWGraphics::GlowText(CDC *pDC,
						   LPRECT lprc,
						   COLORREF colText,
						   int nBlendPercent /*= 40*/,
						   COLORREF colBlend /*= 0xffffffff*/)
{
	COLORREF colTmp = colBlend == 0xffffffff ? CrossColor(colText, CCType::cc_mono) : colBlend;

	for(int y=lprc->top; y<lprc->bottom; y++)
	{
		for(int x=lprc->left; x<lprc->right; x++)
		{
			if(pDC->GetPixel(x, y) == colText)
			{
				if(pDC->GetPixel(x-1, y) != colText)
					pDC->SetPixel(x-1, y, CPWGraphics::Blend(pDC->GetPixel(x-1, y), colTmp, nBlendPercent));

				if(pDC->GetPixel(x+1, y) != colText)
					pDC->SetPixel(x+1, y, CPWGraphics::Blend(pDC->GetPixel(x+1, y), colTmp, nBlendPercent));

				if(pDC->GetPixel(x, y-1) != colText)
					pDC->SetPixel(x, y-1, CPWGraphics::Blend(pDC->GetPixel(x-1, y-1), colTmp, nBlendPercent));

				if(pDC->GetPixel(x, y+1) != colText)
					pDC->SetPixel(x, y+1, CPWGraphics::Blend(pDC->GetPixel(x, y+1), colTmp, nBlendPercent));

			}
		}
	}
}

void CPWGraphics::DrawTransparentBg(CDC *pDC,
									LPRECT lprc,
									int iDotSize /*= 4*/,
									COLORREF colDark /*= 0xB4B4B4*/,
									COLORREF colLight /*= 0xFFFFFF*/)
{
	COLORREF cols[2] = { colDark, colLight };
	int cx = lprc->right - lprc->left;
	int cy = lprc->bottom - lprc->top;
	int icx = 1, icy, dx, dy;

	for(int x = lprc->left; x <= lprc->right; x += iDotSize)
	{
		icx = icy = !icx;
		if(x + iDotSize >= lprc->right) dx = lprc->right - x; else dx = iDotSize;
		for(int y = lprc->top; y <= lprc->bottom; y += iDotSize)
		{
			if(y + iDotSize >= lprc->bottom) dy = lprc->bottom - y; else dy = iDotSize;
			pDC->FillSolidRect(x, y, dx, dy, cols[icy]); 
			icy = !icy;
		}
	}

}

void CPWGraphics::FillAlphaRect(CDC *pDC, LPRECT lprc, COLORREF crAlphaColor)
{
	int x, y;
	CRect rc(lprc);
	BYTE srcRed   = GetRValue(crAlphaColor),
		 srcGreen = GetGValue(crAlphaColor),
		 srcBlue  = GetBValue(crAlphaColor),
		 srcAlpha = GetAValue(crAlphaColor);


	/* too simple but too slow !
	for(x = lprc->left; x < lprc->right; x++)
	{
		for(y = lprc->top; y < lprc->bottom; y++)
		{
			dest = pDC->GetPixel(x, y);
			dest = RGB(ALPHABYTE(srcRed,   GetRValue(dest), srcAlpha),
					   ALPHABYTE(srcGreen, GetGValue(dest), srcAlpha),
					   ALPHABYTE(srcBlue,  GetBValue(dest), srcAlpha));
			pDC->SetPixel(x, y, dest);
		}
	}*/

	// let's write something faster ...

	CDC memDC;
	CBitmap memBmp;
	BITMAP bmpMem;
	RGBQUAD *lpBmpBits;

	memDC.CreateCompatibleDC(pDC);
	memBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	memBmp.GetBitmap(&bmpMem);

	HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(memBmp);

	memDC.BitBlt(0, 0, bmpMem.bmWidth, bmpMem.bmHeight, pDC, rc.left, rc.top, SRCCOPY);

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bmpMem.bmWidth;
	bi.bmiHeader.biHeight = bmpMem.bmHeight;
	bi.bmiHeader.biPlanes = bmpMem.bmPlanes;
	bi.bmiHeader.biBitCount = bmpMem.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = bmpMem.bmWidthBytes * bmpMem.bmHeight;

	lpBmpBits = (RGBQUAD*)GlobalAlloc(GPTR, bmpMem.bmWidthBytes * bmpMem.bmHeight);

	GetDIBits(memDC,
			  memBmp,
			  0,
			  bmpMem.bmHeight,
			  lpBmpBits,
			  &bi,
			  DIB_RGB_COLORS);

	int pos;
	for(x = 0; x < bmpMem.bmWidth; x++)
	{
		for(y = 0; y < bmpMem.bmHeight; y++)
		{
			pos = ((bmpMem.bmHeight - y) * bmpMem.bmWidth) - (bmpMem.bmWidth - x);
			lpBmpBits[pos].rgbRed   = ALPHABYTE(srcRed,   lpBmpBits[pos].rgbRed,   srcAlpha);
			lpBmpBits[pos].rgbGreen = ALPHABYTE(srcGreen, lpBmpBits[pos].rgbGreen, srcAlpha);
			lpBmpBits[pos].rgbBlue  = ALPHABYTE(srcBlue,  lpBmpBits[pos].rgbBlue,  srcAlpha);
		}
	}

	SetDIBits(memDC,
			  memBmp,
			  0,
			  bmpMem.bmHeight,
			  lpBmpBits,
			  &bi,
			  DIB_RGB_COLORS);

	pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	GlobalFree(lpBmpBits);

	memDC.SelectObject(hOldMemBmp);
	memDC.DeleteDC();
	memBmp.DeleteObject();
}

// Wu antializing functions -----------------------------------------------------

void CPWGraphics::DrawPixel(CDC *pDC, short x, short y, short color)
{
	pDC->SetPixel (x,y,RGB(color,color,color));
}

void CPWGraphics::DrawWuCirlce (CDC * pDC, int x, int y, int r)
{
	short x1, y1, x2, y2;
	float dt = 5;

	for ( float theta= 0; theta< 360; theta += dt )
	{
		x1 = (short)( r*cos(theta*3.141593/180.0)+x);
		y1 = (short)(-r*sin(theta*3.141593/180.0)+y);

		x2 = (short)( r*cos((theta+dt)*3.141593/180.0)+x);
		y2 = (short)(-r*sin((theta+dt)*3.141593/180.0)+y);
		
		DrawWuLine ( pDC, x1, y1, x2, y2, RGB(192,0,0));
	}
}

void CPWGraphics::DrawWuLine(CDC *pDC, short X0, short Y0, short X1, short Y1, short BaseColor, short NumLevels, unsigned short IntensityBits)
{
   unsigned short IntensityShift, ErrorAdj, ErrorAcc;
   unsigned short ErrorAccTemp, Weighting, WeightingComplementMask;
   short DeltaX, DeltaY, Temp, XDir;

   /* Make sure the line runs top to bottom */
   if (Y0 > Y1) {
      Temp = Y0; Y0 = Y1; Y1 = Temp;
      Temp = X0; X0 = X1; X1 = Temp;
   }
   /* Draw the initial pixel, which is always exactly intersected by
      the line and so needs no weighting */
   DrawPixel(pDC,X0, Y0, BaseColor);

   if ((DeltaX = X1 - X0) >= 0) {
      XDir = 1;
   } else {
      XDir = -1;
      DeltaX = -DeltaX; /* make DeltaX positive */
   }
   /* Special-case horizontal, vertical, and diagonal lines, which
      require no weighting because they go right through the center of
      every pixel */
   if ((DeltaY = Y1 - Y0) == 0) {
      /* Horizontal line */
      while (DeltaX-- != 0) {
         X0 += XDir;
         DrawPixel(pDC,X0, Y0, BaseColor);
      }
      return;
   }
   if (DeltaX == 0) {
      /* Vertical line */
      do {
         Y0++;
         DrawPixel(pDC,X0, Y0, BaseColor);
      } while (--DeltaY != 0);
      return;
   }
   if (DeltaX == DeltaY) {
      /* Diagonal line */
      do {
         X0 += XDir;
         Y0++;
         DrawPixel(pDC,X0, Y0, BaseColor);
      } while (--DeltaY != 0);
      return;
   }
   /* Line is not horizontal, diagonal, or vertical */
   ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
   /* # of bits by which to shift ErrorAcc to get intensity level */
   IntensityShift = 16 - IntensityBits;
   /* Mask used to flip all bits in an intensity weighting, producing the
      result (1 - intensity weighting) */
   WeightingComplementMask = NumLevels - 1;
   /* Is this an X-major or Y-major line? */
   if (DeltaY > DeltaX) {
      /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
      ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
      /* Draw all pixels other than the first and last */
      while (--DeltaY) {
         ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
         ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
         if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the X coord */
            X0 += XDir;
         }
         Y0++; /* Y-major, so always advance Y */
         /* The IntensityBits most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
         Weighting = ErrorAcc >> IntensityShift;
         DrawPixel(pDC,X0, Y0, BaseColor + Weighting);
         DrawPixel(pDC,X0 + XDir, Y0,
               BaseColor + (Weighting ^ WeightingComplementMask));
      }
      /* Draw the final pixel, which is always exactly intersected by the line
         and so needs no weighting */
      DrawPixel(pDC,X1, Y1, BaseColor);
      return;
   }
   /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
      pixel that Y advances each time X advances 1 pixel, truncating the
      result to avoid overrunning the endpoint along the X axis */
   ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
   /* Draw all pixels other than the first and last */
   while (--DeltaX) {
      ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
      ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
      if (ErrorAcc <= ErrorAccTemp) {
         /* The error accumulator turned over, so advance the Y coord */
         Y0++;
      }
      X0 += XDir; /* X-major, so always advance X */
      /* The IntensityBits most significant bits of ErrorAcc give us the
         intensity weighting for this pixel, and the complement of the
         weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      DrawPixel(pDC,X0, Y0, BaseColor + Weighting);
      DrawPixel(pDC,X0, Y0 + 1,
            BaseColor + (Weighting ^ WeightingComplementMask));
   }
   /* Draw the final pixel, which is always exactly intersected by the line
      and so needs no weighting */
   DrawPixel(pDC,X1, Y1, BaseColor);
}

void CPWGraphics::DrawWuLine( CDC *pDC, int X0, int Y0, int X1, int Y1, COLORREF clrLine )
{
    /* Make sure the line runs top to bottom */
    if (Y0 > Y1)
    {
        int Temp = Y0; Y0 = Y1; Y1 = Temp;
        Temp = X0; X0 = X1; X1 = Temp;
    }
    
    /* Draw the initial pixel, which is always exactly intersected by
    the line and so needs no weighting */
    pDC->SetPixel( X0, Y0, clrLine );
    
    int XDir, DeltaX = X1 - X0;
    if( DeltaX >= 0 )
    {
        XDir = 1;
    }
    else
    {
        XDir   = -1;
        DeltaX = 0 - DeltaX; /* make DeltaX positive */
    }
    
    /* Special-case horizontal, vertical, and diagonal lines, which
    require no weighting because they go right through the center of
    every pixel */
    int DeltaY = Y1 - Y0;
    if (DeltaY == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
            pDC->SetPixel( X0, Y0, clrLine );
        }
        return;
    }
    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
            pDC->SetPixel( X0, Y0, clrLine );
        } while (--DeltaY != 0);
        return;
    }
    
    if (DeltaX == DeltaY)
    {
        /* Diagonal line */
        do
        {
            X0 += XDir;
            Y0++;
            pDC->SetPixel( X0, Y0, clrLine );
        } while (--DeltaY != 0);
        return;
    }
    
    unsigned short ErrorAdj;
    unsigned short ErrorAccTemp, Weighting;
    
    /* Line is not horizontal, diagonal, or vertical */
    unsigned short ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
    
    BYTE rl = GetRValue( clrLine );
    BYTE gl = GetGValue( clrLine );
    BYTE bl = GetBValue( clrLine );
    double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114;
    
    /* Is this an X-major or Y-major line? */
    if (DeltaY > DeltaX)
    {
    /* Y-major line; calculate 16-bit fixed-point fractional part of a
    pixel that X advances each time Y advances 1 pixel, truncating the
        result so that we won't overrun the endpoint along the X axis */
        ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
        /* Draw all pixels other than the first and last */
        while (--DeltaY) {
            ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
            ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
            if (ErrorAcc <= ErrorAccTemp) {
                /* The error accumulator turned over, so advance the X coord */
                X0 += XDir;
            }
            Y0++; /* Y-major, so always advance Y */
                  /* The IntensityBits most significant bits of ErrorAcc give us the
                  intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
            Weighting = ErrorAcc >> 8;
            ASSERT( Weighting < 256 );
            ASSERT( ( Weighting ^ 255 ) < 256 );
            
            COLORREF clrBackGround = ::GetPixel( pDC->m_hDC, X0, Y0 );
            BYTE rb = GetRValue( clrBackGround );
            BYTE gb = GetGValue( clrBackGround );
            BYTE bb = GetBValue( clrBackGround );
            double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
            
            BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
            BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
            BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
            pDC->SetPixel( X0, Y0, RGB( rr, gr, br ) );
            
            clrBackGround = ::GetPixel( pDC->m_hDC, X0 + XDir, Y0 );
            rb = GetRValue( clrBackGround );
            gb = GetGValue( clrBackGround );
            bb = GetBValue( clrBackGround );
            grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
            
            rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
            gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
            br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
            pDC->SetPixel( X0 + XDir, Y0, RGB( rr, gr, br ) );
        }
        /* Draw the final pixel, which is always exactly intersected by the line
        and so needs no weighting */
        pDC->SetPixel( X1, Y1, clrLine );
        return;
    }
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
    pixel that Y advances each time X advances 1 pixel, truncating the
    result to avoid overrunning the endpoint along the X axis */
    ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
    /* Draw all pixels other than the first and last */
    while (--DeltaX) {
        ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
        ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
        if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the Y coord */
            Y0++;
        }
        X0 += XDir; /* X-major, so always advance X */
                    /* The IntensityBits most significant bits of ErrorAcc give us the
                    intensity weighting for this pixel, and the complement of the
        weighting for the paired pixel */
        Weighting = ErrorAcc >> 8;
        ASSERT( Weighting < 256 );
        ASSERT( ( Weighting ^ 255 ) < 256 );
        
        COLORREF clrBackGround = ::GetPixel( pDC->m_hDC, X0, Y0 );
        BYTE rb = GetRValue( clrBackGround );
        BYTE gb = GetGValue( clrBackGround );
        BYTE bb = GetBValue( clrBackGround );
        double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
        
        BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
        BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
        BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
        
        pDC->SetPixel( X0, Y0, RGB( rr, gr, br ) );
        
        clrBackGround = ::GetPixel( pDC->m_hDC, X0, Y0 + 1 );
        rb = GetRValue( clrBackGround );
        gb = GetGValue( clrBackGround );
        bb = GetBValue( clrBackGround );
        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
        
        rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
        gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
        br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
        
        pDC->SetPixel( X0, Y0 + 1, RGB( rr, gr, br ) );
    }
    
    /* Draw the final pixel, which is always exactly intersected by the line
    and so needs no weighting */
    pDC->SetPixel( X1, Y1, clrLine );
}


// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL CPWGraphics::WriteDIB(LPCTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	file.Close();

	return TRUE;
}

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE CPWGraphics::DDBToDIB(CBitmap& bitmap, DWORD dwCompression /*= BI_RGB*/  /*, CPalette* pPal*/ )
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	//hPal = (HPALETTE) pPal->GetSafeHandle();
	//if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 )
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}
