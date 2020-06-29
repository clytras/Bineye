#pragma once
#include "afxwin.h"
#include "imgbtn.h"

// CBineyeSettings dialog

class CBineyeSettings : public CDialog
{
	DECLARE_DYNAMIC(CBineyeSettings)

public:
	CBineyeSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBineyeSettings();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	CImgBtn m_cBtnOK;
	CImgBtn m_cBtnCancel;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
