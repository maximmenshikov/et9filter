/* 
	ET9Filter
	IME that filters pressed keys
	Main idea: make EzInput work better on XPERIA X1 devices

	If you want to compile this project, you might (or mightn't) want to comment out

	// protype of IME APIs
	BOOL    WINAPI ImeInquire(LPIMEINFO, LPWSTR lpszUIClass, DWORD dwSystemInfoFlags);
	BOOL    WINAPI ImeConfigure(HKL, HWND, DWORD, LPVOID);
	DWORD   WINAPI ImeConversionListW(HIMC, LPCWSTR, LPCANDIDATELIST, DWORD dwBufLen, UINT uFlag);
	BOOL    WINAPI ImeDestroy(UINT);
	LRESULT WINAPI ImeEscape(HIMC, UINT, LPVOID);
	BOOL    WINAPI ImeProcessKey(HIMC, UINT, LPARAM, CONST LPBYTE);
	BOOL    WINAPI ImeSelect(HIMC, BOOL);
	BOOL    WINAPI ImeSetActiveContext(HIMC, BOOL);
	UINT    WINAPI ImeToAsciiEx(UINT uVirtKey, UINT uScaCode, CONST LPBYTE lpbKeyState, LPDWORD lpdwTransBuf, UINT fuState, HIMC);
	BOOL    WINAPI NotifyIME(HIMC, DWORD, DWORD, DWORD);
	BOOL    WINAPI ImeRegisterWord(LPCWSTR, DWORD, LPCWSTR);
	BOOL    WINAPI ImeUnregisterWord(LPCWSTR, DWORD, LPCWSTR);
	UINT	WINAPI ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUF);
	UINT    WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROC, LPCWSTR, DWORD, LPCWSTR, LPVOID);
	BOOL    WINAPI ImeSetCompositionString(HIMC, DWORD dwIndex, LPCVOID lpComp, DWORD, LPCVOID lpRead, DWORD);
	DWORD	WINAPI ImeGetImeMenuItems(HIMC hIMC, DWORD dwFlags, DWORD dwType, LPIMEMENUITEMINFO lpParentMenu, LPIMEMENUITEMINFO lpMenu, DWORD dwSize);

	in imm.h
*/
#include "stdafx.h"
#include "et9filter.h"
#include "regext.h"
#include "htc_ime.h"
#include "logger.h"

const int virKeyMask=0xFFFF;

bool isLoaded = false;
DWORD dwEnableNamePrediction = 0;
DWORD currentEt9Style = 0;
HREGNOTIFY hNotify = NULL;

PROCESSKEY HTC_ImeProcessKey;
TOASCIIEX HTC_ImeToAsciiEx;

#define VK_DEADKEY 0x7A
#define DEFINE_DUMMY(a) void a() { } 

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

void Notify(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
	RegistryGetDWORD(HKEY_LOCAL_MACHINE, L"Software\\Tegic\\eT9\\Ime",
										 L"CurrentET9Style",
										 &currentEt9Style);
}

inline void LoadHTCFunctions()
{
	if (isLoaded == false)
	{
		HMODULE library = LoadLibrary(L"et9ime.dll");
		if (library)
		{
			HTC_ImeProcessKey = (PROCESSKEY)GetProcAddress(library,L"ImeProcessKey");
			HTC_ImeToAsciiEx = (TOASCIIEX)GetProcAddress(library,L"ImeToAsciiEx");

			RegistryGetDWORD(HKEY_LOCAL_MACHINE,L"Software\\ultrashot\\eT9filter",L"ContactsPrediction",&dwEnableNamePrediction);

			RegistryNotifyCallback(HKEY_LOCAL_MACHINE,
				L"Software\\Tegic\\eT9\\Ime",
				L"CurrentET9Style",
				Notify,
				0,
				0,
				&hNotify);
		}
		isLoaded = true;
	}
}

