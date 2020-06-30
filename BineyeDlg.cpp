// BineyeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Bineye.h"
#include "BineyeDlg.h"
#include ".\inc\PWGraphics.h"
#include ".\BEShowCtrl.h"
#include "afxwin.h"
#include ".\inc\LinkCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CImgBtn m_cBtnClose;
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_cBtnClose);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CBineyeDlg dialog




CBineyeDlg::CBineyeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBineyeDlg::IDD, pParent)
	, m_cLn32(&m_beBitmaps)
	, m_cLn16(&m_beBitmaps)
	, m_cLn8(&m_beBitmaps)
	, m_cLnClr(&m_beBitmaps)
	, m_bDisplayLogo(false)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	m_hIconBig = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_beBitmaps.LoadBitmaps();
	m_bDisplayLogo = theApp.Settings.IsDisplayMainDialogLogo();
}

void CBineyeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNCLOSE, m_cBtnClose);
	DDX_Control(pDX, IDC_BTNABOUT, m_cBtnAbout);
	DDX_Control(pDX, IDC_BTNSETTINGS, m_cBtnSettings);
}

BEGIN_MESSAGE_MAP(CBineyeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTNCLOSE, &CBineyeDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_BTNSETTINGS, &CBineyeDlg::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_BTNABOUT, &CBineyeDlg::OnBnClickedAbout)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CBineyeDlg message handlers

