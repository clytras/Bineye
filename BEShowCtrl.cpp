// BEShowCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Bineye.h"
#include "BEShowCtrl.h"


// CBEShowCtrl

//IMPLEMENT_DYNAMIC(CBEShowCtrl, CWnd)

const int CBEShowCtrl::CBEValue::_signed(0x10);
const int CBEShowCtrl::CBEValue::_unsigned(0x20);

const int CBEShowCtrl::CBEValue::bit32(0x1);
const int CBEShowCtrl::CBEValue::bit16(0x2);
const int CBEShowCtrl::CBEValue::bit8(0x3);

const int CBEShowCtrl::CBEValue::i32(0x11);
const int CBEShowCtrl::CBEValue::i16(0x12);
const int CBEShowCtrl::CBEValue::i8(0x13);

const int CBEShowCtrl::CBEValue::ui32(0x21);
const int CBEShowCtrl::CBEValue::ui16(0x22);
const int CBEShowCtrl::CBEValue::ui8(0x23);

const int CBEShowCtrl::CBEValue::low(0x08);
const int CBEShowCtrl::CBEValue::hiw(0x04);
const int CBEShowCtrl::CBEValue::lob(0x01);
const int CBEShowCtrl::CBEValue::hib(0x02);

const int CBEShowCtrl::bigEndian(0x1);
const int CBEShowCtrl::littleEndian(0x2);

const int CBEShowCtrl::CBESCType::type32Bit(1);
const int CBEShowCtrl::CBESCType::type16Bit(2);
const int CBEShowCtrl::CBESCType::type8Bit(3);
const int CBEShowCtrl::CBESCType::typeColor(4);

const int CBEShowCtrl::CClrSliderCtrl::ctRed  (CBEShowCtrl::CBEValue::low | CBEShowCtrl::CBEValue::lob);
const int CBEShowCtrl::CClrSliderCtrl::ctGreen(CBEShowCtrl::CBEValue::low | CBEShowCtrl::CBEValue::hib);
const int CBEShowCtrl::CClrSliderCtrl::ctBlue (CBEShowCtrl::CBEValue::hiw | CBEShowCtrl::CBEValue::lob);
const int CBEShowCtrl::CClrSliderCtrl::ctAlpha(CBEShowCtrl::CBEValue::hiw | CBEShowCtrl::CBEValue::hib);
// Edit controls ---------------------------------------------------------------

BEGIN_MESSAGE_MAP(CBEShowCtrl::CDecEdit, CEdits)
END_MESSAGE_MAP()

CBEShowCtrl::CDecEdit::CDecEdit()
{
	m_bSigned = FALSE;
	m_iBase = 10;
}

BEGIN_MESSAGE_MAP(CBEShowCtrl::CHexEdit, CEdits)
END_MESSAGE_MAP()

CBEShowCtrl::CHexEdit::CHexEdit()
{
	m_crTextColor = 0x9900CC;
	m_iBase = 16;
}

BEGIN_MESSAGE_MAP(CBEShowCtrl::COctEdit, CEdits)
END_MESSAGE_MAP()

CBEShowCtrl::COctEdit::COctEdit()
{
	m_crTextColor = 0x66FF; /*0x66CC;*/
	m_iBase = 8;
}

BEGIN_MESSAGE_MAP(CBEShowCtrl::CBinEdit, CEdits)
END_MESSAGE_MAP()

CBEShowCtrl::CBinEdit::CBinEdit()
{
	m_crTextColor = 0xFFFFFF;
	m_iBase = 2;
}

BEGIN_MESSAGE_MAP(CBEShowCtrl::CHexClrEdit, CEdits)
END_MESSAGE_MAP()

CBEShowCtrl::CHexClrEdit::CHexClrEdit(int iEndian)
: m_iEndian(iEndian)
{
	//m_crTextColor = ((CHexClrEdit*)this)->IsBigEndian() ? /*0xCC9900*/ 0xD553FF : 0x9900CC;
	m_crTextColor = 0xFFFFFF;
	m_iBase = 16;
	m_iByteIndex = -1;
}

// -----------------------------------------------------------------------------

CBEShowCtrl::CBEShowCtrl(CBEBitmaps* pBitmaps)
: m_iType(0)
, m_pBitmaps(pBitmaps)
, m_iSelected(0)
, m_iSUHot(0)
, m_iFuncHot(0)
, m_uiID(0)
, m_uipTimerID(0)
, m_bIsColorInit(false)
, m_iFieldsCount(0)
/*, m_bSigned(FALSE)*/
{

	m_szWndSize.SetSize(483, 294);
	//RegisterWindowClass();

	m_fntDecEdt.CreateStockObject(DEFAULT_GUI_FONT);
	m_fntDecEdt.SetFontFace(_T("Verdana"));
	m_fntDecEdt.SetFontPointSize(14);

	m_fntCmnEdt.CreateStockObject(DEFAULT_GUI_FONT);
	m_fntCmnEdt.SetFontFace(_T("Courier New"));
	m_fntCmnEdt.SetFontPointSize(8);

	m_crDecEdtBkColorOn = 0xFFFFFF;
	m_crDecEdtBkColorOff = 0xCCCCCC;
	m_crDecEdtTextColorOn = 0x0;
	m_crDecEdtTextColorOff = 0x505050; //0xC7B138;
	m_crDecEdtTextColorChange = 0xFF;
	m_crCmnEdtBkColorOn = 0x0;
	m_crCmnEdtBkColorOff = 0x333333;
	
	m_brDecEdtBkBrushOn.CreateStockObject(WHITE_BRUSH);
	m_brDecEdtBkBrushOff.CreateSolidBrush(m_crDecEdtBkColorOff);
	m_brCmnEdtBkBrushOn.CreateStockObject(BLACK_BRUSH);
	m_brCmnEdtBkBrushOff.CreateSolidBrush(m_crCmnEdtBkColorOff);
}

CBEShowCtrl::~CBEShowCtrl()
{
	if(Is32())
	{
		delete m_pDecEdit32;
		delete m_pHexEdit32;
		delete m_pOctEdit32;
		delete m_pBinEdit32;
	} else
	if(Is16())
	{
		delete m_pDecEdit16L;
		delete m_pDecEdit16H;
		delete m_pHexEdit16L;
		delete m_pHexEdit16H;
		delete m_pOctEdit16L;
		delete m_pOctEdit16H;
		delete m_pBinEdit16L;
		delete m_pBinEdit16H;
	} else
	if(Is8() || IsColor())
	{
		delete m_pDecEdit8LL;
		delete m_pDecEdit8LH;
		delete m_pDecEdit8HL;
		delete m_pDecEdit8HH;
		if(Is8())
		{
			delete m_pHexEdit8LL;
			delete m_pHexEdit8LH;
			delete m_pHexEdit8HL;
			delete m_pHexEdit8HH;
			delete m_pOctEdit8LL;
			delete m_pOctEdit8LH;
			delete m_pOctEdit8HL;
			delete m_pOctEdit8HH;
			delete m_pBinEdit8LL;
			delete m_pBinEdit8LH;
			delete m_pBinEdit8HL;
			delete m_pBinEdit8HH;
		} else {
			delete m_pClrSliderRed;
			delete m_pClrSliderGreen;
			delete m_pClrSliderBlue;
			delete m_pClrSliderAlpha;

			delete m_pHexClrEditWeb;
			delete m_pHexClrEditWin;
		}
	}

	m_fntDecEdt.DeleteObject();
	m_fntCmnEdt.DeleteObject();

	m_brDecEdtBkBrushOn.DeleteObject();
	m_brDecEdtBkBrushOff.DeleteObject();
	m_brCmnEdtBkBrushOn.DeleteObject();
	m_brCmnEdtBkBrushOff.DeleteObject();

	if(!IsColor()) ::KillTimer(m_hWnd, m_uipTimerID);
}


BEGIN_MESSAGE_MAP(CBEShowCtrl, CDialog /*CWnd*/)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CBEShowCtrl message handlers



BOOL CBEShowCtrl::Create(int iType, int x, int y, CWnd* pParent, UINT nID)
{
	m_iType = iType;
	m_uiID = nID;

	int i, j, ix, iy;
	UINT uiIDSet;
	RECT wrc;
	BOOL bRet;
	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | /*WS_TABSTOP |*/ WS_VISIBLE;
	DWORD dwEdtStyle = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_CENTER |
					   WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VISIBLE;

	//SetRect(&wrc, x, y, x + 483, y + 123 + 2);
	//bRet = CWnd::CreateEx(WS_EX_TRANSPARENT, BESHOWCTRL_CLASSNAME, NULL, dwStyle, wrc, pParent, nID );
	bRet = CDialog::Create(IDD_LINE_DLG, pParent);
	SetWindowPos(NULL, x, y, 483, IsColor() ? 130 : 123 + 1, SWP_NOZORDER|SWP_SHOWWINDOW);
	//this->

	if(!bRet) return FALSE;

	if(Is32())
	{
		m_iFieldsCount = 1;
		m_pDecEdits[0]    = m_pDecEdit32 = new CDecEdit();
		m_pCmnEdits[0][0] = m_pHexEdit32 = new CHexEdit();
		m_pCmnEdits[0][1] = m_pOctEdit32 = new COctEdit();
		m_pCmnEdits[0][2] = m_pBinEdit32 = new CBinEdit();

		iy = 26;

		SetRect(&wrc, 3 + m_pBitmaps->FuncBmpSize.SmallWidth,
				iy + 2, 483 - 3 - m_pBitmaps->FuncBmpSize.BigWidth, iy + 26 - 2);
		m_pDecEdit32->Create(dwEdtStyle, wrc, this, m_uiID + 1);
		m_pDecEdit32->SetFont(&m_fntDecEdt);
		m_pDecEdit32->SetByteIndex(1);

		dwEdtStyle &= ~ES_CENTER;
		uiIDSet = m_uiID + 1;
		iy += 26;

		for(i = 0; i < 3; i++)
		{
			SetRect(&wrc, 37, iy + 2, 483 - 3, iy + 18 - 2);
			m_pCmnEdits[0][i]->Create(dwEdtStyle, wrc, this, ++uiIDSet);
			m_pCmnEdits[0][i]->SetFont(&m_fntCmnEdt);
			((CEdits*)m_pCmnEdits[0][i])->SetByteIndex(1);

			iy += 18 + 1;
		}
	} else
	if(Is16())
	{
		m_iFieldsCount = 2;
		m_pDecEdits[0]    = m_pDecEdit16L = new CDecEdit();
		m_pDecEdits[1]    = m_pDecEdit16H = new CDecEdit();
		m_pCmnEdits[0][0] = m_pHexEdit16L = new CHexEdit();
		m_pCmnEdits[1][0] = m_pHexEdit16H = new CHexEdit();
		m_pCmnEdits[0][1] = m_pOctEdit16L = new COctEdit();
		m_pCmnEdits[1][1] = m_pOctEdit16H = new COctEdit();
		m_pCmnEdits[0][2] = m_pBinEdit16L = new CBinEdit();
		m_pCmnEdits[1][2] = m_pBinEdit16H = new CBinEdit();

		iy = 26;
		ix = 0;

		for(i = 0; i < 2; i++)
		{
			SetRect(&wrc, ix + 3 + m_pBitmaps->FuncBmpSize.SmallWidth,
					iy + 2, ix + 240 - m_pBitmaps->FuncBmpSize.BigWidth, iy + 26 - 2);
			m_pDecEdits[i]->Create(dwEdtStyle, wrc, this, m_uiID + i + 1);
			m_pDecEdits[i]->SetFont(&m_fntDecEdt);
			((CEdits*)m_pDecEdits[i])->SetByteIndex(i + 1);

			ix += 240;
		}

		dwEdtStyle &= ~ES_CENTER;
		uiIDSet = m_uiID + 2;
		ix = 37;

		for(i = 0; i < 2; i++)
		{
			iy = 26 * 2;
			for(j = 0; j < 3; j++)
			{
				SetRect(&wrc, ix, iy + 2, ix + 240 - 37 - 0, iy + 18 - 2);
				m_pCmnEdits[i][j]->Create(dwEdtStyle, wrc, this, ++uiIDSet);
				m_pCmnEdits[i][j]->SetFont(&m_fntCmnEdt);
				((CEdits*)m_pCmnEdits[i][j])->SetByteIndex(i + 1);

				iy += 18 + 1;
			}
			ix += 240;
		}

	} else
	if(Is8() || IsColor())
	{
		m_iFieldsCount = 4;
		m_pDecEdits[0] = m_pDecEdit8LL = new CDecEdit();
		m_pDecEdits[1] = m_pDecEdit8LH = new CDecEdit();
		m_pDecEdits[2] = m_pDecEdit8HL = new CDecEdit();
		m_pDecEdits[3] = m_pDecEdit8HH = new CDecEdit();

		if(Is8())
		{
			m_pCmnEdits[0][0] = m_pHexEdit8LL = new CHexEdit();
			m_pCmnEdits[1][0] = m_pHexEdit8LH = new CHexEdit();
			m_pCmnEdits[2][0] = m_pHexEdit8HL = new CHexEdit();
			m_pCmnEdits[3][0] = m_pHexEdit8HH = new CHexEdit();
			m_pCmnEdits[0][1] = m_pOctEdit8LL = new COctEdit();
			m_pCmnEdits[1][1] = m_pOctEdit8LH = new COctEdit();
			m_pCmnEdits[2][1] = m_pOctEdit8HL = new COctEdit();
			m_pCmnEdits[3][1] = m_pOctEdit8HH = new COctEdit();
			m_pCmnEdits[0][2] = m_pBinEdit8LL = new CBinEdit();
			m_pCmnEdits[1][2] = m_pBinEdit8LH = new CBinEdit();
			m_pCmnEdits[2][2] = m_pBinEdit8HL = new CBinEdit();
			m_pCmnEdits[3][2] = m_pBinEdit8HH = new CBinEdit();
		}

		iy = 26;
		ix = 0;

		for(i = 0; i < 4; i++)
		{
			SetRect(&wrc, ix + (Is8() ? (3 + m_pBitmaps->FuncBmpSize.SmallWidth) : 23),
					iy + 2, ix + (Is8() ? (120 - m_pBitmaps->FuncBmpSize.BigWidth) : 100), iy + 26 - 2);
			m_pDecEdits[i]->Create(dwEdtStyle, wrc, this, m_uiID + i + 1);
			m_pDecEdits[i]->SetFont(&m_fntDecEdt);
			((CEdits*)m_pDecEdits[i])->SetByteIndex(i + 1);

			ix += 120;
		}

		dwEdtStyle &= ~ES_CENTER;
		uiIDSet = m_uiID + 4;

		if(Is8())
		{
			ix = 37;

			for(i = 0; i < 4; i++)
			{
				iy = 26 * 2;
				for(j = 0; j < 3; j++)
				{
					SetRect(&wrc, ix, iy + 2, ix + 120 - 37 - 0, iy + 18 - 2);
					m_pCmnEdits[i][j]->Create(dwEdtStyle, wrc, this, ++uiIDSet);
					m_pCmnEdits[i][j]->SetFont(&m_fntCmnEdt);
					((CEdits*)m_pCmnEdits[i][j])->SetByteIndex(i + 1);

					iy += 18 + 1;
				}
				ix += 120;
			}
		} else {
			// create web edit

			m_pHexClrEditWeb = new CHexClrEdit(CBEShowCtrl::bigEndian);
			m_pHexClrEditWin = new CHexClrEdit(CBEShowCtrl::littleEndian);

			int cx = 110, cy = 98, ydst = 19;

			SetRect(&wrc, cx, cy, cx + 53, cy + 14);
			m_pHexClrEditWeb->Create(dwEdtStyle, wrc, this, ++uiIDSet);
			m_pHexClrEditWeb->SetFont(&m_fntCmnEdt);

			cy += 15;

			SetRect(&wrc, cx, cy, cx + 53, cy + 14);
			m_pHexClrEditWin->Create(dwEdtStyle, wrc, this, ++uiIDSet);
			m_pHexClrEditWin->SetFont(&m_fntCmnEdt);

			m_pClrSliders[0] = m_pClrSliderRed   = new CClrSliderCtrl();
			m_pClrSliders[1] = m_pClrSliderGreen = new CClrSliderCtrl();
			m_pClrSliders[2] = m_pClrSliderBlue  = new CClrSliderCtrl();
			m_pClrSliders[3] = m_pClrSliderAlpha = new CClrSliderCtrl();

			cx = 168;
			cy = 54;

			m_pClrSliderRed  ->Create(CClrSliderCtrl::ctRed,   cx, cy        , this, ++uiIDSet);
			m_pClrSliderGreen->Create(CClrSliderCtrl::ctGreen, cx, cy += ydst, this, ++uiIDSet);
			m_pClrSliderBlue ->Create(CClrSliderCtrl::ctBlue,  cx, cy += ydst, this, ++uiIDSet);
			m_pClrSliderAlpha->Create(CClrSliderCtrl::ctAlpha, cx, cy += ydst, this, ++uiIDSet);

			m_rcWWLbls[0].SetRect(74,  96, 111, 112);
			m_rcWWLbls[1].SetRect(74, 112, 111, 128);
		}
	} else
	{
		return FALSE;
	}

	if(!IsColor())
	{
		iy--;
		m_cBitsCtrl.Create(CRect(1, iy, 1 + 481, iy + 16), this, ++uiIDSet);
		SetTimer(m_uipTimerID = ++uiIDSet, 10, NULL);
	}

	return TRUE;
}

