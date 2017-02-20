#include <precomp_header.h>

#include <common/compatability.h>
#include <parser/lex.h>
#include <object/geometry.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/landscape.h>
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

	bufferSize = 0;

    // string indexes
	index = 0;
    prevIndex = 0;

	noNameCounter = 1;
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
	bufferSize = fsize;
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
    while ( WhiteSpace(parse[index]) && index<bufferSize ) index++;

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

    if ( strncmp( &parse[index], "FrameTransformMatrix ", 21 )==0 )
    {
    	index += 21;
        return lexTransformMatrix;
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
		        sprintf(errorString,"Error at line %d, char %d\nspecified material \"%s\" not found",lineCntr, charCntr, yyString );
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
	bool uvupsidedown = false;
	bool minx0 = false;
	bool miny0 = false;
	bool minz0 = false;
	bool maxx0 = false;
	bool maxy0 = false;
	bool maxz0 = false;
	bool bounds = false;
	bool half = false;
	bool createGraph = false;
	bool man = false; // manual overwrite custom transform
	bool scale = false;
	float scalef = 1;
	bool reverseWindings = false;

	float bx1,by1,bz1;
	float bx2,by2,bz2;

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
			if ( temp.substr(0,5)=="-half" )
			{
				WriteString( "Halving mesh size\n" );
				half = true;
			}
			if ( temp.substr(0,8)=="-bounds(" )
			{
				bounds = true;
				WriteString( "Setting mesh boundaries to (" );
				TString params = temp.substr(8);
				bx1 = float(atof(params.GetItem(',',0).c_str()));
				by1 = float(atof(params.GetItem(',',1).c_str()));
				bz1 = float(atof(params.GetItem(',',2).c_str()));
				bx2 = float(atof(params.GetItem(',',3).c_str()));
				by2 = float(atof(params.GetItem(',',4).c_str()));
				bz2 = float(atof(params.GetItem(',',5).c_str()));
				WriteString( "%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f)\n", bx1,by1,bz1,
							 bx2,by2,bz2 );
			}
			if ( temp=="-createnormals" )
			{
				createNormals = true;
				WriteString( "Creating new normals for mesh\n" );
			}
			if ( temp.substr(0,7)=="-scale=" )
			{
				scale = true;
				scalef = float(atof(temp.GetItem('=',1).c_str()));
				WriteString( "Scaling mesh by %2.2f\n", scalef );
			}
			if ( temp=="-flipuv" )
			{
				WriteString( "Flipping UV upside down\n" );
				uvupsidedown = true;
			}
			if ( temp=="-revw" )
			{
				WriteString( "Reversing windings\n" );
				reverseWindings = true;
			}
			if ( temp=="-minx0" )
			{
				minx0 = true;
				WriteString( "Making minx = 0\n" );
			}
			if ( temp=="-man" )
			{
				man = true;
				WriteString( "Manual transformation selected\n" );
			}
			if ( temp=="-miny0" )
			{
				miny0 = true;
				WriteString( "Making miny = 0\n" );
			}
			if ( temp=="-minz0" )
			{
				minz0 = true;
				WriteString( "Making minz = 0\n" );
			}
			if ( temp=="-maxx0" )
			{
				maxx0 = true;
				WriteString( "Making maxx = 0\n" );
			}
			if ( temp=="-maxy0" )
			{
				maxy0 = true;
				WriteString( "Making maxy = 0\n" );
			}
			if ( temp=="-maxz0" )
			{
				maxz0 = true;
				WriteString( "Making maxz = 0\n" );
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
		if ( createGraph && obj->NumMeshes()>1 && !merge )
		{
			delete obj;
			strcpy( errorString, "Error: can't create a graph on a multi-mesh object, use '-merge'\n" );
			return NULL;
		}
		obj->CenterAxes( centerx, centery, centerz );
		if ( normalise ) // first, is posn independent
			obj->Normalise();
		if ( scale ) // second, needs normalisation to work - but is posn. indep.
			obj->Scale( scalef );
		if ( minx0 || miny0 || minz0 ||
			 maxx0 || maxy0 || maxz0 )
			obj->AdjustAxes( minx0, miny0, minz0, maxx0, maxy0, maxz0 );
		if ( createNormals )
			obj->CreateNewNormals();
		if ( reverseWindings )
			obj->ReverseWindings();
		if ( bounds )
			obj->ResizeMesh( bx1,by1,bz1, bx2,by2,bz2 );
		if ( uvupsidedown )
			obj->FlipUV();
		if ( man )
			obj->ManualTransform();
		if ( half )
			obj->HalfMesh();

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
	bool hasMatrix = false;

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
			if ( hasMatrix )
			{
				hasMatrix = false;
			}
			else
			{
				LoadIdentity();
			}

        	UngetToken();
            TMesh* mesh = ParseMesh( obj, texturePath );
            if ( Error() )
            	return false;

            obj->AddMesh( *mesh );
        }
		else if ( token==lexTransformMatrix )
		{
			UngetToken();
			ParseMatrix();
			if ( Error() )
				return false;
			hasMatrix = true;
		}
        else if ( token==lexNULL )
        	finished = true;
    }
	return true;
};


