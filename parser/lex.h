#ifndef __PROLOG_LEX_H_
#define __PROLOG_LEX_H_

///==========================================================================///

#include <common/string.h>

class TMaterial;
class TMesh;
class TBinObject;
class TMapPlayerInfo;
class TMapHeightInfo;
class TPersist;

///==========================================================================///

// import openGL source code - and using a reference object -
// reconstruct what it must have been before
bool ImportOpenGL( const TString& fname, TBinObject& obj, 
				   TBinObject* compareObj, TString& errStr );

///==========================================================================///
// lexical class
class Lex
{
public:
	Lex( void );
    ~Lex( void );

	TBinObject*	DirectX( TString fname ); // load directX file
	TMapPlayerInfo* ParseDefn( TString fname, TString& errStr ); // parse landscape .def file
	TMapHeightInfo* ParseProcedural( TString fname, TString& errStr ); // parse procedural file

    void 		SetString( const char* parseString, long fsize ); // set the string to be parsed
	bool 		Error( void );				// has there been an error?
    const char* ErrorString( void ) const; 	// get error string

    // statics
	static char*		Trim( char* str );
	static const char* 	ToString( long token ); // token back to string

	bool		CreateMesh( TString fname, TString strip,
							size_t seaHeight, TString& errStr );

private:
	// token and parse help
	long 	GetNextToken( void );	// parse next token
	long 	GetNextToken( long& _prevToken );	// parse next token
	void 	UngetToken( void );		// rewind 1 token (can only be used once!)
	bool 	WhiteSpace( char ch );	// is a character a whitespace char?
	bool 	Error( long token );		// is a token an error token?
	bool 	CheckToken( long token1, long token2 ); // make sure two tokens match

	// parser
	bool				Parse( TBinObject* obj, const char* texturePath ); // main parser entry
	TMaterial* 			ParseMaterial( const char* texturePath ); // parse material defn.
	TMesh*				ParseMesh( TBinObject* obj, const char* texturePath ); // parse mesh defn.

	// parser help routines
	bool 	Float( void );
	bool 	Int( void );
	bool 	Sep( void );
	bool 	TextureOpt( TMaterial* mat, const char* texturePath );

	void 	MeshMaterialOpt( TMesh* mesh, TBinObject* obj, const char* texturePath );
	void 	MeshNormalOpt( TMesh* mesh );
	void 	MeshTextureOpt( TMesh* mesh, const char* texturePath );
	void	ParseMatrix( void );
	void	LoadIdentity( void );

	void	WriteObject(  TPersist* file, TString name,
						  float tx, float ty, float tz,
						  float rx, float ry, float rz );

public:
	enum Symbols
    {
        lexNULL = 0,
		lexMinus,
        lexAssign,
		lexSquareLeft,
		lexSquareRight,
		lexCurlyLeft,
		lexCurlyRight,
		lexLeft,
		lexRight,
		lexPeriod,
        lexSemicolon,
        lexComma,
		lexLessThan,
		lexGreaterThan,
		lexRealNumber,
		lexIntNumber,
		lexStringLiteral,
        lexConstant,

        lexMaterial,
        lexTextureFilename,
        lexFrame,
        lexMesh,
        lexMeshMaterialList,
        lexMeshNormals,
        lexMeshTextureCoords,
		lexTransformMatrix,

        lexIgnore, // symbol to be ignored
        
        lexLast // last - for ID purposes
    };

    enum Errors
    {
		lexErrorUnterminatedString = 1000,
		lexErrorMalformedReal,
		lexErrorMalformedScientificReal,
		lexErrorUnknownSymbol,
        lexErrorOpeningFile,
        lexErrorNotATriangle,
        lexErrorMaterialNotFound,
        lexErrorUnexpectedToken
    };

private:
	char*		parse;
    char		yyString[4096];
    char		errorString[256];
    char		path[256];
    double		yyReal;
    long		yyInt;

    long		index;
    long		lineCntr;
    long		charCntr;
    long		prevIndex;
    long		prevToken;
	long		currToken;

	long		bufferSize;

	size_t		noNameCounter; // automatic naming of objects with no name

    // error handling
   	long		errorIndex;
	long		errorToken;
    bool		error;

	// matrix
	float		matrix[16];
};

///==========================================================================///

#endif

