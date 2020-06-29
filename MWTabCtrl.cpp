// MWTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include ".\Bineye.h"
#include ".\mwtabctrl.h"


// CMWTabCtrl

IMPLEMENT_DYNAMIC(CMWTabCtrl, CWnd)
CMWTabCtrl::CMWTabCtrl(COLORREF crDefBack /*= -1*/)
: m_nTabDistCx(-9)
, m_bInit(false)
, m_iCurSel(-1)
, m_iCurHot(-1)
, m_nBmButsCy(-1)
, m_nBmPanelCy(-1)
, m_crTrans1(0xFF00FF)
, m_crTrans2(0xFFFF00)
, m_bDrawPanel(true)
, m_pParentBmp(NULL)
{
	m_font.CreateStockObject(DEFAULT_GUI_FONT);
	m_font.SetFontFace(CString(_T("MS Shell Dlg 2")));
	m_font.SetFontPointSize(8);
	
	RegisterWindowClass();

	m_crTransBack = GetSysColor(COLOR_BTNFACE);

	if(int(crDefBack) == -1)
		m_crDefBack = GetSysColor(COLOR_BTNFACE);
	else
		m_crDefBack = crDefBack;
}

CMWTabCtrl::~CMWTabCtrl()
{
	CMWTabBtn *pbut;

	for(int i=0; i<m_buts.GetCount(); i++)
	{
		pbut = (CMWTabBtn*)m_buts[i];
		delete pbut;
	}

	m_buts.RemoveAll();
}


BEGIN_MESSAGE_MAP(CMWTabCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CMWTabCtrl message handlers

BOOL CMWTabCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, MWTABCTRL_CLASSNAME, &wndcls)) )
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = MWTABCTRL_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CMWTabCtrl::Create(DWORD dwStyle, CRect& rect, CWnd* pParent, UINT nID)
{
	RECT wrc;

	CopyRect(&wrc, rect);

	return CWnd::Create(MWTABCTRL_CLASSNAME, _T(""), dwStyle, wrc, pParent, nID );
}

void CMWTabCtrl::OnPaint()
{
	if(!m_bInit)
	{
		Init();
		m_bInit = true;
	}

	CPaintDC dc(this);
	CDC memDC;
	CRect wrc;

	GetWindowRect(wrc);

	// draw buttons bitmap

	memDC.CreateCompatibleDC(&dc);

	CBitmap *oldbm = memDC.SelectObject(&m_bmButs);
	//dc.TransparentBlt(0, 0, wrc.Width(), m_nBmButsCy, &memDC, 0, 0, wrc.Width(), m_nBmButsCy, m_crTrans1);
	dc.BitBlt(0, 0, wrc.Width(), wrc.Height(), &memDC, 0, 0, SRCCOPY);

	
	// draw panel bitmap

	if(m_bDrawPanel)
	{
		memDC.SelectObject(&m_bmPanel);
		//dc.TransparentBlt(0, m_nBmButsCy, wrc.Width(), m_nBmPanelCy, &memDC, 0, 0, wrc.Width(), m_nBmPanelCy,  m_crTrans1);
		dc.BitBlt(0, m_nBmButsCy, wrc.Width(), wrc.Height(), &memDC, 0, 0, SRCCOPY);

		//if(m_iCurSel != -1)
		//{
		//	if(((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg)
		//		((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg->RedrawWindow();
		//}
	}

	m_bDrawPanel = true;
	
	memDC.SelectObject(oldbm);
	memDC.DeleteDC();
}

int CMWTabCtrl::AddTab(LPCTSTR lpszText, COLORREF crTabColor /*= -1*/, CDialog *pDlg /*= NULL*/)
{
	int iRet = -1;
	CMWTabBtn *pbtn = new CMWTabBtn();

	pbtn->m_strText = lpszText;
	pbtn->m_crBack = crTabColor;
	pbtn->m_pDlg = pDlg;
	
	iRet = (int)m_buts.Add(pbtn);
	CreateTabRgn(iRet);

	if(IsWindow(m_hWnd))
	{
		CreateBkBitmaps();
		FixDlgPos(pDlg);
		DrawButsBmp();
		DrawPanelBmp();
	}

	if(m_iCurSel == -1) SelectTab(iRet, false);

	return iRet;
}

int CMWTabCtrl::AddTab(LPCTSTR lpszText, CDialog *pDlg)
{
	return AddTab(lpszText, -1, pDlg);
}

