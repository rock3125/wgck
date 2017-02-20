#include <precomp_header.h>

#include <common/compatability.h>

#include <botCompiler/botparser.h>

//==========================================================================

BotLex::BotLex( void )
{
	// init my vars
	yyString = "";
	parse = "";
    errorString = "";
    yyInt = 0;
    yyReal = 0.0;
    lineCntr = 1;
    charCntr = 1;
	bufferSize = 0;

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


BotLex::~BotLex( void )
{
};


void BotLex::SetString( const TString& parseString )
{
	index = 0;
    prevIndex = 0;
   	errorIndex = 0;
	errorToken = 0;
    errorString = "";
	yyString = "";
    prevToken = 0;
    currToken = 0;
    lineCntr = 1;
    charCntr = 1;
    error = false;

    parse = parseString;
	bufferSize = parse.length()+1;
};


const TString& BotLex::ErrorString( void ) const
{
	return errorString;
}


long BotLex::GetNextToken( void )
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


long BotLex::GetNextToken( long& _prevToken )
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
        return lexEOF;
    };

    //
    //	pre-defined constants
    //
    if ( strncmp( &parse[index], "[init]", 6 )==0 )
    {
    	index += 6;
        return lexInit;
    }

    if ( strncmp( &parse[index], "type", 4 )==0 )
    {
    	index += 4;
        return lexType;
    }

    if ( strncmp( &parse[index], "version", 7 )==0 )
    {
    	index += 7;
        return lexVersion;
    }

    if ( strncmp( &parse[index], "name", 4 )==0 )
    {
    	index += 4;
        return lexName;
    }

    if ( strncmp( &parse[index], "tankbot", 7 )==0 )
    {
    	index += 7;
        return lexTankBot;
    }

    if ( strncmp( &parse[index], "others", 6 )==0 )
    {
    	index += 6;
        return lexOthers;
    }

    if ( strncmp( &parse[index], "me", 2 )==0 )
    {
    	index += 2;
        return lexMe;
    }

    if ( strncmp( &parse[index], "state", 5 )==0 )
    {
    	index += 5;
        return lexState;
    }

    if ( strncmp( &parse[index], "initPos", 7 )==0 )
    {
    	index += 7;
        return lexInitPos;
    }

    if ( strncmp( &parse[index], "fuel", 4 )==0 )
    {
    	index += 4;
        return lexFuel;
    }

    if ( strncmp( &parse[index], "shields", 7 )==0 )
    {
    	index += 7;
        return lexShields;
    }

    if ( strncmp( &parse[index], "ammo", 4 )==0 )
    {
    	index += 4;
        return lexAmmo;
    }

    if ( strncmp( &parse[index], "health", 6 )==0 )
    {
    	index += 6;
        return lexHealth;
    }

    if ( strncmp( &parse[index], "destination", 11 )==0 )
    {
    	index += 11;
        return lexDestination;
    }

    if ( strncmp( &parse[index], "primaryTarget", 13 )==0 )
    {
    	index += 13;
        return lexPrimaryTarget;
    }

    if ( strncmp( &parse[index], "secondaryTarget", 15 )==0 )
    {
    	index += 15;
        return lexSecondaryTarget;
    }

    if ( strncmp( &parse[index], "NULL", 4 )==0 )
    {
    	index += 4;
        return lexNULL;
    }

    if ( strncmp( &parse[index], "Distance", 8 )==0 )
    {
    	index += 8;
        return lexDistance;
    }

    if ( strncmp( &parse[index], "Random", 6 )==0 )
    {
    	index += 6;
        return lexRandom;
    }

    if ( strncmp( &parse[index], "TurnTowards", 11 )==0 )
    {
    	index += 11;
        return lexTurnTowards;
    }

    if ( strncmp( &parse[index], "TurnTurretTowards", 17 )==0 )
    {
    	index += 17;
        return lexTurnTurretTowards;
    }

    if ( strncmp( &parse[index], "MoveTo", 6 )==0 )
    {
    	index += 6;
        return lexMoveTo;
    }

    if ( strncmp( &parse[index], "FireAt", 6 )==0 )
    {
    	index += 6;
        return lexFireAt;
    }

    if ( strncmp( &parse[index], "NearestFuel", 11 )==0 )
    {
    	index += 11;
        return lexNearestFuel;
    }

    if ( strncmp( &parse[index], "NearestAmmo", 11 )==0 )
    {
    	index += 11;
        return lexNearestAmmo;
    }

    if ( strncmp( &parse[index], "NearestHealth", 13 )==0 )
    {
    	index += 13;
        return lexNearestHealth;
    }

    if ( strncmp( &parse[index], "if", 2 )==0 )
    {
    	index += 2;
        return lexIf;
    }

    if ( strncmp( &parse[index], "else", 4 )==0 )
    {
    	index += 4;
        return lexElse;
    }

    if ( strncmp( &parse[index], "for each i in others", 20 )==0 )
    {
    	index += 20;
        return lexForEachI;
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
/*
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
*/
        case '{':
        {
        	index++;
        	return lexCurlyLeft;
        };

        case '+':
        {
        	index++;
        	return lexPlus;
        };

        case '-':
        {
        	index++;
        	return lexMinus;
        };

        case '*':
        {
        	index++;
        	return lexTimes;
        };

		case 'i':
		{
			index++;
			return lexI;
		};

		case '|':
		{
			index++;
			if ( parse[index]=='|' )
			{
				index++;
				return lexOr;
			}
			index--;
			break;
		}

		case '&':
		{
			index++;
			if ( parse[index]=='&' )
			{
				index++;
				return lexAnd;
			}
			index--;
			break;
		}

		case '!':
		{
			index++;
			if ( parse[index]=='=' )
			{
				index++;
				return lexNotEqual;
			}
			return lexExclamation;
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
			if ( parse[index]=='=' )
			{
				index++;
				return lexLessEqualThan;
			}
        	return lexLessThan;
        };

        case '>':
        {
        	index++;
 			if ( parse[index]=='=' )
			{
				index++;
				return lexGreaterEqualThan;
			}
			return lexGreaterThan;
        };

        case '=':
        {
        	index++;
  			if ( parse[index]=='=' )
			{
				index++;
				return lexEqual;
			}
			return lexAssign;
        };
    }

    //
    // Literal string?
    //
    if ( parse[index]=='"' || parse[index]=='[' )
    {
    	index++;
    	long strIndex = 0;
        bool found = (parse[index]=='"' || parse[index]==']' );
		char str[256];
        while ( !found && parse[index]!=0 && strIndex<200 )
        {
           	str[strIndex++] = parse[index++];
            if ( parse[index]=='"' || parse[index]==']' )
            {
            	found = true;
                index++;
            }
        }
        str[strIndex] = 0;
		yyString = str;

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


bool BotLex::WhiteSpace( char ch )
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


bool BotLex::CheckToken( long token1, long token2 )
{
	if ( token1!=token2 )
    {
    	Error( lexErrorUnexpectedToken );
        TString buf = ", expected token '"+ToString(token1)+"' but found token '"+ToString(token2)+"'" ;
        errorString = errorString + buf;
        return false;
    }
    return true;
};


bool BotLex::Error( long token )
{
	if ( token==lexErrorUnknownSymbol ||
    	 token==lexErrorUnexpectedToken ||
    	 token==lexErrorUnterminatedString ||
    	 token==lexErrorMalformedReal ||
    	 token==lexErrorMalformedScientificReal )
    {
    	errorIndex = index;
        errorToken = token;
        switch ( token )
        {
        	case lexErrorUnknownSymbol:
		        errorString = "Error at line " + Int2Str(lineCntr) + ", char "+ Int2Str(charCntr) + "\n";
				errorString = errorString + "Unknown symbol";
                break;

        	case lexErrorUnterminatedString:
		        errorString = "Error at line " + Int2Str(lineCntr) + ", char "+ Int2Str(charCntr) + "\n";
				errorString = errorString + "Unterminated literal string";
                break;

        	case lexErrorMalformedReal:
		        errorString = "Error at line " + Int2Str(lineCntr) + ", char "+ Int2Str(charCntr) + "\n";
				errorString = errorString + "Malformed real";
                break;

        	case lexErrorMalformedScientificReal:
		        errorString = "Error at line " + Int2Str(lineCntr) + ", char "+ Int2Str(charCntr) + "\n";
				errorString = errorString + "Malformed exponential on real";
                break;

        	case lexErrorUnexpectedToken:
		        errorString = "Error at line " + Int2Str(lineCntr) + ", char "+ Int2Str(charCntr) + "\n";

				char temp[50];
				strncpy( temp, &parse[index-20], 40 );
				temp[40] = 0;
				strcat(temp,"\n");
				errorString = errorString + temp;
                break;

        	default:
		        errorString = "Unknown Error at line " + Int2Str(lineCntr) + ", char "+ Int2Str(charCntr) + "\n";
                break;
        };
        error = true;
    	return true;
    }
    return false;
};


bool BotLex::Error( void )
{
	return error;
}


void BotLex::UngetToken( void )
{
	index = prevIndex;
};


TString BotLex::ToString( long token )
{
	switch ( token )
    {
		case lexEOF:
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

		case lexOthers:
        	return "others";

		case lexMe:
        	return "me";

		case lexState:
        	return "state";

		case lexInitPos:
        	return "initPos";

		case lexFuel:
        	return "fuel";

		case lexShields:
        	return "shields";

		case lexAmmo:
			return "ammo";

		case lexHealth:
			return "health";

		case lexDestination:
        	return "destination";

		case lexPrimaryTarget:
        	return "primaryTarget";

		case lexSecondaryTarget:
        	return "secondaryTarget";

		case lexNULL:
        	return "NULL";

		case lexDistance:
			return "Distance";

		case lexRandom:
        	return "Random";

		case lexTurnTowards:
        	return "TurnTowards";

		case lexTurnTurretTowards:
        	return "TurnTurretTowards";

		case lexMoveTo:
        	return "MoveTo";

		case lexFireAt:
        	return "FireAt";

		case lexNearestFuel:
        	return "NearestFuel";

		case lexNearestAmmo:
        	return "NearestAmmo";

		case lexNearestHealth:
        	return "NearestHealth";

		case lexIf:
        	return "if";

		case lexElse:
        	return "else";

		case lexForEachI:
        	return "for each i in others";

		case lexInit:
			return "[init]";

		case lexType:
			return "type";

		case lexVersion:
			return "version";

		case lexName:
			return "name";

		case lexTankBot:
			return "tankbot";

		case lexI:
			return "i";

		case lexExclamation:
			return "!";

        default:
        	break;
    };
    return "? (unknown)";
};


TString BotLex::Trim( const TString& str )
{
    long index = 0;
    while ( str[index]==' ' ) index++;

	TString temp = &str[index];
    index = temp.length()-1;
    while ( temp[index]==' ' && index>0 ) index--;
    temp[index+1] = 0;

	TString temp2 = temp;
	return temp2;
};

//==========================================================================

