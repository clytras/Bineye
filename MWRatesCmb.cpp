// MWRatesCmb.cpp : implementation file
//

#include "stdafx.h"
#include "Bineye.h"
#include ".\MWRatesCmb.h"


#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW   0x00020000
#endif

// CDropList _________________________________________________________________________

CMWRatesCmb::CDropList::CDropList()
: m_bShadow(false)
, m_bFade(false)
, m_hMouseHook(NULL)
, m_nItemHeight(0)
, m_fnAnimateWindow(NULL)
, m_iItemSel(-1)
, m_dwTick(0)
{
	RegisterWindowClass();
	
	MWRCKeybHookThunk<CMWRatesCmb::CDropList>::InitThunk((TMFP)&CMWRatesCmb::CDropList::KeyboardHookProc, this);
	MWRCMouseHookThunk<CMWRatesCmb::CDropList>::InitThunk((TMFP)&CMWRatesCmb::CDropList::MouseHookProc, this);

	// retrieve window animation API if available
	HMODULE hUser32 = GetModuleHandle(_T("user32"));
	// can't imagine why that would fail, but might as well *look* safe...  ;~)
	if (hUser32)
		m_fnAnimateWindow = (FN_ANIMATE_WINDOW)GetProcAddress(hUser32, LPCSTR("AnimateWindow"));

	m_ptKey.SetPoint(-100, -100);
}

CMWRatesCmb::CDropList::CDropList(CFontExt *pFont, CMWRatesCmb *pCmb)
: m_pFont(pFont)
, m_pCmb(pCmb)

{
	CDropList();
}

CMWRatesCmb::CDropList::~CDropList()
{
}

BEGIN_MESSAGE_MAP(CMWRatesCmb::CDropList, CWnd)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

LRESULT CMWRatesCmb::CDropList::KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CMWRatesCmb::CDropList::MouseHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	HWND hPtWnd;

	if (code>=0 /*&& NULL != m_hWnd*/ )
	{
		switch ( (UINT)wParam )
		{
		case WM_LBUTTONDOWN:

			//GetCursorPos(&pt);
			//if(::WindowFromPoint(pt) == m_hWnd) m_pCmb->SetCurSel(m_iItemSel);
			// no break, continue with list hiding
		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_RBUTTONDOWN:

			////PostMessage(WM_CLOSE);
			GetCursorPos(&pt);
			hPtWnd = ::WindowFromPoint(pt);
			if(hPtWnd != m_hWnd)
			{
				HideList();
				if(hPtWnd != m_pCmb->m_hWnd) ::PostMessage(hPtWnd, wParam, NULL, MAKELPARAM(pt.x, pt.y));
				return TRUE;
			}
			break;
		//case WM_NCLBUTTONUP:
		//case WM_NCMBUTTONUP:
		//case WM_MBUTTONUP:
		//case WM_NCRBUTTONUP:
		//case WM_RBUTTONUP:
		//case WM_LBUTTONUP:
		//	GetCursorPos(&pt);
		//	if(::WindowFromPoint(pt) == m_hWnd) HideList();
		//	break;
		}
	}

	return ::CallNextHookEx(m_hMouseHook, code, wParam, lParam);;
}

void CMWRatesCmb::CDropList::SetMouseHook()
{
	if(!m_hMouseHook)
	{
		m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE,
				(HOOKPROC)MWRCMouseHookThunk<CMWRatesCmb::CDropList>::GetThunk(),
				NULL, ::GetCurrentThreadId());
	}
}

void CMWRatesCmb::CDropList::RemoveMouseHook()
{
	if(m_hMouseHook)
	{
		::UnhookWindowsHookEx(m_hMouseHook);
		m_hMouseHook = NULL;
	}
}

BOOL CMWRatesCmb::CDropList::Create()
{
	BOOL bRet;

	if(!IsWindow(m_pCmb->GetSafeHwnd()) || IsWindow(m_hWnd))
		return FALSE;

	if(!(bRet = CreateEx(WS_EX_TOOLWINDOW, MWRATESCMBLISTWND_CLASSNAME, NULL, WS_POPUP, 
		CRect(0, 0, 10, 10), (CWnd*)m_pCmb, 0, NULL)))
	{
		return FALSE;
	}

	return bRet;
}

void CMWRatesCmb::CDropList::ShowList()
{
	PosWindow();

	ShowWindow(SW_SHOWNOACTIVATE);
	//if(NULL == GetCapture()) SetCapture();
	SetMouseHook();
}

void CMWRatesCmb::CDropList::HideList()
{
	ShowWindow(SW_HIDE);

	RemoveMouseHook();

	if (GetCapture() == this) 
		ReleaseCapture();
}

void CMWRatesCmb::CDropList::PosWindow()
{
	CRect prc, wrc;
	int nListWidth;

	if(!IsWindow(m_pCmb->GetSafeHwnd())) return;

	CalcItemSize();

	if(m_nItemHeight > 0) 
	{
		m_nItemHeight += 3 + 3;

		m_pCmb->GetWindowRect(prc);

		nListWidth = m_pCmb->ListAlign().IsHorAbsolute() ? prc.Width()+1 : m_nItemWidth;

		MoveWindow(m_pCmb->ListAlign().IsRight() ? prc.right - nListWidth : prc.left, prc.top + prc.Height(), 
			nListWidth,
			m_nItemHeight * m_pCmb->GetItemsCount() + 3, TRUE);

		GetClientRect(wrc);
		m_rgn.DeleteObject();
		m_rgn.CreateRoundRectRgn(wrc.left, wrc.top, wrc.right, wrc.bottom, 4, 4);
		SetWindowRgn(m_rgn, TRUE);
	}
}

BOOL CMWRatesCmb::CDropList::RegisterWindowClass()
{

	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, MWRATESCMBLISTWND_CLASSNAME, &wndcls)) )
    {
        wndcls.style            = CS_DBLCLKS | /*CS_VREDRAW | CS_HREDRAW |*/ CS_SAVEBITS | CS_DROPSHADOW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = MWRATESCMBLISTWND_CLASSNAME;
		m_bShadow = true;

        if( !AfxRegisterClass(&wndcls) )
        {
			wndcls.style &= ~CS_DROPSHADOW;
			m_bShadow = false;

			if( !AfxRegisterClass(&wndcls) )
			{
				AfxThrowResourceException();
				return FALSE;
			}
        }
    }

    return TRUE;
}

void CMWRatesCmb::CDropList::OnDestroy()
{
	__super::OnDestroy();
	RemoveMouseHook();
	m_nItemHeight = 0;
}