void CMWTabCtrl::CreateTabRgn(int iTab)
{
	CRect brc;
	int xsv, x, y, cx, cy;
	CMWTabBtn *pbut = ((CMWTabBtn*)m_buts[iTab]);

	m_font.SetFontBold();
	cx = m_font.GetRealTextExtent(pbut->m_strText).cx + 4;
	cy = m_font.GetRealTextExtent(pbut->m_strText).cy + 4;
	m_font.SetFontBold(FALSE);

	if(iTab > 0)
	{
		((CMWTabBtn*)m_buts[iTab-1])->m_rgn.GetRgnBox(brc);
		x = xsv = brc.right + m_nTabDistCx;
	}
	else
		x = xsv = 4;

	for(y=cy; y>=0; y--)
	{
		if(y==cy || y==2 || y==1)
		{
			if(y==1) pbut->m_iTextX = x;
			if(y==cy) pbut->m_ptLB.SetPoint(x, y);
			pbut->m_pts.Add(CPoint(x++, y));
			pbut->m_pts.Add(CPoint(x++, y));
			pbut->m_bkpts.Add(CMWTabBtn::xAxis(x, 0, y));
		}
		else if(y==0)
		{
			pbut->m_pts.Add(CPoint(x++, y));
			pbut->m_pts.Add(CPoint(x++, y));
		}
		else
		{
			pbut->m_pts.Add(CPoint(x++, y));
			pbut->m_bkpts.Add(CMWTabBtn::xAxis(x, 0, y));
		}
	}

	pbut->m_pts.Add(CPoint(x+=cx, ++y));
	pbut->m_pts.Add(CPoint(x+=2, y+=2));
	pbut->m_pts.Add(CPoint(x, cy));
	pbut->m_pts.Add(CPoint(xsv, cy));

	for(int i=0; i<pbut->m_bkpts.GetCount(); i++)
	{
		pbut->m_bkpts[i].x2 = i==pbut->m_bkpts.GetCount()-1 ? x-1 : x;
	}

	pbut->m_ptRB.SetPoint(x, cy);

	pbut->m_rgn.CreatePolygonRgn((LPPOINT)pbut->m_pts.GetData(), (int)pbut->m_pts.GetCount(), ALTERNATE);
	
	CArray<CPoint, CPoint> pts;

	pts.Add(CPoint(xsv, cy+1));
	pts.Add(CPoint(x+1, cy+1));
	pts.Add(CPoint(x+1, 2));
	pts.Add(CPoint(x, 1));
	pts.Add(CPoint(x-1, 0));
	pts.Add(CPoint(x-2, 0));
	pts.Add(CPoint(xsv, cy+1));

	pbut->m_rgnchk.CreatePolygonRgn((LPPOINT)pts.GetData(), (int)pts.GetCount(), ALTERNATE);
	pbut->m_rgnchk.CombineRgn(&pbut->m_rgnchk, &pbut->m_rgn, RGN_OR);
}

void CMWTabCtrl::OnTimer(UINT nIDEvent)
{
	int i, tset;
	CString strText;
	CMWTabBtn *pbut;

	CRect rc;
	POINT pt;

	GetCursorPos(&pt);
	BOOL bIsPointWindow = ::WindowFromPoint(pt) == m_hWnd;

	GetWindowRect(rc);
	GetCursorPos(&pt);

	if(rc.PtInRect(pt) && bIsPointWindow)
	{
		tset = -1;
		ScreenToClient(&pt);


		if(m_iCurSel != -1)
		{
			pbut = (CMWTabBtn*)m_buts[m_iCurSel];

			if(pbut->m_rgnchk.PtInRegion(pt))
				tset = m_iCurSel;
		}

		if(tset == -1)
		{
			for(i=0; i<m_buts.GetCount(); i++)
			{
				if(i != m_iCurSel)
				{
					pbut = (CMWTabBtn*)m_buts[i];

					if(pbut->m_rgnchk.PtInRegion(pt))
					{
						tset = i;
						break;
					}
				}
			}
		}

		if(m_iCurHot != tset)
		{
			SetHotTab(tset);
		}
	}
	else if(m_iCurHot != -2)
	{
		SetHotTab(-2);
	}

	CWnd::OnTimer(nIDEvent);
}

void CMWTabCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	if(m_bInit)
	{
		KillTimer(MWTABCTRL_TIMEREVENT);
	}

	m_font.DeleteObject();
}

void CMWTabCtrl::DrawBtnBk(CDC *pDC, int iTab)
{
	CMWTabBtn *pbut;
	//CRect rc, rcreal;
	//CRgn rgnof;
	//CDC maskDC;
	//CDC finalDC;
	//CBitmap maskBmp;
	//CBitmap finalBmp;
	//CBrush pink, cyan;

	pbut = (CMWTabBtn*)m_buts[iTab];
	CMWTabBtn::xAxis xax;

	COLORREF coluse = GetBtnColor(pbut);
	COLORREF colEnd = CPWGraphics::DifColor(coluse, 86);
	COLORREF colorG;
	int r, g, b;
	int r1, g1, b1;
	int r2, g2, b2;
	int nHeight = (int)pbut->m_bkpts.GetCount();

	r1 = GetRValue(colEnd);
	g1 = GetGValue(colEnd);
	b1 = GetBValue(colEnd);

	r2 = GetRValue(coluse);
	g2 = GetGValue(coluse);
	b2 = GetBValue(coluse);

	int iRefAmount = pbut->m_state.IsHot() ? 23 : 35;
	int x, i, ist = pbut->m_state.IsPressed() ? 1 : 0;

	for(i=0, x=(int)pbut->m_bkpts.GetCount()-1; i<=pbut->m_bkpts.GetCount(); i++, x--)
	{
		r = r1 + (i * (r2-r1) / nHeight);
		g = g1 + (i * (g2-g1) / nHeight);
		b = b1 + (i * (b2-b1) / nHeight);

		if(i == nHeight) break;

		xax = (CMWTabBtn::xAxis)pbut->m_bkpts[x];

		if (i < nHeight / 2) {
			colorG = CPWGraphics::OrColor(RGB(r, g, b), 0xffffff, (iRefAmount + 5 * i) > 100 ?
				100 : (iRefAmount + 5 * i));
		} else  {
			colorG = RGB(r, g, b);
		}

		CPWGraphics::DrawLine(pDC->GetSafeHdc(), xax.x1, xax.y, xax.x2, xax.y, colorG);

		coluse = pDC->GetPixel(xax.x2-1, xax.y);
		coluse = CPWGraphics::Blend(0xffffff, coluse, 60);

		if(x >= ist) pDC->SetPixel(xax.x2-1, xax.y, coluse);
	}

	if(!pbut->m_state.IsHot()) pbut->m_crFinishBack = RGB(r, g, b);


	/* this code was simple draw the bk by using CPWGraphics::DrawGlassRect
	   on a mask created by the btn region. unfortunately it throws an assertion
	   running on win98 after some hovers or some tab selections and I did not figure
	   out why (!?) so i desided to save all x1, x2 axis of the background 
	   and paint lines by using the points...

	rgnof.CreateRectRgn(0, 0, 1, 1);
	rgnof.CopyRgn(&pbut->m_rgn);

	rgnof.GetRgnBox(rc);
	pbut->m_rgn.GetRgnBox(rcreal);

	rgnof.OffsetRgn(-rc.left, 0);
	rgnof.GetRgnBox(rc);

	pink.CreateSolidBrush(m_crTrans1);
	cyan.CreateSolidBrush(m_crTrans2);

	maskDC.CreateCompatibleDC(NULL);
	finalDC.CreateCompatibleDC(NULL);

	maskBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	finalBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

	CBitmap *oldmask = maskDC.SelectObject(&maskBmp);
	CBitmap *oldfinal = finalDC.SelectObject(&finalBmp);

	maskDC.FillRect(rc, &pink);
	maskDC.FillRgn(&rgnof, &cyan);

	COLORREF coluse = GetBtnColor(pbut);
	int iRefAmount = pbut->m_state.IsHot() ? 23 : 35;

	COLORREF colFinish = CPWGraphics::DrawGlassRect((CDC*)&finalDC, rc, coluse, iRefAmount);
	if(!pbut->m_state.IsHot()) pbut->m_crFinishBack = colFinish;

	finalDC.TransparentBlt(0, 0, rc.Width(), rc.Height(), &maskDC, 0, 0, rc.Width(), rc.Height(), m_crTrans2); 

	int x,y, imax = pbut->m_state.IsPressed() ? 16 : 17;

	for(int i=0; i<imax; i++)
	{
		x = i==0 ? rc.right-2 : rc.right-1;
		y = rc.top + i;

		coluse = finalDC.GetPixel(x-1, y);
		coluse = CPWGraphics::Blend(0xffffff, coluse, 60);

		finalDC.SetPixel(x, y, coluse);
	}

	pDC->TransparentBlt(rcreal.left+0, rcreal.top+1, rc.Width(), rc.Height(), &finalDC, 0, 0, rc.Width(), rc.Height(), m_crTrans1);

	rgnof.DeleteObject();

	finalDC.SelectObject(oldfinal);
	maskDC.SelectObject(oldmask);

	finalBmp.DeleteObject();
	maskBmp.DeleteObject();

	pink.DeleteObject();
	cyan.DeleteObject();

	finalDC.DeleteDC();
	maskDC.DeleteDC();
	
	*/
}

