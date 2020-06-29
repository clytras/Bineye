#pragma once

#include <afxtempl.h>
#include ".\inc\FontExt.h"
#include ".\inc\PWGraphics.h"

#define MWTABCTRL_CLASSNAME		_T("MWTabCtrl")
#define MWTABCTRL_TIMEREVENT	1001

// CMWTabCtrl

class CMWTabCtrl : public CWnd
{
	DECLARE_DYNAMIC(CMWTabCtrl)

public:
	CMWTabCtrl(COLORREF crDefBack = -1);
	virtual ~CMWTabCtrl();

public:
	class CMWTabBtn
	{
	public:
		class xAxis
		{
		public:
			xAxis(int ix1=0, int ix2=0, int iy=0)
				: x1(ix1), x2(ix2), y(iy) {}
			int x1, x2, y;
		};
	public:
		CMWTabBtn(void) : m_pDlg(NULL), m_crBack(-1) {}
		~CMWTabBtn(void) {}

		CString m_strText;
		COLORREF m_crBack;
		COLORREF m_crFinishBack;
		CRgn m_rgn, m_rgnchk;
		CArray<CPoint, CPoint> m_pts;
		CArray<xAxis, xAxis> m_bkpts;
		CPWGraphics::CBtnStateEx m_state;
		CPoint m_ptLB, m_ptRB;
		CDialog *m_pDlg;
		int m_iTextX;
	};

public:
	BOOL Create(DWORD dwStyle, CRect& rect, CWnd* pParent, UINT nID);

	int AddTab(LPCTSTR lpszText, COLORREF crTabColor = -1, CDialog *pDlg = NULL);
	int AddTab(LPCTSTR lpszText, CDialog *pDlg);
	
	int SelectTab(int nNewSel, bool bReDraw = true);
	void SetDefBackColor(COLORREF crDefBack);
	COLORREF GetDefBackColor(void);

private:
	BOOL RegisterWindowClass();
	void Init(void);
	void CreateBkBitmaps(void);
	void CreateTabRgn(int iTab);
	void DrawButsBmp(void);
	void DrawPanelBmp(void);
	void DrawBtnBk(CDC *pDC, int iTab);
	int SetHotTab(int iNewHotTab);
	COLORREF GetBtnColor(CMWTabBtn *pbtn);
	COLORREF GetBtnColor(int idx);
	void FixDlgPos(CDialog* pDlg);
	void FixDlgPos(int idx);

public:
	CBitmap m_bmButs;
	CBitmap m_bmPanel;
	CBitmap *m_pParentBmp;
	CFontExt m_font;
	CArray<CMWTabCtrl::CMWTabBtn*, CMWTabCtrl::CMWTabBtn*> m_buts;
	COLORREF m_crTrans1;
	COLORREF m_crTrans2;
	COLORREF m_crDefBack;
	COLORREF m_crTransBack;
	int m_nTabDistCx;
	int m_iCurSel;
	int m_iCurHot;
	int m_nBmButsCy;
	int m_nBmPanelCy;
	bool m_bInit;
	bool m_bDrawPanel;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


