#pragma once


// CMsgBox dialog

class CMsgBox : public CDialog
{
	DECLARE_DYNAMIC(CMsgBox)

public:
	CMsgBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMsgBox();

// Dialog Data
	enum { IDD = IDD_MSGBOX };

private:
	CBitmap m_bmBg;
	CBitmap m_bmIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
