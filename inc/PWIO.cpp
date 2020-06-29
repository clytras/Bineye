#include <afxwin.h>
#include <shlobj.h>
#include <objbase.h>
#include ".\pwio.h"


const int CPWIO::osWin95(1400);
const int CPWIO::osWin98(1410);
const int CPWIO::osWinME(1490);
const int CPWIO::osWinNT351(1499);
const int CPWIO::osWinNT40(2400);
const int CPWIO::osWin2K(2500);
const int CPWIO::osWinXP(2510);
const int CPWIO::osWin2K3(2520);
const int CPWIO::osWinVst(0000);


CPWIO::CPWIO(void)
{
}

CPWIO::~CPWIO(void)
{
}


CString CPWIO::GetModuleFilePath(HMODULE hModule)
{
	CString strRet;
	TCHAR szBuffer[MAX_PATH];

	strRet.Empty();

	if(hModule == NULL) hModule = AfxGetApp()->m_hInstance;
	DWORD dwRes = GetModuleFileName(hModule, szBuffer, sizeof(szBuffer));

	if(dwRes) strRet = szBuffer;

	return strRet;
}

CString CPWIO::GetModulePathSlashed(HMODULE hModule)
{
	CString strRet;

	strRet = GetModuleFilePath();

	if(!strRet.IsEmpty())
		strRet = strRet.Left(strRet.ReverseFind('\\') + 1);

	return strRet;
}

CString CPWIO::GetModuleFile(HMODULE hModule)
{
	CString strRet;

	strRet = GetModuleFilePath();

	if(!strRet.IsEmpty())
		strRet = strRet.Right(strRet.GetLength() - strRet.ReverseFind('\\') - 1);

	return strRet;
}

void CPWIO::ReplaceExtension(CString& strFilePath, LPCTSTR lpszNewExt)
{
	int nSlashIndex = strFilePath.ReverseFind('\\'),
		nDotIndex = strFilePath.ReverseFind('.');

	if(nDotIndex > nSlashIndex)
		strFilePath = strFilePath.Left(nDotIndex + 1) + lpszNewExt;
}

BOOL CPWIO::FileExist(LPCTSTR lpszFile)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFindFile;
	BOOL bRet = FALSE;

	hFindFile = FindFirstFile(lpszFile, &wfd);
	bRet = hFindFile != INVALID_HANDLE_VALUE;
	if(bRet) FindClose(hFindFile);

	return bRet;
}

CString CPWIO::GetToken(LPCTSTR lpszString, int nOneBaseIndex, TCHAR tcTokenChar)
{
	int nCurToken = 1;
	CString strRet;
	TCHAR c;
	int nStrLen = (int)_tcslen(lpszString);

	for(int i=0; i<nStrLen; i++)
	{
		c = (TCHAR)(LPTSTR)lpszString[i];

		if(c == tcTokenChar || i == nStrLen - 1)
		{
			if(c != tcTokenChar) strRet += c;

			if(nCurToken == nOneBaseIndex)
				break;
			else
			{
				nCurToken++;
				strRet.Empty();
			}
		}
		else
			strRet += c;
	}
	
	return strRet;
}

BOOL CPWIO::CreateShellLink(LPCTSTR pszShortcutFile, LPCTSTR pszLink, LPCTSTR pszParams, LPCTSTR pszDesc, LPCTSTR pszIconLocation /*= NULL*/, int iIcon /*= 0*/)
{
	HRESULT hres;
	IShellLink* psl;
	CString strFmt;

	// Get a pointer to the IShellLink interface.
	CoInitialize(NULL);
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);

	if(SUCCEEDED(hres))
	{
		IPersistFile* ppf;

		// Query IShellLink for the IPersistFile interface for 
		// saving the shell link in persistent storage.
		hres = psl->QueryInterface(IID_IPersistFile,(void**)&ppf);


		if(SUCCEEDED(hres))
		{   
			WORD wsz[MAX_PATH];

			// Set the path to the shell link target.
			hres = psl->SetPath(pszShortcutFile);

			if(!SUCCEEDED(hres)) return false;

			hres = psl->SetDescription(pszDesc);
			hres = psl->SetArguments(pszParams);
			if(pszIconLocation) hres = psl->SetIconLocation(pszIconLocation, iIcon);

			strFmt = pszShortcutFile;
			strFmt = strFmt.Left(strFmt.ReverseFind(_T('\\')));

			hres = psl->SetRelativePath(strFmt, 0);

			memset(&wsz, NULL, sizeof(wsz));

			// Ensure string is ANSI.
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszLink, (int)_tcslen(pszLink), (LPWSTR)wsz, MAX_PATH);

			// Save the link via the IPersistFile::Save method.
			hres = ppf->Save((LPCOLESTR)&wsz, TRUE);

			TRACE("ppf = %d\n", ppf->Save((LPCOLESTR)pszLink, TRUE));

			// Release pointer to IPersistFile.
			ppf->Release();
		}

		// Release pointer to IShellLink.
		psl->Release();
	}

	return hres==S_OK;
}

