#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#ifdef _MESA
#include <MesaGL/gl.h>
#include <MesaGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <common/compatability.h>
#include <common/tstring.h>
#pragma hdrstop