void CMWRatesCmb::CDropList::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
	//PostMessage(WM_CLOSE);
}

BOOL CMWRatesCmb::CDropList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	__super::OnEraseBkgnd(pDC);

	CRect wrc;
	GetClientRect(wrc);

	for(int i=0; i<m_pCmb->GetItemsCount(); i++)
	{
		DrawItem(pDC, wrc, i);
	}

	DrawNC(pDC, wrc);

	return true;
}

void CMWRatesCmb::CDropList::DrawNC(CDC* pDC, LPRECT lprc)
{

	HDC hDC = pDC->GetSafeHdc();
	int l = lprc->left,
		r = lprc->right,
		t = lprc->top,
		b = lprc->bottom;

	COLORREF colTmp = m_pCmb->GetBorderColor(); // m_pCmb->GetBaseColor();

	//colTmp = CPWGraphics::OffsetColor(colTmp, -0x58);

	CPWGraphics::DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
	CPWGraphics::DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
	CPWGraphics::DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 1*/, r-2 /*cx - 2*/, b-2 /*cy - 1*/, colTmp);
	CPWGraphics::DrawLine(hDC, r-2 /*cx - 1*/, t+1 /*1*/, r-2 /*cx - 1*/, b-2 /*cy - 1*/, colTmp);

	SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, l+1 /*1*/, b-3 /*cy - 2*/, colTmp);
	SetPixel(hDC, r-3 /*cx - 2*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, r-3 /*cx - 2*/, b-3 /*cy - 2*/, colTmp);
}

