// ToolTipEx.cpp : implementation file
//

#include "afxcmn.h"
#include "ToolTipEx.h"

////////////////////////////////////////////////////////////////////////////
// CToolTipEx

class CToolInfoEx : public TOOLINFO {
public:
	CToolInfoEx() { init(); }
	void SetFlags(UINT uToolFlags) { uFlags = uToolFlags; }
	void SetID(UINT uID) { uId = uID; }
	void SetRect ( CRect rc) { ::CopyRect(&rect,&rc); }
	void SetWnd( HWND m_hToolWnd) { hwnd = m_hToolWnd; }
	void SetText( LPCTSTR lpszToolText) { lpszText = (LPTSTR) lpszToolText; }

protected:
	void init() { 
		memset( this, 0, sizeof(CToolInfoEx)); 
		cbSize = sizeof(CToolInfoEx); 
		hinst = AfxGetInstanceHandle();
	}

};


CToolTipEx::CToolTipEx()
{
	bTrackingState = FALSE;
	srand(::GetTickCount());//CTime::GetCurrentTime().GetTime());

	m_hLastTrackedWnd = NULL; //tracked window
	m_uLastTrackedTipID = 0; //tracked part of window

//	m_CountedToolID = 1;//tool ids that are given to any tool
}

CToolTipEx::~CToolTipEx()
{
}

CSize CToolTipEx::szOffset = CSize(15,15); 
// Tooltip position when tracking relative to cursor point (left and bottom)

CString CToolTipEx::strStandardTextAddition = _T("\nCToolTipCtrl , supports \nData-Tips and Tracking tips");
//standard text to be appended in the end of the text string
//This exist so you can add you name or logo of company on any of tooltips
//you create this way (until you annoy everyone)

BEGIN_MESSAGE_MAP(CToolTipEx, CToolTipCtrl)
	//{{AFX_MSG_MAP(CToolTipEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTipEx message handlers

BOOL CToolTipEx::Create(CWnd *pParentWnd, BOOL bTrackState, DWORD dwInitial, DWORD dwAutoPop, BOOL bActivate)
{
	if (!CToolTipCtrl::Create( pParentWnd, TTS_ALWAYSTIP))
		return FALSE;

	bTrackingState = bTrackState;
	SendMessage( TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
    SendMessage( TTM_SETDELAYTIME, TTDT_AUTOPOP, dwAutoPop); // SHRT_MAX);
    SendMessage( TTM_SETDELAYTIME, TTDT_INITIAL, dwInitial);
    SendMessage( TTM_SETDELAYTIME, TTDT_RESHOW, 200);

	Activate(bActivate);
    return TRUE;
}

BOOL CToolTipEx::AddTool( CWnd *pWnd, UINT uStringToolID, const CRect *pRect)
{
	if (!uStringToolID)  return FALSE;
	CString strTip;
	if (strTip.LoadString(uStringToolID))
		return InternalAddTool( pWnd, (LPCTSTR)strTip, pRect);
	return FALSE;
}

BOOL CToolTipEx::InternalAddTool( CWnd *pWnd, LPCTSTR lpszTip, const CRect *pRect)
{
    if (!pWnd) { return FALSE; }
	if (!lpszTip)  return FALSE;
	pWnd->EnableToolTips();

	CRect  rect;
	if (!pRect) pWnd->GetWindowRect(&rect);
	else  ::CopyRect( &rect, pRect);

	CString strTip = lpszTip /*+ strStandardTextAddition*/;

	UINT uFlags = (bTrackingState) ? 
				(TTF_IDISHWND|TTF_TRACK|TTF_ABSOLUTE) //TTF_SUBCLASS| TTF_IDISHWND||TTF_SUBCLASS
			  :	(TTF_IDISHWND);  //);  //|TTF_SUBCLASS 

	CToolInfoEx ti;
	ti.SetFlags(uFlags);
	ti.SetID((UINT)pWnd->GetSafeHwnd());
	ti.SetRect(rect);
	ti.SetWnd(GetParent()->GetSafeHwnd()); //pWnd->GetSafeHwnd());
	ti.SetText((LPCTSTR)strTip);

	return (BOOL) SendMessage( TTM_ADDTOOL, 0, (LPARAM)&ti);
}


//  Good function !! for TTF_IDISHWND
//this is rather a demanding function that works a LOT!!!(called from PreTranslateMessage)
void CToolTipEx::CheckForTrack(LPMSG lpMsg)
{
	static CToolInfo  ti; //1 construction only
	HWND hWndFromPoint = ::WindowFromPoint(CPoint(lpMsg->pt.x,lpMsg->pt.y));

	SendMessage( TTM_TRACKPOSITION,0, MAKELONG(lpMsg->pt.x + szOffset.cx,lpMsg->pt.y + szOffset.cy));
	//previous tip equals current one , do nothing 
	if (m_hLastTrackedWnd == hWndFromPoint) //(both non NULL : if one is null : in begining , both : do none ,
		return;	 //second NULL : non equal

	//previous tip diffenerent from current 1 so delete tracking
	//of previous tooltip
	if ((!hWndFromPoint) || (hWndFromPoint != m_hLastTrackedWnd)) {
		if (m_hLastTrackedWnd) { //got some window
			if (GetToolInfo( ti, CWnd::FromHandle(m_hLastTrackedWnd)))
				SendMessage( TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&ti);
		}
	}

	//assign the new tracked window
	m_hLastTrackedWnd = hWndFromPoint;
	//activate this tip
	if (GetToolInfo( ti, CWnd::FromHandle(m_hLastTrackedWnd)))
		SendMessage( TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&ti);
}


void CToolTipEx::RelayEvent(LPMSG lpMsg)
{
	switch (lpMsg->message)
	{
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
				if (bTrackingState) {
					CheckForTrack(lpMsg);
				}
				else
					CToolTipCtrl::RelayEvent(lpMsg);
				break;
	}

}