void CMWTabCtrl::Init(void)
{
	SetTimer(MWTABCTRL_TIMEREVENT, 10, NULL);
	CreateBkBitmaps();
	DrawButsBmp();
	DrawPanelBmp();
}

void CMWTabCtrl::CreateBkBitmaps(void)
{
	CRect wrc;
	CDC *pDC;

	m_nBmButsCy = -1;
	m_nBmPanelCy = -1;

	if(!IsWindow(m_hWnd)) return;

	GetWindowRect(wrc);

	m_nBmPanelCy = wrc.Height();

	if(m_buts.GetCount() > 0)
	{
		m_nBmButsCy = m_font.GetRealTextExtent(_T("Just a string")).cy + 5;
		m_nBmPanelCy -= m_nBmButsCy;
	}

	pDC = GetDC();

	if(m_nBmButsCy != -1)
	{
		m_bmButs.DeleteObject();
		m_bmButs.CreateCompatibleBitmap(pDC, wrc.Width(), m_nBmButsCy);
	}

	m_bmPanel.DeleteObject();
	m_bmPanel.CreateCompatibleBitmap(pDC, wrc.Width(), m_nBmPanelCy);

	ReleaseDC(pDC);
}

void CMWTabCtrl::DrawButsBmp(void)
{
	CDC *pDC;
	CDC memDC;
	CPoint ppt;
	CMWTabBtn *pbut;
	int nHeight = 20;
	int iButFix, i, x;
	BITMAP bm;
	CPen ppn;
	CRect rc;
	COLORREF crborder;


	if(m_nBmButsCy == -1 || m_buts.GetCount() == 0) return;

	m_bmButs.GetBitmap(&bm);

	pDC = GetDC();
	memDC.CreateCompatibleDC(pDC);
	
	CBitmap *oldbm = memDC.SelectObject(&m_bmButs);

	if(m_pParentBmp)
	{
		CDC parDC;

		GetWindowRect(rc);
		GetParent()->ScreenToClient(rc);

		parDC.CreateCompatibleDC(pDC);
		CBitmap *oldbm = parDC.SelectObject(m_pParentBmp);
		memDC.BitBlt(0, 0, rc.Width(), m_nBmButsCy, &parDC, rc.left, rc.top, SRCCOPY);

		parDC.SelectObject(oldbm);
		parDC.DeleteDC();
	}
	else
	{
		memDC.FillSolidRect(0, 0, bm.bmWidth, bm.bmHeight, m_crTransBack); //m_crTrans1);
	}

	ReleaseDC(pDC);

	//GetWindowRect(rc);
	//GetParent()->ScreenToClient(rc);
	//rc.bottom -= m_nBmPanelCy;


	//CDC *ppDC = GetParent()->GetDC();
	//memDC.BitBlt(0, 0, rc.Width(), rc.Height(), ppDC, rc.left, rc.top, SRCCOPY);
	//GetParent()->ReleaseDC(ppDC);



	for(i=int(m_buts.GetCount())-1; i >= -1; i--)
	{
		if(i==m_iCurSel)
			continue;
		else if(i==-1)
		{
			if(m_iCurSel==-1)
				break;
			else
				iButFix = m_iCurSel;
		}
		else
			iButFix = i;

		pbut = (CMWTabBtn*)m_buts[iButFix];

		DrawBtnBk(&memDC, iButFix);

		crborder = CPWGraphics::OffsetColor(GetBtnColor(pbut), 0x8);
		crborder = CPWGraphics::OffsetColor(crborder, -0x60);
		ppn.CreatePen(PS_SOLID, 1, crborder);
		CPen *oldpen = memDC.SelectObject(&ppn);

		ppt = (CPoint)pbut->m_pts[0];
		memDC.MoveTo(ppt.x, ppt.y);

		for(x=1; x<pbut->m_pts.GetCount(); x++)
		{
			ppt = (CPoint)pbut->m_pts[x];
			if(ppt != pbut->m_ptLB)
			{
				if(ppt == pbut->m_ptRB) ppt.y++;
				memDC.LineTo(ppt.x, ppt.y+0);
			}
		}

		memDC.SelectObject(oldpen);
		ppn.DeleteObject();

		if(!pbut->m_strText.IsEmpty())
		{
			memDC.SetBkMode(TRANSPARENT);
			pbut->m_rgn.GetRgnBox(rc);

			m_font.AttachDC(&memDC);
			if(pbut->m_state.IsPressed()) m_font.SetFontBold();

			COLORREF crtext = CPWGraphics::Blend(GetBtnColor(pbut), 0x0, 70);

			memDC.SetTextColor(crtext);
			memDC.DrawText(pbut->m_strText, CRect(pbut->m_iTextX , rc.top, rc.right, rc.bottom), DT_CENTER|DT_SINGLELINE|DT_VCENTER);

			//CBitmap *pMemBmp = memDC.GetCurrentBitmap();
			//BITMAP bmMem;

			//pMemBmp->GetBitmap(&bmMem);

			int iPerShadow = 40;
			COLORREF cdShadow = 0xffffff;

			for(int y=rc.top; y<rc.bottom; y++)
			{
				for(x=pbut->m_iTextX; x<rc.right; x++)
				{
					if(memDC.GetPixel(x, y) == crtext)
					{
						if(memDC.GetPixel(x-1, y) != crtext)
							memDC.SetPixel(x-1, y, CPWGraphics::Blend(memDC.GetPixel(x-1, y), cdShadow, iPerShadow));

						if(memDC.GetPixel(x+1, y) != crtext)
							memDC.SetPixel(x+1, y, CPWGraphics::Blend(memDC.GetPixel(x+1, y), cdShadow, iPerShadow));

						if(memDC.GetPixel(x, y-1) != crtext)
							memDC.SetPixel(x, y-1, CPWGraphics::Blend(memDC.GetPixel(x-1, y-1), cdShadow, iPerShadow));

						if(memDC.GetPixel(x, y+1) != crtext)
							memDC.SetPixel(x, y+1, CPWGraphics::Blend(memDC.GetPixel(x, y+1), cdShadow, iPerShadow));
					}
				}
			}

			if(pbut->m_state.IsPressed()) m_font.SetFontBold(FALSE);
			m_font.DetachDC();
		}
	}

	memDC.SelectObject(oldbm);
	memDC.DeleteDC();
}

