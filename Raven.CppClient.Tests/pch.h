//
// pch.h
// Header for standard system include files.
//

#pragma once

#ifndef GTEST_HAS_STD_TUPLE_
#define GTEST_HAS_STD_TUPLE_ 1
#endif

#ifndef GTEST_HAS_TR1_TUPLE
#define GTEST_HAS_TR1_TUPLE  0
#endif

#include "gtest/gtest.h"


#ifdef _DEBUG
	#pragma comment(lib, "libcurl-d.lib")
	#pragma comment(lib, "zlibd.lib")
#else
	#pragma comment(lib, "libcurl.lib")
	#pragma comment(lib, "zlib.lib")
#endif
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Rpcrt4.lib")

