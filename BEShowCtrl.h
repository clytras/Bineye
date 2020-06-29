#pragma once
#include "afxwin.h"
#include ".\BEBitmaps.h"
#include ".\inc\FontExt.h"
#include ".\inc\PWGraphics.h"
//#include "f:\programming\c\bineye\inc\tooltipex.h"

#define LOWORD64(l)           ((DWORD)((DWORD64)(l) & 0xffffffff))
#define HIWORD64(l)           ((DWORD)((DWORD64)(l) >> 32))

//#define BESHOWCTRL_CLASSNAME		_T("BEShowCtrl")
#define BEBITSCTRL_CLASSNAME		_T("BEBitsCtrl")
#define BECLRSLIDERCTRL_CLASSNAME	_T("BEClrSliderCtrl")

#define BEVALUE_VALIDCHARS		_T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define BEVALUE_SIGNES			_T("-+")
#define BEVALUE_CPP_HEX_SIGN	_T("0X")
#define BEVALUE_CPP_OCT_SIGN	_T("O")
#define BEVALUE_WIN_HEX_SIGN	_T("X")
#define BEVALUE_WEB_HEX_SIGN	_T("#")


// CBEShowCtrl

class CBEShowCtrl : public CDialog /*CWnd*/
{
	//DECLARE_DYNAMIC(CBEShowCtrl)

public:
	static const int bigEndian, littleEndian;

	typedef enum {
		Unset = 1,
		Set,
		BitNot
	} Funcs ;

public:
	typedef class CBESCType
	{
	public:
		CBESCType() {};
	public:
		static const int type32Bit;
		static const int type16Bit;
		static const int type8Bit;
		static const int typeColor;
	private:
		int m_iType;
	} BESCType ;

public:
	class CBEValue
	{
	public:
		CBEValue() : m_i32Value(0), m_bSigned(FALSE) {};
	public:
		static const int _signed, _unsigned;
		static const int bit32, bit16, bit8;
		static const int   i32,   i16,   i8;
		static const int  ui32,  ui16,  ui8;
		static const int low, hiw, lob, hib;

	public:
		INT32 m_i32Value;
		BOOL m_bSigned;

		INT32  GetInt32(void)    { return m_i32Value; }
		INT16  GetInt16Lo(void)  { return INT16(LOWORD(m_i32Value)); }
		INT16  GetInt16Hi(void)  { return INT16(HIWORD(m_i32Value)); }
		INT8   GetInt8LoLo(void) { return INT8(LOBYTE(LOWORD(m_i32Value))); }
		INT8   GetInt8LoHi(void) { return INT8(HIBYTE(LOWORD(m_i32Value))); }
		INT8   GetInt8HiLo(void) { return INT8(LOBYTE(HIWORD(m_i32Value))); }
		INT8   GetInt8HiHi(void) { return INT8(HIBYTE(HIWORD(m_i32Value))); }

		UINT32 GetUInt32(void)    { return UINT32(GetInt32()); }
		UINT16 GetUInt16Lo(void)  { return UINT16(GetInt16Lo()); }
		UINT16 GetUInt16Hi(void)  { return UINT16(GetInt16Hi()); }
		UINT8  GetUInt8LoLo(void) { return UINT8(GetInt8LoLo()); }
		UINT8  GetUInt8LoHi(void) { return UINT8(GetInt8LoHi()); }
		UINT8  GetUInt8HiLo(void) { return UINT8(GetInt8HiLo()); }
		UINT8  GetUInt8HiHi(void) { return UINT8(GetInt8HiHi()); }

		BYTE GetRed(void)   { return (BYTE)GetUInt8LoLo(); }
		BYTE GetGreen(void) { return (BYTE)GetUInt8LoHi(); }
		BYTE GetBlue(void)  { return (BYTE)GetUInt8HiLo(); }
		BYTE GetAlpha(void) { return (BYTE)GetUInt8HiHi(); }

		COLORREF GetRGB(void)  { return (COLORREF)((DWORD)((DWORD)m_i32Value) & 0xFFFFFF); }
		COLORREF GetRGBA(void) { return (COLORREF)m_i32Value; }