/*BOOL CBEShowCtrl::RegisterWindowClass(void)
{
	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, BESHOWCTRL_CLASSNAME, &wndcls)) )
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
        wndcls.lpszClassName    = BESHOWCTRL_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}*/

void CBEShowCtrl::OnPaint()
{
	CPaintDC dc(this);

	if(Is32())          Paint32(&dc);
	else if(Is16())     Paint16(&dc);
	else if(Is8())      Paint8(&dc);
	else if(IsColor())  PaintColor(&dc);
}

void CBEShowCtrl::Paint32(CDC* pDC)
{
	CDC fixDC, lblDC;
	CBitmap* pOldFixBmp, pOldLblBmp;
	BITMAP fixBmp, suBmp, lblBmp;
	int i, y;
	COLORREF crDecBkColor,
			 crCmnBkColor;
	CBitmap *pHeadBmp, *pSUBmp, *pHexBmp, *pOctBmp, *pBinBmp;

	fixDC.CreateCompatibleDC(pDC);
	lblDC.CreateCompatibleDC(pDC);

	if(GetSelected() == 0)
	{
		pHeadBmp = &m_pBitmaps->m_bm32HeadOff;
		//pSUBmp = m_bSigned ? &m_pBitmaps->m_bmSignedOff : &m_pBitmaps->m_bmUnsignedOff;
		pSUBmp = ((CDecEdit*)m_pDecEdits[0])->IsSigned() ? &m_pBitmaps->m_bmSignedOff :
														   &m_pBitmaps->m_bmUnsignedOff ;
		crDecBkColor = m_crDecEdtBkColorOff;
		crCmnBkColor = m_crCmnEdtBkColorOff;
		pHexBmp = &m_pBitmaps->m_bmHexOff;
		pOctBmp = &m_pBitmaps->m_bmOctOff;
		pBinBmp = &m_pBitmaps->m_bmBinOff;
	} else {
		pHeadBmp = &m_pBitmaps->m_bm32HeadOn;
		//pSUBmp = m_bSigned ? &m_pBitmaps->m_bmSignedOn : &m_pBitmaps->m_bmUnsignedOn;
		pSUBmp = ((CDecEdit*)m_pDecEdits[0])->IsSigned() ? (m_iSUHot > 0 ? &m_pBitmaps->m_bmSignedOver : &m_pBitmaps->m_bmSignedOn) :
														   (m_iSUHot > 0 ? &m_pBitmaps->m_bmUnsignedOver : &m_pBitmaps->m_bmUnsignedOn) ;
		crDecBkColor = m_crDecEdtBkColorOn;
		crCmnBkColor = m_crCmnEdtBkColorOn;
		pHexBmp = &m_pBitmaps->m_bmHexOn;
		pOctBmp = &m_pBitmaps->m_bmOctOn;
		pBinBmp = &m_pBitmaps->m_bmBinOn;
	}

	pHeadBmp->GetBitmap(&fixBmp);
	pOldFixBmp = (CBitmap*)fixDC.SelectObject(pHeadBmp);
	pDC->TransparentBlt(0, 0, fixBmp.bmWidth, fixBmp.bmHeight, &fixDC, 0, 0, fixBmp.bmWidth, fixBmp.bmHeight, m_pBitmaps->m_crTransCol);

	pSUBmp->GetBitmap(&suBmp);
	fixDC.SelectObject(pSUBmp);
	pDC->BitBlt(57, 2, suBmp.bmWidth, suBmp.bmHeight, &fixDC, 0, 0, SRCCOPY);

	pDC->FillSolidRect(1, fixBmp.bmHeight, fixBmp.bmWidth - 2, 26, crDecBkColor);

	PaintFuncs(pDC);

	pDC->SelectStockObject(WHITE_PEN);

	y = fixBmp.bmHeight + 26;
	for(i = 1; i <= 3; i++)
	{
		pDC->FillSolidRect(1, y, fixBmp.bmWidth - 2, 18, crCmnBkColor);
		if(i < 3)
		{
			pDC->MoveTo(1, y + 18);
			pDC->LineTo(fixBmp.bmWidth - 1, y + 18);
		}
		y += 18 + 1;
	}

	y = fixBmp.bmHeight + 26;

	pHexBmp->GetBitmap(&lblBmp);
	lblDC.SelectObject(pHexBmp);
	pDC->BitBlt(1, y, lblBmp.bmWidth, lblBmp.bmHeight, &lblDC, 0, 0, SRCCOPY);

	y += 18 + 1;

	lblDC.SelectObject(pOctBmp);
	pDC->BitBlt(1, y, lblBmp.bmWidth, lblBmp.bmHeight, &lblDC, 0, 0, SRCCOPY);

	y += 18 + 1;

	lblDC.SelectObject(pBinBmp);
	pDC->BitBlt(1, y, lblBmp.bmWidth, lblBmp.bmHeight, &lblDC, 0, 0, SRCCOPY);

	fixDC.DeleteDC();
	lblDC.DeleteDC();
}

void CBEShowCtrl::Paint16(CDC* pDC)
{
	CDC fixDC, lblDC;
	CBitmap *pOldFixBmp, *pOldLblBmp, *pSUBmp;
	BITMAP fixBmp, suBmp, lblBmp;
	int i, j, x, y;

	fixDC.CreateCompatibleDC(pDC);
	lblDC.CreateCompatibleDC(pDC);

	m_pBitmaps->m_bm16HeadOff.GetBitmap(&fixBmp);
	pOldFixBmp = (CBitmap*)fixDC.SelectObject(m_pBitmaps->m_bm16HeadOff);

	pDC->TransparentBlt(0, 0, fixBmp.bmWidth, fixBmp.bmHeight, &fixDC, 0, 0, fixBmp.bmWidth, fixBmp.bmHeight, m_pBitmaps->m_crTransCol);

	for(i = 0; i < 2; i++)
	{
		pSUBmp = ((CDecEdit*)m_pDecEdits[i])->IsSigned() ? &m_pBitmaps->m_bmSignedOff :
														   &m_pBitmaps->m_bmUnsignedOff ;

		pSUBmp->GetBitmap(&suBmp);
		fixDC.SelectObject(pSUBmp);

		pDC->BitBlt(240 * i + 57, 2, suBmp.bmWidth, suBmp.bmHeight, &fixDC, 0, 0, SRCCOPY);
	}

	pDC->FillSolidRect(1, fixBmp.bmHeight, fixBmp.bmWidth - 2, 26, m_crDecEdtBkColorOff);

	if(GetSelected() > 0)
	{	x = (GetSelected() - 1) * 241;
		fixDC.SelectObject(m_pBitmaps->m_bm16HeadOn);
		pDC->TransparentBlt(x, 0, 241, fixBmp.bmHeight, &fixDC, x, 0, 241, fixBmp.bmHeight, m_pBitmaps->m_crTransCol);

		pSUBmp = ((CDecEdit*)m_pDecEdits[GetSelected() - 1])->IsSigned() ? 
			(m_iSUHot > 0 ? &m_pBitmaps->m_bmSignedOver : &m_pBitmaps->m_bmSignedOn) : 
			(m_iSUHot > 0 ? &m_pBitmaps->m_bmUnsignedOver : &m_pBitmaps->m_bmUnsignedOn) ;

		fixDC.SelectObject(pSUBmp);
		pDC->BitBlt((GetSelected() - 1) * 240 + 57, 2, suBmp.bmWidth, suBmp.bmHeight, &fixDC, 0, 0, SRCCOPY);

		x = (GetSelected() - 1) * 240 + 1;
		pDC->FillSolidRect(x, fixBmp.bmHeight, 240 + 1, 26, m_crDecEdtBkColorOn);
	}

	PaintFuncs(pDC);

	pDC->SelectStockObject(WHITE_PEN);

	y = fixBmp.bmHeight + 26;
	for(i = 1; i <= 3; i++)
	{
		pDC->FillSolidRect(1, y, fixBmp.bmWidth - 2, 18, m_crCmnEdtBkColorOff);
		if(i < 3)
		{
			pDC->MoveTo(1, y + 18);
			pDC->LineTo(fixBmp.bmWidth - 1, y + 18);
		}
		y += 18 + 1;
	}

	CBitmap* pBmpsOff[] = { &m_pBitmaps->m_bmHexOff,
						    &m_pBitmaps->m_bmOctOff,
						    &m_pBitmaps->m_bmBinOff };

	x = 1;
	for(i = 1; i <= 2; i++)
	{
		y = fixBmp.bmHeight + 26;
		for(j = 1; j <= 3; j++)
		{
			pBmpsOff[j - 1]->GetBitmap(&lblBmp);
			lblDC.SelectObject(pBmpsOff[j - 1]);
			pDC->BitBlt(x, y, lblBmp.bmWidth, lblBmp.bmHeight, &lblDC, 0, 0, SRCCOPY);
			y += 18 + 1;
		}
		x += 240;
	}

	if(GetSelected() > 0)
	{
		CBitmap* pBmpsOn[] = { &m_pBitmaps->m_bmHexOn,
							   &m_pBitmaps->m_bmOctOn,
							   &m_pBitmaps->m_bmBinOn };

		x = (GetSelected() - 1) * 241 + 1;
		y = fixBmp.bmHeight + 26;
		for(i = 1; i <= 3; i++)
		{
			pDC->FillSolidRect(x, y, 240 + (GetSelected() == 1 ? 1 : 0), 18, m_crCmnEdtBkColorOn);

			pBmpsOn[i - 1]->GetBitmap(&lblBmp);
			lblDC.SelectObject(pBmpsOn[i - 1]);
			pDC->BitBlt(x - (GetSelected() == 2 ? 1 : 0), y, lblBmp.bmWidth, lblBmp.bmHeight,
				&lblDC, 0, 0, SRCCOPY);

			y += 18 + 1;
		}
	}

	fixDC.DeleteDC();
	lblDC.DeleteDC();
}

void CBEShowCtrl::Paint8(CDC* pDC)
{
	CDC fixDC, lblDC;
	CBitmap *pOldFixBmp, /**pOldLblBmp,*/ *pSUBmp;
	BITMAP fixBmp, suBmp, lblBmp;
	int i, j, x, y;

	fixDC.CreateCompatibleDC(pDC);
	lblDC.CreateCompatibleDC(pDC);

	m_pBitmaps->m_bm8HeadOff.GetBitmap(&fixBmp);
	pOldFixBmp = (CBitmap*)fixDC.SelectObject(m_pBitmaps->m_bm8HeadOff);

	pDC->TransparentBlt(0, 0, fixBmp.bmWidth, fixBmp.bmHeight, &fixDC, 0, 0, fixBmp.bmWidth, fixBmp.bmHeight, m_pBitmaps->m_crTransCol);

	//pSUBmp = m_bSigned ? &m_pBitmaps->m_bmSOff : &m_pBitmaps->m_bmUOff;
	//pSUBmp->GetBitmap(&suBmp);
	//fixDC.SelectObject(pSUBmp);

	for(i = 0; i < 4; i++)
	{
		pSUBmp = ((CDecEdit*)m_pDecEdits[i])->IsSigned() ? &m_pBitmaps->m_bmSOff : 
														   &m_pBitmaps->m_bmUOff;
		pSUBmp->GetBitmap(&suBmp);
		fixDC.SelectObject(pSUBmp);
		
		pDC->BitBlt(i * 120 + 48, 2, suBmp.bmWidth, suBmp.bmHeight, &fixDC, 0, 0, SRCCOPY);
	}

	pDC->FillSolidRect(1, fixBmp.bmHeight, fixBmp.bmWidth - 2, 26, m_crDecEdtBkColorOff);

	if(GetSelected() > 0)
	{	x = (GetSelected() - 1) * 121;
		if(GetSelected() == 1) x += 1;
		if(GetSelected() == 4) x -= 2;

		fixDC.SelectObject(m_pBitmaps->m_bm8HeadOn);
		pDC->TransparentBlt(x, 0, 120, fixBmp.bmHeight, &fixDC, x, 0, 120, fixBmp.bmHeight, m_pBitmaps->m_crTransCol);

		pSUBmp = ((CDecEdit*)m_pDecEdits[GetSelected() - 1])->IsSigned() ?
			(m_iSUHot > 0 ? &m_pBitmaps->m_bmSOver : &m_pBitmaps->m_bmSOn) : 
			(m_iSUHot > 0 ? &m_pBitmaps->m_bmUOver : &m_pBitmaps->m_bmUOn) ;

		fixDC.SelectObject(pSUBmp);

		pDC->BitBlt((GetSelected() - 1) * 120 + 48, 2, suBmp.bmWidth, suBmp.bmHeight, &fixDC, 0, 0, SRCCOPY);

		x = (GetSelected() - 1) * 120 + 1;
		pDC->FillSolidRect(x, fixBmp.bmHeight, 120 + 1, 26, m_crDecEdtBkColorOn);
	}

	PaintFuncs(pDC);

	pDC->SelectStockObject(WHITE_PEN);

	y = fixBmp.bmHeight + 26;
	for(i = 1; i <= 3; i++)
	{
		pDC->FillSolidRect(1, y, fixBmp.bmWidth - 2, 18, 0x333333);
		if(i < 3)
		{
			pDC->MoveTo(1, y + 18);
			pDC->LineTo(fixBmp.bmWidth - 1, y + 18);
		}
		y += 18 + 1;
	}

	CBitmap* pBmpsOff[] = { &m_pBitmaps->m_bmHexOff,
						 &m_pBitmaps->m_bmOctOff,
						 &m_pBitmaps->m_bmBinOff };

	x = 1;
	for(i = 1; i <= 4; i++)
	{
		y = fixBmp.bmHeight + 26;
		for(j = 1; j <= 3; j++)
		{
			pBmpsOff[j - 1]->GetBitmap(&lblBmp);
			lblDC.SelectObject(pBmpsOff[j - 1]);
			pDC->BitBlt(x, y, lblBmp.bmWidth, lblBmp.bmHeight, &lblDC, 0, 0, SRCCOPY);
			y += 18 + 1;
		}
		x += 120;
	}

	if(GetSelected() > 0)
	{
		CBitmap* pBmpsOn[] = { &m_pBitmaps->m_bmHexOn,
							   &m_pBitmaps->m_bmOctOn,
							   &m_pBitmaps->m_bmBinOn };

		x = (GetSelected() - 1) * 121 + 1;
		y = fixBmp.bmHeight + 26;

		if(GetSelected() == 3) x -= 2;
		if(GetSelected() == 4) x -= 3;

		for(i = 1; i <= 3; i++)
		{
			pDC->FillSolidRect(x, y, 120 + (GetSelected() == 2 ? 0 : 1), 18, m_crCmnEdtBkColorOn);

			pBmpsOn[i - 1]->GetBitmap(&lblBmp);
			lblDC.SelectObject(pBmpsOn[i - 1]);
			pDC->BitBlt(x - (GetSelected() == 2 ? 1 : 0), y, lblBmp.bmWidth, lblBmp.bmHeight,
				&lblDC, 0, 0, SRCCOPY);

			y += 18 + 1;
		}
	}

	fixDC.DeleteDC();
	lblDC.DeleteDC();
}

