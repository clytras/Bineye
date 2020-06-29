// BineyeDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include ".\BEBitmaps.h"
#include ".\BEShowCtrl.h"
#include "imgbtn.h"
#include "inc\ButtonExt.h"
#include ".\MWRatesCmb.h"
#include "BineyeSettings.h"

// CBineyeDlg dialog
class CBineyeDlg : public CDialog
{
// Construction
public:
	CBineyeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BINEYE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIconBig;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnBnClickedAbout();
private:
	CBitmap m_bgBmp;
	CBitmap m_logoBmp;
	CBitmap m_8BitLineOff;
	CBitmap m_8BitLineOn;
	CBitmap m_LineUOn;
	CBitmap m_LineUOff;
	CBitmap m_LineSOff;
	CBitmap m_LineSOn;

	CBEBitmaps m_beBitmaps;
	CBEShowCtrl m_cLn32;
	CBEShowCtrl m_cLn16;
	CBEShowCtrl m_cLn8;
	CBEShowCtrl m_cLnClr;

public:
	BITMAP m_bmpLogo;
	bool m_bDisplayLogo;

	CBitmap* GetBgBmp() { return &m_bgBmp; };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CImgBtn /*CMWRatesCmb*/ m_cBtnClose;
	CImgBtn m_cBtnAbout;
	CImgBtn m_cBtnSettings;
	void DisplayLogo(bool bShow);
};
