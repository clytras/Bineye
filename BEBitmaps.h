#pragma once

class CBEBitmaps
{
public:
	CBEBitmaps(void);
	~CBEBitmaps(void);
public:
	class CFuncBmpSize
	{
	public:
		CFuncBmpSize()
		: FullWidth(26)
		, FullHeight(26)
		, SmallWidth(13)
		, SmallHeight(13)
		, BigWidth(13)
		, BigHeight(26) {}
	public:
		const int FullWidth;
		const int FullHeight;
		const int SmallWidth;
		const int SmallHeight;
		const int BigWidth;
		const int BigHeight;
	} FuncBmpSize ;
public:
	CBitmap m_bm32HeadOn;
	CBitmap m_bm32HeadOff;
	CBitmap m_bm16HeadOn;
	CBitmap m_bm16HeadOff;
	CBitmap m_bm8HeadOn;
	CBitmap m_bm8HeadOff;
	CBitmap m_bmClrHeadOn;
	CBitmap m_bmClrHeadOff;

	CBitmap m_bmSOn;
	CBitmap m_bmSOff;
	CBitmap m_bmSOver;
	CBitmap m_bmUOn;
	CBitmap m_bmUOff;
	CBitmap m_bmUOver;

	CBitmap m_bmSignedOn;
	CBitmap m_bmSignedOff;
	CBitmap m_bmSignedOver;
	CBitmap m_bmUnsignedOn;
	CBitmap m_bmUnsignedOff;
	CBitmap m_bmUnsignedOver;

	CBitmap m_bmHexOn;
	CBitmap m_bmHexOff;
	CBitmap m_bmOctOn;
	CBitmap m_bmOctOff;
	CBitmap m_bmBinOn;
	CBitmap m_bmBinOff;
	CBitmap m_bmWWOn;
	CBitmap m_bmWWOff;

	CBitmap m_bmBitsOn;
	CBitmap m_bmBitsOff;

	CBitmap m_bmMMOn;
	CBitmap m_bmMMOff;

	CBitmap m_bmMMSelOn;
	CBitmap m_bmMMSelOff;

	CBitmap m_bmCircleOn;
	CBitmap m_bmCircleOff;
	CBitmap m_bmCircleWhiteOn;
	CBitmap m_bmCircleWhiteOff;

	CBitmap m_bmLblRGB;
	CBitmap m_bmLblRGBA;

	CBitmap m_bmCircleMask;
	CBitmap m_bmFuncs;

	COLORREF m_crTransCol;
	void LoadBitmaps(void);
};