		CString GetHexRGBA(LPCTSTR lpszSign = _T(""), BOOL bReturnAlpha = TRUE);
		CString GetHexRGB(LPCTSTR lpszSign = _T(""))  { return GetHexRGBA(lpszSign, FALSE); }
		CString GetHexABGR(LPCTSTR lpszSign = _T(""), BOOL bReturnAlpha = TRUE);
		CString GetHexBGR(LPCTSTR lpszSign = _T(""))  { return GetHexABGR(lpszSign, FALSE); }
		CString GetWebColor(LPCTSTR lpszSign = BEVALUE_WEB_HEX_SIGN) { return GetHexRGB(lpszSign); }
		CString GetWinColor(LPCTSTR lpszSign = BEVALUE_WIN_HEX_SIGN) { return GetHexBGR(lpszSign); }

		void SetRed(BYTE red)     { SetUInt8LoLo((UINT8)red); }
		void SetGreen(BYTE green) { SetUInt8LoHi((UINT8)green); }
		void SetBlue(BYTE blue)   { SetUInt8HiLo((UINT8)blue); }
		void SetAlpha(BYTE alpha) { SetUInt8HiHi((UINT8)alpha); }

		void SetRGB(BYTE r, BYTE g, BYTE b) { SetRGB((COLORREF)RGB(r, g, b)); }
		void SetRGB(COLORREF rgb)   { m_i32Value = INT32((((DWORD)rgb) & 0xFFFFFF) | (((DWORD)m_i32Value) & 0xFF000000)); }
		void SetRGBA(COLORREF rgba) { m_i32Value = INT32(rgba); }

		BOOL SetHexRGBA(CString strRGBA, BOOL bApplyAlpha = TRUE, BOOL bIsBigEndian = TRUE);
		BOOL SetHexRGB(CString strRGB) { return SetHexRGBA(strRGB, FALSE); }
		BOOL SetHexABGR(CString strABGR, BOOL bApplyAlpha = TRUE) { return SetHexRGBA(strABGR, bApplyAlpha, FALSE); }
		BOOL SetHexBGR(CString strBGR) { return SetHexRGBA(strBGR, FALSE, FALSE); }
		BOOL SetWebColor(CString strWebClr) { return SetHexRGB(strWebClr); }
		BOOL SetWinColor(CString strWinClr) { return SetHexBGR(strWinClr); }

		void SetInt32(INT32 i32)      { m_i32Value = i32; }
		void SetInt16Lo(INT16 i16Lo)  { m_i32Value = INT32(MAKELONG(i16Lo, HIWORD(m_i32Value))); }
		void SetInt16Hi(INT16 i16Hi)  { m_i32Value = INT32(MAKELONG(LOWORD(m_i32Value), i16Hi)); }
		void SetInt8LoLo(INT8 i8LoLo) { m_i32Value = INT32(MAKELONG(MAKEWORD(i8LoLo, HIBYTE(LOWORD(m_i32Value))), HIWORD(m_i32Value))); }
		void SetInt8LoHi(INT8 i8LoHi) { m_i32Value = INT32(MAKELONG(MAKEWORD(LOBYTE(LOWORD(m_i32Value)), i8LoHi), HIWORD(m_i32Value))); }
		void SetInt8HiLo(INT8 i8HiLo) { m_i32Value = INT32(MAKELONG(LOWORD(m_i32Value), MAKEWORD(i8HiLo, HIBYTE(HIWORD(m_i32Value))))); }
		void SetInt8HiHi(INT8 i8HiHi) { m_i32Value = INT32(MAKELONG(LOWORD(m_i32Value), MAKEWORD(LOBYTE(HIWORD(m_i32Value)), i8HiHi))); }

