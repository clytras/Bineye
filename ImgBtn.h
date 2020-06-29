#pragma once

#include ".\inc\PWGraphics.h"

// CImgBtn

#define IBIMAGESTYPESCOUNT	5

class CImgBtn : public CButton
{
	DECLARE_DYNAMIC(CImgBtn)

public:
	CImgBtn();
	virtual ~CImgBtn();

public:
	/*typedef enum
	{
		Normal = 0,
		Hot,
		Pressed,
		Disabled,
		Focused
	} IBImgTypes ;*/

	typedef struct
	{
		static const int Normal = 0;
		static const int Hot = 1;
		static const int Pressed = 2;
		static const int Disabled = 3;
		static const int Focused = 4;
	} IBImgTypes ;

private:
	CPoint m_ptImages[IBIMAGESTYPESCOUNT];

/*private:
	class CIBImages
	{
	public:
		CIBImages()
		: DrawFocusOver(true)
		, pNormal(NULL)
		, pHot(NULL)
		, pPressed(NULL)
		, pDisabled(NULL)
		, pFocused(NULL) {}
		~CIBImages()
		{
			CBitmap *pBmps[] = { pNormal,
								 pHot,
								 pPressed,
								 pDisabled,
								 pFocused };

			for(int i = 0; i < IBIMAGESTYPESCOUNT; i++)
			{
				if(pBmps[i])
				{
					pBmps[i]->DeleteObject();
					delete pBmps[i];
				}
			}
		}
	public:
		CBitmap *pNormal;
		CBitmap *pHot;
		CBitmap *pPressed;
		CBitmap *pDisabled;
		CBitmap *pFocused;
		bool DrawFocusOver;
	} IBImages ;
*/
private:
	CBitmap *m_pBmpImages[IBIMAGESTYPESCOUNT];
	void DoTME(DWORD dwFlags = TME_HOVER | TME_LEAVE);
	bool m_bIsHOver;
public:
	void LoadBitmaps(UINT nIDBmpNormal,
					 UINT nIDBmpHot = 0,
					 UINT nIDBmpPressed = 0,
					 UINT nIDBmpDisabled = 0,
					 UINT nIDBmpFocused = 0);
	void SetBitmapXY(int iImgType, LPPOINT lpPt);
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
protected:
	CPWGraphics::BtnState m_enmState;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
private:
	bool m_bUseBgColor;
	COLORREF m_crBgColor;
public:
	void SetUseBgColor(bool bSet = true) { m_bUseBgColor; }
	bool GetUseBgColor(void) { return m_bUseBgColor; }
};