LPCTSTR CPWIO::ParseDirs(LPTSTR pszOut, LPCTSTR lpszDirGet, LPCTSTR lpszFileGet, LPCTSTR lpszStrAdd /*= NULL*/, LPCTSTR lpszExtAdd /*= NULL*/)
{
	_tcscpy(pszOut, lpszDirGet);
	SlashDir(pszOut);
	if(lpszStrAdd) _tcscat(pszOut, lpszStrAdd);
	SlashDir(pszOut);
	_tcscat(pszOut, lpszFileGet);
	if(lpszExtAdd) _tcscat(pszOut, lpszExtAdd);

	return pszOut;
}

LPTSTR CPWIO::SlashDir(LPTSTR lpszDir)
{
	if(((TCHAR)(LPTSTR)lpszDir[_tcslen(lpszDir)-1]) != _T('\\'))
		_tcscat(lpszDir, _T("\\"));

	return lpszDir;
}

BOOL CPWIO::DirExists(LPCTSTR lpszDir)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFindFile;
	BOOL bRet = FALSE;
	CString str(lpszDir);

	str.TrimRight(_T('\\'));

	hFindFile = FindFirstFile(str, &wfd);
	bRet = (hFindFile != INVALID_HANDLE_VALUE) && ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	if(bRet) FindClose(hFindFile);

	return bRet;
}

void CPWIO::GoToURL(HWND hWnd, LPCTSTR lpszURL, INT nShowCmd)
{
	ShellExecute(hWnd, NULL, lpszURL, NULL, _T("\\"), nShowCmd);
}

LPBYTE CPWIO::BinSearch(LPBYTE lpDataBaseAddr, LPBYTE lpDataSearchFrom, DWORD dwDataLength, LPCSTR lpSearchFor)
{
	unsigned int i;
	bool res = false;
	size_t dwStrForLen = strlen(lpSearchFor);
	LPBYTE save = NULL;


	while(lpDataSearchFrom < (lpDataBaseAddr + dwDataLength)) {

		if((LPBYTE)*lpDataSearchFrom == (LPBYTE)*lpSearchFor)
		{
			save = lpDataSearchFrom;
			res = true;

			for(i = 1; i < dwStrForLen; i++)
			{
				if((LPBYTE)*(lpDataSearchFrom + i) != (LPBYTE)*(lpSearchFor + i))
				{
					res = false;
					break;
				}
			}

			if(res) break;
		}

		lpDataSearchFrom++;
	}

	return (LPBYTE)res ? save : NULL;
}

int CPWIO::GetOS(void)
{
	OSVERSIONINFOEX osinfo;
	int iRet = 0;

	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((LPOSVERSIONINFO)&osinfo);

	if(osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if(osinfo.dwMinorVersion == 0)
			iRet = osWin95;
		else if(osinfo.dwMinorVersion == 10)
			iRet = osWin98;
		else if(osinfo.dwMinorVersion == 90)
			iRet = osWinME;
	}
	else if(osinfo.dwPlatformId == 2)
	{
		if(osinfo.dwMajorVersion == 3)
			iRet = osWinNT351;
		else if(osinfo.dwMajorVersion == 4)
			iRet = osWinNT40;
		else if(osinfo.dwMajorVersion == 5)
		{
			if(osinfo.dwMinorVersion == 0)
				iRet = osWin2K;
			else
				iRet = osWinXP;
		}
	}

	return iRet;
}

