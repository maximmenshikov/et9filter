//Logging ev

#include "stdafx.h"
#include "logger.h"

#ifdef DEBUG
// Helper
char *append_char ( const char *s, const char c , BOOL appendNewLine)
{
  size_t len = strlen ( s );

  char *ret;
  if (appendNewLine)
      ret = new char[len + 2];
  else
      ret = new char[len + 1];

  strcpy ( ret, s );
  ret[len] = 0xD;

  if (appendNewLine)
    ret[len + 1] = 0xA;

  return ret;
}

void logger(const char *format, ...)
{

    // Open file
    FILE *handle;
    handle = fopen("\\et9filter.txt", "ab");

    // Append newline
    char* newformat = append_char(format, 0xD, true);
    // Print to console
    va_list ap;
    va_start (ap, format);
    vprintf(newformat, ap);
    va_end (ap);

    // Call vfprintf to write with formatting
    va_start (ap, format);
    vfprintf(handle, newformat, ap);
    va_end (ap);
	delete(newformat);
    // Close up
    fclose(handle);
	
}
#endif