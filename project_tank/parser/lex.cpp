#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#pragma hdrstop

#include <common/compatability.h>
#include <parser/lex.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>

///==========================================================================///
//
//	Usage Example:
//
//	Lex parser;
//
//
///==========================================================================///

Lex::Lex( void )
	: parse(NULL)
{
	// init my vars
	yyString[0] = 0;
    errorString[0] = 0;
    yyInt = 0;
    yyReal = 0.0;
    lineCntr = 1;
    charCntr = 1;

    // error handling
   	errorIndex = 0;
	errorToken = 0;
    error = false;
    prevToken = 0;
    currToken = 0;

    // string indexes
	index = 0;
    prevIndex = 0;
};

///==========================================================================///

Lex::~Lex( void )
{
    if ( parse!=NULL )
    	delete parse;
    parse = NULL;
};

///==========================================================================///

void Lex::SetString( const char* parseString, long fsize )
{
	index = 0;
    prevIndex = 0;
   	errorIndex = 0;
	errorToken = 0;
    errorString[0] = 0;
    prevToken = 0;
    currToken = 0;
    lineCntr = 1;
    charCntr = 1;
    error = false;

    if ( parse!=NULL )
    	delete parse;
    parse = new char[fsize];
	memcpy( parse, parseString, fsize );
};

///==========================================================================///

const char* Lex::ErrorString( void ) const
{
	return errorString;
}

///==========================================================================///

//
//	New version that keeps track of tokens
//
long Lex::GetNextToken( void )
{
	do
    {
		long delta = index;
		currToken = GetNextToken( prevToken );
	    charCntr += (index-delta);
    }
    while ( currToken==lexIgnore );

    return currToken;
};

///==========================================================================///