void CBEShowCtrl::PaintColor(CDC* pDC)
{
	CDC fixDC, lblDC;
	HGDIOBJ hOldFixBmp;
	BITMAP fixBmp, lblBmp;
	COLORREF crDecBkColor;
	int i, j, x, y;

	CRect wrc;
	GetClientRect(wrc);

	//pDC->FillSolidRect(wrc, 0xffffff);

	fixDC.CreateCompatibleDC(pDC);

	m_pBitmaps->m_bmClrHeadOff.GetBitmap(&fixBmp);

	hOldFixBmp = (HGDIOBJ)fixDC.SelectObject( IsSelected() ? m_pBitmaps->m_bmClrHeadOn :
															 m_pBitmaps->m_bmClrHeadOff);

	pDC->TransparentBlt(0, 0, fixBmp.bmWidth, fixBmp.bmHeight, &fixDC, 0, 0, fixBmp.bmWidth, fixBmp.bmHeight, m_pBitmaps->m_crTransCol);

	crDecBkColor = IsSelected() > 0 ? m_crDecEdtBkColorOn : m_crDecEdtBkColorOff;

	y = fixBmp.bmHeight;
	x = 1;
	int xadd;

	for(i = 0; i < 4; i++)
	{
		if(i == 3) xadd = 1; else xadd = 0;
		pDC->FillSolidRect(x, y, 100 + xadd, 26, crDecBkColor);

		if(!m_bIsColorInit)
			m_rcDecColors[i].SetRect(x + 100 + xadd, y, x + 100 + 20 + xadd, y + 26);

		x += 120;
	}

	y += 26;

	// Use a mem dc to avoid the fucking flickering
	CDC memDC;
	CBitmap memBmp;

	memDC.CreateCompatibleDC(pDC);
	memBmp.CreateCompatibleBitmap(pDC, wrc.Width(), wrc.Height());

	HGDIOBJ hOldMemBmp = memDC.SelectObject((HGDIOBJ)memBmp);

	memDC.FillSolidRect(1, y, fixBmp.bmWidth - 2, 78, GetSelected() == 0 ? m_crCmnEdtBkColorOff :
																		   m_crCmnEdtBkColorOn);

	m_rcCircle.SetRect(3, y + 3, 3 + 72, y + 3 + 72);

	m_bIsColorInit = true;

	PaintCircle(&memDC);
	PaintBigColors(&memDC);

	m_pBitmaps->m_bmWWOn.GetBitmap(&lblBmp);
	fixDC.SelectObject(IsSelected() ? m_pBitmaps->m_bmWWOn : m_pBitmaps->m_bmWWOff);

	memDC.BitBlt(74, y + 44, lblBmp.bmWidth, lblBmp.bmHeight, &fixDC, 0, 0, SRCCOPY);

	pDC->BitBlt(1, y, fixBmp.bmWidth - 2, 78, &memDC, 1, y, SRCCOPY);

	memDC.SelectObject(hOldMemBmp);
	memDC.DeleteDC();
	memBmp.DeleteObject();

	PaintDecColors(pDC);

	//fixDC.SelectObject(hOldFixBmp);
	fixDC.DeleteDC();
}


void CBEShowCtrl::PaintFuncs(CDC* pDC)
{
	if(!IsSelected() || IsColor()) return;

	CDC memDC;

	memDC.CreateCompatibleDC(pDC);
	HGDIOBJ hOldMemBmp = memDC.SelectObject(m_pBitmaps->m_bmFuncs);

	int x1, x2, y = 26;

	GetFldX1X2(m_iSelected, x1, x2);

	if(Is32() || (Is8() && (m_iSelected == 1 || m_iSelected == 3 || m_iSelected == 4))) x2--;

	pDC->BitBlt(x1, y, m_pBitmaps->FuncBmpSize.SmallWidth, m_pBitmaps->FuncBmpSize.FullHeight,
				&memDC, 0, 0, SRCCOPY);
	pDC->BitBlt(x2 - m_pBitmaps->FuncBmpSize.BigWidth, y, m_pBitmaps->FuncBmpSize.BigWidth, m_pBitmaps->FuncBmpSize.BigHeight,
				&memDC, m_pBitmaps->FuncBmpSize.SmallWidth, 0, SRCCOPY);

	memDC.SelectObject(hOldMemBmp);
	memDC.DeleteDC();
}

CRect CBEShowCtrl::GetFuncRect(Funcs func, int iFieldIndex /*= 0*/)
{
	int x1, x2, y1 = 26, y2;

	if(iFieldIndex == 0) iFieldIndex = m_iSelected;

	GetFldX1X2(m_iSelected, x1, x2);

	if(Is32() || (Is8() && (iFieldIndex == 1 || iFieldIndex == 3 || iFieldIndex == 4))) x2--;

	switch(func)
	{
	case Funcs::Unset:
		x2 = x1 + m_pBitmaps->FuncBmpSize.SmallWidth;
		y2 = y1 + m_pBitmaps->FuncBmpSize.SmallHeight;
		break;
	case Funcs::Set:
		x2 = x1 + m_pBitmaps->FuncBmpSize.SmallWidth;
		y1 += m_pBitmaps->FuncBmpSize.SmallHeight;
		y2 = y1 + m_pBitmaps->FuncBmpSize.SmallHeight;
		break;
	case Funcs::BitNot:
		x1 = x2 - m_pBitmaps->FuncBmpSize.BigWidth;
		y2 = y1 + m_pBitmaps->FuncBmpSize.BigHeight;
		break;
	}

	return CRect(x1, y1, x2, y2);
}

void CBEShowCtrl::ApplyFunc(Funcs func)
{
	if(IsSelected())
	{
		CDecEdit *pEdt = ((CDecEdit*)m_pDecEdits[m_iSUHot - 1]);
		int iPos = FieldToPos(m_iSelected);

		switch(func)
		{
		case Funcs::Unset:
			BEValue.SetUInt(0, iPos);
			break;
		case Funcs::Set:
			BEValue.SetUInt(0xFFFFFFFF, iPos);
			break;
		case Funcs::BitNot:
			BEValue.SetUInt(~BEValue.GetUInt(iPos), iPos);
			break;
		}

		UpdateFields();
		SetUpdateCtrls();
	}
}

HBRUSH CBEShowCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	COLORREF crText;
	bool bIsSelected = false;
	int nDecIDsMin, nDecIDsMax,
		nCmnIDsMin, nCmnIDsMax;

	// TODO:  Change any attributes of the DC here

	if(nCtlColor == CTLCOLOR_DLG && pWnd->m_hWnd == m_hWnd)
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	else
	if(nCtlColor == CTLCOLOR_EDIT)
	{
		if(Is32())
		{
			nDecIDsMin = nDecIDsMax = 1;
			nCmnIDsMin = 2;
			nCmnIDsMax = 4;
			bIsSelected = GetSelected() > 0;
		} else
		if(Is16())
		{
			nDecIDsMin = 1;
			nDecIDsMax = 2;

			nCmnIDsMin = ((GetSelected() - 1) * 3) + 2 + 1;
			nCmnIDsMax = nCmnIDsMin + 2;
			bIsSelected = int(pWnd->GetDlgCtrlID() - m_uiID) >= nCmnIDsMin &&
						  int(pWnd->GetDlgCtrlID() - m_uiID) <= nCmnIDsMax;

			nCmnIDsMin = 3;
			nCmnIDsMax = 8;
		} else
		if(Is8())
		{
			nDecIDsMin = 1;
			nDecIDsMax = 4;

			nCmnIDsMin = ((GetSelected() - 1) * 3) + 4 + 1;
			nCmnIDsMax = nCmnIDsMin + 2;
			bIsSelected = int(pWnd->GetDlgCtrlID() - m_uiID) >= nCmnIDsMin &&
						  int(pWnd->GetDlgCtrlID() - m_uiID) <= nCmnIDsMax;

			nCmnIDsMin = 5;
			nCmnIDsMax = 16;
		} else
		if(IsColor())
		{
			nDecIDsMin = 1;
			nDecIDsMax = 4;
			bIsSelected = IsSelected();
			nCmnIDsMin = 5;
			nCmnIDsMax = 6;
		}

		if(pWnd->GetDlgCtrlID() >= int(m_uiID + nDecIDsMin) &&
		   pWnd->GetDlgCtrlID() <= int(m_uiID + nDecIDsMax))
		{
			if(!IsColor()) bIsSelected = (pWnd->GetDlgCtrlID() - m_uiID) == GetSelected();

			if(bIsSelected)
			{
				if(((CEdits*)pWnd)->IsRealDirty())
					crText = m_crDecEdtTextColorChange;
				else
					crText = m_crDecEdtTextColorOn;
			} else
				crText = m_crDecEdtTextColorOff;

			pDC->SetTextColor(crText);

			pDC->SetBkColor(bIsSelected ? m_crDecEdtBkColorOn : m_crDecEdtBkColorOff);
			hbr = bIsSelected ? m_brDecEdtBkBrushOn : m_brDecEdtBkBrushOff;

		} else
		if(pWnd->GetDlgCtrlID() >= int(m_uiID + nCmnIDsMin) && 
		   pWnd->GetDlgCtrlID() <= int(m_uiID + nCmnIDsMax))
		{
			CEdits *pEdt = (CEdits*)pWnd;

			if(pEdt->IsRealDirty())
				crText = pEdt->m_crTextColorChange;
			else
				crText = pEdt->m_crTextColor;

			pDC->SetTextColor(crText);
			pDC->SetBkColor(bIsSelected ? m_crCmnEdtBkColorOn : m_crCmnEdtBkColorOff);
			hbr = bIsSelected ? m_brCmnEdtBkBrushOn : m_brCmnEdtBkBrushOff;
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


// Bits ctrl ------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CBEShowCtrl::CBitsCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CBEShowCtrl::CBitsCtrl::CBitsCtrl()
: m_pParent(NULL)
{
	RegisterWindowClass();
}

BOOL CBEShowCtrl::CBitsCtrl::Create(LPRECT lpRect, CWnd* pParent, UINT nID)
{
	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | /*WS_TABSTOP |*/ WS_VISIBLE;
	m_pParent = (CBEShowCtrl*)pParent;
	return CWnd::Create(BEBITSCTRL_CLASSNAME, NULL, dwStyle, *lpRect, pParent, nID);
}

BOOL CBEShowCtrl::CBitsCtrl::RegisterWindowClass(void)
{
	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, BEBITSCTRL_CLASSNAME, &wndcls)) )
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
        wndcls.lpszClassName    = BEBITSCTRL_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

void CBEShowCtrl::CBitsCtrl::OnPaint()
{
	CPaintDC pdc(this);

	CRect rc;
	CDC bitsOnDC, bitsOffDC, dc;
	CBitmap cbmp;
	//BITMAP bitBmp;
	int i, bit, x, y;

	GetClientRect(rc);

	dc.CreateCompatibleDC(&pdc);
	cbmp.CreateCompatibleBitmap(&pdc, rc.Width(), rc.Height());

	dc.SelectObject(&cbmp);

	bitsOffDC.CreateCompatibleDC(&pdc);
	bitsOffDC.SelectObject(m_pParent->m_pBitmaps->m_bmBitsOff);

	bitsOnDC.CreateCompatibleDC(&pdc);
	bitsOnDC.SelectObject(m_pParent->m_pBitmaps->m_bmBitsOn);



	dc.FillSolidRect(rc, m_pParent->m_crCmnEdtBkColorOff);

	if(m_pParent->GetSelected() > 0)
	{
		if(m_pParent->Is32())
		{
			dc.FillSolidRect(rc, m_pParent->m_crCmnEdtBkColorOn);
		} else
		if(m_pParent->Is16())
		{
			x = (m_pParent->GetSelected() - 1) * 240;
			dc.FillSolidRect(x, rc.top, 240 + 1, rc.bottom, m_pParent->m_crCmnEdtBkColorOn);
		} else
		if(m_pParent->Is8())
		{
			x = (m_pParent->GetSelected() - 1) * 120;
			dc.FillSolidRect(x, rc.top, 120 + 1, rc.bottom, m_pParent->m_crCmnEdtBkColorOn);
		}
	}

	int iValue = m_pParent->BEValue.m_i32Value;
	int shift, bits = 0, idxadd = theApp.Settings.IsBitsZeroIndex() ? 0 : 1;
	bit = 0;
	x = 31 * (14 + 1) + 1;
	y = 1;

	if(m_pParent->Is32())
		shift = 0;
	else if(m_pParent->Is16())
		shift = 16;
	else if(m_pParent->Is8())
		shift = 24;

	for(i = 1; i <= 32; i++)
	{
		bits = (iValue >> shift++) & 1;
		dc.BitBlt(x, y, 14, 14, bits == 1 ? &bitsOnDC : &bitsOffDC, (bit + idxadd) * 14, 0, SRCCOPY);
		x -= 14 + 1; //x += 14 + 1;

		if(m_pParent->Is16() && bit == 15)
			bit = 0;
		else
		if(m_pParent->Is8() && bit == 7) {
			bit = 0;
			shift = 24 - i;
		} else bit++;

		//bitc >>= 32 - i;
	}

	bitsOffDC.DeleteDC();
	bitsOnDC.DeleteDC();

	pdc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);

	cbmp.DeleteObject();
	dc.DeleteDC();

}

