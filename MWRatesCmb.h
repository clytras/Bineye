#pragma once

#include <afxtempl.h>
#include ".\inc\fontext.h"
#include ".\inc\PWGraphics.h"
#include ".\inc\Color.h"


// This was introduced to me by Maximilian Hanel in his WTL port.  Cool, eh?
////////////////////////////////////////////////////////////////////////////////
// The class _ThunkImpl is a renamed version of Andrew Nosenko CAuxThunk implementation.
// Thanks Andrew, it's a fantastic class!
//
// Copyright (c) 1997-2001 by Andrew Nosenko <andien@nozillium.com>,
// (c) 1997-1998 by Computer Multimedia System, Inc.,
// (c) 1998-2001 by Mead & Co Limited
//
// http://www.nozillium.com/atlaux/
// Version: 1.10.0021
//

#ifndef _M_IX86
	#pragma message("_ThunkImpl/ is implemented for X86 only!")
#endif

#pragma pack(push, 1)
#pragma warning( disable : 4311 )

template <class T>
class _ThunkImpl
{
private:

	BYTE	m_mov;			// mov ecx, %pThis
	DWORD	m_this; 		//
	BYTE	m_jmp;			// jmp func
	DWORD	m_relproc;		// relative jmp

protected:
	
	typedef void (T::*TMFP)();
	void InitThunk(TMFP method, const T* pThis)
	{
		union { DWORD func; TMFP method; } addr;
		addr.method = (TMFP)method;
		m_mov  = 0xB9;
		m_this = (DWORD)pThis;
		m_jmp  = 0xE9;
		m_relproc = addr.func - (DWORD)(this+1);

		::FlushInstructionCache(GetCurrentProcess(), this, sizeof(*this));
	}
	FARPROC GetThunk() const 
   {
		_ASSERTE(m_mov == 0xB9);
		return (FARPROC)this; 
   }
};

#pragma pack(pop) // _ThunkImpl
#pragma warning( default : 4311 )


// we need these three dummy classes so we can 
// derive more than once from _ThunkImpl
template <class T>
class MWRCMouseHookThunk: public _ThunkImpl<T> {};

template <class T>
class MWRCKeybHookThunk: public _ThunkImpl<T> {};

//template <class T>
//class MWRCCallWndRetHookThunk: public _ThunkImpl<T> {};


#define MWRATESCMB_CLASSNAME			_T("MWRatesCmb")
#define MWRATESCMBLISTWND_CLASSNAME		_T("MWRatesCmbList")
#define MWRATESCMB_TIMEREVENT			1002

// CMWRatesCmb

class CMWRatesCmb : public CWnd
{
	//DECLARE_DYNAMIC(CMWRatesCmb)

public:
	class CItemData
	{
	public:
		CItemData(void)
		{
			CItemData(_T(""), _T(""), 0);
		}

		CItemData(LPCTSTR lpszCode, UINT uiResImg)
		{
			CItemData(_T(""), lpszCode, uiResImg);
		}

		CItemData(LPCTSTR lpszText, LPCTSTR lpszCode, UINT uiResImg)
		{
			strCode = lpszCode;
			strText = lpszText;
			
			if((uiImg = uiResImg) != 0)
			{
				pbmImg = new CBitmap();
				pbmImg->LoadBitmap(uiImg);
			} else 
				pbmImg = NULL;
		}

		~CItemData(void)
		{
			if(pbmImg)
			{
				pbmImg->DeleteObject();
				delete pbmImg;
			}
		}

		CString strCode;
		CString strText;
		CBitmap *pbmImg;
		UINT uiImg;
		CPWGraphics::BtnStateEx state;
	};

private:
	class CDropList : public CWnd,
		public MWRCKeybHookThunk<CMWRatesCmb::CDropList>,
		public MWRCMouseHookThunk<CMWRatesCmb::CDropList>
	{
	public:
		CDropList();
		CDropList(CFontExt *pFont, CMWRatesCmb *pCmb);
		virtual ~CDropList();

		BOOL RegisterWindowClass();
		BOOL Create();
		void ShowList();
		void HideList();
		void BuildBkBitmaps(void);
		int SelectItem(int nNewSel, bool bDraw = TRUE);
		void CalcItemSize(void);

	private:
		LRESULT KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam);
		LRESULT MouseHookProc(int code, WPARAM wParam, LPARAM lParam);
		void SetMouseHook();
		void RemoveMouseHook();
		void PosWindow();
		void DrawNC(CDC* pDC, LPRECT lprc);
		void DrawItem(CDC* pDC, LPRECT lprc, int idx, BOOL bDrawFull = FALSE);

