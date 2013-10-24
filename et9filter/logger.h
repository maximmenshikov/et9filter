

#pragma once

//#define DEBUG

#ifdef DEBUG
void logger(const char *format, ...);
#else
#define logger(...)
#endif
