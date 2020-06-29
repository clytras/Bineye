#include "StdAfx.h"
#include "BEBitmaps.h"
#include "resource.h"

CBEBitmaps::CBEBitmaps(void)
{
}

CBEBitmaps::~CBEBitmaps(void)
{
}

void CBEBitmaps::LoadBitmaps(void)
{
	// Load headers ----------------------------------------------------
	m_bm32HeadOn.LoadBitmap(IDB_32BIT_ON);
	m_bm32HeadOff.LoadBitmap(IDB_32BIT_OFF);
	m_bm16HeadOn.LoadBitmap(IDB_16BIT_ON);
	m_bm16HeadOff.LoadBitmap(IDB_16BIT_OFF);
	m_bm8HeadOn.LoadBitmap(IDB_8BIT_ON);
	m_bm8HeadOff.LoadBitmap(IDB_8BIT_OFF);
	m_bmClrHeadOn.LoadBitmap(IDB_CLR_ON);
	m_bmClrHeadOff.LoadBitmap(IDB_CLR_OFF);

	// Load s/u, signed/unsigned ---------------------------------------
	m_bmSOn.LoadBitmap(IDB_S_ON);
	m_bmSOff.LoadBitmap(IDB_S_OFF);
	m_bmSOver.LoadBitmap(IDB_S_OVER);
	m_bmUOn.LoadBitmap(IDB_U_ON);
	m_bmUOff.LoadBitmap(IDB_U_OFF);
	m_bmUOver.LoadBitmap(IDB_U_OVER);
	m_bmSignedOn.LoadBitmap(IDB_SIGNED_ON);
	m_bmSignedOff.LoadBitmap(IDB_SIGNED_OFF);
	m_bmSignedOver.LoadBitmap(IDB_SIGNED_OVER);
	m_bmUnsignedOn.LoadBitmap(IDB_UNSIGNED_ON);
	m_bmUnsignedOff.LoadBitmap(IDB_UNSIGNED_OFF);
	m_bmUnsignedOver.LoadBitmap(IDB_UNSIGNED_OVER);

	// Load hex, oct, bin labels ---------------------------------------
	m_bmHexOn.LoadBitmap(IDB_HEX_ON);
	m_bmHexOff.LoadBitmap(IDB_HEX_OFF);
	m_bmOctOn.LoadBitmap(IDB_OCT_ON);
	m_bmOctOff.LoadBitmap(IDB_OCT_OFF);
	m_bmBinOn.LoadBitmap(IDB_BIN_ON);
	m_bmBinOff.LoadBitmap(IDB_BIN_OFF);

	m_bmWWOn.LoadBitmap(IDB_WW_ON);
	m_bmWWOff.LoadBitmap(IDB_WW_OFF);

	// Load bit lines --------------------------------------------------
	m_bmBitsOn.LoadBitmap(IDB_BITS_ON);
	m_bmBitsOff.LoadBitmap(IDB_BITS_OFF);

	// Load min/max bks ------------------------------------------------
	m_bmMMOn.LoadBitmap(IDB_MM_ON);
	m_bmMMOff.LoadBitmap(IDB_MM_OFF);
	m_bmMMSelOn.LoadBitmap(IDB_MM_SEL_ON);
	m_bmMMSelOff.LoadBitmap(IDB_MM_SEL_OFF);

	// Load circles ----------------------------------------------------
	m_bmCircleOn.LoadBitmap(IDB_CIRCLE_ON);
	m_bmCircleOff.LoadBitmap(IDB_CIRCLE_OFF);
	m_bmCircleWhiteOn.LoadBitmap(IDB_CIRCLE_WHITE_ON);
	m_bmCircleWhiteOff.LoadBitmap(IDB_CIRCLE_WHITE_OFF);
	m_bmCircleMask.LoadBitmap(IDB_CIRCLE_MASK);

	// Load color labels -----------------------------------------------
	m_bmLblRGB.LoadBitmap(IDB_CLR_LBL_RGB);
	m_bmLblRGBA.LoadBitmap(IDB_CLR_LBL_RGBA);

	m_bmFuncs.LoadBitmap(IDB_FUNCS);

	m_crTransCol = 0x666666;
}
