#pragma once
#include "afxwin.h"

class CZKRImage :
	public CBitmap
{
public:
	static const int ReadWhole;
	static const int ReadHalf;
	static const int ReadQuarter;
	static const int ReadEighth;
public:
	int GetWidth() { return(m_nWidth); };
	int GetHeight() { return(m_nHeight); };
	CZKRImage(void);
	~CZKRImage(void);
	bool LoadBMPFile(LPCTSTR lpszFile);
	bool LoadJPGFile(LPCTSTR lpszFile, int iJPegScale = ReadWhole);
	bool LoadGIFFile(LPCTSTR lpszFile);
	int BytesPerScanLine(int biWidth, int biBitCount);
private:
	struct COLOR
	{
		unsigned char b,g,r,x;
	};	// Windows GDI expects 4bytes per color

	COLOR* m_palPalette;

	int LZWDecoder(char * bufIn, char * bufOut, short InitCodeSize, int AlignedWidth, int Width, int Height, const int Interlace);
protected:
	int m_nTransparentIndex;
	bool m_bTransparent;
	int m_nY;
	int m_nX;
	int m_nHeight;
	int m_nWidth;
};