//==========================================================================
//
//	Matrix grammar
//

//	matrix:		TRANSFORMMATRIX CURLYLEFT
//				float sep float sep float sep float sep
//				float sep float sep float sep float sep
//				float sep float sep float sep float sep
//				float sep float sep float sep float sep sep
//				CURLYRIGHT
//
void Lex::ParseMatrix( void )
{
	long token = GetNextToken();
    if ( !CheckToken( lexTransformMatrix, token ) )  { return; }
	
	token = GetNextToken();
	if ( !CheckToken( lexCurlyLeft, token ) )  { return; }

	for ( size_t i=0; i<16; i++ )
	{
		if ( !Float() ) { return; }
		matrix[i] = float(yyReal);
	    if ( !Sep() ) { return; }
	}
	if ( !Sep() ) { return; }
	token = GetNextToken();
	CheckToken( lexCurlyRight, token );
};


void Lex::LoadIdentity( void )
{
	matrix[0] = 1; matrix[1] = 0; matrix[2] = 0; matrix[3] = 0;
	matrix[4] = 0; matrix[5] = 1; matrix[6] = 0; matrix[7] = 0;
	matrix[8] = 0; matrix[9] = 0; matrix[10] = 1; matrix[11] = 0;
	matrix[12] = 0; matrix[13] = 0; matrix[14] = 0; matrix[15] = 1;
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
	if ( token==lexCurlyLeft )
	{
		char temp[256];
		sprintf( temp, "noname_%d", noNameCounter++ );
	    mat->MaterialName( temp );
	}
	else
	{
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
		}
	    mat->MaterialName( temp );

		token = GetNextToken();
		if ( !CheckToken( lexCurlyLeft, token ) )  { delete mat; return NULL; }
	}

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

    mat->SetMaterialShininess( float(yyReal) );

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
    if ( token==lexRealNumber )
		return true;
	if ( token==lexIntNumber )
	{
		yyReal = float(yyInt);
		return true;
	}
	CheckToken( lexRealNumber, token ); // force error
   	return false;
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
		float x,y,z;
	    if ( !Float() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        x = float(yyReal);

	    if ( !Float() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        y = float(yyReal);

	    if ( !Float() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
    	if ( !Sep() ) { delete mesh; return NULL; }
        z = -float(yyReal);

		// matrix mult these 3 coords
		x = matrix[0]*x + matrix[1]*y + matrix[2]*z + matrix[3];
		y = matrix[4]*x + matrix[5]*y + matrix[6]*z + matrix[7];
		z = matrix[8]*x + matrix[9]*y + matrix[10]*z + matrix[11];

        vert[i*3+0] = x;
        vert[i*3+1] = y;
        vert[i*3+2] = z;

		// special case
		if ( (i+1)==num )
		if ( x==3.141592f )
		{
	        vert[i*3+2] = -z;
		}
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

	MeshMaterialOpt( mesh, obj, texturePath );
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
void Lex::MeshMaterialOpt( TMesh* mesh, TBinObject* obj, const char* texturePath )
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
        if ( Error( token ) )
        	return;

        if ( token==lexMaterial )
        {
        	UngetToken();
            TMaterial* mat = ParseMaterial(texturePath);
            if ( Error() )
            	return;
                
            obj->AddMaterial( *mat );
	        mesh->Materials( i, i, mat );
        }
		else
		{
			if ( !CheckToken( lexCurlyLeft, token ) ) return;

			token = GetNextToken();
	    	if ( !CheckToken( lexConstant, token ) ) return;

	        TMaterial* mat = obj->GetMaterial( yyString );
	        if ( mat==NULL )
	        {
	        	Error( lexErrorMaterialNotFound );
	            return;
	        }
	        mesh->Materials( i, i, mat );

			token = GetNextToken();
	    	if ( !CheckToken( lexCurlyRight, token ) ) return;
		}
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
        uvs[i*2+1] = -float(yyReal);
    }
    mesh->TextureCoords( uvs );
    delete uvs;

	token = GetNextToken();
   	if ( !CheckToken( lexCurlyRight, token ) ) return;
};


void Lex::WriteObject(  TPersist* file, TString name,
						float tx, float ty, float tz,
						float rx, float ry, float rz )
{
	TString temp;

	temp = "Mesh " + name + " {\n";
	temp = temp + "\t2;\n";
	file->FileWrite( temp.c_str(), temp.length() );

	temp = "\t" + FloatToString(1,6,tx);
	temp = temp + ";" + FloatToString(1,6,ty);
	temp = temp + ";" + FloatToString(1,6,tz) + ";,     // translation\n";
	file->FileWrite( temp.c_str(), temp.length() );

	temp = "\t" + FloatToString(1,6,rx);
	temp = temp + ";" + FloatToString(1,6,ry);
	temp = temp + ";" + FloatToString(1,6,rz) + ";;     // rotation\n\n";
	file->FileWrite( temp.c_str(), temp.length() );

	// make it valid by adding a fake surface
	temp = "\t1;		// fake surface for validation\n";
	temp = temp + "\t3;0,1,0;;\n}\n\n";
	file->FileWrite( temp.c_str(), temp.length() );
}


bool Lex::CreateMesh( TString fileName, TString strip,
					  size_t seaHeight, TString& errStr )
{
	size_t x,y,i;
	TTexture bmp;

	TString path,fname;
	SplitPath( fileName, path, fname );

	float bx1 = -500;
	float by1 = 0;
	float bz1 = -500;
	float bx2 = 500;
	float by2 = 75;
	float bz2 = 500;

	bmp.DontDestroyRGB
		(true);
	if ( !bmp.LoadBinary( fname, path, errStr ) )
		return false;

	const byte* rgb = bmp.Rgb();
	PostCond( rgb!=NULL );

	size_t w = bmp.Width();
	size_t h = bmp.Height();

	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;

	// create valid directX text based mesh from height map
	TPersist file( fileWrite );
	TString newFname = fileName.GetItem('.',0) + ".x";
	if ( !file.FileOpen( newFname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	TString temp;

	temp = "xof 0302txt 0064\n\n";

	temp = temp + "Header {\n";
	temp = temp + "1;\n";
	temp = temp + "0;\n";
	temp = temp + "1;\n";
	temp = temp + "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "Material treeMaterial {\n";
	temp = temp + "\t0.900000;0.900000;0.900000;1.000000;;\n";
	temp = temp + "\t4.000000;\n";
	temp = temp + "\t0.100000;0.100000;0.100000;;\n";
	temp = temp + "\t0.000000;0.000000;0.000000;;\n";
	temp = temp + "\tTextureFilename {\n";
	temp = temp + "\t\t\"tree.jpg\";\n";
	temp = temp + "\t}\n";
	temp = temp + "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "Material heightMap {\n";
	temp = temp + "\t1.000000;1.000000;1.000000;1.000000;;\n";
	temp = temp + "\t4.000000;\n";
	temp = temp + "\t0.100000;0.100000;0.100000;;\n";
	temp = temp + "\t0.000000;0.000000;0.000000;;\n";
	temp = temp + "\tTextureFilename {\n";
	temp = temp + "\t\t\"bump.jpg\";\n";
	temp = temp + "\t}\n";
	temp = temp + "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "Material colourMap {\n";
	temp = temp + "\t1.000000;1.000000;1.000000;1.000000;;\n";
	temp = temp + "\t4.000000;\n";
	temp = temp + "\t0.100000;0.100000;0.100000;;\n";
	temp = temp + "\t0.000000;0.000000;0.000000;;\n";
	temp = temp + "\tTextureFilename {\n";
	temp = temp + "\t\t\"" + strip + "\";\n";
	temp = temp + "\t}\n";
	temp = temp + "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "Material seaMaterial {\n";
	temp = temp + "\t0.307059;0.345882;0.462353;0.450000;;\n";
	temp = temp + "\t4.000000;\n";
	temp = temp + "\t0.034118;0.038431;0.051373;;\n";
	temp = temp + "\t0.000000;0.000000;0.000000;;\n";
	temp = temp + "\tTextureFilename {\n";
    temp = temp + "\t\t\"[wat00##.jpg,1,0,8,8]\";\n";
//    temp = temp + "\t\t\"wat0000.jpg\";\n";
	temp = temp + "\t}\n";
	temp = temp + "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	// mesh start
	temp = "Mesh landscapeMesh {\n";
	file.FileWrite( temp.c_str(), temp.length() );

	// num vertices
	temp = "\t" + Int2Str( w*h+1 ) + "; // vertex count\n";
	file.FileWrite( temp.c_str(), temp.length() );

	// vertices: 	-2.501811;2.691057;2.584104;,
	for ( y=0; y<h; y++ )
	for ( x=0; x<w; x++ )
	{
		size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

		float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
		float v2 = (float(height) / 256) * (by2-by1) + by1;
		float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
		temp = "\t" + FloatToString( 1,6, v1 ) + ";";
		temp = temp + FloatToString( 1,6, v2 ) + ";";
		temp = temp + FloatToString( 1,6, v3 );
		temp = temp + ";,\n";
		file.FileWrite( temp.c_str(), temp.length() );
	}

	// add descriptive vertex:   marker;width;height;;
	temp = "\t3.141592;" + FloatToString(1,6,w) + ";" + FloatToString(1,6,h);
	temp = temp + ";;\n";
	file.FileWrite( temp.c_str(), temp.length() );

	// numfaces
	temp = "\n\t" + Int2Str( (w-1)*(h-1)*2 ) + "; // surface count\n";
	file.FileWrite( temp.c_str(), temp.length() );

	// surfaces: 	3;423,426,8;,
	for ( y=0; y<(h-1); y++ )
	for ( x=0; x<(w-1); x++ )
	{
		size_t i1,i2,i3,i4;
		i1 = y*w+x;
		i2 = (y+1)*w+x;
		i3 = y*w+(x+1);
		i4 = (y+1)*w+(x+1);

		temp = "\t3;" + Int2Str(i1) + "," + Int2Str(i2) + "," + Int2Str(i3) + ";,\n";
		file.FileWrite( temp.c_str(), temp.length() );

		temp = "\t3;" + Int2Str(i2) + "," + Int2Str(i4) + "," + Int2Str(i3);
		if ( (x+2)==w && (y+2)==h )
			temp = temp + ";;\n";
		else
			temp = temp + ";,\n";
		file.FileWrite( temp.c_str(), temp.length() );
	}

	temp = "\tMeshMaterialList {\n";
	temp = temp + "\t\t2;1;0;;   // #materials,#faces,faces\n";
	temp = temp + "\t\t{heightMap}\n";
	temp = temp + "\t\t{colourMap}\n";
	temp = temp + "\t}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	// add sea
	size_t vcount = 0;
	size_t scount = 0;
	size_t* surfaces = new size_t[w*h*12];
	float* vertices = new float[w*h*12];

	// create sea
	for ( y=0; y<(h-1); y++ )
	for ( x=0; x<(w-1); x++ )
	{
		size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

		if ( height<seaHeight )
		{
			// add two triangles to make this part of the sea
			float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
			float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;

			float v2 = ((float(seaHeight)/256.0f) * (by2-by1) + by1) * 0.5f;

			AddVertex( vertices, vcount, v1,v2,v3 );

			v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,v3 );

			v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,v3 );

			v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,v3 );

			// add two surfaces defined by these triangles
			AddSurface( surfaces, scount, vcount-4, vcount-1, vcount-3 );
			AddSurface( surfaces, scount, vcount-2, vcount-3, vcount-1 );
		}
	}

	// write sea mesh
	if ( vcount>0 && scount>0 )
	{
		temp = "Mesh seaMesh {\n";
		temp = temp + "\t" + Int2Str(vcount) + ";		// num vertices\n";
		file.FileWrite( temp.c_str(), temp.length() );

		for ( i=0; i<vcount; i++ )
		{
			temp = "\t" + FloatToString(1,6,vertices[i*3+0]);
			temp = temp + ";" + FloatToString(1,6,vertices[i*3+1]);
			temp = temp + ";" + FloatToString(1,6,vertices[i*3+2]);
			if ( (i+1)!=vcount )
				temp = temp + ";,\n";
			else
				temp = temp + ";;\n\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}

		temp = "\t" + Int2Str(scount) + ";		// num faces\n";
		file.FileWrite( temp.c_str(), temp.length() );
	
		for ( i=0; i<scount; i++ )
		{
			temp = "\t3;" + Int2Str(surfaces[i*3+0]);
			temp = temp + "," + Int2Str(surfaces[i*3+1]);
			temp = temp + "," + Int2Str(surfaces[i*3+2]);
			if ( (i+1)!=scount )
				temp = temp + ";,\n";
			else
				temp = temp + ";;\n\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}

		temp = "\tMeshMaterialList {\n";
		temp = temp + "\t\t1;1;0;;\n";
		temp = temp + "\t\t{seaMaterial}\n";
		temp = temp + "\t}\n\n";
		file.FileWrite( temp.c_str(), temp.length() );

		temp = "\tMeshNormals {\n";
		temp = temp + "\t\t" + Int2Str(1) + ";	// num normals\n";
		file.FileWrite( temp.c_str(), temp.length() );

		temp = "\t\t0.000000;1.000000;0.000000;;\n\n";
		file.FileWrite( temp.c_str(), temp.length() );

		temp = "\t\t" + Int2Str(scount) + ";		// num faces\n";
		file.FileWrite( temp.c_str(), temp.length() );
	
		for ( i=0; i<scount; i++ )
		{
			temp = "\t\t3;1,1,1";
			if ( (i+1)!=scount )
				temp = temp + ";,\n";
			else
				temp = temp + ";;\n\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
		temp = "\t}\n\n";
		file.FileWrite( temp.c_str(), temp.length() );
			
		temp = "\tMeshTextureCoords {\n";
		temp = temp + "\t" + Int2Str(vcount) + ";		// num texture coords\n";
		file.FileWrite( temp.c_str(), temp.length() );
		//temp = temp + "\t0.463879;0.009085;,\n";
		for ( i=0; i<(vcount/4); i++ )
		{
			temp = "\t0.000000;0.000000;,\n";
			temp = temp + "\t1.000000;0.000000;,\n";
			temp = temp + "\t1.000000;1.000000;,\n";
			temp = temp + "\t0.000000;1.000000";
			if ( (i+1)!=(vcount/4) )
				temp = temp + ";,\n";
			else
				temp = temp + ";;\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
		temp = "\t}\n}\n\n";
		file.FileWrite( temp.c_str(), temp.length() );
	}

	file.FileClose();

	delete []surfaces;
	delete []vertices;

	return true;
};

//==========================================================================

bool ImportOpenGL( const TString& fname, TBinObject& obj, 
				   TBinObject* compareObj, TString& errStr )
{
	TPersist  file(fileRead);

	if ( !file.FileOpen(fname) )
	{
		errStr = file.ErrorString();
		return false;
	}

	TString line;
	bool eof;
	bool started = false;

	// see if we can find a [mergelist]
	size_t numMergeMeshes = 0;
	size_t startMeshes[100];
	size_t endMeshes[100];
	TString meshNames[100];
	eof = !file.ReadLine( line );
	if ( line.contains("mergelist=") )
	{
		numMergeMeshes = atoi( line.GetItem('=',1).c_str() );
		for ( size_t i=0; i<numMergeMeshes; i++ )
		{
			file.ReadLine( line );
			meshNames[i] = line.GetItem('=',0);
			line = line.GetItem('=',1);
			startMeshes[i] = atoi(line.GetItem('-',0).c_str());
			endMeshes[i] = atoi(line.GetItem('-',1).c_str());
		}
	};

	TMesh meshes[100];

	float normals[30000];
	float vertices[30000];
	size_t surfaces[30000];
	size_t normalFaces[30000];
	size_t materialFaces[30000];
	float uvs[20000];

	size_t currentMerge = 0;
	size_t numMeshes = 0;
	size_t numSurfaces = 0;
	size_t numNormalFaces = 0;
	size_t numVertices = 0;
	size_t numUVs = 0;
	size_t numNormals = 0;
	size_t numMaterialFaces = 0;

	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
	float rspec = 0;
	float gspec = 0;
	float bspec = 0;
	float aspec = 0;
	float remis = 0;
	float gemis = 0;
	float bemis = 0;
	float aemis = 0;
	float shininess = 0;
	bool   usingTexture = false;
	size_t textureId = 0;

	bool hasNormal = false;
	bool hasUV = false;

	TMesh mesh;
	do
	{
		eof = !file.ReadLine( line );

		if ( line.contains("glBegin(") )
		{
			started = true;
		}

		if ( line.contains("glEnd(") )
		{
			started = false;

			PreCond( (numNormals==numVertices) && (numVertices==numUVs) );

			if ( numMergeMeshes > 0 )
			{
				if ( endMeshes[currentMerge] <= numMeshes )
				{
					if ( numVertices>0 )
					{
						mesh.Name( meshNames[currentMerge] );
						mesh.NumVertices( numVertices );
						mesh.Vertices( vertices );
						mesh.NumFaces( numSurfaces/3 );
						mesh.Surfaces( surfaces );
						mesh.NumMaterialFaces( numSurfaces/3 );
						mesh.MaterialFaces( materialFaces );
					}
					if ( numUVs>0 )
					{
						mesh.NumTextureCoords( numUVs );
						mesh.TextureCoords( uvs );
					}
					if ( numNormals>0 )
					{
						mesh.NumNormals( numNormals );
						mesh.Normals( normals );
						mesh.NumNormalFaces( numNormalFaces/3 );
						mesh.NormalFaces( normalFaces );
					}

					obj.AddMesh( mesh );

					currentMerge++;

					numVertices = 0;
					numSurfaces = 0;
					numUVs = 0;
					numNormals = 0;
					numNormalFaces = 0;
					numMaterialFaces = 0;
				}
			}
			else
			{
				if ( numVertices>0 )
				{
					mesh.NumVertices( numVertices );
					mesh.Vertices( vertices );
					mesh.NumFaces( numSurfaces/3 );
					mesh.Surfaces( surfaces );
					mesh.NumMaterialFaces( numSurfaces/3 );
					mesh.MaterialFaces( materialFaces );
				}
				if ( numUVs>0 )
				{
					mesh.NumTextureCoords( numUVs );
					mesh.TextureCoords( uvs );
				}
				if ( numNormals>0 )
				{
					mesh.NumNormals( numNormals );
					mesh.Normals( normals );
					mesh.NumNormalFaces( numNormalFaces/3 );
					mesh.NormalFaces( normalFaces );
				}

				obj.AddMesh( mesh );

				numVertices = 0;
				numSurfaces = 0;
				numUVs = 0;
				numNormals = 0;
				numNormalFaces = 0;
				numMaterialFaces = 0;
			}

			// add a material to the object for this mesh
			TMaterial mat;
			TString str = "material" + Int2Str(numMeshes+1);
			mat.MaterialName( str );

			mat.SetMaterialColour( r,g,b,a );
			mat.SetMaterialShininess( shininess );
			mat.SetMaterialEmission( remis,gemis,bemis,aemis );
			mat.SetMaterialSpecularity( rspec,gspec,bspec,aspec );
			if ( usingTexture )
			{
				// get original texture filename
				TMaterial* m;
				bool found = false;
				size_t t = textureId - 1;
				size_t tcounter = 0;
				for ( size_t i=0; i<compareObj->NumMeshes() && !found; i++ )
				{
					size_t j;
					TMesh* mesh = compareObj->GetMesh(i);
					bool* faces = new bool[mesh->NumMaterials()];
					for ( j=0; j<mesh->NumMaterials(); j++ )
					{
						faces[j] = false;
					}
					size_t numMaterialFaces = mesh->NumMaterialFaces();
					size_t* materialFaces = mesh->MaterialFaces();
					for ( j=0; j<numMaterialFaces && !found; j++ )
					{
						size_t mf = materialFaces[j];
						if ( !faces[mf] )
						{
							faces[mf] = true;
							m = mesh->Materials( mf );
							if ( m->FileName().length() > 0 )
							{
								if ( t==tcounter )
									found = true;
								else
									tcounter++;
							}
						}
					}
				}
				
				if ( found )
				{
					mat.FileName( m->FileName() );
					TString matName = m->FileName();
					if ( matName[0]=='[' )
					{
						size_t index = 0;
						while ( matName[index]!='#' && index<matName.length() ) index++;
						if ( matName[index]=='#' )
						{
							matName = matName.substr(1,index-1) + "Material";
						}
						else
						{
							matName = matName.substr(1);
							matName = matName.GetItem('.',0) + "Material";
						}
					}
					else
					{
						matName = matName.GetItem('.',0) + "Material";
					}
					mat.MaterialName( matName );
				}
				else
				{
					mat.FileName( Int2Str(textureId) );
				}
			}
			else // not a texture - just a colour
			{
				float r,g,b,a;
				mat.GetMaterialColour(r,g,b,a);
				if ( r==0 && b==0 && g==0 && (a==0 || a==1) )
				{
					mat.MaterialName( "colourBlack" );
				}
				else if ( r==1 && b==1 && g==1 && (a==0 || a==1) )
				{
					mat.MaterialName( "colourWhite" );
				}
				else
				{
					float rb,gb,bb,ab;
					rb = r * 255;
					gb = g * 255;
					bb = b * 255;
					ab = a * 255;
					char buf[256];
					sprintf( buf, "colour%02X%02X%02X%02X", byte(rb),byte(gb),
															byte(bb),byte(ab) );
					mat.MaterialName( buf );
				}
			}

			obj.AddMaterial( mat );

			// reset texture assumptions
			usingTexture = false;
			textureId = 0;

			numMeshes++;
		}

		if ( started )
		{
			// glNormal3fv( { -0.013661, -0.823048, -0.567808 } )
			if ( line.contains("glNormal3fv(") )
			{
				size_t index = 0;
				while ( line[index]!='{' && index<line.length() ) index++;
				if ( line[index]=='{' )
				{
					index++;
					float f1 = float(atof(&line[index]));
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
						float f2 = float(atof(&line[index]));
						while ( line[index]!=',' && index<line.length() ) index++;
						if ( line[index]==',' )
						{
							index++;
							float f3 = float(atof(&line[index]));

							normalFaces[numNormalFaces++] = numNormals;

							PreCond( numNormals < 10000 );

							normals[numNormals*3+0] = f1;
							normals[numNormals*3+1] = f2;
							normals[numNormals*3+2] = f3;
							numNormals++;
							hasNormal = true;
						}
					};
				}
			}

			// glTexCoord2fv( { 0.886006, 0.649343 } )
			if ( line.contains("glTexCoord2fv(") )
			{
				size_t index = 0;
				while ( line[index]!='{' && index<line.length() ) index++;
				if ( line[index]=='{' )
				{
					index++;
					float f1 = float(atof(&line[index]));
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
						float f2 = float(atof(&line[index]));

						PreCond( numUVs < 10000 );

						uvs[numUVs*2+0] = f1;
						uvs[numUVs*2+1] = f2;
						numUVs++;
						hasUV = true;
					};
				}
			}

			// glArrayElement(0/*[-1.240399,-0.095210,-1.173353]*/)
			if ( line.contains("glArrayElement(") )
			{
				if ( !hasUV )
				{
					uvs[numUVs*2+0] = 0;
					uvs[numUVs*2+1] = 0;
					numUVs++;
				};

				hasUV = false; // for next line
				hasNormal = false;

				size_t index = 0;
				while ( line[index]!='[' && index<line.length() ) index++;
				if ( line[index]=='[' )
				{
					index++;
					float f1 = float(atof(&line[index]));
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
						float f2 = float(atof(&line[index]));
						while ( line[index]!=',' && index<line.length() ) index++;
						if ( line[index]==',' )
						{
							index++;
							float f3 = float(atof(&line[index]));

							surfaces[numSurfaces] = numVertices;
							materialFaces[numSurfaces/3] = numMeshes;
							numSurfaces++;

							PreCond( numVertices < 10000 );

							vertices[numVertices*3+0] = f1;
							vertices[numVertices*3+1] = f2;
							vertices[numVertices*3+2] = f3;
							numVertices++;
						}
					};
				}
			}
		}
		else // scan for materials
		{
			//glBindTexture(GL_TEXTURE_2D,1) [optional]
			if ( line.contains("glBindTexture(") )
			{
				usingTexture = true;
				size_t index = 0;
				while ( line[index]!=',' && index<line.length() ) index++;
				if ( line[index]==',' )
				{
					index++;
					textureId = atoi( &line[index] );
				}
			}

			//glColor4fv( { 1.000000, 1.000000, 1.000000, 1.000000 } )
			if ( line.contains("glColor4fv(") )
			{
				size_t index = 0;
				while ( line[index]!='{' && index<line.length() ) index++;
				if ( line[index]=='{' )
				{
					index++;
					r = float(atof(&line[index]));
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
						g = float(atof(&line[index]));
						while ( line[index]!=',' && index<line.length() ) index++;
						if ( line[index]==',' )
						{
							index++;
							b = float(atof(&line[index]));
							while ( line[index]!=',' && index<line.length() ) index++;
							if ( line[index]==',' )
							{
								index++;
								a = float(atof(&line[index]));
							}
						}
					}
				}
			}


			//glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, { 0.000000, 0.000000, 0.000000, 1.000000 } )
			if ( line.contains("glMaterialfv(") && line.contains("GL_SPECULAR") )
			{
				size_t index = 0;
				while ( line[index]!='{' && index<line.length() ) index++;
				if ( line[index]=='{' )
				{
					index++;
					rspec = float(atof(&line[index]));
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
						gspec = float(atof(&line[index]));
						while ( line[index]!=',' && index<line.length() ) index++;
						if ( line[index]==',' )
						{
							index++;
							bspec = float(atof(&line[index]));
							while ( line[index]!=',' && index<line.length() ) index++;
							if ( line[index]==',' )
							{
								index++;
								aspec = float(atof(&line[index]));
							}
						}
					}
				}
			}

			//glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION, { 0.000000, 0.000000, 0.000000, 1.000000 } )
			if ( line.contains("glMaterialfv(") && line.contains("GL_EMISSION") )
			{
				size_t index = 0;
				while ( line[index]!='{' && index<line.length() ) index++;
				if ( line[index]=='{' )
				{
					index++;
					remis = float(atof(&line[index]));
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
						gemis = float(atof(&line[index]));
						while ( line[index]!=',' && index<line.length() ) index++;
						if ( line[index]==',' )
						{
							index++;
							bemis = float(atof(&line[index]));
							while ( line[index]!=',' && index<line.length() ) index++;
							if ( line[index]==',' )
							{
								index++;
								aemis = float(atof(&line[index]));
							}
						}
					}
				}
			}

			//glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,12.799999)
			if ( line.contains("glMaterialf(") && line.contains("GL_SHININESS") )
			{
				size_t index = 0;
				while ( line[index]!=',' && index<line.length() ) index++;
				if ( line[index]==',' )
				{
					index++;
					while ( line[index]!=',' && index<line.length() ) index++;
					if ( line[index]==',' )
					{
						index++;
					
						shininess = float(atof(&line[index]));
					}
				}
			}


		}
	}
	while ( !eof );

	// add materials to individual meshes too
	for ( size_t i=0; i<obj.NumMeshes(); i++ )
	{
		TMesh* mesh = obj.GetMesh(i);
		mesh->NumMaterials( obj.NumMaterials() );
		for ( size_t j=0; j<obj.NumMaterials(); j++ )
		{
			mesh->Materials( j, j, obj.GetMaterial(j) );
		}
	};
	file.FileClose();
	return true;
};

//==========================================================================


