/* 
	Htc Ime definitions.
*/
#pragma once
#ifndef HTC_IME_H
#define HTC_IME_H

typedef HRESULT (__stdcall *ENUMREGISTERWORD)(LPWSTR szReading,
    DWORD dwStyle,
    LPWSTR szRegister,
    LPVOID pData,
    LPVOID test
);

typedef HRESULT (__stdcall *DESTROY)(UINT uReserved);

typedef HRESULT (__stdcall *CONVERSIONLIST)(HIMC hIMC,
    LPWSTR szSource,
    UINT uFlag,
    UINT uBufLen,
    CANDIDATELIST *pDest,
    UINT *puCopied
);
typedef HRESULT (__stdcall *INQUIRE)(DWORD dwSystemInfoFlags,
    IMEINFO *pIMEInfo,
    LPWSTR szWndClass,
    DWORD *pdwPrivate
);
typedef HRESULT (__stdcall *SETCOMPOSITIONSTRING)(          HIMC hIMC,
    DWORD dwIndex,
    void *pComp,
    DWORD dwCompLen,
    void *pRead,
    DWORD dwReadLen
);

typedef int (__stdcall *UI_WNDPROC)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef char* (__stdcall *GETVERSION)();

typedef HRESULT (__stdcall * CONFIGURE)(HKL hKL,
    HWND hWnd,
    DWORD dwMode,
    REGISTERWORDW *pRegisterWord);

typedef HRESULT (__stdcall * ESCAPE)(HIMC hIMC,
    UINT uEscape,
    void *pData,
    LRESULT *plResult
);


typedef DWORD (__stdcall *GETIMEMENUITEMS)(HIMC hIMC, 
										   DWORD dwFlags, 
										   DWORD dwType, 
										   LPIMEMENUITEMINFO lpParentMenu, 
										   LPIMEMENUITEMINFO lpMenu, 
										   DWORD dwSize);

typedef HRESULT (__stdcall * GETREGISTERWORDSTYLE)(UINT nItem,
    STYLEBUFW *pStyleBuf,
    UINT *puBufSize
);

typedef HRESULT (__stdcall *PROCESSKEY)(HIMC hIMC,
    UINT uVirKey,
    DWORD lParam,
    BYTE *pbKeyState
);

typedef HRESULT (__stdcall * __REGISTERWORD)(LPWSTR szReading,
    DWORD dwStyle,
    LPWSTR szString
);

typedef HRESULT (__stdcall * SELECT)(HIMC hIMC,
    BOOL fSelect
);


typedef HRESULT (__stdcall * SETACTIVECONTEXT)(HIMC hIMC,
    BOOL fFlag);

typedef HRESULT (__stdcall * TOASCIIEX)(UINT uVirKey,
    UINT uScanCode,
    BYTE *pbKeyState,
    UINT fuState,
    HIMC hIMC,
    DWORD *pdwTransBuf,
    UINT *puSize
);

typedef HRESULT (__stdcall * UNREGISTERWORD)(LPWSTR szReading,
    DWORD dwStyle,
    LPWSTR szString
);

typedef HRESULT (__stdcall * NOTIFYIME)(HIMC hIMC,
    DWORD dwAction,
    DWORD dwIndex,
    DWORD dwValue
);
#endif