void CMWTabCtrl::DrawPanelBmp(void)
{
	CDC *pDC;
	CDC memDC;
	CMWTabBtn *pbut;
	CRect wrc;
	COLORREF colBk, colTmp;


	if(!IsWindow(m_hWnd)) return;

	GetWindowRect(wrc);
	ScreenToClient(wrc);

	//wrc.bottom -= m_nBmButsCy;

	int l = wrc.left,
		t = wrc.top,
		r = wrc.right,
		b = wrc.bottom - m_nBmButsCy;

	pDC = GetDC();
	memDC.CreateCompatibleDC(pDC);
	CBitmap *oldbm = memDC.SelectObject(&m_bmPanel);
	
	HDC hDC = memDC.GetSafeHdc();

	//memDC.FillSolidRect(0, 0, wrc.Width(), m_nBmPanelCy, m_crTrans1);

	CDC parDC;

	if(m_pParentBmp)
	{
		GetWindowRect(wrc);
		GetParent()->ScreenToClient(wrc);
		wrc.top += m_nBmButsCy;

		parDC.CreateCompatibleDC(pDC);
		CBitmap *oldbm = parDC.SelectObject(m_pParentBmp);

		memDC.SetPixel(l, t, parDC.GetPixel(wrc.left, wrc.top));
		memDC.SetPixel(l, b-1, parDC.GetPixel(wrc.left, wrc.bottom-1));
		memDC.SetPixel(r-1, t, parDC.GetPixel(wrc.right-1, wrc.top));
		memDC.SetPixel(r-1, b-1, parDC.GetPixel(wrc.right-1, wrc.bottom-1));
	}
	else
	{
		memDC.SetPixel(l, t, m_crTransBack);
		memDC.SetPixel(l, b-1, m_crTransBack);
		memDC.SetPixel(r-1, t, m_crTransBack);
		memDC.SetPixel(r-1, b-1, m_crTransBack);
	}

	ReleaseDC(pDC);

	if(m_iCurSel == -1)
	{
		colBk = m_crDefBack;
		t++;
		b--;
	}
	else
	{
		pbut = (CMWTabBtn*)m_buts[m_iCurSel];
		colBk = GetBtnColor(pbut);
	}

	colTmp = CPWGraphics::OffsetColor(colBk, 0x8);

	CPWGraphics::DrawVGradient(hDC, 
			CPWGraphics::OffsetColor(colTmp,  0xF), 
			CPWGraphics::OffsetColor(colTmp, -0xF), 
			l+1 /*1*/, t+1 /*1*/, r-2 /*cx - 2*/, b-3 /*cy - 3*/);

	CPWGraphics::DrawVGradient(hDC, 
			CPWGraphics::OffsetColor(colTmp,  0x15), 
			CPWGraphics::OffsetColor(colTmp, -0x15), 
			l+1 /*1*/, t+2 /*2*/, l+2 /*2*/, b-5 /*cy - 5*/);

	CPWGraphics::DrawVGradient(hDC, 
			CPWGraphics::OffsetColor(colTmp, -0x5), 
			CPWGraphics::OffsetColor(colTmp, -0x20), 
			r-2 /*cx - 2*/, t+2 /*2*/, r-3 /*cx - 3*/, b-3 /*cy - 5*/);

	colTmp = CPWGraphics::OffsetColor(colTmp, -0x60);

	CPWGraphics::DrawLine(hDC, l+2 /*2*/, t /*0*/, r-2 /*cx - 2*/, t /*0*/, colTmp);
	CPWGraphics::DrawLine(hDC, l /*0*/, t+2 /*2*/, l /*0*/, b-2 /*cy - 2*/, colTmp);
	CPWGraphics::DrawLine(hDC, l+2 /*2*/, b-1 /*cy - 1*/, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);
	CPWGraphics::DrawLine(hDC, r-1 /*cx - 1*/, t+2 /*1*/, r-1 /*cx - 1*/, b-2 /*cy - 1*/, colTmp);

	SetPixel(hDC, l+1 /*1*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, l+1 /*1*/, b-2 /*cy - 2*/, colTmp);
	SetPixel(hDC, r-2 /*cx - 2*/, t+1 /*1*/, colTmp);
	SetPixel(hDC, r-2 /*cx - 2*/, b-2 /*cy - 2*/, colTmp);

	CDC screenDC;
	int nBlendPoints = 8;
	COLORREF parpx;
	POINT parpt;
	POINT ptparent[8] = { { l+1, t   },
						  { r-2, t   },

						  { l  , t+1 },
						  { l  , b-2 },

						  { l+1, b-1 },
						  { r-2, b-1 },

						  { r-1, t+1 },
						  { r-1, b-2 } };

	colTmp = CPWGraphics::OffsetColor(colBk, -0x15);

	for(int px = 0; px < nBlendPoints; px++)
	{
		if(m_pParentBmp)
		{
			parpt.x = wrc.left + ptparent[px].x;
			parpt.y = wrc.top +  ptparent[px].y;

			parpx = parDC.GetPixel(parpt);
			SetPixel(hDC, ptparent[px].x, ptparent[px].y, CPWGraphics::Blend(parpx, colBk, 30));
		}
		else
		{
			colTmp = CPWGraphics::OffsetColor(m_crTransBack, -0x15);
			SetPixel(hDC, ptparent[px].x, ptparent[px].y, CPWGraphics::OffsetColor(parpx, -0x15));
		}
	}

	if(m_pParentBmp)
		parDC.DeleteDC();

	//SetPixel(hDC, l+1 /*1*/, t /*0*/, colTmp);
	//SetPixel(hDC, r-2 /*cx - 2*/, t /*0*/, colTmp);

	//SetPixel(hDC, l /*0*/, t+1 /*1*/, colTmp);
	//SetPixel(hDC, l /*0*/, b-2 /*cy - 2*/, colTmp);

	//SetPixel(hDC, l+1 /*1*/, b-1 /*cy - 1*/, colTmp);
	//SetPixel(hDC, r-2 /*cx - 2*/, b-1 /*cy - 1*/, colTmp);

	//SetPixel(hDC, r-1 /*cx - 1*/, t+1 /*1*/, colTmp);
	//SetPixel(hDC, r-1 /*cx - 1*/, b-2 /*cy - 2*/, colTmp);

	CPWGraphics::DrawLine(hDC, l+2 /*2*/, t+1 /*1*/, r-2 /*cx - 2*/, t+1 /*1*/, CPWGraphics::OffsetColor(colBk, 0x15));
	CPWGraphics::DrawLine(hDC, l+2 /*2*/, b-2 /*cy - 2*/, r-2 /*cx - 2*/, b-2 /*cy - 2*/, CPWGraphics::OffsetColor(colBk, -0x25));

	if(m_iCurSel != -1)
	{
		CPWGraphics::DrawLine(hDC, pbut->m_ptLB.x, t, pbut->m_ptRB.x+1, t,
			pbut->m_crFinishBack);
	}

}