		void SetUInt32(UINT32 ui32)      { SetInt32(INT32(ui32)); }
		void SetUInt16Lo(UINT16 ui16Lo)  { SetInt16Lo(INT16(ui16Lo)); }
		void SetUInt16Hi(UINT16 ui16Hi)  { SetInt16Hi(INT16(ui16Hi)); }
		void SetUInt8LoLo(UINT8 ui8LoLo) { SetInt8LoLo(INT8(ui8LoLo)); }
		void SetUInt8LoHi(UINT8 ui8LoHi) { SetInt8LoHi(INT8(ui8LoHi)); }
		void SetUInt8HiLo(UINT8 ui8HiLo) { SetInt8HiLo(INT8(ui8HiLo)); }
		void SetUInt8HiHi(UINT8 ui8HiHi) { SetInt8HiHi(INT8(ui8HiHi)); }

		CString GetHex(int iPos = 0);
		BOOL SetHex(CString strValue, int iPos = 0) { return SetCmnValue(strValue, iPos, 16); }

		CString GetOct(int iPos = 0);
		BOOL SetOct(CString strValue, int iPos = 0) { return SetCmnValue(strValue, iPos, 8); }

		CString GetBin(int iPos = 0);
		BOOL SetBin(CString strValue, int iPos = 0) { return SetCmnValue(strValue, iPos, 2); }

		BOOL SetDec(CString strValue, int iPos = 0, BOOL bSigned = FALSE);

		BOOL SetCmnValue(CString strValue, int iPos , int iBase);
		CString GetCmnValue(int iPos, int iBase);

		int GetInt(int iPos = 0);
		void SetInt(int iValue, int iPos = 0);

		void SetUInt(unsigned int uiValue, int iPos = 0) { SetInt(int(uiValue), iPos); }
		unsigned int GetUInt(int iPos = 0) { return unsigned int(GetInt(iPos)); }

		BOOL CheckValidChars(CString strValue, int iBase = 10, BOOL bIgnoreSign = TRUE);
		BOOL CheckValidChars(LPCTSTR lpszValue, int iBase = 10, BOOL bIgnoreSign = TRUE)
		{
			return CheckValidChars(CString(lpszValue), iBase, bIgnoreSign);
		}

		BOOL HasSign(CString strValue)  { return CString(BEVALUE_SIGNES).Find(strValue[0]) != -1; }
		BOOL HasSign(LPCTSTR lpszValue) { return HasSign(CString(lpszValue)); }
		void RemoveSign(CString &strValue);

		BOOL HasAdditionRemove(CString &strValue, int iBase, BOOL bRemove = FALSE)
		{
			CString strCheck(strValue.MakeUpper());
			if(iBase == 16)
			{
				if(strCheck.Left((int)_tcslen(BEVALUE_CPP_HEX_SIGN)) == BEVALUE_CPP_HEX_SIGN)
				{
					if(bRemove) strValue = strValue.Right(strValue.GetLength() - int(_tcslen(BEVALUE_CPP_HEX_SIGN)));
					return TRUE;
				} else
				if(strCheck.Left((int)_tcslen(BEVALUE_WEB_HEX_SIGN)) == BEVALUE_WEB_HEX_SIGN)
				{
					if(bRemove) strValue = strValue.Right(strValue.GetLength() - int(_tcslen(BEVALUE_WEB_HEX_SIGN)));
					return TRUE;
				} else
				if(strCheck.Left((int)_tcslen(BEVALUE_WIN_HEX_SIGN)) == BEVALUE_WIN_HEX_SIGN)
				{
					if(bRemove) strValue = strValue.Right(strValue.GetLength() - int(_tcslen(BEVALUE_WIN_HEX_SIGN)));
					return TRUE;
				}
			} else
			if(iBase == 8)
			{
				if(strCheck.Left((int)_tcslen(BEVALUE_CPP_OCT_SIGN)) == BEVALUE_CPP_OCT_SIGN)
				{
					if(bRemove) strValue = strValue.Right(strValue.GetLength() - int(_tcslen(BEVALUE_CPP_OCT_SIGN)));
					return TRUE;
				}
			}

			return FALSE;
		}

		BOOL CheckDecValue(CString strValue, int iType);
		BOOL CheckDecValue(LPCTSTR lpszValue, int iType) { CheckDecValue(CString(lpszValue), iType); };

		BOOL CheckCmnValue(CString strValue, int iBase, int iBits);
		BOOL CheckCmnValue(LPCTSTR lpszValue, int iBase, int iBits) { CheckCmnValue(CString(lpszValue), iBase, iBits); };