void CBEShowCtrl::CBitsCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();

	ClientToScreen((LPPOINT)&point);
	pPar->ScreenToClient((LPPOINT)&point);

	GetParent()->SendMessage(WM_LBUTTONDOWN, (WPARAM)NULL, MAKELPARAM(point.x, point.y));

	if(pPar->m_iSelected > 0)
	{
		int iBit = BitFromPoint(point);

		if(iBit > 0)
		{
			iBit = FixBitPos(iBit);
			int pos = pPar->FieldToPos(pPar->m_iSelected);
			int iValue = pPar->BEValue.GetInt(pos);
			int shift = 1, ushift;

			shift <<= iBit - 1;

			if((shift & iValue) == shift)
				iValue &= ~shift;
			else
				iValue |= shift;

			pPar->BEValue.SetInt(iValue, pos);
			pPar->UpdateFields();
			pPar->SetUpdateCtrls();
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

int CBEShowCtrl::CBitsCtrl::BitFromPoint(CPoint point)
{
	int x1, x2, iBit = 0;

	for(int i = 1; i <= 32; i++)
	{
		x1 = i + (i - 1) * 14;
		x2 = i * 14 + (i - 1);

		if(point.x >= x1 && point.x <= x2)
		{
			iBit = i;
			break;
		}
	}

	if(iBit > 0) iBit = 32 - iBit + 1;

	return iBit;
}

int CBEShowCtrl::CBitsCtrl::FixBitPos(int iBit)
{
	CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();

	if(pPar->Is16())
	{
		if(iBit > 16) iBit -= 16;
	} else
	if(pPar->Is8())
	{
		while(iBit > 8) iBit -= 8;
	}

	return iBit;
}

CRect CBEShowCtrl::CBitsCtrl::RectFromBit(int iBit /*= 0*/)
{
	int x1, y1 = 1;
	int x2, y2 = y1 + 14;

	if(iBit == 0)
	{
		x1 = 0;
		x2 = (32 * (14 + 1)) + 1;
	} else
	{
		iBit = 32 - (iBit - 1);
		x1 = (iBit - 1) * (14 + 1) + 1;
		x2 = x1 + 14; //(iBit * (14 + 1)) + 1;
	}

	return CRect(x1, y1, x2, y2);
}

void CBEShowCtrl::SetSelected(int iSelected, bool bForceRedraw /*= false*/)
{
	if(m_iSelected != iSelected || bForceRedraw)
	{
		if(Is32())
		{
			m_iSelected = iSelected;
			Invalidate(FALSE);
			InvalidateFieldEdits();
			m_cBitsCtrl.Invalidate(FALSE);
		} else
		if(IsColor())
		{
			InvalidateFieldEdits();
			if(m_iSelected <= 0 || iSelected <= 0 || bForceRedraw)
			{
				Invalidate(FALSE);
				InvalidateColorSlider();
			}
			m_iSelected = iSelected;
		} else {
			int x1, x2;

			GetFldX1X2(m_iSelected, x1, x2);

			CRect wrc;
			GetClientRect(wrc);

			if(m_iSelected > 0)
			{
				wrc.SetRect(x1, wrc.top, x2, wrc.bottom);
				InvalidateRect(wrc, FALSE);
				InvalidateFieldEdits(m_iSelected);
			}

			m_iSelected = iSelected;
			GetFldX1X2(m_iSelected, x1, x2);

			if(m_iSelected > 0)
			{
				wrc.SetRect(x1, wrc.top, x2, wrc.bottom);
				InvalidateRect(wrc, FALSE);
				InvalidateFieldEdits(m_iSelected);
			}

			m_cBitsCtrl.Invalidate(FALSE);
		}
	}
}

void CBEShowCtrl::AddUpdateCtrl(CBEShowCtrl* pUpdateCtrl)
{
	m_UpdateCtrls.Add(pUpdateCtrl);
}

void CBEShowCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(!IsColor())
	{
		if(m_iSUHot > 0)
		{
			CDecEdit *pEdt = ((CDecEdit*)m_pDecEdits[m_iSUHot - 1]);
			pEdt->SetSigned(!pEdt->IsSigned());
			UpdateDecFields(m_iSUHot);
			m_iSUHot = 0;
			return;
		} else
		if(m_iFuncHot > 0)
		{
			ApplyFunc((Funcs)m_iFuncHot);
			return;
		}
	} else {
		if(m_rcBigColors.PtInRect(point))
		{
			if(IsSelected())
			{
				ShowColorDialog();
				return;
			}
		}
		else
		if(m_rcWWLbls[0].PtInRect(point))
		{
			m_pHexClrEditWeb->SetFocus();
			return;
		} else
		if(m_rcWWLbls[1].PtInRect(point))
		{
			m_pHexClrEditWin->SetFocus();
			return;
		}
	}

	int iSelected;
	int iYIndex = 0;

	if(Is32())
	{
		iSelected = 1;
	} else
	if(Is16())
	{
		if(point.x <= 240)
			iSelected = 1;
		else
			iSelected = 2;
	} else
	if(Is8() || IsColor())
	{
		if(point.x <= 120)
			iSelected = 1;
		else
		if(point.x <= 240)
			iSelected = 2;
		else
		if(point.x <= 360)
			iSelected = 3;
		else
			iSelected = 4;
	}

	if(!IsColor())
	{
		int i, iy = 26 + 26;

		for(i = 1; i <= 3; i++)
		{
			if(point.y >= iy && point.y <= (iy + 18))
			{
				iy += 18 + 1;
				iYIndex = i;
				break;
			}
			iy += 18 + 1;
		}

		if(point.y >= iy && iSelected == m_iSelected ) iYIndex = -1;
	}

	if(iSelected > 0)
	{
		if(iYIndex > 0 && !IsColor())
			m_pCmnEdits[iSelected - 1][iYIndex - 1]->SetFocus();
		else if(iYIndex == 0)
			m_pDecEdits[iSelected - 1]->SetFocus();
	}

	if(m_iSelected != iSelected)
	{
		m_iSelected = iSelected;
		ResetSelectedUpdateCtrls();
		Invalidate(FALSE);
	}

	CDialog::OnLButtonDown(nFlags, point);
}
BEGIN_MESSAGE_MAP(CBEShowCtrl::CEdits, CEdit)
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CEdits::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CEdits::OnEnKillfocus)
	ON_CONTROL_REFLECT(EN_UPDATE, &CEdits::OnEnUpdate)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CBEShowCtrl::CEdits::OnEnSetfocus()
{
	// TODO: Add your control notification handler code here
	CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();

	pPar->ResetSelectedUpdateCtrls();
	pPar->SetSelected(m_iByteIndex);
	FixSel();
}

void CBEShowCtrl::ResetSelectedUpdateCtrls(void)
{
	CBEShowCtrl *pUpdCtrl;

	for(int i = 0; i < m_UpdateCtrls.GetCount(); i++)
	{
		pUpdCtrl = (CBEShowCtrl*)m_UpdateCtrls[i];
		pUpdCtrl->SetSelected(0);
	}
}

void CBEShowCtrl::CBEValue::RemoveSign(CString &strValue)
{
	if(HasSign(strValue)) strValue = strValue.Right(strValue.GetLength() - 1);
}

