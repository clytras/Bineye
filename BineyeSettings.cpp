// BineyeSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Bineye.h"
#include "BineyeSettings.h"


// CBineyeSettings dialog

IMPLEMENT_DYNAMIC(CBineyeSettings, CDialog)

CBineyeSettings::CBineyeSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CBineyeSettings::IDD, pParent)
{

}

CBineyeSettings::~CBineyeSettings()
{
}

void CBineyeSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDCANCEL, m_cBtnCancel);
}


BEGIN_MESSAGE_MAP(CBineyeSettings, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CBineyeSettings::OnBnClickedOk)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CBineyeSettings message handlers

void CBineyeSettings::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	//CPen linep(PS_SOLID, 1, RGB(0,0,0));

	//HGDIOBJ hOldPen = dc.SelectObject(linep);

	//dc.MoveTo(0, 210);
	//dc.LineTo(300, 210);

	//CBrush br;
	//br.CreateStockObject(BLACK_BRUSH);

	//HGDIOBJ hOldBr = dc.SelectObject(br);

	dc.FillSolidRect(CRect(0, 173, 500, 300), RGB(0, 0, 0));
}

BOOL CBineyeSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_cBtnOK.SetUseBgColor(false);
	m_cBtnCancel.SetUseBgColor(true);

	m_cBtnOK.LoadBitmaps(IDB_BTN_OK_OFF, IDB_BTN_OK_ON);
	m_cBtnCancel.LoadBitmaps(IDB_BTN_EXIT_OFF, IDB_BTN_EXIT_ON);

	CheckDlgButton(IDC_ApplyAlphaOnTriCirlce, theApp.Settings.IsApplyAlphaOnTriCircle() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_AddWebColorSign, theApp.Settings.IsApplyWebColorSign() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_AddWinColorSign, theApp.Settings.IsApplyWinColorSign() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_BitsZeroIndex, theApp.Settings.IsBitsZeroIndex() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_HexUpperCase, theApp.Settings.IsHexUpperCase() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_SelectAllOnFocus, theApp.Settings.IsSelectAllOnFocus() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_ShowSignedPlusSign, theApp.Settings.IsShowSignedPlusSign() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_ZeroFill, theApp.Settings.IsZeroFill() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_DisplayMainDialogLogo, theApp.Settings.IsDisplayMainDialogLogo() ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBineyeSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	/*IDC_ApplyAlphaOnTriCirlce
	IDC_AddWebColorSign
	IDC_AddWinColorSign
	IDC_BitsZeroIndex

	IDC_HexUpperCase
	IDC_SelectAllOnFocus
	IDC_ShowSignedPlusSign
	IDC_ZeroFill*/

	theApp.Settings.SetApplyAlphaOnTriCircle(IsDlgButtonChecked(IDC_ApplyAlphaOnTriCirlce) == BST_CHECKED);
	theApp.Settings.SetApplyWebColorSign(IsDlgButtonChecked(IDC_AddWebColorSign) == BST_CHECKED);
	theApp.Settings.SetApplyWinColorSign(IsDlgButtonChecked(IDC_AddWinColorSign) == BST_CHECKED);
	theApp.Settings.SetBitsZeroIndex(IsDlgButtonChecked(IDC_BitsZeroIndex) == BST_CHECKED);
	theApp.Settings.SetHexUpperCase(IsDlgButtonChecked(IDC_HexUpperCase) == BST_CHECKED);
	theApp.Settings.SetSelectAllOnFocus(IsDlgButtonChecked(IDC_SelectAllOnFocus) == BST_CHECKED);
	theApp.Settings.SetShowSignedPlusSign(IsDlgButtonChecked(IDC_ShowSignedPlusSign) == BST_CHECKED);
	theApp.Settings.SetZeroFill(IsDlgButtonChecked(IDC_ZeroFill) == BST_CHECKED);
	theApp.Settings.SetDisplayMainDialogLogo(IsDlgButtonChecked(IDC_DisplayMainDialogLogo) == BST_CHECKED);

	theApp.Settings.SaveSettings();

	OnOK();
}


BOOL CBineyeSettings::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//CRect wrc;
	//GetClientRect(wrc);

	//pDC->FillSolidRect(wrc, RGB(0,0,0));

	return CDialog::OnEraseBkgnd(pDC);
}