		int PosToType(int iPos);
		int TypeToBits(int iType);
		int PosToBits(int iPos);
	} BEValue ;

private:

	class CEdits : public CEdit
	{
	public:
		CEdits()
		 : m_crTextColor(0)
		 , m_crTextColorChange(0xFF)
		 , m_iByteIndex(0)
		 , m_bTextSource(FALSE)
		 , m_bIsApplyed(FALSE) {};
		virtual ~CEdits() {};
	private:
		BOOL IsTextSource(void) { return m_bTextSource; }
		void SetTextSource(BOOL bSet = TRUE) { m_bTextSource = bSet; }
	public:
		BOOL IsSigned() { return m_bSigned; }
		BOOL IsDec() { return m_iBase == 10; }
		BOOL IsHex() { return m_iBase == 16; }
		BOOL IsOct() { return m_iBase == 8; }
		BOOL IsBin() { return m_iBase == 2; }
		BOOL IsHexColor() { return 	m_iByteIndex == -1 /*&& m_iBase == 16*/ ; }

		void SetSigned(BOOL bSet = TRUE) { m_bSigned = bSet; }

		void SetByteIndex(int iByteIndex) { m_iByteIndex = iByteIndex; }
		int GetByteIndex(void) { return m_iByteIndex ; }
		BOOL IsRealDirty(void)
		{
			CString strText, strCmp = m_strText;
			GetWindowTextW(strText);

			//strText = strText.TrimLeft(_T("0"));
			//strCmp = strCmp.TrimLeft(_T("0"));
			TrimZeroes(strText);
			TrimZeroes(strCmp);
			return strText.CompareNoCase(strCmp) != 0;
		}
		BOOL IsDirty(void)
		{
			CString strText;
			GetWindowTextW(strText);

			return strText.Compare(m_strText) != 0;
		}
		COLORREF m_crTextColor, m_crTextColorChange;
		BOOL m_bTextSource;
		BOOL m_bSigned;
		BOOL m_bIsApplyed;
		int m_iByteIndex;
		int m_iBase;
		CString m_strText;
		DECLARE_MESSAGE_MAP()
		afx_msg void OnEnSetfocus();
		int GetBitsCount(void);
		int GetBase(void) { return m_iBase; }
	public:
		void SetText(CString strText);
		afx_msg void OnEnKillfocus();
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnEnUpdate();
		BOOL OnCharValidate(UINT *nChar, UINT nRepCnt, UINT nFlags, int iBase, BOOL bSigned = FALSE);
		BOOL ApplyText(CString strText);
		void FixSel(BOOL bMoveToEnd = FALSE);
	};

	// Dec edit ctrl --------------------------------------------------------
	class CDecEdit : public CEdits
	{
	public:
		CDecEdit();
		virtual ~CDecEdit() {};

	protected:
		DECLARE_MESSAGE_MAP()
	public:

		int GetBEValueType(void);
	} *m_pDecEdit32,
	  *m_pDecEdit16L, *m_pDecEdit16H,
	  *m_pDecEdit8LL, *m_pDecEdit8LH, *m_pDecEdit8HL, *m_pDecEdit8HH ;

	// Hex edit ctrl --------------------------------------------------------
	class CHexEdit : public CEdits
	{
	public:
		CHexEdit();
		virtual ~CHexEdit() {};
	public:

	protected:
		DECLARE_MESSAGE_MAP()

	} *m_pWebEdit,
	  *m_pHexEdit32,
	  *m_pHexEdit16L, *m_pHexEdit16H,
	  *m_pHexEdit8LL, *m_pHexEdit8LH, *m_pHexEdit8HL, *m_pHexEdit8HH ;

	// Oct edit ctrl --------------------------------------------------------
	class COctEdit : public CEdits
	{
	public:
		COctEdit();
		virtual ~COctEdit() {};
	public:

	protected:
		DECLARE_MESSAGE_MAP()

	} *m_pOctEdit32,
	  *m_pOctEdit16L, *m_pOctEdit16H,
	  *m_pOctEdit8LL, *m_pOctEdit8LH, *m_pOctEdit8HL, *m_pOctEdit8HH ;

