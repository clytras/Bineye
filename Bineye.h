// Bineye.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBineyeApp:
// See Bineye.cpp for the implementation of this class
//

class CBineyeApp : public CWinApp
{
public:
	CBineyeApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	class CSettins
	{
	public:
		CSettins()
		: m_bHexUpperCase(true)
		, m_bZeroFill(true)
		, m_bSelectAllOnFocus(true)
		, m_bApplyAlphaOnTriCircle(true)
		, m_bApplyWebColorSign(true)
		, m_bApplyWinColorSign(true)
		, m_bBitsZeroIndex(false)
		, m_bShowSignedPlusSign(false)
		, m_bDisplayLogo(false)
		, m_strSection(_T("Settings"))
		, m_ui32BITSigned(0x1)
		, m_ui16BITSigned(0x3)
		, m_ui8BITSigned(0x0) { /*LoadSettings();*/ }
		virtual ~CSettins() {}
	public:
		void LoadSettings()
		{
			CWinApp* pApp = AfxGetApp();

			m_bHexUpperCase = pApp->GetProfileInt(m_strSection, _T("HexUpperCase"), (INT)m_bHexUpperCase) != 0;
			m_bZeroFill = pApp->GetProfileInt(m_strSection, _T("ZeroFill"), (INT)m_bZeroFill) != 0;
			m_bSelectAllOnFocus = pApp->GetProfileInt(m_strSection, _T("SelectAllOnFocus"), (INT)m_bSelectAllOnFocus) != 0;
			m_bApplyAlphaOnTriCircle = pApp->GetProfileInt(m_strSection, _T("ApplyAlphaOnTriCircle"), (INT)m_bApplyAlphaOnTriCircle) != 0;
			m_bApplyWebColorSign = pApp->GetProfileInt(m_strSection, _T("ApplyWebColorSign"), (INT)m_bApplyWebColorSign) != 0;
			m_bApplyWinColorSign = pApp->GetProfileInt(m_strSection, _T("ApplyWinColorSign"), (INT)m_bApplyWinColorSign) != 0;
			m_bBitsZeroIndex = pApp->GetProfileInt(m_strSection, _T("BitsZeroIndex"), (INT)m_bBitsZeroIndex) != 0;
			m_bShowSignedPlusSign = pApp->GetProfileInt(m_strSection, _T("ShowSignedPlusSign"), (INT)m_bShowSignedPlusSign) != 0;
			m_bDisplayLogo = pApp->GetProfileInt(m_strSection, _T("DisplayMainDialogLogo"), (INT)m_bDisplayLogo) != 0;

			m_ui32BITSigned = pApp->GetProfileInt(m_strSection, _T("32BITSignSet"), (INT)m_ui32BITSigned);
			m_ui16BITSigned = pApp->GetProfileInt(m_strSection, _T("16BITSignSet"), (INT)m_ui16BITSigned);
			m_ui8BITSigned = pApp->GetProfileInt(m_strSection, _T("8BITSignSet"), (INT)m_ui8BITSigned);
		}
		void SaveSettings()
		{
			CWinApp* pApp = AfxGetApp();

			pApp->WriteProfileInt(m_strSection, _T("HexUpperCase"), (int)m_bHexUpperCase);
			pApp->WriteProfileInt(m_strSection, _T("ZeroFill"), (int)m_bZeroFill);
			pApp->WriteProfileInt(m_strSection, _T("SelectAllOnFocus"), (int)m_bSelectAllOnFocus);
			pApp->WriteProfileInt(m_strSection, _T("ApplyAlphaOnTriCircle"), (int)m_bApplyAlphaOnTriCircle);
			pApp->WriteProfileInt(m_strSection, _T("ApplyWebColorSign"), (int)m_bApplyWebColorSign);
			pApp->WriteProfileInt(m_strSection, _T("ApplyWinColorSign"), (int)m_bApplyWinColorSign);
			pApp->WriteProfileInt(m_strSection, _T("BitsZeroIndex"), (int)m_bBitsZeroIndex);
			pApp->WriteProfileInt(m_strSection, _T("ShowSignedPlusSign"), (int)m_bShowSignedPlusSign);
			pApp->WriteProfileInt(m_strSection, _T("DisplayMainDialogLogo"), (int)m_bDisplayLogo);

			pApp->WriteProfileInt(m_strSection, _T("32BITSignSet"), (int)m_ui32BITSigned);
			pApp->WriteProfileInt(m_strSection, _T("16BITSignSet"), (int)m_ui16BITSigned);
			pApp->WriteProfileInt(m_strSection, _T("8BITSignSet"), (int)m_ui8BITSigned);
		}
	private:
		bool m_bHexUpperCase;
		bool m_bZeroFill;
		bool m_bSelectAllOnFocus;
		bool m_bApplyAlphaOnTriCircle;
		bool m_bApplyWebColorSign;
		bool m_bApplyWinColorSign;
		bool m_bBitsZeroIndex;
		bool m_bShowSignedPlusSign;
		bool m_bDisplayLogo;

		UINT m_ui32BITSigned;
		UINT m_ui16BITSigned;
		UINT m_ui8BITSigned;

		CString m_strSection;
	public:
		bool IsHexUpperCase() { return m_bHexUpperCase; }
		bool IsZeroFill() { return m_bZeroFill; }
		bool IsSelectAllOnFocus() { return m_bSelectAllOnFocus; }
		bool IsApplyAlphaOnTriCircle() { return m_bApplyAlphaOnTriCircle; }
		bool IsApplyWebColorSign() { return m_bApplyWebColorSign; }
		bool IsApplyWinColorSign() { return m_bApplyWinColorSign; }
		bool IsBitsZeroIndex() { return m_bBitsZeroIndex; }
		bool IsShowSignedPlusSign() { return m_bShowSignedPlusSign; }
		bool IsDisplayMainDialogLogo() { return m_bDisplayLogo; }

		void SetHexUpperCase(bool bval) { m_bHexUpperCase = bval; }
		void SetZeroFill(bool bval) { m_bZeroFill = bval; }
		void SetSelectAllOnFocus(bool bval) { m_bSelectAllOnFocus = bval; }
		void SetApplyAlphaOnTriCircle(bool bval) { m_bApplyAlphaOnTriCircle = bval; }
		void SetApplyWebColorSign(bool bval) { m_bApplyWebColorSign = bval; }
		void SetApplyWinColorSign(bool bval) { m_bApplyWinColorSign = bval; }
		void SetBitsZeroIndex(bool bval) { m_bBitsZeroIndex = bval; }
		void SetShowSignedPlusSign(bool bval) { m_bShowSignedPlusSign = bval; }
		void SetDisplayMainDialogLogo(bool bval) { m_bDisplayLogo = bval; }

		UINT Get32BITSigned() { return m_ui32BITSigned; }
		UINT Get16BITSigned() { return m_ui16BITSigned; }
		UINT Get8BITSigned() { return m_ui8BITSigned; }

		void Set32BITSigned(UINT uiset) { m_ui32BITSigned = uiset; }
		void Set16BITSigned(UINT uiset) { m_ui16BITSigned = uiset; }
		void Set8BITSigned(UINT uiset) { m_ui8BITSigned = uiset; }

	} Settings ;
	bool CheckScreen24Bits(bool bShowMsg = true);
};

extern CBineyeApp theApp;