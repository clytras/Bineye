#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
//#include <afxtempl.h>		// Let manage CArray


#pragma comment(lib, "F:\\Programming\\C\\BineyeVC10\\inc\\ijl15.lib")
#pragma pack(1)

#include "zkrimage.h"
#include "ijl.h"

const int CZKRImage::ReadWhole = -1;
const int CZKRImage::ReadHalf = -2;
const int CZKRImage::ReadQuarter = -3;
const int CZKRImage::ReadEighth = -4;

#define ALIGN sizeof(int)				// Windows GDI expects all int-aligned

CZKRImage::CZKRImage(void) :
m_palPalette(NULL)
{
}

CZKRImage::~CZKRImage(void)
{
}

bool CZKRImage::LoadBMPFile(LPCTSTR lpszFile)
{
	HANDLE hBmpRet;

	hBmpRet = LoadImage(NULL, lpszFile, IMAGE_BITMAP, NULL, NULL, LR_DEFAULTSIZE|LR_LOADFROMFILE);

	DeleteObject();
	Detach();

	if(hBmpRet)
	{
		Attach((HGDIOBJ)hBmpRet);
		return true;
	}

	BITMAP bmp;
	GetObject(sizeof(bmp), &bmp);

	m_nWidth = bmp.bmWidth;
	m_nHeight = bmp.bmHeight;

	return false;
}

bool CZKRImage::LoadJPGFile(LPCTSTR lpszFile, int iJPegScale)
{
	HBITMAP hBmpRet = NULL;
	void* hBits = NULL;
	BITMAPINFO bmi;
	IJLERR jerr;
	JPEG_CORE_PROPERTIES jcprops;
	int lJPGWidth,
		lJPGHeight,
		nChannels;

	DeleteObject();
	Detach();

	memset(&bmi, NULL, sizeof(bmi));

	jerr = ijlInit(&jcprops);
	if(jerr != IJL_OK)
		return false;

	LPSTR lpzFile = new char[MAX_PATH];
	memset(lpzFile, 0, MAX_PATH);

	//TRACE("tcslen = %d, strlen = %d\n", _tcslen(lpszFile), strlen((LPSTR)lpszFile));

	WideCharToMultiByte(CP_ACP, 0, lpszFile, -1, lpzFile, (int)_tcslen(lpszFile), NULL, NULL);

	//TRACE("lpszFile = %s, lpzFile = %s\n", lpszFile, lpzFile);


	jcprops.JPGFile = (const char*)lpzFile;
	jerr = ijlRead(&jcprops, IJL_JFILE_READPARAMS);
	if(jerr != IJL_OK)
	{
		ijlFree(&jcprops);
		return false;
	}

	m_nWidth = lJPGWidth = jcprops.JPGWidth;
	m_nHeight = lJPGHeight = jcprops.JPGHeight;

	switch((const int)(iJPegScale))
	{
	case ReadWhole: break;
	case ReadHalf:
		lJPGWidth = (lJPGWidth + 1) / 2;
		lJPGHeight = (lJPGHeight + 1) / 2;
		break;
	case ReadQuarter:
		lJPGWidth = (lJPGWidth + 3) / 4;
		lJPGHeight = (lJPGHeight + 3) / 4;
		break;
	case ReadEighth:
		lJPGWidth = (lJPGWidth + 7) / 8;
		lJPGHeight = (lJPGHeight + 7) / 8;
		break;
	}

	if(jcprops.JPGChannels == 1) {
		jcprops.JPGColor = IJL_G;
		jcprops.DIBColor = IJL_BGR;
		nChannels = 3;
	} else if(jcprops.JPGChannels == 3) {
		jcprops.JPGColor = IJL_YCBCR;
		jcprops.DIBColor = IJL_BGR;
		nChannels = 3;
	} else if(jcprops.JPGChannels == 4)	{
		jcprops.JPGColor = IJL_YCBCRA_FPX;
		jcprops.DIBColor = IJL_RGBA_FPX;
		nChannels = 4;
	}

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = lJPGWidth;
	bmi.bmiHeader.biHeight = lJPGHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = nChannels == 3 ? 24 : 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = BytesPerScanLine(lJPGWidth, bmi.bmiHeader.biBitCount) * lJPGHeight;

	hBmpRet = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&hBits, NULL, NULL);

	jcprops.DIBWidth = lJPGWidth;
	jcprops.DIBHeight = -lJPGHeight;
	jcprops.DIBChannels = nChannels;
	jcprops.DIBBytes = (LPBYTE)hBits;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(jcprops.DIBWidth, jcprops.DIBChannels);

	switch((const int)iJPegScale)
	{
	case ReadWhole:
		jerr = ijlRead(&jcprops, IJL_JFILE_READWHOLEIMAGE);
		break;
	case ReadHalf:
		jerr = ijlRead(&jcprops, IJL_JFILE_READONEHALF);
		break;
	case ReadQuarter:
		jerr = ijlRead(&jcprops, IJL_JFILE_READONEQUARTER);
		break;
	case ReadEighth:
		jerr = ijlRead(&jcprops, IJL_JFILE_READONEEIGHTH);
		break;
	}

	ijlFree(&jcprops);

	if(jerr != IJL_OK)
	{
		if(hBmpRet) ::DeleteObject((HGDIOBJ)hBmpRet);
		return false;
	}

	Attach((HGDIOBJ)hBmpRet);
	return true;
}