//
//	yynext() equivalent
//
long Lex::GetNextToken( long& _prevToken )
{
	// keep history of 1 token back
	_prevToken = currToken;

    // skip whitespaces
    while ( WhiteSpace(parse[index]) ) index++;

    // back single symbol ability
   	prevIndex = index;

    // NULL / end of string?
	if ( parse[index]==0 )
    {
        return lexNULL;
    };

    //
    //	pre-defined functions
    //
    if ( strncmp( &parse[index], "Material ", 9 )==0 )
    {
    	index += 9;
        return lexMaterial;
    }

    if ( strncmp( &parse[index], "TextureFilename ", 16 )==0 )
    {
    	index += 16;
        return lexTextureFilename;
    }

    if ( strncmp( &parse[index], "Frame ", 6 )==0 )
    {
    	index += 6;
        return lexFrame;
    }

    if ( strncmp( &parse[index], "Mesh ", 5 )==0 )
    {
    	index += 5;
        return lexMesh;
    }

    if ( strncmp( &parse[index], "MeshMaterialList ", 17 )==0 )
    {
    	index += 17;
        return lexMeshMaterialList;
    }

    if ( strncmp( &parse[index], "MeshNormals ", 12 )==0 )
    {
    	index += 12;
        return lexMeshNormals;
    }

    if ( strncmp( &parse[index], "MeshTextureCoords ", 18 )==0 )
    {
    	index += 18;
        return lexMeshTextureCoords;
    }

    //
    //	Simple symbols
    //
    switch ( parse[index] )
    {
    	// comment - skip till eol
    	case '/':
        {
        	index++;
        	if ( parse[index]=='/' )
            	while ( parse[index]!=13 &&
                		parse[index]!=10 &&
                        parse[index]!='\n' &&
                        parse[index]!=0 ) index++;
		   	return lexIgnore;
        };

        case ',':
        {
        	index++;
        	return lexComma;
        };

        case '[':
        {
        	index++;
        	return lexSquareLeft;
        };

        case ']':
        {
        	index++;
        	return lexSquareRight;
        };

        case '{':
        {
        	index++;
        	return lexCurlyLeft;
        };

        case '}':
        {
        	index++;
        	return lexCurlyRight;
        };

        case '(':
        {
        	index++;
        	return lexLeft;
        };

        case ')':
        {
        	index++;
        	return lexRight;
        };

        case '.':
        {
        	index++;
        	return lexPeriod;
        };

        case ';':
        {
        	index++;
        	return lexSemicolon;
        };

        case '<':
        {
        	index++;
        	return lexLessThan;
        };

        case '>':
        {
        	index++;
        	return lexGreaterThan;
        };

        case '=':
        {
        	index++;
            return lexAssign;
        };
    }

    //
    // Literal string?
    //
    if ( parse[index]=='"' )
    {
    	index++;
    	long strIndex = 0;
        bool found = (parse[index]=='"');
        while ( !found && parse[index]!=0 && strIndex<200 )
        {
           	yyString[strIndex++] = parse[index++];
            if ( parse[index]=='"' )
            {
            	found = true;
                index++;
            }
        }
        yyString[strIndex] = 0;

        if ( !found )
        	return lexErrorUnterminatedString;
        return lexStringLiteral;
    }

    //
    //	Integer/real?
    //
    if ( (parse[index]>='0' && parse[index]<='9') ||
    	 (parse[index]=='-' && (parse[index+1]>='0' && parse[index+1]<='9')) )
    {
    	// process minus sign
    	long minus = 1;
        double dminus = 1.0;
    	if ( parse[index]=='-' )
        {
        	minus = -1;
            dminus = -1.0;
            index++;
            if ( !(parse[index]>='0' && parse[index]<='9') )
            	return lexErrorMalformedReal;
        };

    	yyInt = long(parse[index]-'0');
        index++;
        while ( parse[index]>='0' && parse[index]<='9' )
        {
        	yyInt = (yyInt * 10) + long(parse[index]-'0');
            index++;
        }

        if ( parse[index]=='.' )
        {
        	index++;
            yyReal = double(yyInt); // copy first part of number
            yyInt = 0;

            if ( ! (parse[index]>='0' && parse[index]<='9') )
            	return lexErrorMalformedReal;

            yyInt = long(parse[index]-'0');
            index++;
            double divideBy = 10.0;
            while ( parse[index]>='0' && parse[index]<='9' )
            {
                yyInt = (yyInt * 10) + long(parse[index]-'0');
                index++;
                divideBy = divideBy * 10.0;
            }

            // add second part to number
            double temp = double(yyInt);
            temp = temp / divideBy;
            yyInt = 0;
            yyReal = yyReal + temp;

            // optional e|E
            if ( parse[index]=='E' || parse[index]=='e' )
            {
            	temp = yyReal;

            	index++;
                long tempToken = GetNextToken();
                if ( tempToken!=lexRealNumber && tempToken!=lexIntNumber )
                    return lexErrorMalformedScientificReal;

                // set e on real
                if ( tempToken==lexRealNumber )
	                yyReal = temp * pow(10.0,yyReal);
                else
                	yyReal = temp * pow(10.0,double(yyInt) );
            }
            // add sign
            yyReal = yyReal * dminus;
            return lexRealNumber;
        }
        else
        {
        	yyInt = yyInt * minus;
        	return lexIntNumber;
        }
    }

    //
    //	Constant or variable?
    //
    if ( (parse[index]>='a' && parse[index]<='z') ||
    	 (parse[index]>='A' && parse[index]<='Z') || parse[index]=='_' )
    {
    	long strIndex = 0;
        yyString[strIndex++] = parse[index++];

        while ( (parse[index]>='a' && parse[index]<='z') ||
    	 		(parse[index]>='A' && parse[index]<='Z') ||
                (parse[index]>='0' && parse[index]<='9') ||
                 parse[index]=='_' )
        {
        	yyString[strIndex++] = parse[index++];
        }
        yyString[strIndex] = 0;

       	return lexConstant;
    }

    index++;
   	return lexIgnore;
};

//==========================================================================

bool Lex::WhiteSpace( char ch )
{
	if ( ch=='\n' || ch==10 || ch==13 )
    {
    	lineCntr++;
	    charCntr = 1;
    }
        
	if ( ch==' ' || ch=='\n' || ch==10 || ch==13 || ch=='\t' )
    	return true;
    return false;
};

//==========================================================================

