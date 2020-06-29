#include <afxwin.h>

#ifndef CPWIO_H
#define CPWIO_H

class CPWIO
{
public:
	CPWIO(void);
	~CPWIO(void);
	static CString GetModuleFilePath(HMODULE hModule = NULL);
	static CString GetModulePathSlashed(HMODULE hModule = NULL);
	static CString GetModuleFile(HMODULE hModule = NULL);
	static void ReplaceExtension(CString& strFilePath, LPCTSTR lpszNewExt);
	static BOOL FileExist(LPCTSTR lpszFile);
	static CString GetToken(LPCTSTR lpszString, int nOneBaseIndex, TCHAR tcTokenChar);
	static BOOL CreateShellLink(LPCTSTR pszShortcutFile, LPCTSTR pszLink, LPCTSTR pszParams, LPCTSTR pszDesc, LPCTSTR pszIconLocation = NULL, int iIcon = 0);
	static LPCTSTR ParseDirs(LPTSTR pszOut, LPCTSTR lpszDirGet, LPCTSTR lpszFileGet, LPCTSTR lpszStrAdd = NULL, LPCTSTR lpszExtAdd = NULL);
	static LPTSTR SlashDir(LPTSTR lpszDir);
	static BOOL DirExists(LPCTSTR lpszDir);
	static void GoToURL(HWND hWnd, LPCTSTR lpszURL, INT nShowCmd);
	static LPBYTE BinSearch(LPBYTE lpDataBaseAddr, LPBYTE lpDataSearchFrom, DWORD dwDataLength, LPCSTR lpSearchFor);
	static const int osWin95,
					 osWin98,
					 osWinME,
					 osWinNT351,
					 osWinNT40,
					 osWin2K,
					 osWinXP,
					 osWin2K3,
					 osWinVst;
	static int GetOS(void);
	static CString GetInBrackets(LPCTSTR lpszString, LPCTSTR lpszBrackets);
	static CString GetEqualNameValue(LPCTSTR lpszLine, bool bGetValue = FALSE, TCHAR chrEqualSing = _T('='));
	static void SlashDir(CString& strDir);
	static CString GetTempFile(void);
    static int wildcmp(char *wild, char *string);
	static int CountFiles(LPCTSTR lpszDir, LPCTSTR lpszMask = _T("*.*"), bool bRecurseSubfolders = FALSE);
};

#endif