bool CZKRImage::LoadGIFFile(LPCTSTR lpszFile)
{
	bool bRet = true;
	int n;
	int GraphicExtensionFound = 0;
	int nBitsPerPixel,
		nBytesPerRow;

	LPBYTE hBits = NULL;
	BITMAPINFO *pbmi;
	CFile giffile;

	// Global GIF variables:
	int GlobalBPP;						// Bits per Pixel.
	COLOR * GlobalColorMap;				// Global colormap (allocate)

	// GRAPHIC CONTROL EXTENSION
	struct GIFGCEtag
	{
		unsigned char BlockSize;		// Block Size: 4 bytes
		unsigned char PackedFields;		// Packed Fields. Bits detail:
										//    0: Transparent Color Flag
										//    1: User Input Flag
										//  2-4: Disposal Method
		unsigned char Delay;			// Delay Time (1/100 seconds)
		unsigned char Transparent;		// Transparent Color Index
	} gifgce;

	Detach();
	DeleteObject();

	// OPEN FILE
	if(!giffile.Open(lpszFile, CFile::modeRead|CFile::shareDenyWrite))
		return false;

	// *1* READ HEADER (SIGNATURE + VERSION)
	char szSignature[6];				// First 6 bytes (GIF87a or GIF89a)
	giffile.Read(szSignature, 6);
	if(memcmp(szSignature, "GIF", 3) != 0)
		return false;

	// *2* READ LOGICAL SCREEN DESCRIPTOR
	struct GIFLSDtag
	{
		unsigned short ScreenWidth;		// Logical Screen Width
		unsigned short ScreenHeight;	// Logical Screen Height
		unsigned char PackedFields;		// Packed Fields. Bits detail:
										//  0-2: Size of Global Color Table
										//    3: Sort Flag
										//  4-6: Color Resolution
										//    7: Global Color Table Flag
		unsigned char Background;		// Background Color Index
		unsigned char PixelAspectRatio;	// Pixel Aspect Ratio
	} giflsd;

	giffile.Read(&giflsd, sizeof(giflsd));

	GlobalBPP = (giflsd.PackedFields & 0x07) + 1;

	// fill some animation data:
	m_nWidth = giflsd.ScreenWidth;
	m_nHeight = giflsd.ScreenHeight;

	// *3* READ/GENERATE GLOBAL COLOR MAP

	GlobalColorMap = new COLOR[1 << GlobalBPP];

	if (giflsd.PackedFields & 0x80)	// File has global color map?
		for (n = 0; n < (1 << GlobalBPP); n++)
		{
			giffile.Read(&GlobalColorMap[n].r, 1);
			giffile.Read(&GlobalColorMap[n].g, 1);
			giffile.Read(&GlobalColorMap[n].b, 1);
		}
	else	// GIF standard says to provide an internal default Palette:
		for (n = 0; n < 256; n++)
			GlobalColorMap[n].r = GlobalColorMap[n].g = GlobalColorMap[n].b = n;


	// *4* NOW WE HAVE 3 POSSIBILITIES:
	//  4a) Get and Extension Block (Blocks with additional information)
	//  4b) Get an Image Separator (Introductor to an image)
	//  4c) Get the trailer Char (End of GIF File)

	BYTE dbGet1,
		 dbGet2;

	do
	{
		giffile.Read(&dbGet1, 1);

		if(dbGet1 == 0x21)		// *A* EXTENSION BLOCK 
		{
			giffile.Read(&dbGet2, 1);
			switch (dbGet2)
			{
			case 0xF9:			// Graphic Control Extension
				giffile.Read(&gifgce, sizeof(gifgce));
				GraphicExtensionFound++;
				giffile.Seek(1, CFile::current); // Block Terminator (always 0)
				break;
			case 0xFE:			// Comment Extension: Ignored
			case 0x01:			// PlainText Extension: Ignored
			case 0xFF:			// Application Extension: Ignored
			default:			// Unknown Extension: Ignored
				// read (and ignore) data sub-blocks
				BYTE nBlockLength;

				do
				{
					giffile.Read(&nBlockLength, 1);
					if(!nBlockLength) break;
					giffile.Seek(nBlockLength, CFile::current);
				} while(nBlockLength);
				break;
			}
		}
		else if(dbGet1 == 0x2c) {	// *B* IMAGE (0x2c Image Separator)

			// Read Image Descriptor
			struct GIFIDtag
			{	
				unsigned short xPos;			// Image Left Position
				unsigned short yPos;			// Image Top Position
				unsigned short Width;			// Image Width
				unsigned short Height;			// Image Height
				unsigned char PackedFields;		// Packed Fields. Bits detail:
											//  0-2: Size of Local Color Table
											//  3-4: (Reserved)
											//    5: Sort Flag
											//    6: Interlace Flag
											//    7: Local Color Table Flag
			} gifid;

			giffile.Read(&gifid, sizeof(gifid));

			int LocalColorMap = (gifid.PackedFields & 0x08) ? 1 : 0;

			m_bTransparent = false;
			m_nTransparentIndex = -1;
			m_nWidth = nBytesPerRow = gifid.Width;
			m_nHeight = gifid.Height;
			nBitsPerPixel = LocalColorMap ? (gifid.PackedFields & 7) + 1 : GlobalBPP;

			if(nBitsPerPixel == 24)
			{
				nBytesPerRow *= 3;
				pbmi = (BITMAPINFO*)new char[sizeof(BITMAPINFO)];
			}
			else
			{
				pbmi = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER) + (1 << nBitsPerPixel) * sizeof(COLOR)];
				m_palPalette = (COLOR*)((char*)pbmi + sizeof(BITMAPINFOHEADER));
			}

			nBytesPerRow += (ALIGN - m_nWidth % ALIGN) % ALIGN;	// Align BytesPerRow
			
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = m_nWidth;
			pbmi->bmiHeader.biHeight = -m_nHeight;			// negative means up-to-bottom 
			pbmi->bmiHeader.biPlanes = 1;
			pbmi->bmiHeader.biBitCount = (nBitsPerPixel < 8 ? 8: nBitsPerPixel);	// Our raster is byte-aligned
			pbmi->bmiHeader.biCompression = BI_RGB;
			pbmi->bmiHeader.biSizeImage = nBytesPerRow * m_nHeight;
			pbmi->bmiHeader.biXPelsPerMeter = 11811;
			pbmi->bmiHeader.biYPelsPerMeter = 11811;
			pbmi->bmiHeader.biClrUsed = 0;
			pbmi->bmiHeader.biClrImportant = 0;

			m_nX = m_nY = 0;

			m_nX = gifid.xPos;
			m_nY = gifid.yPos;

			if(GraphicExtensionFound)
			{
				m_bTransparent = (gifgce.PackedFields & 0x1c) > 1 ? true : false;
				m_nTransparentIndex = (gifgce.PackedFields & 0x01) ? gifgce.Transparent : -1;
			}

			if(m_palPalette)
			{
				if(LocalColorMap)		// Read Color Map (if descriptor says so)
					giffile.Read(m_palPalette, sizeof(COLOR) * (1 << nBitsPerPixel));
				else					// Otherwise copy Global
					memcpy(m_palPalette, GlobalColorMap, sizeof(COLOR) * (1 << nBitsPerPixel));
			}

			BYTE dbFirstByte;
			giffile.Read(&dbFirstByte, 1);

			// Calculate compressed image block size
			// to fix: this allocates an extra byte per block
			long ImgStart,
				 ImgEnd;
			
			ImgEnd = ImgStart = (long)giffile.GetPosition();

			BYTE dbBlock;

			do
			{
				giffile.Read(&dbBlock, 1);
				if(!dbBlock) break;
				giffile.Seek(ImgEnd += dbBlock + 1, CFile::begin);
			} while(dbBlock);
			
			giffile.Seek(ImgStart, CFile::begin);

			// Allocate Space for Compressed Image
			char * pCompressedImage = new char [ImgEnd - ImgStart + 4];
  
			// Read and store Compressed Image
			char * pTemp = pCompressedImage;

			BYTE dbBlockLength;

			do
			{
				giffile.Read(&dbBlockLength, 1);
				if(!dbBlockLength) break;

				giffile.Read(pTemp, dbBlockLength);
				pTemp += dbBlockLength;
			} while(dbBlockLength);

			HBITMAP hBmpRet;
			hBmpRet = CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, (void**)&hBits, NULL, NULL);

			// Call LZW/GIF decompressor
			n = LZWDecoder(
				(char*) pCompressedImage,
				(char*) hBits,
				(short)dbFirstByte, nBytesPerRow,//NextImage->AlignedWidth,
				gifid.Width, gifid.Height,
				((gifid.PackedFields & 0x40) ? 1 : 0));	//Interlaced?

			if(n)
			{
				if(hBmpRet)
				{
					HDC hDDC;
					HWND hDWND;
					HDC hMemDC;

					hDWND = ::GetDesktopWindow();
					hDDC = ::GetDC(hDWND);
					hMemDC = ::CreateCompatibleDC(hDDC);
					::ReleaseDC(hDWND, hDDC);

					HBITMAP hBmpOld = (HBITMAP)::SelectObject(hMemDC, (HGDIOBJ)hBmpRet);

					if(SetDIBitsToDevice(hMemDC, m_nX, m_nY, m_nWidth, m_nHeight, 0, 0, 0, m_nHeight, (LPVOID)hBits, pbmi, 0))
						Attach((HGDIOBJ)hBmpRet);
					else
					{
						::DeleteObject(hBmpRet);
						hBmpRet = NULL;
						bRet = false;
					}

					::SelectObject(hMemDC, (HGDIOBJ)hBmpOld);
					::DeleteDC(hMemDC);
				}
			}
			else
				bRet = false;

			// Some cleanup
			delete[] pCompressedImage;
			GraphicExtensionFound = 0;
			break;
		}
		else if(dbGet1 == 0x3b)	// *C* TRAILER: End of GIF Info
			break; // Ok. Standard End.

	} while(giffile.GetPosition() <= giffile.GetLength());

	delete[] GlobalColorMap;
	delete[] pbmi;

	giffile.Close();

	return bRet;
}