	// Bin edit ctrl --------------------------------------------------------
	class CBinEdit : public CEdits
	{
	public:
		CBinEdit();
		virtual ~CBinEdit() {};
	public:

	protected:
		DECLARE_MESSAGE_MAP()

	} *m_pBinEdit32,
	  *m_pBinEdit16L, *m_pBinEdit16H,
	  *m_pBinEdit8LL, *m_pBinEdit8LH, *m_pBinEdit8HL, *m_pBinEdit8HH ;

	// Hex color edit ctrl --------------------------------------------------
	class CHexClrEdit : public CEdits
	{
	public:
		CHexClrEdit(int iEndian);
		virtual ~CHexClrEdit() {};
	public:
		int GetEndian() { return m_iEndian; }
		BOOL IsBigEndian() { return m_iEndian == CBEShowCtrl::bigEndian; }
		BOOL IsLittleEndian() { return m_iEndian == CBEShowCtrl::littleEndian; }
		CString GetValueByEndian(void)
		{
			if(IsBigEndian())
				return ((CBEShowCtrl*)GetParent())->BEValue.GetWebColor(theApp.Settings.IsApplyWebColorSign() ? BEVALUE_WEB_HEX_SIGN : NULL);
			else /* if(IsLittleEndian()) */
				return ((CBEShowCtrl*)GetParent())->BEValue.GetWinColor(theApp.Settings.IsApplyWinColorSign() ? _T("x") : NULL);
		}

	private:
		int m_iEndian;

	protected:
		DECLARE_MESSAGE_MAP()

	} *m_pHexClrEditWeb, *m_pHexClrEditWin ;

	class CBitsCtrl : public CWnd
	{
	public:
		CBitsCtrl();
		virtual ~CBitsCtrl() {};

	protected:
		DECLARE_MESSAGE_MAP()
	public:
		BOOL Create(LPRECT lpRect, CWnd* pParent, UINT nID);
	private:
		CBEShowCtrl* m_pParent;
		BOOL RegisterWindowClass(void);
	public:
		afx_msg void OnPaint();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		int BitFromPoint(CPoint point);
		int FixBitPos(int iBit);
		void InvalidateBits(int iBit = 0);
		CRect RectFromBit(int iBit = 0);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	} m_cBitsCtrl ;

public:
	class CClrSliderCtrl : public CWnd
	{
	public:
		static const int ctRed;
		static const int ctGreen;
		static const int ctBlue;
		static const int ctAlpha;
	public:
		BOOL IsRed(void)   { return m_iType == ctRed; }
		BOOL IsGreen(void) { return m_iType == ctGreen; }
		BOOL IsBlue(void)  { return m_iType == ctBlue; }
		BOOL IsAlpha(void) { return m_iType == ctAlpha; }
	public:
		CClrSliderCtrl();
		virtual ~CClrSliderCtrl() {};

	protected:
		DECLARE_MESSAGE_MAP()
	public:
		BOOL Create(int iType, int x, int y, CWnd* pParent, UINT nID);
	private:
		CBEShowCtrl* m_pParent;
		BOOL RegisterWindowClass(void);
		CBitmap m_bmColor;
		CBitmap m_bmPointer;
		bool m_bIsScrolling;
		bool m_bIsSelected;
		int m_iType;
		int m_iLRKeyCount;
		const CSize m_szDefaultSize;
		CRect m_rcColor;
		int m_iValue;
		const int m_iMin;
		const int m_iMax;
	public:
		afx_msg void OnPaint();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		void BuildBgBitmaps(void);
		void UpdateAlphaColor(void);
		void PaintPointer(CDC* pDC = NULL);
		void PaintColor(CDC* pDC = NULL);
	public:
		void UpdateValueFromParent(void);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		void UpdateFromParent(void);
		int ColorToFieldIndex(void);
		void SetValue(int iValue);
		void UpdateValueToParent(void);
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	} *m_pClrSliderRed, *m_pClrSliderGreen, *m_pClrSliderBlue, *m_pClrSliderAlpha;

public:
	CBEShowCtrl(CBEBitmaps* pBitmaps);
	virtual ~CBEShowCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL Create(int iType, int x, int y, CWnd* pParent, UINT nID);

private:
	int m_iType;
	CBEBitmaps* m_pBitmaps;

