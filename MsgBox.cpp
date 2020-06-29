// MsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "Bineye.h"
#include "MsgBox.h"


// CMsgBox dialog

IMPLEMENT_DYNAMIC(CMsgBox, CDialog)

CMsgBox::CMsgBox(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgBox::IDD, pParent)
{

}

CMsgBox::~CMsgBox()
{
}

void CMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgBox, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMsgBox message handlers

BOOL CMsgBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	//m_bmBg.LoadBitmap(IDB_MB_BG);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMsgBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}
