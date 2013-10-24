/* 
	ET9Filter header file
*/
#pragma once
#ifndef ET9FILTER_H
#define ET9FILTER_H

#ifdef ET9FILTER_EXPORTS
#define ET9FILTER_API __declspec(dllexport)
#else
#define ET9FILTER_API __declspec(dllimport)
#endif


#define VK_DEADKEY 0x7A

enum PROCESSRESULT
{
	PROCESSRESULT_DONT_PROCESS = 0,
	PROCESSRESULT_SEND_TO_IME = 1
};

#endif