BOOL CBineyeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIconBig, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//m_bgBmp.CreateCompatibleBitmap(

	/*CRect rc;
	GetClientRect(rc);

	CDC *pDC = GetDC();

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	m_bgBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

	CBitmap *pOldBmp = (CBitmap*)memDC.SelectObject(&m_bgBmp);

	//CPWGraphics::DrawPlasticRect(memDC.GetSafeHdc(), rc, 0x404040);
	CPWGraphics::DrawPlasticRect(memDC.GetSafeHdc(), rc, 0x909090);

	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();

	ReleaseDC(pDC);*/


	m_bgBmp.LoadBitmap(IDB_MAINBG);
	m_logoBmp.LoadBitmap(IDB_MAINLOGO);

	m_logoBmp.GetBitmap(&m_bmpLogo);

	/*m_8BitLineOff.LoadBitmapW(IDB_8BITLINE_OFF);
	m_8BitLineOn.LoadBitmapW(IDB_8BITLINE_ON);

	m_LineUOn.LoadBitmapW(IDB_LINE_U_ON);
	m_LineUOff.LoadBitmapW(IDB_LINE_U_OFF);
	m_LineSOn.LoadBitmapW(IDB_LINE_S_ON);
	m_LineSOff.LoadBitmapW(IDB_LINE_S_OFF);*/




	//CString str;

	//str.Format(_T("cx: %d, cy: %d"), rc.Width(), rc.Height());

	//AfxMessageBox(str);

	int x = /*m_bmpLogo.bmWidth +*/ 10, y = 10;

	if(m_bDisplayLogo)
		x += m_bmpLogo.bmWidth;

	m_cLn32.Create( CBEShowCtrl::BESCType::type32Bit, x, y,  this, 1000 );
	m_cLn16.Create( CBEShowCtrl::BESCType::type16Bit, x, y + 134, this, 1200 );
	m_cLn8.Create(  CBEShowCtrl::BESCType::type8Bit,  x, y + 268, this, 1300 );
	m_cLnClr.Create(CBEShowCtrl::BESCType::typeColor, x, y + 402, this, 1400 );

	m_cLn32.AddUpdateCtrl(&m_cLn16);
	m_cLn32.AddUpdateCtrl(&m_cLn8);
	m_cLn32.AddUpdateCtrl(&m_cLnClr);

	m_cLn16.AddUpdateCtrl(&m_cLn32);
	m_cLn16.AddUpdateCtrl(&m_cLn8);
	m_cLn16.AddUpdateCtrl(&m_cLnClr);

	m_cLn8.AddUpdateCtrl(&m_cLn32);
	m_cLn8.AddUpdateCtrl(&m_cLn16);
	m_cLn8.AddUpdateCtrl(&m_cLnClr);

	m_cLnClr.AddUpdateCtrl(&m_cLn32);
	m_cLnClr.AddUpdateCtrl(&m_cLn16);
	m_cLnClr.AddUpdateCtrl(&m_cLn8);

	//m_cLn32.SetSigned(TRUE);
	//m_cLn16.SetSigned(TRUE);
	//m_cLn8.SetSigned(TRUE);

	m_cLn32.SetSignedSet(theApp.Settings.Get32BITSigned());
	m_cLn16.SetSignedSet(theApp.Settings.Get16BITSigned());
	m_cLn8.SetSignedSet(theApp.Settings.Get8BITSigned());

	/*TRACE("Ln8: %d\n", m_cLn8.GetSignedSet());
	TRACE("Ln16: %d\n", m_cLn16.GetSignedSet());
	TRACE("Ln32: %d\n", m_cLn32.GetSignedSet());*/

	//m_cLn32.BEValue.SetInt32(-1);
	//m_cLn16.BEValue.SetInt32(m_cLn32.BEValue.GetInt32());
	//m_cLn8.BEValue.SetInt32(m_cLn32.BEValue.GetInt32());

	//m_cLn32.BEValue.SetAlpha(128);

	//m_cLn32.BEValue.SetWebColor(_T("#10AF60"));
	//m_cLn32.BEValue.SetHexRGBA(_T("#10AF6042"));
	//m_cLn32.BEValue.SetWinColor(_T("0XEABE4A"));
	//m_cLn32.BEValue.SetHexABGR(_T("0x64EABE4A"));
	m_cLn32.BEValue.SetInt32(0);

	m_cLn32.UpdateFields();
	m_cLn32.SetUpdateCtrls();
	//m_cLn16.UpdateFields();
	//m_cLn8.UpdateFields();

	/*m_cBtnClose.Create(_T("Close"), WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VISIBLE, 
	CRect(470, 536, 563, 562), this, IDCANCEL);*/

	//m_cBtnClose.SetWindowPos(NULL, 444, 544, 32, 32, SWP_NOOWNERZORDER);

	m_cBtnClose.LoadBitmaps(IDB_BTN_EXIT_OFF, IDB_BTN_EXIT_ON);
	m_cBtnAbout.LoadBitmaps(IDB_BTN_INFO_OFF, IDB_BTN_INFO_ON);
	m_cBtnSettings.LoadBitmaps(IDB_BTN_TOOLS_OFF, IDB_BTN_TOOLS_ON);

	if(!m_bDisplayLogo)
	{
		CRect rcBtnsPos;

		m_cBtnClose.GetWindowRect(rcBtnsPos);
		ScreenToClient(rcBtnsPos);
		rcBtnsPos.MoveToX(rcBtnsPos.left - m_bmpLogo.bmWidth);
		m_cBtnClose.MoveWindow(rcBtnsPos, FALSE);

		m_cBtnAbout.GetWindowRect(rcBtnsPos);
		ScreenToClient(rcBtnsPos);
		rcBtnsPos.MoveToX(rcBtnsPos.left - m_bmpLogo.bmWidth);
		m_cBtnAbout.MoveWindow(rcBtnsPos, FALSE);

		m_cBtnSettings.GetWindowRect(rcBtnsPos);
		ScreenToClient(rcBtnsPos);
		rcBtnsPos.MoveToX(rcBtnsPos.left - m_bmpLogo.bmWidth);
		m_cBtnSettings.MoveWindow(rcBtnsPos, FALSE);

		GetWindowRect(rcBtnsPos);
		SetWindowPos(NULL, 0, 0, rcBtnsPos.Width() - m_bmpLogo.bmWidth, rcBtnsPos.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	

	//m_cLn32.SetFocus();
	m_cLn32.m_pDecEdits[0]->SetFocus();

	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CBineyeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBineyeDlg::OnPaint()
{
	//if (IsIconic())
	//{
		CPaintDC dc(this); // device context for painting

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);

		CDC memDC, fixDC;
		CBitmap memBmp;
		BITMAP bmp, fixBmp;

		m_bgBmp.GetBitmap(&bmp);

		memDC.CreateCompatibleDC(&dc);
		fixDC.CreateCompatibleDC(&dc);

		memBmp.CreateCompatibleBitmap(&dc, (m_bDisplayLogo ? m_bmpLogo.bmWidth : 0) + bmp.bmWidth, bmp.bmHeight);
		CBitmap *pOldMemBmp = (CBitmap*)memDC.SelectObject(&memBmp);

		CBitmap *pOldFixBmp = (CBitmap*)fixDC.SelectObject(&m_logoBmp);

		if(m_bDisplayLogo)
			memDC.BitBlt(0, 0, m_bmpLogo.bmWidth, m_bmpLogo.bmHeight, &fixDC, 0, 0, SRCCOPY);


		/*CBitmap *pOldFixBmp = (CBitmap*)*/ fixDC.SelectObject(&m_bgBmp);

		memDC.BitBlt(m_bDisplayLogo ? m_bmpLogo.bmWidth : 0, 0, bmp.bmWidth, bmp.bmHeight, &fixDC, 0, 0, SRCCOPY);

		dc.BitBlt(0, 0, (m_bDisplayLogo ? m_bmpLogo.bmWidth : 0) + bmp.bmWidth, bmp.bmHeight, &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldMemBmp);
		fixDC.SelectObject(pOldFixBmp);
		memDC.DeleteDC();
		fixDC.DeleteDC();
		memBmp.DeleteObject();


	//}
	//else
	//{
	//	CDialog::OnPaint();
	//}
}

void CBineyeDlg::OnClose()
{
	theApp.Settings.Set32BITSigned(m_cLn32.GetSignedSet());
	theApp.Settings.Set16BITSigned(m_cLn16.GetSignedSet());
	theApp.Settings.Set8BITSigned(m_cLn8.GetSignedSet());

	theApp.Settings.SaveSettings();

	CDialog::OnClose();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBineyeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CBineyeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		return FALSE;
	}

	if(pMsg->message == WM_LBUTTONDOWN)
	{
		HWND wfp = ::WindowFromPoint(pMsg->pt);

		if(wfp != m_cBtnClose.m_hWnd &&
		   wfp != m_cBtnAbout.m_hWnd &&
		   wfp != m_cBtnSettings.m_hWnd)
			SendMessage(WM_NCLBUTTONDOWN, HTCAPTION);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CBineyeDlg::OnBnClickedClose()
{
	//OnCancel();
	OnClose();
	OnCancel();
}

void CBineyeDlg::OnBnClickedSettings()
{
	CBineyeSettings sets;
	if(sets.DoModal() == IDOK)
	{
		//m_bDisplayLogo = theApp.Settings.IsDisplayMainDialogLogo();
		DisplayLogo(theApp.Settings.IsDisplayMainDialogLogo());
		//Invalidate(TRUE);
		RedrawWindow(0,0,RDW_UPDATENOW|RDW_ERASE|RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_ALLCHILDREN);

		m_cLn32.UpdateFields();
		m_cLn32.SetUpdateCtrls();

		//m_cLnClr.Invalidate();
		//m_cLnClr.RedrawWindow(NULL, NULL, RDW_ERASENOW|RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN );
	}
}

void CBineyeDlg::OnBnClickedAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	CRect wrc;
	GetClientRect(wrc);

	pDC->FillSolidRect(wrc, RGB(0,0,0));

	return TRUE; //CDialog::OnEraseBkgnd(pDC);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	//_cBtnClose.SetUseBgColor();
	//m_cBtnClose.LoadBitmaps(IDB_BTN_EXIT_OFF, IDB_BTN_EXIT_ON);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	OnOK();
	CDialog::OnLButtonUp(nFlags, point);
}


void CBineyeDlg::DisplayLogo(bool bShow)
{
	if(bShow == m_bDisplayLogo)
		return;

	CRect rcBtnsPos;
	int cxAdd, y = 10;

	if(m_bDisplayLogo = bShow)
		cxAdd = m_bmpLogo.bmWidth;
	else
		cxAdd = -m_bmpLogo.bmWidth;

	m_cLn32.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cLn32.MoveWindow(rcBtnsPos, FALSE);

	m_cLn16.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cLn16.MoveWindow(rcBtnsPos, FALSE);

	m_cLn8.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cLn8.MoveWindow(rcBtnsPos, FALSE);

	m_cLnClr.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cLnClr.MoveWindow(rcBtnsPos, FALSE);

	m_cBtnClose.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cBtnClose.MoveWindow(rcBtnsPos, FALSE);

	m_cBtnAbout.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cBtnAbout.MoveWindow(rcBtnsPos, FALSE);

	m_cBtnSettings.GetWindowRect(rcBtnsPos);
	ScreenToClient(rcBtnsPos);
	rcBtnsPos.MoveToX(rcBtnsPos.left + cxAdd);
	m_cBtnSettings.MoveWindow(rcBtnsPos, FALSE);

	GetWindowRect(rcBtnsPos);
	SetWindowPos(NULL, 0, 0, rcBtnsPos.Width() + cxAdd, rcBtnsPos.Height(), SWP_NOMOVE | SWP_NOZORDER);
}