void CMWRatesCmb::CDropList::DrawItem(CDC* pDC, LPRECT lprc, int idx, BOOL bDrawFull /*= FALSE*/)
{
	CDC memDC;
	CRect irc;
	BITMAP bm;
	CMWRatesCmb::CItemData *pitem = m_pCmb->GetItem(idx);
	CBitmap *pBmBk = pitem->state.IsHot() ? &m_bmBkHot : &m_bmBkNormal;
	
	pBmBk->GetBitmap(&bm);
	irc.SetRect(0, idx * m_nItemHeight, bm.bmWidth, (idx+1) * m_nItemHeight);

	//memDC.CreateCompatibleDC(pDC);

	//CBitmap *oldbm = memDC.SelectObject(pBmBk);

	//if(!bDrawFull)
	//{
		memDC.CreateCompatibleDC(pDC);
		CBitmap *oldbm = memDC.SelectObject(pBmBk);
		pDC->BitBlt(1, irc.top+1, irc.Width(), irc.Height(), &memDC, 0, irc.top, SRCCOPY);
		memDC.SelectObject(oldbm);
		memDC.DeleteDC();
		return;
	//}

	if(pitem->state.IsHot())
		CPWGraphics::DrawGlassRect(pDC, CRect(2, irc.top+2, 0+irc.Width(), irc.top+0+irc.Height()), m_pCmb->GetBaseColor());
	//else
	//	pDC->BitBlt(1, irc.top+1, irc.Width(), irc.Height(), &memDC, 0, irc.top, SRCCOPY);

	//memDC.SelectObject(oldbm);
	//memDC.DeleteDC();


	if(pitem->state.IsHot())
	{
		int x, y, xmin, ymin, xmax, ymax, xget, yget;
		COLORREF cruse;

		for(y=ymin=(irc.top+1), ymax=(irc.bottom+1); y<ymax; y++)
		{
			for(x=xmin=(irc.left+1), xmax=(irc.right+1); x<xmax; x++)
			{
				if(x==xmin || y==ymin || x ==(xmax-1) || y==(ymax-1))
				{
					if(x==xmin && (y==ymin || y==(ymax-1)))
					{
						xget=x+2;
						yget=y;
					}
					else if(x==(xmax-1) && (y==1 || y==(ymax-1)))
					{
						xget=x-2;
						yget=y;
					}
					
					cruse = pDC->GetPixel(xget, yget);
					cruse = CPWGraphics::Blend(cruse, 0xffffff /*m_crBorder*/, 70);

					if(!((x==xmin && (y==ymin || y==(ymax-1))) ||
						(y==ymin && (x==xmin || x==(xmax-1))) ||
						(x==(xmax-1) && y==(ymax-1))))
					{
						pDC->SetPixel(x, y, cruse);
					}
				}
			}
		}
	}


	//COLORREF colCorrect = idx == 0 || idx == m_pCmb->GetItemsCount()-1 ?
	//	m_pCmb->GetBorderColor() :
	//	pDC


	if(idx == 0)
	{
		pDC->SetPixel(irc.left+1 /*1*/, irc.top+1 /*1*/, m_pCmb->GetBorderColor());
		pDC->SetPixel(irc.right /*cx - 2*/, irc.top+1 /*1*/, m_pCmb->GetBorderColor());
	}
	else if(idx == m_pCmb->GetItemsCount()-1)
	{
		pDC->SetPixel(irc.right /*cx - 2*/, irc.bottom /*cy - 2*/, m_pCmb->GetBorderColor());
		pDC->SetPixel(irc.left+1 /*1*/, irc.bottom /*cy - 2*/, m_pCmb->GetBorderColor());
	}

	if(!pitem->strCode.IsEmpty())
	{
		m_pFont->AttachDC(pDC);

		pDC->SetBkMode(TRANSPARENT);

		COLORREF colText = CPWGraphics::Blend(m_pCmb->GetBaseColor(), 
			CPWGraphics::CrossColor(m_pCmb->GetBaseColor(), CPWGraphics::cc_mono), 60);

		pDC->SetTextColor(colText);

		CRect rctext(2, irc.top+2, 0+irc.Width(), irc.top+0+irc.Height());

		pDC->DrawText(pitem->strCode, rctext, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

		m_pFont->DetachDC();

		CPWGraphics::GlowText(pDC, rctext, colText, 60);
	}
}

void CMWRatesCmb::CDropList::OnTimer(UINT nIDEvent)
{
	POINT pt;
	CRect wrc;
	int iSel = m_iItemSel; //-1;

	GetCursorPos(&pt);
	GetWindowRect(wrc);

	wrc.DeflateRect(1, 1);

	if(::WindowFromPoint(pt) == m_hWnd && wrc.PtInRect(pt) && m_ptKey != pt)
	{
		ScreenToClient(&pt);
		pt.y--;

		iSel = pt.y / m_nItemHeight;

		//if(iSel >= m_pCmb->GetItemsCount()) iSel = -1;
	}

	SelectItem(iSel);

	__super::OnTimer(nIDEvent);
}

int CMWRatesCmb::CDropList::SelectItem(int nNewSel, bool bDraw /*= TRUE*/)
{
	CRect wrc;
	CDC *pDC;

	if(nNewSel < 0 || nNewSel > m_pCmb->m_items.GetUpperBound()) return m_iItemSel;

	if(m_iItemSel != nNewSel)
	{
		pDC = GetDC();
		GetWindowRect(wrc);
		wrc.DeflateRect(1, 1);

		if(m_iItemSel != -1)
		{
			m_pCmb->GetItem(m_iItemSel)->state.SetNormal();
			if(bDraw) DrawItem(pDC, wrc, m_iItemSel);
		}

		if(nNewSel != -1)
		{
			m_pCmb->GetItem(nNewSel)->state.SetHot();
			if(bDraw) DrawItem(pDC, wrc, nNewSel);
		}

		ReleaseDC(pDC);
	}

	return m_iItemSel = nNewSel;
}

void CMWRatesCmb::CDropList::BuildBkBitmaps(void)
{
	CMWRatesCmb::CItemData *pitem;
	CRect wrc;
	BITMAP bm, ibm;
	static int nMaxCodeWidth;

	PosWindow();

	GetWindowRect(wrc);
	wrc.DeflateRect(1, 1);
	wrc.right--;
	wrc.bottom--;

	CDC memDC;
	CDC *pDC = GetDC();

	memDC.CreateCompatibleDC(pDC);

	m_bmBkNormal.DeleteObject();
	m_bmBkNormal.CreateCompatibleBitmap(pDC, wrc.Width(), wrc.Height());

	m_bmBkNormal.GetBitmap(&bm);

	m_bmBkHot.DeleteObject();
	m_bmBkHot.CreateCompatibleBitmap(pDC, wrc.Width(), wrc.Height());


	CBitmap *oldbm = memDC.SelectObject(&m_bmBkNormal);

	CPWGraphics::DrawPlasticRect(memDC.GetSafeHdc(), 
		CRect(0, 0, wrc.Width(), wrc.Height()), m_pCmb->GetBaseColor());

	CDC memDC2;

	memDC2.CreateCompatibleDC(pDC);

	CBitmap *oldpbm = memDC2.SelectObject(&m_bmBkHot);

	//CPWGraphics::DrawPlasticRect(memDC2.GetSafeHdc(), 
	//	CRect(0, 0, wrc.Width(), wrc.Height()), CPWGraphics::OffsetColor(m_pCmb->GetBaseColor(), -0x40));


	memDC2.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY);
	memDC2.SelectObject(oldpbm);
	memDC2.DeleteDC();

	CRect irc(wrc);
	int i, j, iCurX;


	nMaxCodeWidth = 0;
	
	m_pFont->SetFontBold();

	for(i=0; i<m_pCmb->GetItemsCount(); i++)
		if(!m_pCmb->GetItem(i)->strCode.IsEmpty())
			nMaxCodeWidth = max(nMaxCodeWidth, m_pFont->GetTextWidth(m_pCmb->GetItem(i)->strCode));

	m_pFont->SetFontBold(FALSE);

	for(j=0; j<2; j++)
	{
		if(j==1)
		{
			memDC.SelectObject(&m_bmBkHot);
		}

		for(i=0; i<m_pCmb->GetItemsCount(); i++)
		{

			pitem = m_pCmb->GetItem(i);

			//if(j=1)
			//	CPWGraphics::DrawGlassRect(pDC, CRect(2, irc.top+2, 0+irc.Width(), irc.top+0+irc.Height()), m_pCmb->GetBaseColor());
			//else
			//	pDC->BitBlt(1, irc.top+1, irc.Width(), irc.Height(), &memDC, 0, irc.top, SRCCOPY);

			//memDC.SelectObject(oldbm);
			//memDC.DeleteDC();

			irc.SetRect(0, i * m_nItemHeight, bm.bmWidth, (i+1) * m_nItemHeight);

			

			if(j==1)
			{
			//	int x, y, xmin, ymin, xmax, ymax, xget, yget;
			//	COLORREF cruse;

				CPWGraphics::DrawGlassRect(&memDC, irc /*CRect(0, irc.top+0, irc.Width(), irc.top+irc.Height())*/, m_pCmb->GetHotColor() /*m_pCmb->GetBaseColor()*/);

			//	for(y=ymin=(irc.top+0), ymax=(irc.bottom+0); y<ymax; y++)
			//	{
			//		for(x=xmin=(irc.left+0), xmax=(irc.right+0); x<xmax; x++)
			//		{
			//			if(x==xmin || y==ymin || x ==(xmax-1) || y==(ymax-1))
			//			{
			//				if(x==xmin && (y==ymin || y==(ymax-1)))
			//				{
			//					xget=x+2;
			//					yget=y;
			//				}
			//				else if(x==(xmax-1) && (y==ymin || y==(ymax-1)))
			//				{
			//					xget=x-2;
			//					yget=y;
			//				}
			//				
			//				cruse = memDC.GetPixel(xget, yget);
			//				cruse = CPWGraphics::Blend(cruse, 0xffffff /*m_crBorder*/, 70);

			//				if(!((x==xmin && (y==ymin || y==(ymax-1))) ||
			//					(y==ymin && (x==xmin || x==(xmax-1))) ||
			//					(x==(xmax-1) && y==(ymax-1))))
			//				{
			//					memDC.SetPixel(x, y, cruse);
			//				}
			//			}
			//		}
			//	}
			}


			//COLORREF colCorrect = idx == 0 || idx == m_pCmb->GetItemsCount()-1 ?
			//	m_pCmb->GetBorderColor() :
			//	pDC


			if(i == 0)
			{
				memDC.SetPixel(irc.left+0 /*1*/, irc.top+0 /*1*/, m_pCmb->GetBorderColor());
				memDC.SetPixel(irc.right-1 /*cx - 2*/, irc.top+0 /*1*/, m_pCmb->GetBorderColor());
			}
			else if(i == m_pCmb->GetItemsCount()-1)
			{
				memDC.SetPixel(irc.right-1 /*cx - 2*/, irc.bottom-1 /*cy - 2*/, m_pCmb->GetBorderColor());
				memDC.SetPixel(irc.left+0 /*1*/, irc.bottom-1 /*cy - 2*/, m_pCmb->GetBorderColor());
			}

			iCurX = 2;

			if(pitem->pbmImg)
			{
				pitem->pbmImg->GetBitmap(&ibm);
				memDC2.CreateCompatibleDC(pDC);
				oldpbm = memDC2.SelectObject(pitem->pbmImg);

				memDC.BitBlt(iCurX, irc.top + (irc.Height() / 2 - ibm.bmHeight / 2), ibm.bmWidth, ibm.bmHeight,
					&memDC2, 0, 0, SRCCOPY);

				memDC2.SelectObject(oldpbm);
				memDC2.DeleteDC();

				iCurX += ibm.bmWidth + 4;
			}

			memDC.SetBkMode(TRANSPARENT);

			//COLORREF colText = j==1 ? CPWGraphics::OffsetColor(m_pCmb->GetBaseColor(), -0x80) : m_pCmb->GetBaseColor();
			COLORREF colText = m_pCmb->GetHotColor(); //m_pCmb->GetBaseColor(); //m_pCmb->GetBaseColor();

			colText = CPWGraphics::Blend(colText, 
				CPWGraphics::CrossColor(colText, CPWGraphics::cc_mono), 80);

			memDC.SetTextColor(colText);

			CRect rctext(iCurX, irc.top+1, 0+irc.Width(), irc.top+0+irc.Height());

			if(!pitem->strCode.IsEmpty())
			{
				m_pFont->AttachDC(&memDC);
				m_pFont->SetFontBold();

				//rctext.left += 4;

				memDC.DrawText(pitem->strCode, rctext, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

				//CPWGraphics::GlowText(&memDC, rctext, colText, 60);

				iCurX += nMaxCodeWidth + 4;

				m_pFont->SetFontBold(FALSE);
				m_pFont->DetachDC();
			}

			if(!pitem->strText.IsEmpty())
			{
				m_pFont->AttachDC(&memDC);

				rctext.left = iCurX;

				memDC.DrawText(pitem->strText, rctext, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

				m_pFont->DetachDC();
			}

		}
	}

	//memDC.SelectObject(&m_bmBkHot);

	//COLORREF colTmp = CPWGraphics::OffsetColor(m_pCmb->GetBaseColor(), 0x20);

	//CPWGraphics::DrawPlasticRect(memDC.GetSafeHdc(), 
	//	CRect(0, 0, wrc.Width(), wrc.Height()), colTmp);

	memDC.SelectObject(oldbm);
	memDC.DeleteDC();

	ReleaseDC(pDC);
}

void CMWRatesCmb::CDropList::CalcItemSize(void)
{
	int i, nImgCy, nImgCx, nTmpCx;
	BITMAP bm;

	if(!IsWindow(m_pCmb->GetSafeHwnd())) return;

	m_nItemHeight = 0;
	m_nItemWidth = 0;

	for(i=0; i<m_pCmb->GetItemsCount(); i++)
	{

		if(m_pCmb->GetItem(i)->pbmImg)
		{
			m_pCmb->GetItem(i)->pbmImg->GetBitmap(&bm);
			nImgCy = bm.bmHeight;
			nImgCx = bm.bmWidth;
		}
		else 
		{
			nImgCy = 0;
			nImgCx = 0;
		}

		m_nItemHeight = max(m_nItemHeight, max(nImgCy, m_pFont->GetFontHeight()));
		nTmpCx = nImgCx + m_pFont->GetTextWidth(m_pCmb->GetItem(i)->strCode) +
			m_pFont->GetTextWidth(m_pCmb->GetItem(i)->strText) + 30;
		m_nItemWidth = max(m_nItemWidth, nTmpCx);
	}
}

void CMWRatesCmb::CDropList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//if(bShow)
	//	SetTimer(1, 10, NULL);
	//else
	//	KillTimer(1);

	m_pCmb->m_bIsListDown = bShow != FALSE;

	__super::OnShowWindow(bShow, nStatus);

	if(m_fnAnimateWindow)
	{
		if (bShow)
		{
			SetTimer(1, 10, NULL);
			m_fnAnimateWindow(m_hWnd, 200, AW_BLEND);
			SetCapture();
			m_dwTick = GetTickCount();
		}
		else
		{
			KillTimer(1);
			SelectItem(-1, FALSE);
			//m_fnAnimateWindow(m_hWnd, 200, AW_HIDE | AW_BLEND );
		}
	}
}

void CMWRatesCmb::CDropList::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	HideList();
	DestroyWindow();

	__super::OnClose();
}


