#ifndef _OBJECT_CREATELANDSCAPE_H_
#define _OBJECT_CREATELANDSCAPE_H_

TLandscape _EXPORT *CreateLandscapeObject( const TDefFile& def, TString& errStr, int );

TLandscape* CreateLandscapeObject( const TDefFile& def, TString& errStr );
TBinObject* CreateBinaryMesh( TString fileName, 
							  TString strip,
							  TString material,
							  size_t seaHeight, 
							  TString& errStr );

#endif
