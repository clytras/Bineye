#if !defined(AFX_TOOLTIPEX_H__29CD7A48_6617_11D4_B8C4_E62AF59E9843__INCLUDED_)
#define AFX_TOOLTIPEX_H__29CD7A48_6617_11D4_B8C4_E62AF59E9843__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTipEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolTipEx window

class CToolTipEx : public CToolTipCtrl
{
public:
	CToolTipEx();
	virtual ~CToolTipEx();

public:
	BOOL Create(CWnd* pParentWnd, BOOL bTrackState = FALSE, DWORD dwInitial = 200, DWORD dwAutoPop = 2000, BOOL bActivate = TRUE);

	BOOL AddTool(CWnd *pWnd, UINT uStringToolID, const CRect *pRect = NULL);
	BOOL AddTool(CWnd *pWnd, LPCTSTR lpszTip, const CRect *pRect = NULL) {
		return InternalAddTool( pWnd, lpszTip, pRect);
	}

	void RelayEvent(LPMSG lpMsg);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipEx)
	//}}AFX_VIRTUAL

	static CSize szOffset;
	static CString strStandardTextAddition;
	//standard text to be appended in the end of the text string

public:

protected:
	//{{AFX_MSG(CToolTipEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	void CheckForTrack(LPMSG lpMsg);
	BOOL InternalAddTool( CWnd *pWnd, LPCTSTR lpszTip, const CRect *pRect = NULL);

protected:
	BOOL bTrackingState;
	HWND  m_hLastTrackedWnd;
	UINT  m_uLastTrackedTipID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPEX_H__29CD7A48_6617_11D4_B8C4_E62AF59E9843__INCLUDED_)