// CCmbEdit __________________________________________________________________________

CMWRatesCmb::CCmbEdit::CCmbEdit()
{
}

CMWRatesCmb::CCmbEdit::~CCmbEdit()
{
}

BEGIN_MESSAGE_MAP(CMWRatesCmb::CCmbEdit, CEdit)
END_MESSAGE_MAP()



// CMWRatesCmb _______________________________________________________________________

//IMPLEMENT_DYNAMIC(CMWRatesCmb, CWnd)
CMWRatesCmb::CMWRatesCmb()
: m_crBase(-1)
, m_crBorder(-1)
, m_crText(-1)
, m_crGlyphArrow(-1)
, m_bTimerSet(false)
, m_bIsListDown(false)
, m_bShowBaseText(true)
, m_bShowBaseCode(true)
, m_bShowBaseImg(true)
, m_bShowTextOnNoSel(true)
, m_crTrans1(0xFF00FF)
, m_crTrans2(0xFFFF00)
, m_nMaxImgWidth(0)
, m_nMaxImgHeight(0)
, m_iCurSel(-1)
, m_pImg(NULL)
, m_align(CPWGraphics::CAlignment::Center, CPWGraphics::CAlignment::Middle)
, m_ListAlign(CPWGraphics::CAlignment::HorAbsolute, CPWGraphics::CAlignment::VerAbsolute)
{
	m_font.CreateStockObject(DEFAULT_GUI_FONT);

	RegisterWindowClass();

	//SetBaseColor(0x33FF99);
	//SetBaseColor(0xFFffff);
	//SetBaseColor(0x0);
	//SetBaseColor(0xC0C000);

	SetBaseColor(GetSysColor(COLOR_BTNFACE));

	SetTextColor(GetSysColor(COLOR_BTNTEXT));

	m_crHot = 0xEFDEC0;

	m_cDropList.m_pFont = &m_font;
	m_cDropList.m_pCmb = this;
}

CMWRatesCmb::~CMWRatesCmb()
{
	CItemData *pitem;

	for(int i=0; i<m_items.GetCount(); i++)
	{
		pitem = (CItemData*)m_items[i];
		delete pitem;
	}

	m_items.RemoveAll();
	m_glyph.DeleteObject();

	if(m_pImg)
	{
		m_pImg->DeleteObject();
		delete m_pImg;
	}
}