bool Lex::CheckToken( long token1, long token2 )
{
	if ( token1!=token2 )
    {
    	Error( lexErrorUnexpectedToken );
        char buf[256];
        sprintf( buf, ", expected token '%s' but found token '%s'", ToString(token1), ToString(token2) );
        strcat( errorString, buf );
        return false;
    }
    return true;
};

//==========================================================================

bool Lex::Error( long token )
{
	if ( token==lexErrorUnknownSymbol ||
    	 token==lexErrorUnexpectedToken ||
    	 token==lexErrorUnterminatedString ||
    	 token==lexErrorMalformedReal ||
    	 token==lexErrorMalformedScientificReal ||
         token==lexErrorOpeningFile ||
         token==lexErrorMaterialNotFound ||
         token==lexErrorNotATriangle )
    {
    	errorIndex = index;
        errorToken = token;
        switch ( token )
        {
        	case lexErrorUnknownSymbol:
		        sprintf(errorString,"Error at line %d, char %d\nUnknown symbol",lineCntr, charCntr);
                break;

        	case lexErrorUnterminatedString:
		        sprintf(errorString,"Error at line %d, char %d\nUnterminated literal string",lineCntr, charCntr);
                break;

        	case lexErrorMalformedReal:
		        sprintf(errorString,"Error at line %d, char %d\nMalformed real",lineCntr, charCntr);
                break;

        	case lexErrorMalformedScientificReal:
		        sprintf(errorString,"Error at line %d, char %d\nMalformed exponential on real",lineCntr, charCntr);
                break;

        	case lexErrorUnexpectedToken:
		        sprintf(errorString,"Error at line %d, char %d\nUnexpected token\n",lineCntr, charCntr);
				char temp[50];
				strncpy( temp, &parse[index-20], 40 );
				temp[40] = 0;
				strcat(temp,"\n");
				strcat(errorString,temp);
                break;

        	case lexErrorOpeningFile:
		        sprintf(errorString,"Error Opening File");
                break;

            case lexErrorNotATriangle:
		        sprintf(errorString,"Error at line %d, char %d\ncan only parse triangular surfaces",lineCntr, charCntr);
                break;

            case lexErrorMaterialNotFound:
		        sprintf(errorString,"Error at line %d, char %d\nspecified material not found",lineCntr, charCntr);
                break;

        	default:
		        sprintf(errorString,"Unknown Error at line %d, char %d",lineCntr, charCntr);
                break;
        };
        error = true;
    	return true;
    }
    return false;
};

//==========================================================================

bool Lex::Error( void )
{
	return error;
}

//==========================================================================

void Lex::UngetToken( void )
{
	index = prevIndex;
};

//==========================================================================
//
//	Convert any lexical token back into a string for debugging purposes
//
const char* Lex::ToString( long token )
{
	switch ( token )
    {
		case lexNULL:
        	return "\\0";

		case lexMinus:
        	return "-";

		case lexComma:
        	return ",";

        case lexAssign:
        	return "=";

		case lexSquareLeft:
        	return "[";

		case lexSquareRight:
        	return "]";

		case lexLeft:
        	return "(";

		case lexRight:
        	return ")";

		case lexCurlyLeft:
        	return "{";

		case lexCurlyRight:
        	return "}";

		case lexPeriod:
        	return ".";

        case lexSemicolon:
        	return ";";

		case lexLessThan:
        	return "<";

		case lexGreaterThan:
        	return ">";

		case lexRealNumber:
        	return "[Real]";

		case lexIntNumber:
        	return "[Int]";

        case lexMaterial:
        	return "[Material]";

        case lexTextureFilename:
        	return "[TextureFilename]";

        case lexFrame:
        	return "[Frame]";

        case lexMesh:
        	return "[Mesh]";

        case lexMeshMaterialList:
        	return "[MeshMaterialList]";

        case lexMeshNormals:
        	return "[MeshNormals]";

        case lexMeshTextureCoords:
        	return "[MeshTextureCoords]";

        default:
        	break;
    };
    return "? (unknown)";
};