int CMWTabCtrl::SelectTab(int nNewSel, bool bReDraw /*= true*/)
{
	if(m_iCurSel == nNewSel) return nNewSel;

	if(m_iCurSel >= 0)
	{
		((CMWTabBtn*)m_buts[m_iCurSel])->m_state.BitPressed(false);
		if(((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg)
			((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg->ShowWindow(SW_HIDE);
	}

	m_iCurSel = nNewSel;

	if(nNewSel >= 0)
	{
		((CMWTabBtn*)m_buts[nNewSel])->m_state.BitPressed();
	}

	DrawButsBmp();
	DrawPanelBmp();

	if(bReDraw)	RedrawWindow(); // Invalidate();

	if(nNewSel >= 0)
		if(((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg)
		{
			((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg->ShowWindow(SW_SHOW);
			((CMWTabBtn*)m_buts[m_iCurSel])->m_pDlg->SetFocus();
		}

	return nNewSel;
}

void CMWTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_iCurHot >= 0) SelectTab(m_iCurHot);
	CWnd::OnLButtonDown(nFlags, point);
}

int CMWTabCtrl::SetHotTab(int iNewHotTab)
{
	if(m_iCurHot == iNewHotTab) return iNewHotTab;

	if(m_iCurHot >= 0)
		((CMWTabBtn*)m_buts[m_iCurHot])->m_state.BitHot(false);

	m_iCurHot = iNewHotTab;

	if(iNewHotTab >= 0)
		((CMWTabBtn*)m_buts[iNewHotTab])->m_state.BitHot();

	//m_bDrawPanel=false;

	DrawButsBmp();
	Invalidate();

	return iNewHotTab;
}

void CMWTabCtrl::SetDefBackColor(COLORREF crDefBack)
{
	m_crDefBack = crDefBack;
	DrawButsBmp();
	DrawPanelBmp();
	Invalidate();
}

COLORREF CMWTabCtrl::GetDefBackColor(void)
{
	return m_crDefBack;
}

COLORREF CMWTabCtrl::GetBtnColor(CMWTabBtn *pbtn)
{
	if(pbtn->m_crBack == -1)
		return m_crDefBack;
	else
		return pbtn->m_crBack;
}

COLORREF CMWTabCtrl::GetBtnColor(int idx)
{
	CMWTabBtn *pbut = m_buts[idx];
	return GetBtnColor(pbut);
}

void CMWTabCtrl::FixDlgPos(CDialog* pDlg)
{
	CRect wrc;

	if(pDlg)
	{
		GetWindowRect(wrc);
		GetParent()->ScreenToClient(wrc);

		wrc.DeflateRect(3, 3);

		if(m_nBmButsCy != -1) wrc.top += m_nBmButsCy;

		pDlg->SetWindowPos(&wndTop, wrc.left, wrc.top, wrc.Width(), wrc.Height(), SWP_HIDEWINDOW);
	}
}

void CMWTabCtrl::FixDlgPos(int idx)
{
	FixDlgPos(((CMWTabBtn*)m_buts[idx])->m_pDlg);
}
BOOL CMWTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return true; //CWnd::OnEraseBkgnd(pDC);
}