int CZKRImage::BytesPerScanLine(int biWidth, int biBitCount)
{
	return ((biWidth * (biBitCount / 8)) + 3) & 0xFFFFFFFC;
}

// ****************************************************************************
// * LZWDecoder (C/C++)                                                       *
// * Codec to perform LZW (GIF Variant) decompression.                        *
// *                         (c) Nov2000, Juan Soulie <jsoulie@cplusplus.com> *
// ****************************************************************************
//
// Parameter description:
//  - bufIn: Input buffer containing a "de-blocked" GIF/LZW compressed image.
//  - bufOut: Output buffer where result will be stored.
//  - InitCodeSize: Initial CodeSize to be Used
//    (GIF files include this as the first byte in a picture block)
//  - AlignedWidth : Width of a row in memory (including alignment if needed)
//  - Width, Height: Physical dimensions of image.
//  - Interlace: 1 for Interlaced GIFs.
//
int CZKRImage::LZWDecoder(char * bufIn, char * bufOut,
						  short InitCodeSize, int AlignedWidth,
						  int Width, int Height, const int Interlace)
{
	int n;
	int row=0,col=0;				// used to point output if Interlaced
	int nPixels, maxPixels;			// Output pixel counter

	short CodeSize;					// Current CodeSize (size in bits of codes)
	short ClearCode;				// Clear code : resets decompressor
	short EndCode;					// End code : marks end of information

	long whichBit;					// Index of next bit in bufIn
	long LongCode;					// Temp. var. from which Code is retrieved
	short Code;						// Code extracted
	short PrevCode;					// Previous Code
	short OutCode;					// Code to output

	// Translation Table:
	short Prefix[4096];				// Prefix: index of another Code
	unsigned char Suffix[4096];		// Suffix: terminating character
	short FirstEntry;				// Index of first free entry in table
	short NextEntry;				// Index of next free entry in table

	unsigned char OutStack[4097];	// Output buffer
	int OutIndex;					// Characters in OutStack

	int RowOffset;					// Offset in output buffer for current row

	// Set up values that depend on InitCodeSize Parameter.
	CodeSize = InitCodeSize+1;
	ClearCode = (1 << InitCodeSize);
	EndCode = ClearCode + 1;
	NextEntry = FirstEntry = ClearCode + 2;

	whichBit=0;
	nPixels = 0;
	maxPixels = Width*Height;
	RowOffset =0;

	while (nPixels<maxPixels) {
		OutIndex = 0;							// Reset Output Stack

		// GET NEXT CODE FROM bufIn:
		// LZW compression uses code items longer than a single byte.
		// For GIF Files, code sizes are variable between 9 and 12 bits 
		// That's why we must read data (Code) this way:
		LongCode=*((long*)(bufIn+whichBit/8));	// Get some bytes from bufIn
		LongCode>>=(whichBit&7);				// Discard too low bits
		Code =(short)(LongCode & ((1<<CodeSize)-1) );	// Discard too high bits
		whichBit += CodeSize;					// Increase Bit Offset

		// SWITCH, DIFFERENT POSIBILITIES FOR CODE:
		if (Code == EndCode)					// END CODE
			break;								// Exit LZW Decompression loop

		if (Code == ClearCode) {				// CLEAR CODE:
			CodeSize = InitCodeSize+1;			// Reset CodeSize
			NextEntry = FirstEntry;				// Reset Translation Table
			PrevCode=Code;				// Prevent next to be added to table.
			continue;							// restart, to get another code
		}
		if (Code < NextEntry)					// CODE IS IN TABLE
			OutCode = Code;						// Set code to output.

		else {									// CODE IS NOT IN TABLE:
			OutIndex++;			// Keep "first" character of previous output.
			OutCode = PrevCode;					// Set PrevCode to be output
		}

		// EXPAND OutCode IN OutStack
		// - Elements up to FirstEntry are Raw-Codes and are not expanded
		// - Table Prefices contain indexes to other codes
		// - Table Suffices contain the raw codes to be output
		while (OutCode >= FirstEntry) {
			if (OutIndex > 4096) return 0;
			OutStack[OutIndex++] = Suffix[OutCode];	// Add suffix to Output Stack
			OutCode = Prefix[OutCode];				// Loop with preffix
		}

		// NOW OutCode IS A RAW CODE, ADD IT TO OUTPUT STACK.
		if (OutIndex > 4096) return 0;
		OutStack[OutIndex++] = (unsigned char) OutCode;

		// ADD NEW ENTRY TO TABLE (PrevCode + OutCode)
		// (EXCEPT IF PREVIOUS CODE WAS A CLEARCODE)
		if (PrevCode!=ClearCode) {
			Prefix[NextEntry] = PrevCode;
			Suffix[NextEntry] = (unsigned char) OutCode;
			NextEntry++;

			// Prevent Translation table overflow:
			if (NextEntry>=4096) return 0;
      
			// INCREASE CodeSize IF NextEntry IS INVALID WITH CURRENT CodeSize
			if (NextEntry >= (1<<CodeSize)) {
				if (CodeSize < 12) CodeSize++;
				else {}				// Do nothing. Maybe next is Clear Code.
			}
		}

		PrevCode = Code;

		// Avoid the possibility of overflow on 'bufOut'.
		if (nPixels + OutIndex > maxPixels) OutIndex = maxPixels-nPixels;

		// OUTPUT OutStack (LAST-IN FIRST-OUT ORDER)
		for (n=OutIndex-1; n>=0; n--) {
			if (col==Width)						// Check if new row.
			{
				if (Interlace) {				// If interlaced::
					     if ((row&7)==0) {row+=8; if (row>=Height) row=4;}
					else if ((row&3)==0) {row+=8; if (row>=Height) row=2;}
					else if ((row&1)==0) {row+=4; if (row>=Height) row=1;}
					else row+=2;
				}
				else							// If not interlaced:
					row++;

				RowOffset=row*AlignedWidth;		// Set new row offset
				col=0;
			}
			bufOut[RowOffset+col]=OutStack[n];	// Write output
			col++;	nPixels++;					// Increase counters.
		}

	}	// while (main decompressor loop)

	return whichBit;
}