BOOL CBEShowCtrl::CBEValue::CheckValidChars(CString strValue, int iBase /*= 10*/, BOOL bIgnoreSign /*= TRUE*/)
{
	BOOL bRet = TRUE;
	if(iBase <= 1 || strValue.IsEmpty()) return FALSE;

	CString strSigns(BEVALUE_SIGNES);
	CString strFullValidChars(BEVALUE_VALIDCHARS),
			strValidChars(strFullValidChars.Left(iBase));
	CString strCheck(strValue.MakeUpper());

	if(bIgnoreSign) RemoveSign(strCheck);

	HasAdditionRemove(strCheck, iBase, TRUE);

	strCheck = strCheck.Trim();

	for(int i = 0; i < strCheck.GetLength(); i++)
	{
		if(strValidChars.Find(strCheck[i]) == -1)
		{
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}

BOOL CBEShowCtrl::CBEValue::CheckDecValue(CString strValue, int iType)
{
	int nMaxLen;
	INT64 i64Min, i64Max, i64Value;
	UINT64 ui64Min, ui64Max, ui64Value;

	if(strValue.GetLength() == 0) return FALSE;

	if(CheckValidChars(strValue))
	{
		if((iType & _unsigned) == _unsigned && HasSign(strValue))
		{
			// error: sign -+ found for an unsigned value
			return FALSE;
		}

		if((iType & bit32) == bit32) nMaxLen = 10;
		else if((iType & bit16) == bit16) nMaxLen = 5;
		else nMaxLen = 3;

		CBEShowCtrl::TrimZeroes(strValue);

		if(HasSign(strValue)) nMaxLen++;

		switch(iType)
		{
		case i32:  i64Min  = -2147483648i64; i64Max  = 2147483647i64; break;
		case i16:  i64Min  = -32768i64;      i64Max  = 32767i64;      break;
		case i8:   i64Min  = -128i64;        i64Max  = 127i64;        break;
		case ui32: ui64Min = 0i64;           ui64Max = 4294967295i64; break;
		case ui16: ui64Min = 0i64;           ui64Max = 65535i64;      break;
		case ui8:  ui64Min = 0i64;           ui64Max = 255i64;        break;
		}

		if(strValue.GetLength() > nMaxLen)
		{
			return FALSE;
		}

		if((iType & _signed) == _signed)
		{
			i64Value = INT64(_tstoi64((LPCTSTR)strValue));
			if(i64Value < i64Min || i64Value > i64Max)
			{
				return FALSE;
			}
		} else {
			ui64Value = UINT64(_tstoi64((LPCTSTR)strValue));
			if(ui64Value < ui64Min || ui64Value > ui64Max)
			{
				return FALSE;
			}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

BOOL CBEShowCtrl::CBEValue::CheckCmnValue(CString strValue, int iBase, int iBits)
{
	int nMaxLen = 0;
	INT64 i64Max;

	CString strCheck(strValue.MakeUpper());

	if(CheckValidChars(strValue, iBase, FALSE))
	{
		HasAdditionRemove(strCheck, iBase, TRUE);
		CBEShowCtrl::TrimZeroes(strCheck);

		if(iBase == 16)
		{
			if(strCheck.GetLength() > (iBits / 4)) return FALSE;
		} else
		if(iBase == 8)
		{
			switch(iBits)
			{
			case 32: nMaxLen = 11; i64Max = INT64(0xFFFFFFFF); break;
			case 24: nMaxLen = 8;  i64Max = INT64(0xFFFFFF); break;
			case 16: nMaxLen = 6;  i64Max = INT64(0xFFFF); break;
			case 8 : nMaxLen = 3;  i64Max = INT64(0xFF); break;
			}

			if(strCheck.GetLength() > nMaxLen) return FALSE;

			if(INT64(_tcstol(strCheck, NULL, iBase)) > i64Max) return FALSE;
		}
		else
		if(iBase == 2)
		{
			if(strCheck.GetLength() > iBits) return FALSE;
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

int CBEShowCtrl::CEdits::GetBitsCount(void)
{
	CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();
	return pPar->GetFldBitsCount();
}

int CBEShowCtrl::GetFldBitsCount(void)
{
	if(Is32()) return 32;
	else if(Is16()) return 16;
	else if(Is8()) return 8;
	return 0;
}

/*int CBEShowCtrl::GetDecType(void)
{
	if(Is32())
		return m_bSigned ? CBEValue::i32 : CBEValue::ui32;
	else
	if(Is16())
		return m_bSigned ? CBEValue::i16 : CBEValue::ui16;
	else
	if(Is8())
		return m_bSigned ? CBEValue::i8 : CBEValue::ui8;
	return 0;
}*/

void CBEShowCtrl::UpdateFields(int iFieldIndex /*= 0*/)
{
	UpdateDecFields(iFieldIndex);
	UpdateCmnFields(iFieldIndex);
	if(IsColor())
	{
		PaintDecColors();
		PaintCircle();
		PaintBigColors();
		m_pClrSliderAlpha->UpdateAlphaColor();
		UpdateColorSliderPointer(iFieldIndex);
	} else
		m_cBitsCtrl.Invalidate(FALSE);
}

void CBEShowCtrl::UpdateDecFields(int iFieldIndex /*= 0*/)
{
	CString strValue, strSignedFmt(theApp.Settings.IsShowSignedPlusSign() ? _T("%+i") : _T("%i"));

	if(Is32())
	{
		if(((CDecEdit*)m_pDecEdits[0])->IsSigned())
			strValue.Format(strSignedFmt, BEValue.GetInt32());
		else
			strValue.Format(_T("%u"), BEValue.GetUInt32());

		m_pDecEdit32->SetText(strValue);
	} else
	if(Is16())
	{
		INT16   i16Values[] = { BEValue.GetInt16Lo(),
							    BEValue.GetInt16Hi() };
		UINT16 ui16Values[] = { BEValue.GetUInt16Lo(),
							    BEValue.GetUInt16Hi() };

		for(int i = 0; i < 2; i++)
		{
			if(iFieldIndex == 0 || iFieldIndex == (i + 1))
			{
				if(((CDecEdit*)m_pDecEdits[i])->IsSigned())
					strValue.Format(strSignedFmt, i16Values[i]);
				else
					strValue.Format(_T("%u"), ui16Values[i]);
				m_pDecEdits[i]->SetText(strValue);
			}
		}

	} else
	if(Is8() || IsColor())
	{
		INT8   i8Values[] = { BEValue.GetInt8LoLo(),
							  BEValue.GetInt8LoHi(),
							  BEValue.GetInt8HiLo(),
							  BEValue.GetInt8HiHi() };
		UINT8 ui8Values[] = { BEValue.GetUInt8LoLo(),
							  BEValue.GetUInt8LoHi(),
							  BEValue.GetUInt8HiLo(),
							  BEValue.GetUInt8HiHi() };

		for(int i = 0; i < 4; i++)
		{
			if(iFieldIndex == 0 || iFieldIndex == (i + 1))
			{
				if(((CDecEdit*)m_pDecEdits[i])->IsSigned())
					strValue.Format(strSignedFmt, i8Values[i]);
				else
					strValue.Format(_T("%u"), ui8Values[i]);
				m_pDecEdits[i]->SetText(strValue);
			}
		}
	}
}

void CBEShowCtrl::UpdateCmnFields(int iFieldIndex /*= 0*/)
{
	if(!IsWindow(m_hWnd)) return;

	if(Is32())
	{
		m_pHexEdit32->SetText(BEValue.GetHex());
		m_pOctEdit32->SetText(BEValue.GetOct());
		m_pBinEdit32->SetText(BEValue.GetBin());
	} else
	if(Is16())
	{
		if(iFieldIndex == 0 || iFieldIndex == 1)
		{
			m_pHexEdit16L->SetText(BEValue.GetHex(BEValue.low));
			m_pOctEdit16L->SetText(BEValue.GetOct(BEValue.low));
			m_pBinEdit16L->SetText(BEValue.GetBin(BEValue.low));
		}
		if(iFieldIndex == 0 || iFieldIndex == 2)
		{
			m_pHexEdit16H->SetText(BEValue.GetHex(BEValue.hiw));
			m_pOctEdit16H->SetText(BEValue.GetOct(BEValue.hiw));
			m_pBinEdit16H->SetText(BEValue.GetBin(BEValue.hiw));
		}
	} else
	if(Is8())
	{
		if(iFieldIndex == 0 || iFieldIndex == 1)
		{
			m_pHexEdit8LL->SetText(BEValue.GetHex(BEValue.low | BEValue.lob));
			m_pOctEdit8LL->SetText(BEValue.GetOct(BEValue.low | BEValue.lob));
			m_pBinEdit8LL->SetText(BEValue.GetBin(BEValue.low | BEValue.lob));
		}
		if(iFieldIndex == 0 || iFieldIndex == 2)
		{
			m_pHexEdit8LH->SetText(BEValue.GetHex(BEValue.low | BEValue.hib));
			m_pOctEdit8LH->SetText(BEValue.GetOct(BEValue.low | BEValue.hib));
			m_pBinEdit8LH->SetText(BEValue.GetBin(BEValue.low | BEValue.hib));
		}
		if(iFieldIndex == 0 || iFieldIndex == 3)
		{
			m_pHexEdit8HL->SetText(BEValue.GetHex(BEValue.hiw | BEValue.lob));
			m_pOctEdit8HL->SetText(BEValue.GetOct(BEValue.hiw | BEValue.lob));
			m_pBinEdit8HL->SetText(BEValue.GetBin(BEValue.hiw | BEValue.lob));
		}
		if(iFieldIndex == 0 || iFieldIndex == 4)
		{
			m_pHexEdit8HH->SetText(BEValue.GetHex(BEValue.hiw | BEValue.hib));
			m_pOctEdit8HH->SetText(BEValue.GetOct(BEValue.hiw | BEValue.hib));
			m_pBinEdit8HH->SetText(BEValue.GetBin(BEValue.hiw | BEValue.hib));
		}
	} else
	if(IsColor())
	{
		m_pHexClrEditWeb->SetText(m_pHexClrEditWeb->GetValueByEndian());
		m_pHexClrEditWin->SetText(m_pHexClrEditWin->GetValueByEndian());
	}
}

CString CBEShowCtrl::CBEValue::GetHex(int iPos /*= 0*/)
{
	CString strFix(theApp.Settings.IsHexUpperCase() ? _T("%X") : _T("%x")), strRet;

	if(theApp.Settings.IsZeroFill())
	{
		if((iPos & lob) == lob ||
		   (iPos & hib) == hib)
		{
			strFix.Insert(1, _T("02"));
		} else
		if((iPos & low) == low ||
		   (iPos & hiw) == hiw)
		{
			strFix.Insert(1, _T("04"));
		} else
			strFix.Insert(1, _T("08"));
	}

	if((iPos & low) == low)
	{
		if((iPos & lob) == lob)
			strRet.Format(strFix, LOBYTE(LOWORD(m_i32Value)));
		else
		if((iPos & hib) == hib)
			strRet.Format(strFix, HIBYTE(LOWORD(m_i32Value)));
		else
			strRet.Format(strFix, LOWORD(m_i32Value));
	} else
	if((iPos & hiw) == hiw)
	{
		if((iPos & lob) == lob)
			strRet.Format(strFix, LOBYTE(HIWORD(m_i32Value)));
		else
		if((iPos & hib) == hib)
			strRet.Format(strFix, HIBYTE(HIWORD(m_i32Value)));
		else
			strRet.Format(strFix, HIWORD(m_i32Value));
	} else
	{
		strRet.Format(strFix, m_i32Value);
	}

	return strRet;
}

CString CBEShowCtrl::CBEValue::GetOct(int iPos /*= 0*/)
{
	CString strFix(_T("%o")), strRet;

	if(theApp.Settings.IsZeroFill())
	{
		if((iPos & lob) == lob ||
		   (iPos & hib) == hib)
		{
			strFix.Insert(1, _T("03"));
		} else
		if((iPos & low) == low ||
		   (iPos & hiw) == hiw)
		{
			strFix.Insert(1, _T("06"));
		} else
			strFix.Insert(1, _T("011"));
	}

	if((iPos & low) == low)
	{
		if((iPos & lob) == lob)
			strRet.Format(strFix, LOBYTE(LOWORD(m_i32Value)));
		else
		if((iPos & hib) == hib)
			strRet.Format(strFix, HIBYTE(LOWORD(m_i32Value)));
		else
			strRet.Format(strFix, LOWORD(m_i32Value));
	} else
	if((iPos & hiw) == hiw)
	{
		if((iPos & lob) == lob)
			strRet.Format(strFix, LOBYTE(HIWORD(m_i32Value)));
		else
		if((iPos & hib) == hib)
			strRet.Format(strFix, HIBYTE(HIWORD(m_i32Value)));
		else
			strRet.Format(strFix, HIWORD(m_i32Value));
	} else
	{
		strRet.Format(strFix, m_i32Value);
	}

	return strRet;
}

CString CBEShowCtrl::CBEValue::GetBin(int iPos /*= 0*/)
{
	CString strRet;
	int i, l, h, bit;

	if((iPos & low) == low)
	{
		if((iPos & lob) == lob) {
			l = 8; h = l - 8;
		} else if((iPos & hib) == hib) {
			l = 16; h = l - 8;
		} else {
			l = 16; h = l - 16;
		}
	} else
	if((iPos & hiw) == hiw)
	{
		if((iPos & lob) == lob) {
			l = 24; h = l - 8;
		} else if((iPos & hib) == hib) {
			l = 32; h = l - 8;
		} else {
			l = 32; h = l - 16;
		}
	} else
	{
		l = 32;
		h = 0;
	}

	for(i = (l - 1); i >= h; i--)
	{
		bit = (m_i32Value >> i) & 1;
		strRet += bit == 1 ? _T("1") : _T("0");
	}

	if(!theApp.Settings.IsZeroFill())
		CBEShowCtrl::TrimZeroes(strRet);

	return strRet;
}

CString CBEShowCtrl::CBEValue::GetHexRGBA(LPCTSTR lpszSign /*= _T("")*/, BOOL bReturnAlpha /*= TRUE*/)
{
	CString strFix( _T("%02x%02x%02x") ), strRet;

	if(bReturnAlpha) strFix.Insert(0, _T("%02x"));

	if(theApp.Settings.IsHexUpperCase()) strFix.MakeUpper();

	if(bReturnAlpha)
		strRet.Format(strFix, GetRed(), GetGreen(), GetBlue(), GetAlpha());
	else
		strRet.Format(strFix, GetRed(), GetGreen(), GetBlue());
	
	if(lpszSign) if(_tcslen(lpszSign) > 0) strRet.Insert(0, lpszSign);
	return strRet;
}

CString CBEShowCtrl::CBEValue::GetHexABGR(LPCTSTR lpszSign /*= _T("")*/, BOOL bReturnAlpha /*= TRUE*/)
{
	CString strFix( _T("%02x%02x%02x") ), strRet;

	if(bReturnAlpha) strFix.Insert(0, _T("%02x"));

	if(theApp.Settings.IsHexUpperCase()) strFix.MakeUpper();

	if(bReturnAlpha)
		strRet.Format(strFix, GetAlpha(), GetBlue(), GetGreen(), GetRed());
	else
		strRet.Format(strFix, GetBlue(), GetGreen(), GetRed());
	
	if(lpszSign) if(_tcslen(lpszSign) > 0) strRet.Insert(0, lpszSign);
	return strRet;
}

BOOL CBEShowCtrl::CBEValue::SetHexRGBA(CString strRGBA, BOOL bApplyAlpha /*= TRUE*/, BOOL bIsBigEndian /*= TRUE*/)
{
	int iValue = bApplyAlpha ? 0 : ((int)((DWORD)GetAlpha() << 24));
	const int iBase = 16;
	const int iBits = bApplyAlpha ? 32 : 24;
	const int iLen = iBits / 4;

	if(CheckCmnValue(strRGBA, iBase, iBits))
	{
		HasAdditionRemove(strRGBA, iBase, TRUE);

		while(strRGBA.GetLength() < iLen)
			strRGBA.Insert(bIsBigEndian ? strRGBA.GetLength() : 0, _T("0"));

		if(bIsBigEndian)
		{
			// change big to little endian
			for(int i = 0; i < ((iLen / 2) - 1); i++)
			{
				strRGBA.Insert(strRGBA.GetLength() - (i * 2), strRGBA.Left(2));
				strRGBA.Delete(0, 2);
			}
		}

		iValue |= (int)_tcstol(strRGBA, NULL, iBase);
		SetInt(iValue);
		return TRUE;
	} else {
		AfxMessageBox(_T("SetHexRGBA error"));
		return FALSE;
	}
}

void CBEShowCtrl::SetSigned(bool bSet, int iFldIdx /*= 0*/)
{
	if(Is32())
		((CDecEdit*)m_pDecEdits[0])->SetSigned(bSet);
	else
	if(Is16())
		for(int i = 0; i < 2; i++)
			((CDecEdit*)m_pDecEdits[i])->SetSigned(bSet);
	else
	if(Is8())
		for(int i = 0; i < 4; i++)
			((CDecEdit*)m_pDecEdits[i])->SetSigned(bSet);
}

void CBEShowCtrl::SetSignedSet(UINT uiSingSet)
{
	if(!IsColor())
	{
		for(int i = 0; i < m_iFieldsCount; i++)
		{
			m_pDecEdits[i]->SetSigned(((uiSingSet >> i) & 1) == 1);
		}
	}
}

UINT CBEShowCtrl::GetSignedSet()
{
	UINT uiRet = 0;

	if(!IsColor())
	{
		for(int i = 0; i < m_iFieldsCount; i++)
		{
			uiRet |= m_pDecEdits[i]->IsSigned() ? (1 << i) : 0;
		}
	}

	return uiRet;

	/*if(Is32())
		uiRet = m_pDecEdits[0]->IsSigned() ? 1 : 0;
	else
	if(Is16())
		uiRet = (m_pDecEdits[0]->IsSigned() ? 1 : 0) |
				(m_pDecEdits[1]->IsSigned() ? (1 << 1) : 0);
	else
	if(Is8())
		uiRet = (m_pDecEdits[0]->IsSigned() ? 1 : 0) |
				(m_pDecEdits[1]->IsSigned() ? (1 << 1) : 0) |
				(m_pDecEdits[2]->IsSigned() ? (1 << 1) : 0) |
				(m_pDecEdits[3]->IsSigned() ? (1 << 1) : 0) |*/

}

void CBEShowCtrl::GetFldX1X2(int iFld, int& x1, int& x2)
{
	if(Is32())
	{
		x1 = 1;
		x2 = 483;
	}
	if(Is16()) {
		x1 = (iFld - 1) * 241 + 1;
		x1 -= iFld - 1;
		x2 = x1 + 240 + 1;
	} else
	if(Is8()) {
		x1 = (iFld - 1) * 121 + 1;
		x1 -= iFld - 1;
		x2 = x1 + 120 + 1;
		if(iFld != 2) x2++;
	}
}

void CBEShowCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CPoint pt;
	CRect surc, selrc;
	GetCursorPos((LPPOINT)&pt);
	ScreenToClient((LPPOINT)&pt);

	if(m_iSUHot > 0) surc = GetSUHotRect(m_iSUHot);
	if(m_iSelected > 0) selrc = GetSUHotRect(m_iSelected);

	if(selrc.PtInRect(pt))
	{
		if(m_iSelected != m_iSUHot)
		{
			if(m_iSUHot > 0) InvalidateRect(surc, FALSE);
			m_iSUHot = m_iSelected;
			if(m_iSelected > 0) InvalidateRect(selrc, FALSE);
		}
	} else
	if(m_iSUHot > 0)
	{
		InvalidateRect(surc, FALSE);
		m_iSUHot = 0;
	} else
	if(IsSelected())
	{
		CRect checkFuncs[] = { GetFuncRect(Funcs::Unset),
			GetFuncRect(Funcs::Set),
			GetFuncRect(Funcs::BitNot) };

		m_iFuncHot = 0;
		for(int i = 0; i < 3; i++)
		{
			if(checkFuncs[i].PtInRect(pt))
			{
				m_iFuncHot = i + 1;
				break;
			}
		}
	} else
		m_iFuncHot = 0;


	CDialog::OnTimer(nIDEvent);
}

CRect CBEShowCtrl::GetSUHotRect(int iFieldIndex)
{
	int sx = 57, sy = 2, cx = 75, cy = 24;

	if(iFieldIndex > 0)
	{
		/*if(Is32())
		{
			sx = 57;
		} else*/
		if(Is16())
		{
			sx = 240 * (iFieldIndex - 1) + 57;
		} else
		if(Is8())
		{
			sx = 120 * (iFieldIndex - 1) + 48;
			cx = 20;
		}
	}

	return CRect(sx, sy, sx + cx, sy + cy);
}

BOOL CBEShowCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		return FALSE;
	} else
	if(pMsg->message == WM_ERASEBKGND)
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CBEShowCtrl::CEdits::OnEnUpdate()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here

	BOOL m_bReseted = FALSE;
	CString strValidChars(CString(BEVALUE_VALIDCHARS).Left(m_iBase)),
			strSigns(BEVALUE_SIGNES),
			strValue, str;

	GetWindowText(strValue);
	strValue.MakeUpper();

	/*for(int i = 0; i < strValue.GetLength(); i += sizeof(TCHAR))
	{
		if(m_iBase == 10)
			if(i == 0 && strSigns.Find(strValue[i]) != -1 && m_bSigned) continue;

		if(strValidChars.Find(strValue[i]) == -1)
		{
			if(m_iBase == 10)
				((CBEShowCtrl*)GetParent())->UpdateDecFields(m_iByteIndex);
			else
				((CBEShowCtrl*)GetParent())->UpdateCmnFields(m_iByteIndex);

			break;
		}
	}*/
}

void CBEShowCtrl::CEdits::SetText(CString strText)
{
	if(!IsTextSource())
	{
		m_strText = strText;
		SetWindowText(strText);
		FixSel(TRUE);
	} else {
		CString strGet, strFix;
		int nStart, nEnd, nSelAdd;

		GetWindowText(strGet);
		GetSel(nStart, nEnd);

		strFix = strText; //strGet;

		if(IsDec()) 
		{
			CBEShowCtrl::TrimZeroes(strFix);
			
			if(strFix != strGet)
			{
				nSelAdd = strFix.GetLength() - strGet.GetLength();
				SetWindowText(strFix);
				SetSel(nStart + nSelAdd, nEnd + nSelAdd);
			}
		} else {
			CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();

			if(IsHexColor())
				strFix = ((CHexClrEdit*)this)->GetValueByEndian();
			else
				strFix = pPar->BEValue.GetCmnValue(pPar->FieldToPos(m_iByteIndex), m_iBase);

			if(strFix.Compare(strGet) != 0)
			{
				nSelAdd = strFix.GetLength() - strGet.GetLength();
				SetWindowText(strFix);
				SetSel(nStart + nSelAdd, nEnd + nSelAdd);
			}
		}

		GetWindowText(m_strText);
		Invalidate(FALSE);
	}
}

void CBEShowCtrl::CEdits::OnEnKillfocus()
{
	if(IsDirty() && !m_bIsApplyed) SetWindowText(m_strText);
}

void CBEShowCtrl::SetUpdateCtrls(BOOL bDoUpdate /*= TRUE*/)
{
	CBEShowCtrl *pUpdCtrl;

	for(int i = 0; i < m_UpdateCtrls.GetCount(); i++)
	{
		pUpdCtrl = (CBEShowCtrl*)m_UpdateCtrls[i];
		pUpdCtrl->BEValue.SetInt32(BEValue.GetInt32());
		if(bDoUpdate) pUpdCtrl->UpdateFields();
	}
}

void CBEShowCtrl::SetUpdateCtrlsByFieldIndex(int iFldIdx, bool bDoUpdate /*= TRUE*/)
{
	CBEShowCtrl *pUpdCtrl;
	int iFldIdxSet;

	for(int i = 0; i < m_UpdateCtrls.GetCount(); i++)
	{
		pUpdCtrl = (CBEShowCtrl*)m_UpdateCtrls[i];

		if(pUpdCtrl->Is32())
			iFldIdxSet = 1;
		else
		if(pUpdCtrl->Is16())
			if(iFldIdx >= 3 && iFldIdx <= 4) iFldIdxSet = 2; else iFldIdxSet = 1;
		else
			iFldIdxSet = iFldIdx;

		pUpdCtrl->BEValue.SetInt32(BEValue.GetInt32());
		//pUpdCtrl->BEValue.SetInt(BEValue.GetInt(FieldToPos(iFldIdx)), FieldToPos(iFldIdx));

		if(bDoUpdate) pUpdCtrl->UpdateFields(iFldIdxSet);
	}
}

void CBEShowCtrl::CEdits::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(OnCharValidate(&nChar, nRepCnt, nFlags, GetBase(), m_bSigned))
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}

int CBEShowCtrl::GetBEValueType(int iFieldIndex /*= 1*/)
{
	return ((CDecEdit*)m_pDecEdits[iFieldIndex - 1])->GetBEValueType();
}

int CBEShowCtrl::CDecEdit::GetBEValueType(void)
{
	CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();
	int iRet = 0;

	if(pPar->Is32())
		iRet |= pPar->BEValue.bit32;
	else
	if(pPar->Is16())
		iRet |= pPar->BEValue.bit16;
	else
	if(pPar->Is8())
		iRet |= pPar->BEValue.bit8;

	if(m_bSigned)
		iRet |= pPar->BEValue._signed;
	else
		iRet |= pPar->BEValue._unsigned;

	return iRet;
}

BOOL CBEShowCtrl::CEdits::OnCharValidate(UINT *nChar, UINT nRepCnt, UINT nFlags, int iBase, BOOL bSigned /*= FALSE*/)
{
	CString strValue;

	if(*nChar == VK_BACK)
	{
		return TRUE;
	}

	if(*nChar == VK_RETURN)
	{
		GetWindowText(strValue);
		ApplyText(strValue);
		return FALSE;
	}

	int ns, ne;
	CString sChar;
	CString strValidChars(CString(BEVALUE_VALIDCHARS).Left(iBase)),
			strSigns(BEVALUE_SIGNES);

	sChar.AppendChar(TCHAR(*nChar));
	sChar.MakeUpper();
	GetSel(ns, ne);
	GetWindowText(strValue);

	if(IsDec())
	{
		if(bSigned && strSigns.Find(sChar) != -1)
		{
			if(ns == 0 && strValue.FindOneOf(strSigns) == -1)
			{
				return TRUE;
			}
			return FALSE;
		}
	}

	if(strValidChars.Find(sChar) != -1)
	{
		if(IsDec())
		{
			if(!(strValue.GetLength() > 0 && 
				 strSigns.Find(strValue[0]) != -1 &&
			     ns == 0 && ne == 0))
			{
				return TRUE;
			}
			return FALSE;
		} else
		if(IsHex())
		{
			if(( theApp.Settings.IsHexUpperCase() && _istlower(*nChar)) ||
			   (!theApp.Settings.IsHexUpperCase() && _istupper(*nChar)))
			{
				*nChar = theApp.Settings.IsHexUpperCase() ? 
					_totupper(*nChar) :
					_totlower(*nChar) ;
				PostMessage(WM_CHAR, *nChar, 0);
				return FALSE;
			}
		}
		return TRUE;
	} else 
	if(sChar == BEVALUE_WEB_HEX_SIGN[0] || sChar == BEVALUE_WIN_HEX_SIGN[0])
		if(_totupper(strValue[0]) != sChar && ns == 0 && ne == 0) return TRUE;

	return FALSE;
}

int CBEShowCtrl::CBEValue::GetInt(int iPos /*= 0*/)
{
	int iValue = 0;

	if((iPos & low) == low)
	{
		if((iPos & lob) == lob)
			iValue = INT32(LOBYTE(LOWORD(m_i32Value)));
		else if((iPos & hib) == hib)
			iValue = INT32(HIBYTE(LOWORD(m_i32Value)));
		else
			iValue = INT32(LOWORD(m_i32Value));
	} else
	if((iPos & hiw) == hiw)
	{
		if((iPos & lob) == lob)
			iValue = INT32(LOBYTE(HIWORD(m_i32Value)));
		else if((iPos & hib) == hib)
			iValue = INT32(HIBYTE(HIWORD(m_i32Value)));
		else
			iValue = INT32(HIWORD(m_i32Value));
	} else
		iValue = INT32(m_i32Value);

	return iValue;
}

void CBEShowCtrl::CBEValue::SetInt(int iValue, int iPos /*= 0*/)
{
	if((iPos & low) == low)
	{
		if((iPos & lob) == lob)
			m_i32Value = INT32(MAKELONG(MAKEWORD(iValue, HIBYTE(LOWORD(m_i32Value))), HIWORD(m_i32Value)));
		else if((iPos & hib) == hib)
			m_i32Value = INT32(MAKELONG(MAKEWORD(LOBYTE(LOWORD(m_i32Value)), iValue), HIWORD(m_i32Value)));
		else
			m_i32Value = INT32(MAKELONG(iValue, HIWORD(m_i32Value)));
	} else
	if((iPos & hiw) == hiw)
	{
		if((iPos & lob) == lob)
			m_i32Value = INT32(MAKELONG(LOWORD(m_i32Value), MAKEWORD(iValue, HIBYTE(HIWORD(m_i32Value)))));
		else if((iPos & hib) == hib)
			m_i32Value = INT32(MAKELONG(LOWORD(m_i32Value), MAKEWORD(LOBYTE(HIWORD(m_i32Value)), iValue)));
		else
			m_i32Value = INT32(MAKELONG(LOWORD(m_i32Value), iValue));
	} else
		m_i32Value = INT32(iValue);
}

BOOL CBEShowCtrl::CBEValue::SetDec(CString strValue, int iPos /*= 0*/, BOOL bSigned /*= FALSE*/)
{
	int iValue = 0;

	if(CheckDecValue(strValue, PosToType(iPos) | (bSigned ? _signed : _unsigned)))
	{
		if(bSigned)
			iValue = (int)_tcstol(strValue, NULL, 10);
		else
			iValue = (int)_tcstoul(strValue, NULL, 10);

		SetInt(iValue, iPos);
		return TRUE;
	} else {
		AfxMessageBox(_T("Number error"));
		return FALSE;
	}
}

BOOL CBEShowCtrl::CBEValue::SetCmnValue(CString strValue, int iPos, int iBase)
{
	int iValue = 0;

	if(CheckCmnValue(strValue, iBase, TypeToBits(PosToType(iPos))))
	{
		iValue = (int)_tcstoul(strValue, NULL, iBase);
		SetInt(iValue, iPos);
		return TRUE;
	} else {
		AfxMessageBox(_T("Common number error"));
		return FALSE;
	}
}

CString CBEShowCtrl::CBEValue::GetCmnValue(int iPos, int iBase)
{
	if(iBase == 16)
		return GetHex(iPos);
	else
	if(iBase == 8)
		return GetOct(iPos);
	else
	if(iBase == 2)
		return GetBin(iPos);

	return CString(_T("0"));
}

int CBEShowCtrl::CBEValue::TypeToBits(int iType /* bit32, bit16, bit8 */ )
{
	int iBits;

	switch(iType)
	{
	case bit32: iBits = 32; break;
	case bit16: iBits = 16; break;
	case bit8:  iBits = 8;  break;
	}

	return iBits;
}


int CBEShowCtrl::CBEValue::PosToType(int iPos)
{
	int iRet;

	if((iPos & low) == low ||
	   (iPos & hiw) == hiw)
	{
		if((iPos & lob) == lob ||
		   (iPos & hib) == hib)
			iRet = bit8;
		else
			iRet = bit16;
	} else
		iRet = bit32;

	return iRet;
}


int CBEShowCtrl::CBEValue::PosToBits(int iPos)
{
	return TypeToBits(PosToType(iPos));
}

BOOL CBEShowCtrl::CEdits::ApplyText(CString strText)
{
	CBEShowCtrl *pPar = (CBEShowCtrl*)GetParent();
	BOOL bDoApply = FALSE;
	m_bIsApplyed = TRUE;

	if(IsDec())
		bDoApply = pPar->BEValue.SetDec(strText, pPar->FieldToPos(m_iByteIndex), m_bSigned);
	else
	if(!IsHexColor())
		bDoApply = pPar->BEValue.SetCmnValue(strText, pPar->FieldToPos(m_iByteIndex), m_iBase);
	else
		bDoApply = pPar->BEValue.SetHexRGBA(strText, FALSE, ((CHexClrEdit*)this)->IsBigEndian());

	if(bDoApply)
	{
		SetTextSource();
		pPar->UpdateFields();
		pPar->SetUpdateCtrls();
		SetTextSource(FALSE);
	}

	m_bIsApplyed = FALSE;
	return bDoApply;
}

int CBEShowCtrl::FieldToPos(int iFld)
{
	int iRet;

	if(Is32())
		iRet = 0;
	else
	if(Is16())
		if(iFld == 1)
			iRet = BEValue.low;
		else
			iRet = BEValue.hiw;
	else
	if(Is8() || IsColor())
	{
		if(iFld >= 1 && iFld <= 2) iRet = BEValue.low; else iRet = BEValue.hiw;
		if(iFld == 1 || iFld == 3) iRet |= BEValue.lob;
		if(iFld == 2 || iFld == 4) iRet |= BEValue.hib;
	}

	return iRet;
}

void CBEShowCtrl::TrimZeroes(CString& strValue)
{
	CString strSigns(BEVALUE_SIGNES), strSignKeep(_T(""));
	int iZLen = 0;

	if(strValue.IsEmpty()) return;

	if(strSigns.Find(strValue[0]) != -1)
	{
		if(strValue.GetLength() == 1) return;
		strSignKeep = strValue[0];
		strValue.Delete(0);
	}

	strValue.TrimLeft(_T('0'));
	if(strValue.IsEmpty())
		strValue = _T("0");
	else
	if(!strSignKeep.IsEmpty())
		strValue.Insert(0, strSignKeep);
}

void CBEShowCtrl::CEdits::FixSel(BOOL bMoveToEnd /*= FALSE*/)
{
	int iEndChar = GetWindowTextLength();
	if(theApp.Settings.IsSelectAllOnFocus() && !bMoveToEnd)
		SetSel(0, -1);
	else
		SetSel(iEndChar, iEndChar);
}

void CBEShowCtrl::PaintDecColors(CDC* pDC /*= NULL*/)
{
	bool bReleaseDC = false;

	if(m_bIsColorInit && IsColor())
	{
		if(!pDC)
		{
			pDC = GetDC();
			bReleaseDC = true;
		}

		for(int i = 0; i < 4; i++)
		{
			if(i < 3)
			{
				COLORREF crFld = BEValue.GetInt(FieldToPos(i + 1)) << (i * 8);
				pDC->FillSolidRect(m_rcDecColors[i], crFld);
			} else {
				CDC memDC;
				CBitmap memBmp;

				memDC.CreateCompatibleDC(pDC);
				memBmp.CreateCompatibleBitmap(pDC, m_rcDecColors[i].Width(), m_rcDecColors[i].Height());

				HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(memBmp);

				CRect rcMemClr(m_rcDecColors[i]);

				rcMemClr.MoveToXY(0, 0);

				CPWGraphics::DrawTransparentBg(&memDC, rcMemClr);
				CPWGraphics::FillAlphaRect(&memDC, rcMemClr, BEValue.GetInt());

				pDC->BitBlt(m_rcDecColors[i].left, m_rcDecColors[i].top, m_rcDecColors[i].Width(), m_rcDecColors[i].Height(),
							&memDC, 0, 0, SRCCOPY);

				memDC.SelectObject(hOldMemBmp);
				memDC.DeleteDC();
				memBmp.DeleteObject();

				//CPWGraphics::DrawTransparentBg(pDC, m_rcDecColors[i]);
				//CPWGraphics::FillAlphaRect(pDC, m_rcDecColors[i], BEValue.GetInt());
			}
		}

		if(bReleaseDC) ReleaseDC(pDC);
	}
}

void CBEShowCtrl::PaintBigColors(CDC* pDC /*= NULL*/)
{
	bool bReleaseDC = false;

	if(m_bIsColorInit && IsColor())
	{
		if(!pDC)
		{
			pDC = GetDC();
			bReleaseDC = true;
		}

		CDC memDC;
		//CBitmap memBmp;
		HBITMAP hMemBmp;
		BITMAP bmpLbl, bmpMem;
		BITMAPINFO bi;
		RGBQUAD *lpMemBits;
		RGBAQUAD *lpLblBits;

		CBrush brColor(BEValue.GetRGB());
		COLORREF crBorder = 0x999999;
		CPen pnBorder(PS_SOLID, 1, crBorder);
		int i, x = 79, y = 55;

		CRect rcRGB (x, y,      x + 84, y + 20);
		CRect rcRGBA(x, y + 19, x + 84, y + 39);
		CRect rcMem(0, 0, 84, 18);

		m_rcBigColors.UnionRect(rcRGB, rcRGBA);

		CRect *prcClr[] = { &rcRGB, &rcRGBA };
		CBitmap *pbmLbls[] = { &m_pBitmaps->m_bmLblRGB, &m_pBitmaps->m_bmLblRGBA };

		HGDIOBJ hOldPen = (HGDIOBJ)pDC->SelectObject(pnBorder);
		HGDIOBJ hOldBrush = (HGDIOBJ)pDC->SelectObject(GetStockObject(NULL_BRUSH));

		pDC->Rectangle(rcRGB);
		pDC->Rectangle(rcRGBA);

		rcRGB.DeflateRect(1, 1);
		rcRGBA.DeflateRect(1, 1);

		memDC.CreateCompatibleDC(pDC);
		
		ZeroMemory(&bi, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 32;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biWidth = rcRGB.Width();
		bi.bmiHeader.biHeight = rcRGB.Height();
		bi.bmiHeader.biSizeImage = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight * 4;

		hMemBmp = CreateDIBSection(pDC->GetSafeHdc(),
								   &bi,
								   DIB_RGB_COLORS,
								   (void**)&lpMemBits, NULL, NULL);

		GetObject(hMemBmp, sizeof(bmpMem), &bmpMem);

		HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(hMemBmp);
		HGDIOBJ hOldMemPen = (HGDIOBJ)memDC.SelectObject(GetStockObject(NULL_PEN));
		HGDIOBJ hOldMemBrush = (HGDIOBJ)memDC.GetCurrentBrush();

		for(i = 0; i < 2; i++)
		{
			if(i == 0)
			{
				memDC.FillRect(rcMem, &brColor);
			} else
			{
				CPWGraphics::DrawTransparentBg(&memDC, rcMem);
				CPWGraphics::FillAlphaRect(&memDC, rcMem, BEValue.GetRGBA());
			}

			pbmLbls[i]->GetBitmap(&bmpLbl);
			ZeroMemory(&bi, sizeof(BITMAPINFO));
			bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
			bi.bmiHeader.biPlanes = bmpLbl.bmPlanes;
			bi.bmiHeader.biBitCount = bmpLbl.bmBitsPixel;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biWidth = bmpLbl.bmWidth;
			bi.bmiHeader.biHeight = bmpLbl.bmHeight;
			bi.bmiHeader.biSizeImage = bmpLbl.bmWidthBytes * bmpLbl.bmHeight;

			lpLblBits = (RGBAQUAD*)GlobalAlloc(GPTR, bmpLbl.bmWidthBytes * bmpLbl.bmHeight);
			GetDIBits(pDC->GetSafeHdc(),
					  *pbmLbls[i],
					  0,
					  bmpLbl.bmHeight,
					  lpLblBits,
					  &bi,
					  DIB_RGB_COLORS);

			int lblpos, mempos;

			for(x = 0; x < bmpLbl.bmWidth; x++)
			{
				for(y = 0; y < bmpLbl.bmHeight; y++)
				{
					lblpos = BITS_XY(x, y, bmpLbl.bmWidth, bmpLbl.bmHeight);
					mempos = BITS_XY(x, y, bmpMem.bmWidth, bmpMem.bmHeight);

					if(lpLblBits[lblpos].rgbaAlpha > 0)
					{
						lpMemBits[mempos].rgbRed   = ALPHABYTE(lpLblBits[lblpos].rgbaRed, lpMemBits[mempos].rgbRed, lpLblBits[lblpos].rgbaAlpha);
						lpMemBits[mempos].rgbGreen = ALPHABYTE(lpLblBits[lblpos].rgbaGreen, lpMemBits[mempos].rgbGreen, lpLblBits[lblpos].rgbaAlpha);
						lpMemBits[mempos].rgbBlue  = ALPHABYTE(lpLblBits[lblpos].rgbaBlue, lpMemBits[mempos].rgbBlue, lpLblBits[lblpos].rgbaAlpha);
					}
				}
			}

			pDC->BitBlt(prcClr[i]->left, prcClr[i]->top, prcClr[i]->Width(), prcClr[i]->Height(),
						&memDC, 0, 0, SRCCOPY);

			GlobalFree(lpLblBits);
		}

		memDC.SelectObject(hOldMemBmp);
		memDC.SelectObject(hOldMemPen);
		memDC.SelectObject(hOldMemBrush);

		memDC.DeleteDC();
		DeleteObject((HGDIOBJ)hMemBmp);

		
		pDC->SelectObject(hOldPen);
		pDC->SelectObject(hOldBrush);

		if(bReleaseDC) ReleaseDC(pDC);
	}
}

void CBEShowCtrl::PaintCircle(CDC* pDC /*= NULL*/)
{
	HBITMAP hTriMask, hFinal;
	CSize szTriMask(66, 63);
	RGBAQUAD *lpTriMaskBits, *lpFinalBits, *lpMaskBits, *lpBgBits;
	BITMAPINFO bi;
	BITMAP maskBmp, bgBmp;
	bool bReleaseDC = false;

	if(m_bIsColorInit && IsColor())
	{
		if(!pDC)
		{
			pDC = GetDC();
			bReleaseDC = true;
		}

		m_pBitmaps->m_bmCircleOn.GetBitmap(&bgBmp);
		m_pBitmaps->m_bmCircleMask.GetBitmap(&maskBmp);

		// create TriMask with alpha channel (32 bit, rgba)
		ZeroMemory(&bi, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 32;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biWidth = szTriMask.cx;
		bi.bmiHeader.biHeight = szTriMask.cy;
		bi.bmiHeader.biSizeImage = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight * 4;

		hTriMask = CreateDIBSection(pDC->GetSafeHdc(),
									&bi,
									DIB_RGB_COLORS,
									(void**)&lpTriMaskBits, NULL, NULL);

		// create Final (24bit, rgb, alpha is ignored)
		ZeroMemory(&bi, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 32;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biWidth = bgBmp.bmWidth;
		bi.bmiHeader.biHeight = bgBmp.bmHeight;
		bi.bmiHeader.biSizeImage = bgBmp.bmWidthBytes * bi.bmiHeader.biHeight;
		hFinal = CreateDIBSection(pDC->GetSafeHdc(),
								  &bi,
								  DIB_RGB_COLORS,
								  (void**)&lpFinalBits, NULL, NULL);

		if(hTriMask && hFinal)
		{
			int i, x, y;
			CPoint ptCircles[] = { CPoint(0, 0),
								   CPoint(22, 0),
								   CPoint(11, 19) };

			// load CircleMask bits
			ZeroMemory(&bi, sizeof(BITMAPINFO));
			bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
			bi.bmiHeader.biWidth = maskBmp.bmWidth;
			bi.bmiHeader.biHeight = maskBmp.bmHeight;
			bi.bmiHeader.biPlanes = maskBmp.bmPlanes;
			bi.bmiHeader.biBitCount = maskBmp.bmBitsPixel;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biSizeImage = maskBmp.bmWidthBytes * maskBmp.bmHeight;

			lpMaskBits = (RGBAQUAD*)GlobalAlloc(GPTR, maskBmp.bmWidthBytes * maskBmp.bmHeight);

			GetDIBits(pDC->GetSafeHdc(),
					  m_pBitmaps->m_bmCircleMask,
					  0,
					  maskBmp.bmHeight,
					  (LPVOID)lpMaskBits,
					  &bi,
					  DIB_RGB_COLORS);

			// load CircleBg bits
			ZeroMemory(&bi, sizeof(BITMAPINFO));
			bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
			bi.bmiHeader.biWidth = bgBmp.bmWidth;
			bi.bmiHeader.biHeight = bgBmp.bmHeight;
			bi.bmiHeader.biPlanes = bgBmp.bmPlanes;
			bi.bmiHeader.biBitCount = bgBmp.bmBitsPixel;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biSizeImage = bgBmp.bmWidthBytes * bgBmp.bmHeight;

			lpBgBits = (RGBAQUAD*)GlobalAlloc(GPTR, bgBmp.bmWidthBytes * bgBmp.bmHeight);

			CBitmap *pBmCircleOn = theApp.Settings.IsApplyAlphaOnTriCircle() ? &m_pBitmaps->m_bmCircleOn :
																			   &m_pBitmaps->m_bmCircleWhiteOn;
			CBitmap *pBmCircleOff = theApp.Settings.IsApplyAlphaOnTriCircle() ? &m_pBitmaps->m_bmCircleOff :
																			    &m_pBitmaps->m_bmCircleWhiteOff;

			GetDIBits(pDC->GetSafeHdc(),
					  GetSelected() == 0 ? *pBmCircleOff : *pBmCircleOn,
					  0,
					  bgBmp.bmHeight,
					  (LPVOID)lpBgBits,
					  &bi,
					  DIB_RGB_COLORS);

			// Copy background bits into the final bitmap and free the mem
			memcpy(lpFinalBits, lpBgBits, bgBmp.bmWidthBytes * bgBmp.bmHeight);
			GlobalFree(lpBgBits);

			// paint TriCircles (RGB)
			int tripos, cirpos;

			RGBQUAD crgb[3];
			memset(crgb, NULL, sizeof(crgb));

			crgb[0].rgbRed   = BEValue.GetRed();
			crgb[1].rgbGreen = BEValue.GetGreen();
			crgb[2].rgbBlue  = BEValue.GetBlue();

			for(i = 0; i < 3; i++)
			{
				for(x = 0; x < maskBmp.bmWidth; x++)
				{
					for(y = 0; y < maskBmp.bmHeight; y++)
					{
						tripos = BITS_XY(ptCircles[i].x + x, ptCircles[i].y + y, szTriMask.cx, szTriMask.cy);
						cirpos = BITS_XY(x, y, maskBmp.bmWidth, maskBmp.bmHeight);

						if(lpMaskBits[cirpos].rgbaAlpha > 0 && ((DWORD*)lpMaskBits)[cirpos] > 0)
						{
							if(i == 0) // apply red
							{
								lpTriMaskBits[tripos].rgbaRed = crgb[i].rgbRed;
								lpTriMaskBits[tripos].rgbaAlpha = lpMaskBits[cirpos].rgbaAlpha;
							} else
							if(i == 1) // apply green
							{
								if(((DWORD*)lpTriMaskBits)[tripos] != 0)
								{
									if(lpMaskBits[cirpos].rgbaAlpha != 0xFF)
									{
										lpTriMaskBits[tripos].rgbaRed   = ALPHABYTE(lpTriMaskBits[tripos].rgbaRed,
																					lpTriMaskBits[tripos].rgbaRed,
																					lpMaskBits[cirpos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaGreen = ALPHABYTE(crgb[i].rgbGreen,
																					0,
																					lpMaskBits[cirpos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaAlpha = 0xFF;
									} else
									if(lpTriMaskBits[tripos].rgbaAlpha != 0xFF)
									{
										lpTriMaskBits[tripos].rgbaRed   = ALPHABYTE(lpTriMaskBits[tripos].rgbaRed,
																					0,
																					lpTriMaskBits[tripos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaGreen = ALPHABYTE(crgb[i].rgbGreen,
																					crgb[i].rgbGreen,
																					lpTriMaskBits[tripos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaAlpha = 0xFF;
									} else {
										lpTriMaskBits[tripos].rgbaRed = lpTriMaskBits[tripos].rgbaRed;
										lpTriMaskBits[tripos].rgbaGreen = crgb[i].rgbGreen;
										lpTriMaskBits[tripos].rgbaAlpha = lpMaskBits[cirpos].rgbaAlpha;
									}
								} else {
									lpTriMaskBits[tripos].rgbaGreen = crgb[i].rgbGreen;
									lpTriMaskBits[tripos].rgbaAlpha = lpMaskBits[cirpos].rgbaAlpha;
								}
							} else { // i == 2, apply blue
								if(((DWORD*)lpTriMaskBits)[tripos] != 0)
								{
									if(lpMaskBits[cirpos].rgbaAlpha != 0xFF)
									{
										lpTriMaskBits[tripos].rgbaRed   = ALPHABYTE(lpTriMaskBits[tripos].rgbaRed,
																					lpTriMaskBits[tripos].rgbaRed,
																					lpMaskBits[cirpos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaGreen = ALPHABYTE(lpTriMaskBits[tripos].rgbaGreen,
																					lpTriMaskBits[tripos].rgbaGreen,
																					lpMaskBits[cirpos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaBlue  = ALPHABYTE(crgb[i].rgbBlue,
																					0,
																					lpMaskBits[cirpos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaAlpha = 0xFF;
									} else
									if(lpTriMaskBits[tripos].rgbaAlpha != 0xFF)
									{
										lpTriMaskBits[tripos].rgbaRed   = ALPHABYTE(lpTriMaskBits[tripos].rgbaRed,
																					0,
																					lpTriMaskBits[tripos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaGreen = ALPHABYTE(lpTriMaskBits[tripos].rgbaGreen,
																					crgb[i].rgbGreen,
																					lpTriMaskBits[tripos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaBlue  = ALPHABYTE(crgb[i].rgbBlue,
																					crgb[i].rgbBlue,
																					lpTriMaskBits[tripos].rgbaAlpha);
										lpTriMaskBits[tripos].rgbaAlpha = 0xFF;
									} else {
										lpTriMaskBits[tripos].rgbaRed = lpTriMaskBits[tripos].rgbaRed;
										lpTriMaskBits[tripos].rgbaGreen = lpTriMaskBits[tripos].rgbaGreen;
										lpTriMaskBits[tripos].rgbaBlue = crgb[i].rgbBlue;
										lpTriMaskBits[tripos].rgbaAlpha = lpMaskBits[cirpos].rgbaAlpha;
									}
								} else {
									lpTriMaskBits[tripos].rgbaBlue = crgb[i].rgbBlue;
									lpTriMaskBits[tripos].rgbaAlpha = lpMaskBits[cirpos].rgbaAlpha;
								}
							}
						}
					}
				}
			}

			// We do not need mask circle any more
			GlobalFree(lpMaskBits);

			BYTE alpha = BEValue.GetAlpha();
			float alphaPercent = float(alpha) / (256.0 / 100.0);

			for(x = 0; x < szTriMask.cx; x++)
			{
				for(y = 0; y < szTriMask.cy; y++)
				{
					tripos = BITS_XY(x, y, szTriMask.cx, szTriMask.cy);
					cirpos = BITS_XY(3 + x, 8 + y, bgBmp.bmWidth, bgBmp.bmHeight);

					if(theApp.Settings.IsApplyAlphaOnTriCircle())
					{
						if(lpTriMaskBits[tripos].rgbaAlpha == 0xFF)
							lpTriMaskBits[tripos].rgbaAlpha = alpha;
						else
						if(lpTriMaskBits[tripos].rgbaAlpha > 0)
							lpTriMaskBits[tripos].rgbaAlpha = int((lpTriMaskBits[tripos].rgbaAlpha / 100.0) * alphaPercent);
					}

					lpFinalBits[cirpos].rgbaRed   = ALPHABYTE(lpTriMaskBits[tripos].rgbaRed,
															  lpFinalBits[cirpos].rgbaRed,
															  lpTriMaskBits[tripos].rgbaAlpha);
					lpFinalBits[cirpos].rgbaGreen = ALPHABYTE(lpTriMaskBits[tripos].rgbaGreen,
															  lpFinalBits[cirpos].rgbaGreen,
															  lpTriMaskBits[tripos].rgbaAlpha);
					lpFinalBits[cirpos].rgbaBlue  = ALPHABYTE(lpTriMaskBits[tripos].rgbaBlue,
															  lpFinalBits[cirpos].rgbaBlue,
															  lpTriMaskBits[tripos].rgbaAlpha);
				}
			}

			DeleteObject(hTriMask);

			CDC finalDC;

			finalDC.CreateCompatibleDC(pDC);
			HGDIOBJ hOldFinalBmp = (HGDIOBJ)finalDC.SelectObject((HGDIOBJ)hFinal);

			pDC->BitBlt(m_rcCircle.left, m_rcCircle.top, bgBmp.bmWidth, bgBmp.bmHeight, &finalDC, 0, 0, SRCCOPY);

			finalDC.SelectObject(hOldFinalBmp);
			finalDC.DeleteDC();

			DeleteObject(hFinal);
		}

		if(bReleaseDC) ReleaseDC(pDC);
	}
}

// Color slider ctrl ----------------------------------------------------------

BEGIN_MESSAGE_MAP(CBEShowCtrl::CClrSliderCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

CBEShowCtrl::CClrSliderCtrl::CClrSliderCtrl()
: m_pParent(NULL)
, m_iType(0)
, m_szDefaultSize(311, 18)
, m_rcColor(28, 1, 284, 14)
, m_iMin(0x00)
, m_iMax(0xFF)
, m_bIsScrolling(false)
, m_bIsSelected(false)
, m_iLRKeyCount(0)
{
	RegisterWindowClass();
	m_bmPointer.LoadBitmap(IDB_MM_POINTER);
	m_iValue = m_iMin;
}

BOOL CBEShowCtrl::CClrSliderCtrl::Create(int iType, int x, int y, CWnd* pParent, UINT nID)
{
	m_iType = iType;
	m_pParent = (CBEShowCtrl*)pParent;

	RECT rect = { x, y, x + m_szDefaultSize.cx, y + m_szDefaultSize.cy };
	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VISIBLE;

	BOOL bRet = CWnd::Create(BECLRSLIDERCTRL_CLASSNAME, NULL, dwStyle, rect, pParent, nID);

	if(bRet) BuildBgBitmaps();
	return bRet;
}

BOOL CBEShowCtrl::CClrSliderCtrl::RegisterWindowClass(void)
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, BECLRSLIDERCTRL_CLASSNAME, &wndcls)) )
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
        wndcls.lpszClassName    = BECLRSLIDERCTRL_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

void CBEShowCtrl::CClrSliderCtrl::OnPaint()
{
	CPaintDC dc(this);

	BITMAP bmpBg;

	CDC memDC, fixDC;
	CBitmap memBmp;

	m_pParent->m_pBitmaps->m_bmMMOn.GetBitmap(&bmpBg);

	memDC.CreateCompatibleDC(&dc);
	fixDC.CreateCompatibleDC(&dc);

	memBmp.CreateCompatibleBitmap(&dc, bmpBg.bmWidth, bmpBg.bmHeight);

	CBitmap *pCtrlBg;
	if(m_pParent->IsSelected())
		if(m_bIsSelected)
			pCtrlBg = &m_pParent->m_pBitmaps->m_bmMMSelOn;
		else
			pCtrlBg = &m_pParent->m_pBitmaps->m_bmMMOn;
	else
		pCtrlBg = &m_pParent->m_pBitmaps->m_bmMMOff;


	HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(memBmp);
	HGDIOBJ hOldFixBmp = (HGDIOBJ)fixDC.SelectObject(pCtrlBg);

	memDC.BitBlt(0, 0, bmpBg.bmWidth, bmpBg.bmHeight, &fixDC, 0, 0, SRCCOPY);
	fixDC.SelectObject(m_bmColor);
	memDC.BitBlt(m_rcColor.left, m_rcColor.top, m_rcColor.Width(), m_rcColor.Height(),
				 &fixDC, 0, 0, SRCCOPY);

	dc.BitBlt(0, 0, bmpBg.bmWidth, bmpBg.bmHeight, &memDC, 0, 0, SRCCOPY);

	PaintPointer(&dc);

	fixDC.SelectObject(hOldFixBmp);
	fixDC.DeleteDC();

	memDC.SelectObject(hOldMemBmp);
	memDC.DeleteDC();

	memBmp.DeleteObject();
}

void CBEShowCtrl::InvalidateFieldEdits(int iFieldIndex /*= -1*/)
{
	int i, j;

	if(!IsColor())
	{
		if(iFieldIndex != -1)
		{
			m_pDecEdits[iFieldIndex - 1]->Invalidate(FALSE);
			for(i = 0; i < 3; i++)
				m_pCmnEdits[iFieldIndex - 1][i]->Invalidate(FALSE);
		} else {
			for(i = 0; i < GetFieldsCount(); i++)
			{
				m_pDecEdits[i]->Invalidate(FALSE);
				for(j = 0; j < 3; j++)
					m_pCmnEdits[i][j]->Invalidate(FALSE);
			}
		}
	} else {
		for(i = 0; i < 4; i++) m_pDecEdits[i]->Invalidate(FALSE);
		m_pHexClrEditWeb->Invalidate(FALSE);
		m_pHexClrEditWin->Invalidate(FALSE);
	}
}

void CBEShowCtrl::CClrSliderCtrl::BuildBgBitmaps(void)
{
	if(!IsWindow(m_hWnd) || !m_iType) return;

	int x, y;
	
	CDC *pDC = GetDC();
	RGBAQUAD *lpColorBits;
	BITMAP bmpColor;
	BITMAPINFO bi;

	RGBAQUAD crgba;

	crgba.rgbaRed   = m_pParent->BEValue.GetRed();
	crgba.rgbaGreen = m_pParent->BEValue.GetGreen();
	crgba.rgbaBlue  = m_pParent->BEValue.GetBlue();
	crgba.rgbaAlpha = m_pParent->BEValue.GetAlpha();

	BYTE colShift;
	int pos;
	int shift = IsBlue()  ? 0  :
				IsGreen() ? 8  :
				IsRed()   ? 16 : 0;

	m_bmColor.DeleteObject();
	m_bmColor.CreateCompatibleBitmap(pDC, m_rcColor.Width(), m_rcColor.Height());
	m_bmColor.GetBitmap(&bmpColor);

	if(IsAlpha())
	{
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CRect rcTrBg(m_rcColor);

		HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(m_bmColor);

		rcTrBg.MoveToXY(0, 0);

		CPWGraphics::DrawTransparentBg(&memDC, rcTrBg);

		memDC.SelectObject(hOldMemBmp);
		memDC.DeleteDC();
	}

	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bmpColor.bmWidth;
	bi.bmiHeader.biHeight = bmpColor.bmHeight;
	bi.bmiHeader.biPlanes = bmpColor.bmPlanes;
	bi.bmiHeader.biBitCount = bmpColor.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = bmpColor.bmWidthBytes * bmpColor.bmHeight;

	lpColorBits = (RGBAQUAD*)GlobalAlloc(GPTR, bmpColor.bmWidthBytes * bmpColor.bmHeight);

	GetDIBits(pDC->GetSafeHdc(),
			  m_bmColor,
			  0,
			  bmpColor.bmHeight,
			  lpColorBits,
			  &bi,
			  DIB_RGB_COLORS);

	colShift = 0xFF;

	for(x = 0; x < bmpColor.bmWidth; x++)
	{
		for(y = 0; y < bmpColor.bmHeight; y++)
		{
			pos = ((bmpColor.bmHeight - y) * bmpColor.bmWidth) - (bmpColor.bmWidth - x);

			if(!IsAlpha())
				((DWORD*)lpColorBits)[pos] = ((DWORD)((DWORD)colShift) << shift);
			else
			{
				lpColorBits[pos].rgbaRed   = ALPHABYTE(crgba.rgbaRed,   lpColorBits[pos].rgbaRed,   colShift);
				lpColorBits[pos].rgbaGreen = ALPHABYTE(crgba.rgbaGreen, lpColorBits[pos].rgbaGreen, colShift);
				lpColorBits[pos].rgbaBlue  = ALPHABYTE(crgba.rgbaBlue,  lpColorBits[pos].rgbaBlue,  colShift);
			}
		}
		colShift--;
	}

	SetDIBits(pDC->GetSafeHdc(),
			  m_bmColor,
			  0,
			  bmpColor.bmHeight,
			  lpColorBits,
			  &bi,
			  DIB_RGB_COLORS);

	GlobalFree(lpColorBits);

	ReleaseDC(pDC);
}


void CBEShowCtrl::CClrSliderCtrl::UpdateAlphaColor(void)
{
	if(IsAlpha())
	{
		BuildBgBitmaps();
		PaintColor();
	}
}

void CBEShowCtrl::InvalidateColorSlider(int iFieldIndex /*= 0*/)
{
	if(IsColor())
	{
		iFieldIndex--;
		if(iFieldIndex == -1)
			for(int i = 0; i < 4; i++) m_pClrSliders[i]->Invalidate(FALSE);
		else
			m_pClrSliders[iFieldIndex]->Invalidate(FALSE);
	}
}

void CBEShowCtrl::UpdateColorSliderPointer(int iFieldIndex /*= 0*/)
{
	if(IsColor())
	{
		iFieldIndex--;
		if(iFieldIndex == -1)
			for(int i = 0; i < 4; i++) m_pClrSliders[i]->UpdateFromParent();
		else
			m_pClrSliders[iFieldIndex]->UpdateFromParent();
	}
}

void CBEShowCtrl::CClrSliderCtrl::PaintPointer(CDC* pDC /*= NULL*/)
{
	if(!IsWindow(m_hWnd) || !m_iType) return;

	CSize szPointer(5, 3);
	BITMAP bmpPtr;
	CDC memDC, ptrDC;
	CBitmap memBmp;
	bool bReleaseDC = false;

	int iPtrPos = m_iMax - m_iValue;

	if(!pDC)
	{
		pDC = GetDC();
		bReleaseDC = true;
	}

	m_bmPointer.GetBitmap(&bmpPtr);

	ptrDC.CreateCompatibleDC(pDC);
	memDC.CreateCompatibleDC(pDC);
	memBmp.CreateCompatibleBitmap(pDC, m_szDefaultSize.cx, szPointer.cy);

	HGDIOBJ hOldPtrBmp = (HGDIOBJ)ptrDC.SelectObject(m_bmPointer);
	HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(memBmp);

	memDC.FillSolidRect(0, 0, m_szDefaultSize.cx, szPointer.cy, m_pParent->IsSelected() ? m_pParent->m_crCmnEdtBkColorOn :
																						  m_pParent->m_crCmnEdtBkColorOff);

	memDC.BitBlt(m_rcColor.left + iPtrPos - 2, 0, szPointer.cx, szPointer.cy,
				 &ptrDC, m_pParent->IsSelected() ? 0 : szPointer.cx, 0, SRCCOPY);

	pDC->BitBlt(0, m_szDefaultSize.cy - szPointer.cy, m_szDefaultSize.cx, szPointer.cy, &memDC, 0, 0, SRCCOPY);

	ptrDC.SelectObject(hOldPtrBmp);
	ptrDC.DeleteDC();
	memDC.SelectObject(hOldMemBmp);
	memDC.DeleteDC();

	memBmp.DeleteObject();

	if(bReleaseDC) ReleaseDC(pDC);
}

void CBEShowCtrl::CClrSliderCtrl::PaintColor(CDC* pDC /*= NULL*/)
{
	if(!IsWindow(m_hWnd) || !m_iType) return;

	bool bReleaseDC = false;

	if(!pDC)
	{
		pDC = GetDC();
		bReleaseDC = true;
	}

	CDC memDC;

	memDC.CreateCompatibleDC(pDC);
	HGDIOBJ hOldMemBmp = (HGDIOBJ)memDC.SelectObject(m_bmColor);

	pDC->BitBlt(m_rcColor.left, m_rcColor.top, m_rcColor.Width(), m_rcColor.Height(),
				&memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(hOldMemBmp);
	memDC.DeleteDC();

	if(bReleaseDC) ReleaseDC(pDC);
}

void CBEShowCtrl::CClrSliderCtrl::UpdateValueFromParent(void)
{
	if(IsRed())   m_iValue = (int)m_pParent->BEValue.GetRed();   else
	if(IsGreen()) m_iValue = (int)m_pParent->BEValue.GetGreen(); else
	if(IsBlue())  m_iValue = (int)m_pParent->BEValue.GetBlue();  else
	if(IsAlpha()) m_iValue = (int)m_pParent->BEValue.GetAlpha();
}

void CBEShowCtrl::CClrSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();
	SetCapture();
	m_bIsScrolling = true;
	OnMouseMove(nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
}

void CBEShowCtrl::CClrSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(m_bIsScrolling)
	{
		if((point.x - m_rcColor.left) >= m_iMin && (point.x - m_rcColor.left) <= m_iMax)
		{
			SetValue(0xFF - point.x + m_rcColor.left);
			UpdateValueToParent();
		} else
		if((point.x - m_rcColor.left) < m_iMin)
		{
			SetValue(m_iMax);
			UpdateValueToParent();
		} else
		if((point.x - m_rcColor.left) > m_iMax)
		{
			SetValue(m_iMin);
			UpdateValueToParent();
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CBEShowCtrl::CClrSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_bIsScrolling = false;
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CBEShowCtrl::CClrSliderCtrl::UpdateFromParent(void)
{
	UpdateValueFromParent();
	PaintPointer();
}

int CBEShowCtrl::CClrSliderCtrl::ColorToFieldIndex(void)
{
	if(IsRed())   return 1;
	if(IsGreen()) return 2;
	if(IsBlue())  return 3;
	if(IsAlpha()) return 4;
	return 0;
}

void CBEShowCtrl::CClrSliderCtrl::SetValue(int iValue)
{
	if(m_iValue == iValue) return;
	if(iValue < m_iMin) m_iValue = m_iMin; else
	if(iValue > m_iMax) m_iValue = m_iMax; else
	m_iValue = iValue;

	PaintPointer();
}

void CBEShowCtrl::CClrSliderCtrl::UpdateValueToParent(void)
{
	m_pParent->BEValue.SetInt(m_iValue, m_pParent->FieldToPos(ColorToFieldIndex()));

	m_pParent->UpdateDecFields(ColorToFieldIndex());
	m_pParent->UpdateCmnFields();

	m_pParent->PaintDecColors();
	m_pParent->PaintCircle();
	m_pParent->PaintBigColors();
	if(!IsAlpha()) m_pParent->m_pClrSliderAlpha->UpdateAlphaColor();

	//m_pParent->SetUpdateCtrls();
	m_pParent->SetUpdateCtrlsByFieldIndex(ColorToFieldIndex());
}

void CBEShowCtrl::CClrSliderCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	m_bIsSelected = false;
	Invalidate(FALSE);
}

void CBEShowCtrl::CClrSliderCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	m_bIsSelected = true;
	Invalidate(FALSE);

	m_pParent->ResetSelectedUpdateCtrls();
	m_pParent->SetSelected(ColorToFieldIndex());
}

void CBEShowCtrl::CClrSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if(nChar == VK_LEFT || nChar == VK_RIGHT)
	{
		int iAddSub = (m_iLRKeyCount > 10 ? 10 : 1);
		if(nChar == VK_RIGHT) iAddSub = -iAddSub;
		SetValue(m_iValue + iAddSub);
		UpdateValueToParent();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CBEShowCtrl::CClrSliderCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->message == WM_KEYDOWN &&
	   (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT))
	{
		m_iLRKeyCount++;
		OnKeyDown((UINT)pMsg->wParam, 0, 0);
		return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CBEShowCtrl::CClrSliderCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	m_iLRKeyCount = 0;
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CBEShowCtrl::ShowColorDialog()
{
	if(IsColor())
	{
		CColorDialog cdlg(BEValue.GetRGB(), CC_FULLOPEN, GetParent());

		if(cdlg.DoModal() == IDOK)
		{
			BEValue.SetRGB(cdlg.GetColor());
			UpdateFields();
			SetUpdateCtrls();
		}
	}
}


BOOL CBEShowCtrl::CBitsCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_LBUTTONDBLCLK)
	{
		pMsg->message = WM_LBUTTONDOWN;
	}

	return CWnd::PreTranslateMessage(pMsg);
}