inline PROCESSRESULT shouldBeProcessed(int key)
{
	if (key == VK_DEADKEY)
		return PROCESSRESULT_SEND_TO_IME;

	if ((currentEt9Style == 8 || currentEt9Style == 11) && dwEnableNamePrediction == 1)
	{
		if ((key >= 0x21 && key <= 0x40) || (key >= VK_SEMICOLON && key <= VK_BACKQUOTE) || (key >= VK_LBRACKET && key <= VK_OFF) || (key >= VK_NUMPAD0 && key <= VK_F24))
		{
			SIPINFO sipInfo;
			sipInfo.cbSize = sizeof(SIPINFO);
			if (SipGetInfo(&sipInfo) == TRUE)
			{
				int flags = sipInfo.fdwFlags;
				logger("isTextingKey(%d): SipGetInfo returned: %d", key, flags & SIPF_ON); 
				if (flags & SIPF_ON)
					return PROCESSRESULT_SEND_TO_IME;
				return PROCESSRESULT_DONT_PROCESS;
			}
		}
		return PROCESSRESULT_SEND_TO_IME;
	}	
	//if pressed key is a default hardware key (like CAMERA key):
	if	(! ((key < VK_LWIN) || 
		(key>=VK_SEMICOLON && key<=VK_BACKQUOTE) || 
		(key>=VK_LBRACKET && key<=VK_OFF) || 
		(key>=VK_NUMPAD0 && key<=VK_F24)
		)
		)
	{
		logger("isTextingKey(%d): NOT texting, sending to default IME", key); 

		return PROCESSRESULT_DONT_PROCESS; //key won't be processed
	}
	else
	{
		//it is possible that pressed key belongs to QWERTY keyboard or to EzInput, look at sip status
		SIPINFO sipInfo;
		sipInfo.cbSize = sizeof(SIPINFO);
		if (SipGetInfo(&sipInfo) == TRUE)
		{
			int flags = sipInfo.fdwFlags;
			logger("isTextingKey(%d): SipGetInfo returned: %d", key, flags & SIPF_ON); 
			if (flags & SIPF_ON)
				return PROCESSRESULT_SEND_TO_IME; //will be sent to IME
			return PROCESSRESULT_DONT_PROCESS; //won't be processed
		}
		else
		{
			logger("isTextingKey(%d): SipGetInfoFailed", key); 
		}
	}
	return PROCESSRESULT_DONT_PROCESS;
}

HRESULT ImeProcessKey(HIMC hIMC, UINT uVirKey, DWORD lParam, BYTE *pbKeyState)
{
#ifdef DEBUG
	int current = GetTickCount();
#endif
	
	logger("ImeProcessKey(hIMC,%X,%X,%X)", uVirKey, lParam, pbKeyState);
	
	int key = uVirKey & virKeyMask;

	if (shouldBeProcessed(key) == PROCESSRESULT_SEND_TO_IME)
	{
		LoadHTCFunctions();
		logger("ImeProcessKey -> HTC_IME (took %d to process)", GetTickCount()-current);
		return HTC_ImeProcessKey(hIMC,uVirKey,lParam,pbKeyState);
	}

	return S_OK;
}


HRESULT ImeToAsciiEx(UINT uVirKey, UINT uScanCode, BYTE *pbKeyState,
					UINT fuState, HIMC hIMC, DWORD *pdwTransBuf, UINT *puSize)
{
#ifdef DEBUG
	int current = GetTickCount();
#endif
	
	int key = uVirKey & virKeyMask;

	if (shouldBeProcessed(key) == PROCESSRESULT_SEND_TO_IME)
	{
		LoadHTCFunctions();

		logger("ImeToAsciiEx -> HTC_IME (took %d to process)", GetTickCount() - current);

		return HTC_ImeToAsciiEx(uVirKey, uScanCode, pbKeyState, fuState, hIMC, pdwTransBuf, puSize);
	}

	return S_OK;
}

DEFINE_DUMMY(ImeEnumRegisterWord)
DEFINE_DUMMY(ImeDestroy)
DEFINE_DUMMY(ImeConversionList)
DEFINE_DUMMY(ImeInquire)
DEFINE_DUMMY(ImeSetCompositionString)
DEFINE_DUMMY(ImeUi_WndProc)
DEFINE_DUMMY(GetVersion)
DEFINE_DUMMY(ImeConfigure)
DEFINE_DUMMY(ImeEscape)
DEFINE_DUMMY(ImeGetImeMenuItems)
DEFINE_DUMMY(ImeGetRegisterWordStyle)
DEFINE_DUMMY(ImeRegisterWord)
DEFINE_DUMMY(ImeSelect)
DEFINE_DUMMY(ImeSetActiveContext)
DEFINE_DUMMY(ImeUnregisterWord)
DEFINE_DUMMY(NotifyIME)