//==========================================================================
//
//	Standard string trim function (static)
//
char* Lex::Trim( char* str )
{
	if ( str==NULL ) return "";
    long index = 0;
    while ( str[index]==' ' ) index++;
    char temp[256];
    strcpy( temp, &str[index] );
    index = strlen( temp ) - 1;
    while ( temp[index]==' ' && index>0 ) index--;
    temp[index+1] = 0;
    strcpy( str, temp );
    return str;
};

//==========================================================================
//
//	DirectX ascii grammar (parts I'm interested in anyway - rest is skipped)
//
TBinObject* Lex::DirectX( TString fname )
{
	// do merge?
	bool merge = false;
	bool centerx = false;
	bool centery = false;
	bool centerz = false;
	bool normalise = false;
	bool createNormals = false;

	TString fileName;
	TString texturePath;

	fileName = fname.GetItem(' ',0);
	texturePath = fname.GetItem(' ',1);

	size_t ni = fname.NumItems(' ');
	if ( ni>2 )
	{
		for ( size_t i=2; i<ni; i++ )
		{
			TString temp = fname.GetItem(' ',i);
			if ( temp=="-merge" )
			{
				merge = true;
				WriteString( "Merging meshes into one\n" );
			}
			if ( temp=="-createnormals" )
			{
				createNormals = true;
				WriteString( "Creating new normals for mesh\n" );
			}
			if ( temp=="-centerx" )
			{
				centerx = true;
				WriteString( "Centering around the X axis\n" );
			}
			if ( temp=="-centery" )
			{
				centery = true;
				WriteString( "Centering around the Y axis\n" );
			}
			if ( temp=="-centerz" )
			{
				centerz = true;
				WriteString( "Centering around the Z axis\n" );
			}
			if ( temp=="-normalise" )
			{
				normalise = true;
				WriteString( "Normalising\n" );
			}
		}
	}

    // process file
	FILE* fh = fopen( fileName.c_str(), "rb" );
    if ( fh==NULL )
    {
    	Error( lexErrorOpeningFile );
        char buf[256];
        sprintf( buf, ", '%s'", fileName.c_str() );
        strcat( errorString, buf );
    	return NULL;
    }

    // aquire file size
    fseek(fh,0,SEEK_END);
    long fsize = ftell(fh);
    fseek(fh,0,SEEK_SET);

    if ( fsize<1 )
    {
        strcpy( errorString, "Error: zero filesize\n" );
    	return NULL;
    }

    // load file
    char* buf = new char[fsize+1];
    fread(buf,fsize,1,fh);
    buf[fsize] = 0;
    SetString( buf, fsize );
    fclose(fh);
    delete buf;

    TBinObject* obj = new TBinObject();
    if ( Parse( obj, texturePath.c_str() ) )
	{
		if ( merge )
			obj->MergeMeshesIntoOne();
		obj->CenterAxes( centerx, centery, centerz );
		if ( normalise )
			obj->Normalise();
		if ( createNormals )
			obj->CreateNewNormals();
    	return obj;
	}
    return NULL;
};

//==========================================================================
//
//	directx:	material directx
//			|	mesh directx
//			|	e
//
bool Lex::Parse( TBinObject* obj, const char* texturePath )
{
	bool finished = false;
    while ( !finished )
    {
		long token = GetNextToken();
        if ( Error( token ) )
        	return false;

        if ( token==lexMaterial )
        {
        	UngetToken();
            TMaterial* mat = ParseMaterial(texturePath);
            if ( Error() )
            	return false;
                
            obj->AddMaterial( *mat );
        }
        else if ( token==lexMesh )
        {
        	UngetToken();
            TMesh* mesh = ParseMesh( obj, texturePath );
            if ( Error() )
            	return false;

            obj->AddMesh( *mesh );
        }
        else if ( token==lexNULL )
        	finished = true;
    }
	return true;
};

//==========================================================================
//
//	Material grammar
//

//
//	material:	MATERIAL CONST CURLYLEFT
//				float sep float sep float sep float sep sep
//				float sep
//				float sep float sep float sep sep
//				float sep float sep float sep sep
//				textureOpt
//				CURLYRIGHT
//

