#ifndef _PARSER_IMP3DS_H_
#define _PARSER_IMP3DS_H_

//==========================================================================

#include <lib3ds/file.h>
#include <object/binobject.h>

TBinObject* ToBin( const TString& orgFile, Lib3dsFile* obj3ds );

//==========================================================================

#endif