CString CPWIO::GetInBrackets(LPCTSTR lpszString, LPCTSTR lpszBrackets)
{
	CString strRet;
	TCHAR szBuf[1024] = {0};
	int nStrLen = (int)_tcslen(lpszString),
		nStart = 0, nEnd = 0;

	if(_tcslen(lpszBrackets) != 2)
		return(strRet);


	for(int i=0; i<nStrLen; i++)
	{
		if((TCHAR)lpszString[i] == (TCHAR)lpszBrackets[0])
			nStart = i+1;
		else if((TCHAR)lpszString[i] == (TCHAR)lpszBrackets[1])
		{
			nEnd = i;
			break;
		}
	}

	if(nEnd > nStart)
		strRet = _tcsncpy(&szBuf[0], &lpszString[nStart], nEnd - nStart);

	return strRet;
}

CString CPWIO::GetEqualNameValue(LPCTSTR lpszLine, bool bGetValue /*= FALSE*/, TCHAR chrEqualSing /*= _T('=')*/)
{
	CString strRet;
	TCHAR szBuf[1024] = {0};
	int nStrLen = (int)_tcslen(lpszLine),
		nStart = 0, nEnd = 0;


	if(nStrLen > 0)
	{
		for(int i=0; i<nStrLen; i++)
		{
			if((TCHAR)lpszLine[i] == chrEqualSing)
			{
				if(bGetValue)
					strRet = _tcsncpy(&szBuf[0], &lpszLine[i+1], nStrLen-i);
				else
					strRet = _tcsncpy(&szBuf[0], lpszLine, i);

				break;
			}

		}
	}

	return strRet;
}


void CPWIO::SlashDir(CString& strDir)
{
	if(strDir[strDir.GetLength() - 1] != _T('\\'))
		strDir += _T("\\");
}

CString CPWIO::GetTempFile(void)
{

	TCHAR szTempDir[MAX_PATH], szTempFile[MAX_PATH];
	CString strTempFile, strTempDir;

	if(GetTempPath(MAX_PATH, (LPTSTR)&szTempDir))
	{
		if(GetTempFileName((LPCTSTR)&szTempDir, _T("zkttmp~fl"), 103, (LPTSTR)&szTempFile)) 
		{
			strTempFile = szTempFile;
		}
	}

	return strTempFile;
}


int CPWIO::wildcmp(char *wild, char *string)
{
	char *cp, *mp;

    while((*string) && (*wild != '*')) 
	{
        if ((*wild != *string) && (*wild != '?')) return 0;
		wild++;
		string++;
	}

    while(*string)
	{
		if (*wild == '*')
		{
			if (!*++wild) return 1;
			mp = wild;
			cp = string+1;
		}
		else if((*wild == *string) || (*wild == '?'))
		{
			wild++;
			string++;
		}
		else
		{
			wild = mp;
			string = cp++;
		}
	}

    while (*wild == '*') wild++;

	return !*wild;
}

int CPWIO::CountFiles(LPCTSTR lpszDir, LPCTSTR lpszMask /*= _T("*.*")*/, bool bRecurseSubfolders /*= FALSE*/)
{
	int iRes = 0;
	CString strSearchExp(lpszDir), strMask(lpszMask);
	CFileFind finder;

	SlashDir(strSearchExp);
	strSearchExp += _T("*");
	strMask.MakeLower();

	BOOL bWorking = finder.FindFile(strSearchExp);

	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		if(finder.IsDots())
			continue;

		if(finder.IsDirectory() && bRecurseSubfolders)
		{
			iRes += CountFiles(finder.GetFilePath(), lpszMask, TRUE);
		}
		else if(wildcmp((char*)(LPCTSTR)strMask, (char*)(LPCTSTR)finder.GetFileName().MakeLower()) == 1)
		{
			iRes++;
		}
	}

	return iRes;
}