	//BOOL RegisterWindowClass(void);
public:
	BOOL Is32(void)    { return m_iType == CBEShowCtrl::BESCType::type32Bit; }
	BOOL Is16(void)    { return m_iType == CBEShowCtrl::BESCType::type16Bit; }
	BOOL Is8(void)     { return m_iType == CBEShowCtrl::BESCType::type8Bit;  }
	BOOL IsColor(void) { return m_iType == CBEShowCtrl::BESCType::typeColor; }
	int GetFieldsCount(void) { return m_iFieldsCount; }
	afx_msg void OnPaint();
private:
	void Paint32(CDC* pDC);
	void Paint16(CDC* pDC);
	void Paint8(CDC* pDC);
	void PaintColor(CDC* pDC);
	void PaintFuncs(CDC* pDC);
	int m_iSelected;
	int m_iSUHot;
	int m_iFuncHot;
	bool m_bIsColorInit;
	//BOOL m_bSigned;
	UINT_PTR m_uipTimerID;
	CArray<CBEShowCtrl*, CBEShowCtrl*> m_UpdateCtrls;
	CEdits* m_pCmnEdits[4][3];
	CRect m_rcDecColors[4];
	CRect m_rcBigColors;
	CRect m_rcWWLbls[2];
	CClrSliderCtrl *m_pClrSliders[4];
	CRect m_rcCircle;
	int m_iFieldsCount;
public:
	CFontExt m_fntDecEdt;
	CFontExt m_fntCmnEdt;
	UINT m_uiID;
	COLORREF m_crDecEdtBkColorOn, m_crDecEdtBkColorOff,
			 m_crDecEdtTextColorOn, m_crDecEdtTextColorOff, m_crDecEdtTextColorChange,
			 m_crCmnEdtBkColorOn, m_crCmnEdtBkColorOff;
	CBrush m_brDecEdtBkBrushOn, m_brDecEdtBkBrushOff,
		   m_brCmnEdtBkBrushOn, m_brCmnEdtBkBrushOff;
	CSize m_szWndSize;
public:
	CEdits* m_pDecEdits[4];
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	int GetSelected(void) { return m_iSelected; }
	bool IsSelected(void) { return m_iSelected != 0; }
	void SetSelected(int iSelected, bool bForceRedraw = false);
	void AddUpdateCtrl(CBEShowCtrl* pUpdateCtrl);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void ResetSelectedUpdateCtrls(void);
	int GetFldBitsCount(void);
	//int GetDecType(void);
	void UpdateFields(int iFieldIndex = 0);
	void UpdateDecFields(int iFieldIndex = 0);
	void UpdateCmnFields(int iFieldIndex = 0);
	void SetSigned(bool bSet, int iFldIdx = 0);
private:
	void GetFldX1X2(int iFld, int& x1, int& x2);
	void InvalidateFieldEdits(int iFieldIndex = -1);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CRect GetSUHotRect(int iFieldIndex);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetUpdateCtrls(BOOL bDoUpdate = TRUE);
	void SetUpdateCtrlsByFieldIndex(int iFldIdx, bool bDoUpdate = TRUE);
	int GetBEValueType(int iFieldIndex = 1);
	int FieldToPos(int iFld);
	static void TrimZeroes(CString& strValue);
	void PaintDecColors(CDC* pDC = NULL);
	void PaintBigColors(CDC* pDC = NULL);
	void PaintCircle(CDC* pDC = NULL);
	void InvalidateColorSlider(int iFieldIndex = 0);
	void UpdateColorSliderPointer(int iFieldIndex = 0);
	CRect GetFuncRect(Funcs func, int iFieldIndex = 0);
	void ApplyFunc(Funcs func);
	void ShowColorDialog(void);
	void SetSignedSet(UINT uiSingSet);
	UINT GetSignedSet();

};