		// animate window API, if available
		typedef BOOL (WINAPI* FN_ANIMATE_WINDOW)(HWND,DWORD,DWORD);
		FN_ANIMATE_WINDOW m_fnAnimateWindow;

	public:
		CFontExt *m_pFont;
		CMWRatesCmb *m_pCmb;
		int m_iItemSel;

	private:
		bool m_bFade;
		bool m_bShadow;
		HHOOK m_hMouseHook;
		int m_nItemHeight;
		int m_nItemWidth;
		int m_dwTick;
		CRgn m_rgn;
		CBitmap m_bmBkNormal;
		CBitmap m_bmBkHot;
		CPoint m_ptKey;

	public:
		afx_msg void OnDestroy();
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnClose();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	protected:
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	} m_cDropList ;

private:
	class CCmbEdit : public CEdit
	{
	public:
		CCmbEdit();
		virtual ~CCmbEdit();

	protected:
		DECLARE_MESSAGE_MAP()
	} m_cCmbEdit ;

public:
	CMWRatesCmb();
	virtual ~CMWRatesCmb();
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, CRect& rect, CWnd* pParent, UINT nID);
	int AddItem(LPCTSTR lpszText, UINT uiImg = 0);
	int AddItemCode(LPCTSTR lpszCode, LPCTSTR lpszText, UINT uiImg = 0);
	int AddItemCode(LPCTSTR lpszCode, UINT uiImg = 0);
	void SetCurSel(int idx, BOOL bRedraw = TRUE);
	int GetCurSel(void) { return m_iCurSel; };
	void SetCurSelByCode(LPCTSTR lpszCode);

public: // properties
	void SetBaseColor(COLORREF color, bool bFixBorderColor = true);
	COLORREF GetBaseColor(void);

	void SetTextColor(COLORREF color, bool bFixGlyphColor = true);
	COLORREF GetTextColor(void);

	void SetBorderColor(COLORREF color);
	COLORREF GetBorderColor(void);

	COLORREF GetHotColor(void);
	void SetHotColor(COLORREF color);

	void SetGlyphArrowColor(COLORREF color);
	COLORREF GetGlyphArrowColor(void);

	void SetShowBaseText(bool bSet = true);
	bool GetShowBaseText(void);

	void SetShowBaseCode(bool bSet = true);
	bool GetShowBaseCode(void);

	void SetShowBaseImg(bool bSet = true);
	bool GetShowBaseImg(void);

	void SetResImg(UINT uiImg, COLORREF crTrans = 0xFF00FF);
	UINT GetResImg(void);

	CPWGraphics::Alignment &ListAlign() { return m_ListAlign; }
	CPWGraphics::Alignment &Align() { return m_align; }

	COLORREF GetTransColor1(void);
	COLORREF GetTransColor2(void);

	CItemData *GetItem(int idx) { return (CItemData*)m_items[idx]; }
	int GetItemsCount() { return (int)m_items.GetCount(); }

private:
	BOOL RegisterWindowClass();
	void DrawBtnBmp(bool bRedraw = false);
	bool CreateBtnBmp(void);

private:
	CArray<CItemData*, CItemData*> m_items;
	CPWGraphics::BtnStateEx m_state;
	CPWGraphics::Alignment m_align;
	CPWGraphics::Alignment m_ListAlign;
	CBitmap m_bmWnd;
	CBitmap m_glyph;
	CFontExt m_font;
	CRgn m_rgn;
	COLORREF m_crBase;
	COLORREF m_crBorder;
	COLORREF m_crText;
	COLORREF m_crHot;
	COLORREF m_crGlyphArrow;
	COLORREF m_crImgTrans;
	COLORREF m_crTrans1;
	COLORREF m_crTrans2;
	bool m_bTimerSet;
	bool m_bShowBaseText;
	bool m_bShowBaseCode;
	bool m_bShowBaseImg;
	bool m_bShowTextOnNoSel;
	int m_nMaxImgWidth, m_nMaxImgHeight;
	int m_iCurSel;
	UINT m_uiImg;
	CBitmap *m_pImg;

public:
	bool m_bIsListDown;

public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

protected:
	DECLARE_MESSAGE_MAP()
public:
	void DropList(bool bRunTimer = true);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