BEGIN_MESSAGE_MAP(CMWRatesCmb, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	//ON_WM_INITMENUPOPUP()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CMWRatesCmb message handlers

BOOL CMWRatesCmb::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, MWRATESCMB_CLASSNAME, &wndcls)) )
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) ::GetStockObject(NULL_BRUSH);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = MWRATESCMB_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CMWRatesCmb::Create(LPCTSTR lpszText, DWORD dwStyle, CRect& rect, CWnd* pParent, UINT nID)
{
	RECT wrc;
	BOOL bRet;

	CopyRect(&wrc, rect);


	bRet = CWnd::CreateEx(WS_EX_TRANSPARENT, MWRATESCMB_CLASSNAME, lpszText, dwStyle, wrc, pParent, nID );

	m_rgn.CreateRoundRectRgn(0, 0, rect.Width()+1, rect.Height()+1, 4, 4);

	SetWindowRgn(m_rgn, FALSE);

	int nHeight = rect.bottom-rect.top;

	/*m_cCmbEdit.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL|ES_WANTRETURN,
		CRect(4, 4, rect.Width()-16, nHeight-4), this, 4001);


	m_cCmbEdit.SetFont(&m_font);*/

	//m_nMnuWidth = 0;
	//m_nMnuHeight = 0;

	//m_cDrop.CreatePopupMenu();

	m_cDropList.Create();

	return bRet;
}

void CMWRatesCmb::OnPaint()
{

	if(!m_bTimerSet)
	{
		SetTimer(1, 10, NULL);
		CreateBtnBmp();
		DrawBtnBmp();
		m_bTimerSet = true;
	}

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CDC memDC;
	BITMAP bm;

	m_bmWnd.GetBitmap(&bm);

	memDC.CreateCompatibleDC(&dc);
	CBitmap *oldbm = memDC.SelectObject(&m_bmWnd);

	dc.TransparentBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, bm.bmWidth, bm.bmHeight, m_crTrans1);

	memDC.SelectObject(oldbm);
	memDC.DeleteDC();
}