TMaterial* Lex::ParseMaterial( const char* texturePath )
{
	TMaterial* mat = new TMaterial;

	long token = GetNextToken();
    if ( !CheckToken( lexMaterial, token ) )  { delete mat; return NULL; }

	token = GetNextToken();
    if ( !CheckToken( lexConstant, token ) )  { delete mat; return NULL; }

	// check if its a name like object.ext
	char temp[256];
	strcpy( temp, yyString );
	token = GetNextToken();
	if ( token!=lexPeriod )
	{
		UngetToken();
	    mat->MaterialName( temp );
	}
	else
	{
		strcat( temp, "." );

		token = GetNextToken();
		if ( token!=lexConstant )
		{
			UngetToken();
		}
		else
		{
			strcat( temp, yyString );
		}
	    mat->MaterialName( temp );
	}

	token = GetNextToken();
    if ( !CheckToken( lexCurlyLeft, token ) )  { delete mat; return NULL; }

    float r,g,b,a;
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    r = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    g = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    b = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    a = float(yyReal);

    mat->SetMaterialColour( r,g,b,a );

    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }

    mat->SetMaterialPower( float(yyReal) );

    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    r = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    g = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    b = float(yyReal);

    mat->SetMaterialEmission( r,g,b );

    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    r = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    g = float(yyReal);
    if ( !Float() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    if ( !Sep() ) { delete mat; return NULL; }
    b = float(yyReal);

    mat->SetMaterialSpecularity( r,g,b );

    if ( !TextureOpt( mat, texturePath ) ) { delete mat; return NULL; }

	token = GetNextToken();
    if ( !CheckToken( lexCurlyRight, token ) )  { delete mat; return NULL; }

    return mat;
};

//==========================================================================
//
//	textureOpt:	TEXTUREFILENAME CURLYLEFT STRINGCONST sep CURLYRIGHT
//
bool Lex::TextureOpt( TMaterial* mat, const char* texturePath )
{
	long token = GetNextToken();
    if ( token!=lexTextureFilename )
    {
    	UngetToken();
        return true;
    }

	token = GetNextToken();
    if ( !CheckToken( lexCurlyLeft, token ) )  { return false; }

	token = GetNextToken();
    if ( !CheckToken( lexStringLiteral, token ) )  { return false; }
	TString errStr;
    if ( texturePath[0]==0 )
	    mat->FileName( yyString, "", errStr );
    else
	    mat->FileName( yyString, texturePath, errStr );

    if ( !Sep() ) { return false; }

	token = GetNextToken();
    if ( !CheckToken( lexCurlyRight, token ) )  { return false; }

    return true;
};

//==========================================================================

bool Lex::Float( void )
{
	long token = GetNextToken();
    if ( !CheckToken( lexRealNumber, token ) )
    	return false;
    return true;
};

//==========================================================================

bool Lex::Int( void )
{
	long token = GetNextToken();
    if ( !CheckToken( lexIntNumber, token ) )
    	return false;
    return true;
};

//==========================================================================
//
//	sep:		SEMICOLON
//			|	COMMA
//
bool Lex::Sep( void )
{
	long token = GetNextToken();
    if ( token!=lexComma && token!=lexSemicolon )
    {
    	CheckToken( lexSemicolon, token );
        return false;
    }
    return true;
};

