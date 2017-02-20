#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#define _EXPORT __declspec( dllexport )
#define _EXPORT

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef _MESA
#include <gm/gm.h>
#include <mesagl/gl.h>
#include <GL/glu.h>
#include <ztimer.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <common/compatability.h>
#include <common/string.h>
#include <common/font.h>
#include <common/languageStrings.h>

#pragma hdrstop