void CMWRatesCmb::OnTimer(UINT nIDEvent)
{
	CRect rc;
	POINT pt;

	if(!m_cDropList.IsWindowVisible()  /*m_bIsListDown*/)
	{
		GetWindowRect(rc);
		GetCursorPos(&pt);

		if(rc.PtInRect(pt) && ::WindowFromPoint(pt) == m_hWnd)
		{
			if(LOWORD(GetAsyncKeyState(VK_LBUTTON)) & 0x8000)
			{
				if(!m_state.IsPressed())
				{
					m_state.SetPressed();
					DrawBtnBmp(true);
					//if(!m_bIsListDown) OnLButtonDown(NULL, CPoint(0,0));
				}
			}
			else if(!m_state.IsHot() && !m_bIsListDown)
			{
				m_state.SetHot();
				DrawBtnBmp(true);
			}
			else if(m_bIsListDown)
			{
				if(!m_state.IsNormal())
				{
					m_state.SetNormal();
					DrawBtnBmp(true);
				}
			}
		}
		else if(!m_state.IsNormal())
		{
			m_state.SetNormal();
			DrawBtnBmp(true);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CMWRatesCmb::OnEraseBkgnd(CDC* pDC)
{
	return true; //CWnd::OnEraseBkgnd(pDC);
}

void CMWRatesCmb::DrawBtnBmp(bool bRedraw /*= false*/)
{
	CDC *pDC;
	CDC memDC;
	CRect wrc;
	CBrush bruse;

	pDC = GetDC();

	memDC.CreateCompatibleDC(pDC);
	CBitmap *oldbm = memDC.SelectObject(&m_bmWnd);

	ReleaseDC(pDC);

	GetClientRect(wrc);

	memDC.FillSolidRect(wrc, m_crTrans1);

	wrc.DeflateRect(1, 1);

	int iReflectAmount;
	COLORREF crGlassBk = m_state.IsHot() ? 
		CPWGraphics::Blend(m_crBase, GetSysColor(COLOR_HIGHLIGHT), 15) :  m_crBase;

	if(m_state.IsHot())
	{
		iReflectAmount = 13;
		crGlassBk = m_crHot; //0x00EFDEC1; //0x00FACE29;
	}
	else if(m_state.IsPressed())
	{
		iReflectAmount = 10;

		CColor clr(m_crHot);

		clr.SetLuminance(0.6);
		//clr.SetSaturation(0.900);
		//clr.SetHue(300);

		crGlassBk = (COLORREF)clr; // CPWGraphics::DifColor(m_crHot, 80); // 0xE7B858; //0xE3AE40; //0xFF9933; //CPWGraphics::OffsetColor(crGlassBk, -0x10);
	}
	else
	{
		iReflectAmount = 13;
		crGlassBk = m_crBase;
	}

	//if(m_state.IsPressed())
	//{
	//	iReflectAmount = 40;
	//	crGlassBk = CPWGraphics::OffsetColor(crGlassBk, -0x10);
	//}
	//else if(m_state.IsHot())
	//	iReflectAmount = 15;
	//else
	//	iReflectAmount = 13; //33;

	//crGlassBk = 0x00FACE29;

	CPWGraphics::DrawGlassRect(&memDC, wrc, crGlassBk, iReflectAmount);

	//bruse.CreateSolidBrush(m_crBorder);
	//memDC.FrameRgn(&m_rgn, &bruse, 1, 1);
	//bruse.DeleteObject();

	GetClientRect(wrc);
	HDC hDC = memDC.GetSafeHdc();
	int l = wrc.left,
		r = wrc.right+1,
		t = wrc.top,
		b = wrc.bottom+1;

	COLORREF colTmp = m_state.IsFocused() ? CPWGraphics::Blend(0, m_crHot /*0x00EFDEC1*/, 55) /*0x00B17F3C */
										: m_crBorder; // m_pCmb->GetBaseColor();

	//colTmp = CPWGraphics::OffsetColor(colTmp, -0x58);

	CPWGraphics::DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
	CPWGraphics::DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
	CPWGraphics::DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 1*/, r-2 /*cx - 2*/, b-2 /*cy - 1*/, colTmp);
	CPWGraphics::DrawLine(hDC, r-2 /*cx - 1*/, t+1 /*1*/, r-2 /*cx - 1*/, b-2 /*cy - 1*/, colTmp);

	SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, l+1 /*1*/, b-3 /*cy - 2*/, colTmp);
	SetPixel(hDC, r-3 /*cx - 2*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, r-3 /*cx - 2*/, b-3 /*cy - 2*/, colTmp);

	wrc.DeflateRect(1, 1);


	// -------


	int x, y, x2, y2, xmax, ymax;
	COLORREF cruse, crblend = m_state.IsFocused() ? (m_state.IsPressed() ? colTmp /*0xffffff*/ : 0xFBD848) : 0xffffff /*m_crBorder*/;
	int iRefAm = m_state.IsPressed() ? 35 : 80;

	for(y=1, ymax=wrc.bottom; y<ymax; y++)
	{
		for(x=1, xmax=wrc.right; x<xmax; x++)
		{
			if(x==1 || y==1 || x ==(xmax-1) || y==(ymax-1))
			{
				//if(x==1 && (y==1 || y==(ymax-1)))
				//{
				//	xget=x+1; //2;
				//	yget=y;
				//}
				//else if(x==(xmax-1) && (y==1 || y==(ymax-1)))
				//{
				//	xget=x-1; //2;
				//	yget=y;
				//}

				//iRefAm = m_state.IsPressed() ? 40 : 80;

				if(!((x==1 && (y==1 || y==(ymax-1))) ||
				     (y==1 && (x==1 || x==(xmax-1))) ||
				     (x==(xmax-1) && y==(ymax-1))))
				{
					cruse = memDC.GetPixel(x, y);
					//cruse = CPWGraphics::Blend(cruse, m_state.IsFocused() ? (m_state.IsPressed() ? /*colTmp*/ 0xffffff : 0xFBD848) : 0xffffff /*m_crBorder*/, (m_state.IsPressed() ? 40 : 80));
					cruse = CPWGraphics::Blend(cruse, crblend, iRefAm);

					memDC.SetPixel(x, y, cruse);
				}
				else
				{
					if(x==1) x2=x+1; else x2=x-1;
					if(y==1) y2=y+1; else y2=y-1;

					cruse = memDC.GetPixel(x2, y2);
					//cruse = CPWGraphics::Blend(cruse, m_state.IsFocused() ? (m_state.IsPressed() ? /*colTmp*/ 0xffffff : 0xFBD848) : 0xffffff /*m_crBorder*/, (m_state.IsPressed() ? 40 : 80));
					cruse = CPWGraphics::Blend(cruse, crblend, iRefAm);

					memDC.SetPixel(x2, y2, cruse);
				}
			}
		}
	}

	CDC bmDC;
	BITMAP bm;
	CRect grc;

	if(m_items.GetCount() > 0)
	{
		m_glyph.GetBitmap(&bm);

		bmDC.CreateCompatibleDC(&memDC);

		CBitmap *oldglbm = bmDC.SelectObject(&m_glyph);

		grc.left = wrc.right-bm.bmWidth-4;
		grc.top = (wrc.Height() / 2 - bm.bmHeight / 2) + 1;
		grc.right = grc.left + bm.bmWidth;
		grc.bottom = grc.top + bm.bmHeight;

		//grc.SetRect(wrc.right-bm.bmWidth-4, (wrc.Height() / 2 - bm.bmHeight / 2) + 1, 
		//	bm.bmWidth, bm.bmHeight);

		memDC.TransparentBlt(grc.left, grc.top, bm.bmWidth, bm.bmHeight, 
			&bmDC, 0, 0, bm.bmWidth, bm.bmHeight, 0xFF00FF);

		bmDC.SelectObject(oldglbm);
		bmDC.DeleteDC();


		int iPerShadow = 60;
		COLORREF cdShadow = 0xffffff;


		for(y=grc.top; y<grc.bottom; y++)
		{
			for(x=grc.left; x<grc.right; x++)
			{
				if(memDC.GetPixel(x, y) == m_crGlyphArrow)
				{
					if(memDC.GetPixel(x-1, y) != m_crGlyphArrow)
						memDC.SetPixel(x-1, y, CPWGraphics::Blend(memDC.GetPixel(x-1, y), cdShadow, iPerShadow));

					if(memDC.GetPixel(x+1, y) != m_crGlyphArrow)
						memDC.SetPixel(x+1, y, CPWGraphics::Blend(memDC.GetPixel(x+1, y), cdShadow, iPerShadow));

					if(memDC.GetPixel(x, y-1) != m_crGlyphArrow)
						memDC.SetPixel(x, y-1, CPWGraphics::Blend(memDC.GetPixel(x, y-1), cdShadow, iPerShadow));

					if(memDC.GetPixel(x, y+1) != m_crGlyphArrow)
						memDC.SetPixel(x, y+1, CPWGraphics::Blend(memDC.GetPixel(x, y+1), cdShadow, iPerShadow));

				}
			}
		}
	}

	int iCurX; // = m_bShowBaseImg ? 3 : 4;

	int iContCx, iContCy;
	int iImgX, iImgY,
		iCodX, iCodY,
		iTxtX, iTxtY;
	
	if(m_iCurSel != -1)
	{
		CItemData *pitem = (CItemData*)m_items[m_iCurSel];

		if(pitem->pbmImg && m_bShowBaseImg)
		{
			iCurX = 3;
			bmDC.CreateCompatibleDC(&memDC);

			pitem->pbmImg->GetBitmap(&bm);

			CBitmap *oldibm = bmDC.SelectObject(pitem->pbmImg);

			memDC.BitBlt(iCurX, wrc.top + (wrc.Height() / 2 - bm.bmHeight / 2), bm.bmWidth, bm.bmHeight, 
				&bmDC, 0, 0, SRCCOPY);

			bmDC.SelectObject(oldibm);
			bmDC.DeleteDC();

			iCurX += bm.bmWidth + 4;
		}
		else
			iCurX = 4;

		memDC.SetBkMode(TRANSPARENT);
		memDC.SetTextColor(m_crText);
		m_font.AttachDC(&memDC);

		if(!pitem->strCode.IsEmpty() && m_bShowBaseCode)
		{
			m_font.SetFontBold();
			memDC.DrawText(pitem->strCode, CRect(iCurX, wrc.top, wrc.right, wrc.bottom), DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			iCurX += m_font.GetTextWidth(pitem->strCode) + 4;
			m_font.SetFontBold(FALSE);
		}

		if(!pitem->strText.IsEmpty() && m_bShowBaseText)
		{
			memDC.DrawText(pitem->strText, CRect(iCurX, wrc.top, wrc.right, wrc.bottom), DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}

		m_font.DetachDC();
	}
	else if(m_items.GetCount() == 0 || m_bShowTextOnNoSel)
	{
		CString strText;
		GetWindowText(strText);

		if(m_pImg) m_pImg->GetBitmap(&bm);

		if(m_pImg && !strText.IsEmpty())
		{
			iContCx = bm.bmWidth + m_font.GetTextWidth(strText) + 4;
			iContCy = max(bm.bmHeight, m_font.GetFontHeight());

			if(m_align.IsLeft())
			{
				iImgX = 3;
				iTxtX = iImgX + bm.bmWidth + 4;
			}
			else if(m_align.IsRight())
			{
				iTxtX = wrc.right - m_font.GetTextWidth(strText) - 4;
				iImgX = iTxtX - bm.bmWidth - 4;
			}
			else if(m_align.IsCenter() || m_align.IsHorAbsolute())
			{
				iImgX = wrc.Width() / 2 - iContCx / 2;
				iTxtX = iImgX + bm.bmWidth + 4;
			}

			if(bm.bmHeight >= m_font.GetFontHeight())
			{
				iImgY = 3;
				iTxtY = iImgY + (bm.bmHeight / 2 - m_font.GetFontHeight() / 2);
			}
			else
			{
				iTxtY = 3;
				iImgY = iTxtY; // + (m_font.GetFontHeight() / 2 - bm.bmHeight / 2);
			}

			if(m_align.IsBottom())
			{
				iImgY = wrc.bottom - iImgY - bm.bmHeight; // iContCy;
				iTxtY = wrc.bottom - iTxtY - m_font.GetFontHeight(); //iContCy;
			}
			else if(m_align.IsMiddle() || m_align.IsVerAbsolute())
			{
				iImgY = wrc.Height() / 2 - bm.bmHeight / 2;
				iTxtY = wrc.Height() / 2 - m_font.GetFontHeight() / 2;
			}
		}
		else
		{
			iContCx = m_pImg ? bm.bmWidth  : m_font.GetTextWidth(strText);
			iContCy = m_pImg ? bm.bmHeight : m_font.GetFontHeight();

			if(m_align.IsLeft())
				iTxtX = m_pImg ? 3 : 4;
			else if(m_align.IsRight())
				iTxtX = wrc.right - iContCx - (m_pImg ? 3 : 4);
			else if(m_align.IsCenter() || m_align.IsHorAbsolute())
				iTxtX = wrc.Width() / 2 - iContCx / 2;

			iTxtY = m_pImg ? 3 : 4;

			if(m_align.IsBottom())
				iTxtY = wrc.bottom - iTxtY - iContCy;
			else if(m_align.IsMiddle() || m_align.IsVerAbsolute())
				iTxtY = wrc.Height() / 2 - iContCy / 2;

			if(m_pImg)
			{
				iImgX = iTxtX;
				iImgY = iTxtY;
			}
		}

		if(m_pImg)
		{
			//iCurX = 3;
			bmDC.CreateCompatibleDC(&memDC);

			CBitmap *oldibm = bmDC.SelectObject(m_pImg);

			memDC.TransparentBlt(/*iCurX*/ iImgX, iImgY /*wrc.top + (wrc.Height() / 2 - bm.bmHeight / 2)*/,
				bm.bmWidth, bm.bmHeight, 
				&bmDC, 0, 0, bm.bmWidth, bm.bmHeight, m_crImgTrans);

			bmDC.SelectObject(oldibm);
			bmDC.DeleteDC();

			//iCurX += bm.bmWidth + 4;
		}
		//else
		//	iCurX = 4;

		if(!strText.IsEmpty())
		{
			memDC.SetBkMode(TRANSPARENT);
			memDC.SetTextColor(m_crText);

			BOOL bFontItalic = m_font.GetFontItalic();

			if(m_items.GetCount() > 0)
				m_font.SetFontItalic();

			m_font.AttachDC(&memDC);

			memDC.DrawText(strText, CRect(/*iCurX*/ iTxtX, /*wrc.top*/ iTxtY, wrc.right, iTxtY + m_font.GetFontHeight() /*wrc.bottom*/), DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			m_font.DetachDC();

			if(m_items.GetCount() > 0)
				m_font.SetFontItalic(bFontItalic);

		}
	}

	memDC.SelectObject(oldbm);

	if(bRedraw) RedrawWindow();
}

bool CMWRatesCmb::CreateBtnBmp(void)
{
	CRect wrc;

	if(!IsWindow(m_hWnd)) return false;

	GetWindowRect(wrc);

	CDC *pDC = GetDC();

	m_bmWnd.DeleteObject();
	m_bmWnd.CreateCompatibleBitmap(pDC, wrc.Width(), wrc.Height());

	ReleaseDC(pDC);

	return m_bmWnd.m_hObject != NULL;
}

int CMWRatesCmb::AddItem(LPCTSTR lpszText, UINT uiImg /*= 0*/)
{
	return AddItemCode(NULL, lpszText, uiImg);
}

int CMWRatesCmb::AddItemCode(LPCTSTR lpszCode, LPCTSTR lpszText, UINT uiImg /*= 0*/)
{
	CItemData *pitem = new CItemData(lpszText, lpszCode, uiImg);
	BITMAP bm;

	int nOffset = (int)m_items.Add(pitem);

	if(uiImg)
	{
		pitem->pbmImg->GetBitmap(&bm);

		m_nMaxImgWidth = max(m_nMaxImgWidth, bm.bmWidth);
		m_nMaxImgHeight = max(m_nMaxImgHeight, bm.bmHeight);
	}

	m_cDropList.BuildBkBitmaps();

	return nOffset;
}

int CMWRatesCmb::AddItemCode(LPCTSTR lpszCode, UINT uiImg /*= 0*/)
{
	return AddItemCode(lpszCode, NULL, uiImg);
}


void CMWRatesCmb::OnDestroy()
{
	CWnd::OnDestroy();
	KillTimer(1);

	//m_cDropList.HideList();
	//m_cDropList.PostMessage(WM_CLOSE);
	m_cDropList.DestroyWindow();
}


void CMWRatesCmb::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	DropList();
	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CMWRatesCmb::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_LBUTTONDOWN)
	{
		if(m_bIsListDown) 
		{
			m_bIsListDown = false;
			return FALSE;
		}
	}

	if(pMsg->message == WM_KEYDOWN)
	{
		if(m_bIsListDown)
		{
			m_cDropList.PostMessage(WM_KEYDOWN, (WPARAM)pMsg->wParam, (LPARAM)pMsg->lParam);
			m_cDropList.SetCapture();
			return TRUE;
		}
		else
		{
			switch(pMsg->wParam)
			{
			case VK_RETURN:
			case VK_SPACE:
				m_state.SetPressed();
				DrawBtnBmp(true);
				DropList(false);
				if(GetParent())
				{
					GetParent()->PostMessage(WM_COMMAND, 
						MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), 
						(LPARAM)GetSafeHwnd());
				}
				return TRUE;
			case VK_DOWN:
			case VK_RIGHT:
				if( m_iCurSel < (GetItemsCount()-1)) SetCurSel(m_iCurSel+1);
				return TRUE;
			case VK_UP:
			case VK_LEFT:
				if( m_iCurSel > 0) SetCurSel(m_iCurSel-1);
				return TRUE;
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CMWRatesCmb::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	m_cDropList.HideList();
	m_state.SetFocused(false);
	OnTimer(1);
	DrawBtnBmp(true);
}

void CMWRatesCmb::SetCurSel(int idx, BOOL bRedraw /*= TRUE*/)
{
	m_iCurSel = idx;
	DrawBtnBmp(bRedraw != FALSE);

	if(GetParent() && m_items.GetCount() > 0)
	{
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM)GetSafeHwnd());
	}
}

void CMWRatesCmb::SetCurSelByCode(LPCTSTR lpszCode)
{
	CItemData *pitem;

	for(int i=0; i<m_items.GetCount(); i++)
	{
		pitem = (CItemData*)m_items[i];
		if(pitem->strCode.CompareNoCase(lpszCode) == 0)
		{
			SetCurSel(i);
			return;
		}
	}
}

// properties

COLORREF CMWRatesCmb::GetBaseColor(void) { return m_crBase; }
void CMWRatesCmb::SetBaseColor(COLORREF color, bool bFixBorderColor /*= true*/)
{
	m_crBase = color;

	if(bFixBorderColor)
	{
		m_crBorder = CPWGraphics::OffsetColor(m_crBase, 0x8);
		m_crBorder = CPWGraphics::OffsetColor(m_crBorder, -0x60);
	}
}

COLORREF CMWRatesCmb::GetTextColor(void) { return m_crText; }
void CMWRatesCmb::SetTextColor(COLORREF color, bool bFixGlyphColor /*= true*/)
{
	m_crText = color;
	if(bFixGlyphColor) SetGlyphArrowColor(m_crText);
}

COLORREF CMWRatesCmb::GetHotColor(void) { return m_crHot; }
void CMWRatesCmb::SetHotColor(COLORREF color) { m_crHot = color; }


COLORREF CMWRatesCmb::GetBorderColor(void) { return m_crBorder; }
void CMWRatesCmb::SetBorderColor(COLORREF color) { m_crBorder = color; }

COLORREF CMWRatesCmb::GetGlyphArrowColor(void) { return m_crGlyphArrow; }
void CMWRatesCmb::SetGlyphArrowColor(COLORREF color)
{
	m_crGlyphArrow = color;

	m_glyph.DeleteObject();
	m_glyph.LoadBitmap(IDB_SMALLGLYPH);
	CPWGraphics::BlitReplaceBitmapColor((HBITMAP)m_glyph.GetSafeHandle(), 0xFFFF00, m_crGlyphArrow);
}

void CMWRatesCmb::SetShowBaseText(bool bSet /*= true*/) { m_bShowBaseText = bSet; }
bool CMWRatesCmb::GetShowBaseText(void) { return m_bShowBaseText; }

void CMWRatesCmb::SetShowBaseCode(bool bSet /*= true*/) { m_bShowBaseCode = bSet; }
bool CMWRatesCmb::GetShowBaseCode(void) { return m_bShowBaseCode; }

void CMWRatesCmb::SetShowBaseImg(bool bSet /*= true*/) { m_bShowBaseImg = bSet; }
bool CMWRatesCmb::GetShowBaseImg(void) { return m_bShowBaseImg; }

UINT CMWRatesCmb::GetResImg(void) { return m_uiImg; }
void CMWRatesCmb::SetResImg(UINT uiImg, COLORREF crTrans /*= 0xFF00FF*/)
{
	m_uiImg = uiImg;
	m_crImgTrans = crTrans;

	if(m_pImg)
	{
		m_pImg->DeleteObject();
		delete m_pImg;
	}

	m_pImg = new CBitmap();
	m_pImg->LoadBitmap(uiImg);
}

COLORREF CMWRatesCmb::GetTransColor1(void) { return m_crTrans1; }
COLORREF CMWRatesCmb::GetTransColor2(void) { return m_crTrans2; }


void CMWRatesCmb::CDropList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	POINT pt;

	switch(nChar)
	{
	case VK_DOWN:
	case VK_RIGHT:
		GetCursorPos(&pt);
		m_ptKey.SetPoint(pt.x, pt.y);
		if( m_iItemSel < (m_pCmb->GetItemsCount()-1))
		{
			SelectItem(m_iItemSel+1);
			m_pCmb->SetCurSel(m_iItemSel);
		}
		break;
	case VK_UP:
	case VK_LEFT:
		GetCursorPos(&pt);
		m_ptKey.SetPoint(pt.x, pt.y);
		if( m_iItemSel > 0)
		{
			SelectItem(m_iItemSel -1);
			m_pCmb->SetCurSel(m_iItemSel);
		}
		break;
	case VK_RETURN:
	case VK_SPACE:
		m_pCmb->SetCurSel(m_iItemSel);
		HideList();
		break;
	case VK_ESCAPE:
		HideList();
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMWRatesCmb::DropList(bool bRunTimer /*= true*/)
{
	CRect rc;

	if(m_bIsListDown || m_items.GetCount() == 0) return;

	GetClientRect(rc);
	ClientToScreen((LPPOINT)&rc);

	if(bRunTimer) OnTimer(1);
	m_cDropList.SelectItem(m_iCurSel);
	m_cDropList.ShowList();
}

void CMWRatesCmb::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	m_state.SetFocused();
	DrawBtnBmp(true);
}

void CMWRatesCmb::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(GetParent())
	{
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMWRatesCmb::CDropList::OnLButtonUp(UINT nFlags, CPoint point)
{
	POINT pt;

	if(GetTickCount() - m_dwTick < 50) return;

	GetCursorPos(&pt);
	if(::GetCapture() == m_hWnd && ::WindowFromPoint(pt) == m_hWnd)
	{
		m_pCmb->SetCurSel(m_iItemSel);
		HideList();
	}

	__super::OnLButtonUp(nFlags, point);
}