//==========================================================================
//
//	mesh:	MESH CONST CURLYLEFT
//			long sep	// =vertice_count
//			float sep float sep float sep sep [vertice_count]
//
//			long sep	// =face_count
//			3 sep long sep long sep long sep sep [face_count]
//
//			meshMaterialOpt
//			meshNormalOpt
//			meshTextureOpt
//
//			CURLYRIGHT
//
TMesh* Lex::ParseMesh( TBinObject* obj, const char* texturePath )
{
	long i;
	TMesh* mesh = new TMesh();

	long token = GetNextToken();
    if ( !CheckToken( lexMesh, token ) )  { delete mesh; return NULL; }

    // name of mesh can be NULL
	token = GetNextToken();
    if ( token==lexCurlyLeft )
	{
    	mesh->Name( "unknown" );
	}
    else if ( !CheckToken( lexConstant, token ) )
    { 
		delete mesh; return NULL; 
	}


	// check if its a name like object.ext
	char temp[256];
	strcpy( temp, yyString );
	token = GetNextToken();
	if ( token!=lexPeriod )
	{
		UngetToken();
	    mesh->Name( temp );
	}
	else
	{
		strcat( temp, "." );

		token = GetNextToken();
		if ( token!=lexConstant )
		{
			UngetToken();
		}
		else
		{
			strcat( temp, yyString );
		}
	    mesh->Name( temp );
	}
    
	token = GetNextToken();
	if ( !CheckToken( lexCurlyLeft, token ) )  { delete mesh; return NULL; }

    //
    // # vertices
    //
    if ( !Int() ) { delete mesh; return NULL; }
    if ( !Sep() ) { delete mesh; return NULL; }
    mesh->NumVertices( yyInt );

    // read x vertices
    long num = yyInt;
    float* vert = new float[3*num];
    for ( i=0; i<num; i++ )
    {
	    if ( !Float() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        vert[i*3+0] = float(yyReal);

	    if ( !Float() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        vert[i*3+1] = float(yyReal);

	    if ( !Float() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        vert[i*3+2] = float(yyReal);
    }
    mesh->Vertices( vert );
    delete vert;

    //
    // # faces
    //
    if ( !Int() ) { delete mesh; return NULL; }
    if ( !Sep() ) { delete mesh; return NULL; }
    mesh->NumFaces( yyInt );

    // read x face indexes
    num = yyInt;
    int* faces = new int[3*num];
    for ( i=0; i<num; i++ )
    {
	    if ( !Int() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        if ( yyInt!=3 )
        {
        	Error( lexErrorNotATriangle );
        	delete mesh;
            return NULL;
        }

	    if ( !Int() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        faces[i*3+0] = yyInt;

	    if ( !Int() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        faces[i*3+1] = yyInt;

	    if ( !Int() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        faces[i*3+2] = yyInt;
    }
    mesh->Surfaces( (size_t*)faces );
    delete faces;

	MeshMaterialOpt( mesh, obj );
    if ( Error() ) { delete mesh; return NULL; }

	MeshNormalOpt( mesh );
    if ( Error() ) { delete mesh; return NULL; }

	MeshTextureOpt( mesh, texturePath );
    if ( Error() ) { delete mesh; return NULL; }

    return mesh;
}


//==========================================================================
//
// meshMaterialOpt:		MESHMATERIALLIST CURLYLEFT
//						long sep	// =mat_count
//						long sep // =face_count
//						long sep [face_count] sep
//						CURLYLEFT CONST CURLYRIGHT [mat_count]
//						CURLYRIGHT
//
void Lex::MeshMaterialOpt( TMesh* mesh, TBinObject* obj )
{
	long i;

	long token = GetNextToken();
    if ( token!=lexMeshMaterialList )
    {
    	UngetToken();
        return;
    }

	token = GetNextToken();
    if ( !CheckToken( lexCurlyLeft, token ) ) return;

    // # materials
    if ( !Int() ) return;
   	if ( !Sep() ) return;
	long matCount = yyInt;
    mesh->NumMaterials( yyInt );

    // # material faces
    if ( !Int() ) return;
   	if ( !Sep() ) return;
	long faceCount = yyInt;
    mesh->NumMaterialFaces( yyInt );

    // read x material faces
    long num = faceCount;
    int* materialFaces = new int[num];
    for ( i=0; i<num; i++ )
    {
	    if ( !Int() ) return;
    	if ( !Sep() ) return;

        materialFaces[i] = yyInt;
    }
    mesh->MaterialFaces( (size_t*)materialFaces );
    delete materialFaces;

   	if ( !Sep() ) return;

    // read x material references
    num = matCount;
    for ( i=0; i<num; i++ )
    {
		token = GetNextToken();
    	if ( !CheckToken( lexCurlyLeft, token ) ) return;

		token = GetNextToken();
    	if ( !CheckToken( lexConstant, token ) ) return;

        TMaterial* mat = obj->GetMaterial( yyString );
        if ( mat==NULL )
        {
        	Error( lexErrorMaterialNotFound );
            return;
        }
        mesh->Materials( i, mat );

		token = GetNextToken();
    	if ( !CheckToken( lexCurlyRight, token ) ) return;
    }

	token = GetNextToken();
   	if ( !CheckToken( lexCurlyRight, token ) ) return;
};

//==========================================================================
//
// meshNormalOpt:	MESHNORMALS CURLYLEFT
//					long sep // =normal_count
//					float sep float sep float sep sep [normal_count]
//					long sep // =face_count
//					3 sep long sep long sep long sep sep [face_count]
//					CURLYRIGHT
//
void Lex::MeshNormalOpt( TMesh* mesh )
{
	long i;

	long token = GetNextToken();
    if ( token!=lexMeshNormals )
    {
    	UngetToken();
        return;
    }

	token = GetNextToken();
    if ( !CheckToken( lexCurlyLeft, token ) ) return;

    //
    // # normals
    //
    if ( !Int() ) return;
    if ( !Sep() ) return;
    mesh->NumNormals( yyInt );

    // read x normals
    long num = yyInt;
    float* normals = new float[3*num];
    for ( i=0; i<num; i++ )
    {
	    if ( !Float() ) return;
    	if ( !Sep() ) return;
        normals[i*3+0] = float(yyReal);

	    if ( !Float() ) return;
    	if ( !Sep() ) return;
        normals[i*3+1] = float(yyReal);

	    if ( !Float() ) return;
    	if ( !Sep() ) return;
    	if ( !Sep() ) return;
        normals[i*3+2] = float(yyReal);
    }
    mesh->Normals( normals );
    delete normals;

    //
    // # normal faces
    //
    if ( !Int() ) return;
    if ( !Sep() ) return;
    mesh->NumNormalFaces( yyInt );

    // read x normal surfaces
    num = yyInt;
    int* faces = new int[3*num];
    for ( i=0; i<num; i++ )
    {
	    if ( !Int() ) return;
    	if ( !Sep() ) return;
        if ( yyInt!=3 )
        {
        	Error( lexErrorNotATriangle );
        	delete mesh;
            return;
        }

	    if ( !Int() ) return;
    	if ( !Sep() ) return;
        faces[i*3+0] = yyInt;

	    if ( !Int() ) return;
    	if ( !Sep() ) return;
        faces[i*3+1] = yyInt;

	    if ( !Int() ) return;
    	if ( !Sep() ) return;
    	if ( !Sep() ) return;
        faces[i*3+2] = yyInt;
    }
    mesh->NormalFaces( (size_t*)faces );
    delete faces;

	token = GetNextToken();
   	if ( !CheckToken( lexCurlyRight, token ) ) return;
};

//==========================================================================
//
// meshTextureOpt	MESHTEXTURECOORDS CURLYLEFT
//					long sep // =texture_count
//					float sep float sep sep [texture_count]
//					CURLYRIGHT
//
void Lex::MeshTextureOpt( TMesh* mesh, const char* texturePath )
{
	long token = GetNextToken();
    if ( token!=lexMeshTextureCoords )
    {
    	UngetToken();
        return;
    }

	token = GetNextToken();
    if ( !CheckToken( lexCurlyLeft, token ) ) return;

    //
    // # uvs
    //
    if ( !Int() ) return;
    if ( !Sep() ) return;
    mesh->NumTextureCoords( yyInt );

    // read x normals
    long num = yyInt;
    float* uvs = new float[2*num];
    for ( long i=0; i<num; i++ )
    {
	    if ( !Float() ) return;
    	if ( !Sep() ) return;
        uvs[i*2+0] = float(yyReal);

	    if ( !Float() ) return;
    	if ( !Sep() ) return;
    	if ( !Sep() ) return;
        uvs[i*2+1] = float(yyReal);
    }
    mesh->TextureCoords( uvs );
    delete uvs;

	token = GetNextToken();
   	if ( !CheckToken( lexCurlyRight, token ) ) return;
};

//==========================================================================


