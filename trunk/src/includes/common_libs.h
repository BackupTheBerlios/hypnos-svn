/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file common_libs.h
\brief Common library header

This is a small abstraction layer for threading and STL including
with the explicit purpose of tearing away some platform dependant
warnings/errors/issues.
*/

#ifndef __COMMON_LIBS_H__
#define __COMMON_LIBS_H__

// First of all, include the config.h and then libhypnos commons
#include "libhypnos/commons.h"

#if defined(__GNUC__) && ( __GNUC__ < 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ < 1 ) )
	#error You need at least GCC 3.2 to compile this!
#endif

#ifdef __BORLANDC__
	#define NDEBUG
	#define WIN32
	#define _CONSOLE
	#include <stlport/hash_map>

	#define strncasecmp strncmpi
	#define strcasecmp strcmpi
#endif

#if defined WIN32
#endif

#ifdef __APPLE__
	#define __unix__
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include <ctype.h>
#include <unistd.h>

// Only the one which is used it's actually included, the others will be
// ignored.
#include "archs/hypunix.h"
#include "archs/hypwin32.h"

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#include "typedefs.h"
#include "constants.h"

#define TIMEOUT(X) ((X) <= getclock())

extern std::string getOSVersionString();
enum OSVersion { OSVER_UNKNOWN, OSVER_WIN9X, OSVER_WINNT, OSVER_NONWINDOWS };
extern OSVersion getOSVersion();

//@{
/*!
\name System dependent functions

Functions defined into archs/hypunix and archs/hypwin32
*/

/*!
\brief Manages a socket error
\param bcode Return value of bind() call

\note This is an OS-dependant function, because WinSock version diregard
	\c bcode parameter and get a new one from WinSock system.
*/
void sockManageError(int bcode);
//@}

using namespace nLibhypnos;

#endif //__COMMON_LIBS_H__
