#include <precomp_header.h>

#include <common/compatability.h>

#include <botCompiler/botgrammar.h>

//==========================================================================

#ifndef _TANKGAME
#ifndef _PARSER
#ifndef _VIEWER
#ifndef _EDITOR

TString _dir;

//==========================================================================

//#define _USEBIN

//
//	Parse bot language
//
void main( int argc, char* argv[] )
{
#ifdef _USEBIN

	BotParser parse;
	TString errStr;

	if ( !parse.LoadBot( "bot1.bin", errStr ) )
	{
		printf( "\nError loading byte-code\n***\n%s\n***\n", errStr.c_str() );
	}
	else
	{
		parse.PrintTree();
	}

#else

	TPersist file(fileRead);

	if ( !file.FileOpen( "bot1.txt" ) )
	{
		printf( "Error opening file 'bot1.txt'\n" );
		exit(0);
	}

	long fsize = file.FileSize();
	char* buf = new char[fsize+1];
	memset(buf,0,fsize+1);

	file.FileRead( buf, fsize );
	file.FileClose();

	BotLex lex;
	lex.SetString( buf );

	TString errStr;
	BotParser parse(lex);

	if ( !parse.Parse( errStr ) )
	{
		printf( "\n***\n%s\n***\n", errStr.c_str() );
	}
	else
	{
		parse.PrintTree();
		if ( !parse.SaveBot( "bot1.bin", errStr ) )
			printf( "\nError saving byte-code\n***\n%s\n***\n", errStr.c_str() );
	}
#endif
};

#endif
#endif
#endif
#endif

//==========================================================================

BotParser::BotParser( void )
	: lex(NULL),
	  initialState(NULL)
{
	signature = "PDVBOT01";

	for ( size_t i=0; i<kMaxStates; i++ )
		states[i] = NULL;

	numStates = 0;
	error = false;
};

BotParser::BotParser( BotLex& _lex )
	: lex(&_lex),
	  initialState(NULL)
{
	signature = "PDVBOT01";

	for ( size_t i=0; i<kMaxStates; i++ )
		states[i] = NULL;

	numStates = 0;
	error = false;
};


BotParser::~BotParser( void )
{
	for ( size_t i=0; i<kMaxStates; i++ )
	{
		if ( states[i]!=NULL )
			delete states[i];
		states[i] = NULL;
	}
	numStates = 0;
	initialState = NULL;
	lex = NULL;
};


void BotParser::PrintTree( void ) const
{
	for ( size_t i=0; i<numStates; i++ )
	{
		states[i]->PrintTree();
	}
};

BotTree* BotParser::InitialState( void ) const
{
	return initialState;
};

bool BotParser::SaveBot( const TString& fname, TString& errStr )
{
	size_t i;

	TPersist file(fileWrite);
	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	file.FileWrite( signature.c_str(), signature.length()+1 );

	// write bot header info
	file.FileWrite( &version, sizeof(float) );
	file.FileWrite( botName.c_str(), botName.length()+1 );
	file.FileWrite( initialStateName.c_str(), initialStateName.length()+1 );

	file.FileWrite( &numStates, sizeof(size_t) );

	for ( i=0; i<numStates; i++ )
	{
		file.FileWrite( stateNames[i].c_str(), stateNames[i].length()+1 );
		states[i]->Write( file, errStr );
	}

	file.FileClose();

	return true;
};


bool BotParser::LoadBot( const TString& fname, TString& errStr )
{
	size_t i;

	TPersist file(fileRead);
	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	TString sig;
	if ( !file.ReadString(sig) )
	{
		errStr = "Error reading file signature";
		return false;
	}
	if ( sig!=signature )
	{
		errStr = "Incorrect file version or type (" + sig + ")";
	}

	file.FileRead( &version, sizeof(float) );
	file.ReadString( botName );
	file.ReadString( initialStateName );

	file.FileRead( &numStates, sizeof(size_t) );
	if ( numStates > kMaxStates )
	{
		errStr = "Bot states exceed maximum number of possible states";
		return false;
	}

	for ( i=0; i<numStates; i++ )
	{
		if ( !file.ReadString(stateNames[i]) )
		{
			errStr = "Error reading file signature";
			return false;
		}
		states[i] = new BotTree(0);
		if ( !states[i]->Read( file, errStr ) )
			return false;
	}

	file.FileClose();

	return true;
};


bool BotParser::CheckToken( long token1, long token2, TString& errStr )
{
    if ( !lex->CheckToken( token1, token2 ) ) 
	{
		errStr = lex->ErrorString();
		error = true;
		return false;
	}
	return true;
};


bool BotParser::Float( float& f, TString& errStr )
{
	long token = lex->GetNextToken();
    if ( token==BotLex::lexRealNumber )
	{
		f = float(lex->yyReal);
		return true;
	}
	if ( token==BotLex::lexIntNumber )
	{
		f = float(lex->yyInt);
		return true;
	}
	CheckToken( BotLex::lexRealNumber, token, errStr ); // force error
   	return false;
};


bool BotParser::Int( int& i, TString& errStr )
{
	long token = lex->GetNextToken();
    if ( !CheckToken( BotLex::lexIntNumber, token, errStr ) )
    	return false;
	i = lex->yyInt;
    return true;
};


bool BotParser::String( TString& str, TString& errStr )
{
	long token = lex->GetNextToken();
    if ( !CheckToken( BotLex::lexStringLiteral, token, errStr ) )
    	return false;
	str = lex->yyString;
    return true;
};


bool BotParser::Parse( TString& errStr )
{
	if ( !ParseInit( errStr ) )
		return false;

	TString state;
	BotTree* BotTree;
	while ( GetState( state, BotTree, errStr ) )
	{
		if ( error )
			return false;
		AddNewState( state, BotTree );
	};

	if ( initialState==NULL )
	{
		errStr = "Error: Initial state [" + initialStateName + "] not found\n";
		return false;
	}
	return true;
};


// parse:
//[init]
//type=tankbot
//version=1.0
//name="Tank Bot 1"
//state=[guard]
//
bool BotParser::ParseInit( TString& errStr )
{
	// [init]
	long token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexInit, token, errStr ) )
		return false;

	// type=tankbot
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexType, token, errStr ) )
		return false;
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
		return false;
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexTankBot, token, errStr ) )
		return false;

	// version=1.0
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexVersion, token, errStr ) )
		return false;
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
		return false;
	if ( !Float( version, errStr ) )
		return false;

	// name="str"
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexName, token, errStr ) )
		return false;
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
		return false;
	if ( !String( botName, errStr ) )
		return false;

	// state=initialState
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexState, token, errStr ) )
		return false;
	token = lex->GetNextToken();
	if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
		return false;
	if ( !String( initialStateName, errStr ) )
		return false;

	return true;
};

BotTree* BotParser::GetState( const TString& state ) const
{
	for ( size_t i=0; i<numStates; i++ )
		if ( stateNames[i]==state )
			return states[i];
	return NULL;
};

bool BotParser::GetState( TString& state, BotTree*& stateTree,
					   TString& errStr )
{
	if ( !String( state, errStr ) )
		return false;

	BotTree* statement = NULL;
	if ( !Statement( statement, errStr ) )
		return false;

	stateTree = new BotTree( tstateId, state, statement );
	return true;
};


bool BotParser::Statement( BotTree*& statement, TString& errStr )
{
	statement = NULL;

	long token = lex->GetNextToken();
	switch ( token )
	{
	case BotLex::lexState:
		{
			TString newState;
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
				return false;
			if ( !String( newState, errStr ) )
				return false;
			statement = new BotTree( tsetState, newState, NULL );
			break;
		}
	case BotLex::lexPrimaryTarget:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
				return false;
			BotTree* loc;
			if ( !Location( loc, errStr ) )
			{
				errStr = "Expected location following primaryTarget=\n";
				return false;
			}
			statement = new BotTree( tprimaryTarget, loc, NULL );
			break;
		}
	case BotLex::lexSecondaryTarget:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexAssign, token, errStr ) )
				return false;
			BotTree* loc;
			if ( !Location( loc, errStr ) )
			{
				errStr = "Expected location following secondaryTarget=\n";
				return false;
			}
			statement = new BotTree( tsecondaryTarget, loc, NULL );
			break;
		}
	case BotLex::lexTurnTowards:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
				return false;
			BotTree* loc;
			if ( !Location( loc, errStr ) )
			{
				errStr = "Expected location following TurnTowards\n";
				return false;
			}
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexRight, token, errStr ) )
				return false;
			statement = new BotTree( tturnTowards, loc, NULL );
			break;
		}
	case BotLex::lexTurnTurretTowards:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
				return false;
			BotTree* loc;
			if ( !Location( loc, errStr ) )
			{
				errStr = "Expected location following TurnTurretTowards\n";
				return false;
			}
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexRight, token, errStr ) )
				return false;
			statement = new BotTree( tturnTurretTowards, loc, NULL );
			break;
		}
	case BotLex::lexMoveTo:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
				return false;
			BotTree* loc;
			if ( !Location( loc, errStr ) )
			{
				errStr = "Expected location following TurnTurretTowards\n";
				return false;
			}
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexRight, token, errStr ) )
				return false;
			statement = new BotTree( tmoveTo, loc, NULL );
			break;
		}
	case BotLex::lexFireAt:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
				return false;
			BotTree* loc;
			if ( !Location( loc, errStr ) )
			{
				errStr = "Expected location following TurnTurretTowards\n";
				return false;
			}
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexRight, token, errStr ) )
				return false;
			statement = new BotTree( tfireAt, loc, NULL );
			break;
		}
	case BotLex::lexIf:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
				return false;
			BotTree* expr;
			if ( !BooleanExpr( expr, errStr ) )
				return false;
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexRight, token, errStr ) )
				return false;
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexCurlyLeft, token, errStr ) )
				return false;
			BotTree* body;
			Statement( body, errStr );
			if ( error )
				return false;
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexCurlyRight, token, errStr ) )
				return false;

			BotTree* _else = NULL;
			token = lex->GetNextToken();
			if ( token==BotLex::lexElse )
			{
				token = lex->GetNextToken();
				if ( token==BotLex::lexIf )
				{
					lex->UngetToken();
					if ( !Statement( _else, errStr ) )
						return false;
				}
				else if ( token==BotLex::lexCurlyLeft )
				{
					if ( !Statement( _else, errStr ) )
						return false;
					token = lex->GetNextToken();
					if ( !CheckToken( BotLex::lexCurlyRight, token, errStr ) )
						return false;
				}
				else
				{
					error = true;
					errStr = "Error: else can only be followed by '{' or 'if'\n";
					return false;
				}
			}
			else
			{
				lex->UngetToken();
			}
			statement = new BotTree( tif, expr, body, _else );
			break;
		}
	case BotLex::lexForEachI:
		{
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexCurlyLeft, token, errStr ) )
				return false;
			BotTree* body;
			Statement( body, errStr );
			if ( error )
				return false;
			token = lex->GetNextToken();
			if ( !CheckToken( BotLex::lexCurlyRight, token, errStr ) )
				return false;

			statement = new BotTree( tforeachi, body, NULL );
			break;
		}
	}

	if ( statement!=NULL )
	{
		BotTree* extraStatement;
		if ( Statement( extraStatement, errStr ) )
		{
			statement->Next( extraStatement );
		}
		return !error;
	}

	lex->UngetToken();
	return false;
};


bool BotParser::Location( BotTree*& location, TString& errStr )
{
	long token = lex->GetNextToken();
	switch ( token )
	{
	case BotLex::lexMe:
		{
			location = new BotTree( tme );
			return true;
		}
	case BotLex::lexInitPos:
		{
			location = new BotTree( tinitPos );
			return true;
		}
	case BotLex::lexPrimaryTarget:
		{
			location = new BotTree( tprimaryTarget );
			return true;
		}
	case BotLex::lexSecondaryTarget:
		{
			location = new BotTree( tsecondaryTarget );
			return true;
		}
	case BotLex::lexNULL:
		{
			location = new BotTree( tnull );
			return true;
		}
	case BotLex::lexNearestFuel:
		{
			location = new BotTree( tnearestFuel );
			return true;
		}
	case BotLex::lexNearestAmmo:
		{
			location = new BotTree( tnearestAmmo );
			return true;
		}
	case BotLex::lexNearestHealth:
		{
			location = new BotTree( tnearestHealth );
			return true;
		}
	case BotLex::lexI:
		{
			location = new BotTree( tvar_i );
			return true;
		}
	};

	lex->UngetToken();
	return false;
};

size_t BotParser::Lex2BotParser( long token )
{
	switch (token)
	{
	case BotLex::lexLessThan:
		return tlessthan;
	case BotLex::lexGreaterThan:
		return tgreaterthan;
	case BotLex::lexGreaterEqualThan:
		return tgreaterequalthan;
	case BotLex::lexLessEqualThan:
		return tlessequalthan;
	case BotLex::lexEqual:
		return tequal;
	case BotLex::lexNotEqual:
		return tnotequal;
	case BotLex::lexOr:
		return tor;
	case BotLex::lexAnd:
		return tand;
	case BotLex::lexPlus:
		return tplus;
	case BotLex::lexMinus:
		return tminus;
	case BotLex::lexTimes:
		return ttimes;
	case BotLex::lexDivide:
		return tdivide;
	default:
		return 0;
	}
};

bool BotParser::BooleanExpr( BotTree*& expr, TString& errStr )
{
	long token = lex->GetNextToken();
	if ( token==BotLex::lexLeft )
	{
		if ( !BooleanExpr( expr, errStr ) )
			return false;
		token = lex->GetNextToken();
		if ( !CheckToken( BotLex::lexRight, token, errStr ) )
			return false;
	}
	else if ( token==BotLex::lexExclamation )
	{
		BotTree* ex;
		if ( !BooleanExpr( ex, errStr ) )
			return false;
		expr = new BotTree( tnot, ex, NULL );
	}
	else
	{
		lex->UngetToken();
		BotTree* ex1;
		if ( !Expression( ex1, errStr ) )
			return false;
		token = lex->GetNextToken();
		if ( token==BotLex::lexLessThan ||
			 token==BotLex::lexGreaterThan ||
			 token==BotLex::lexGreaterEqualThan ||
			 token==BotLex::lexLessEqualThan ||
			 token==BotLex::lexEqual ||
			 token==BotLex::lexNotEqual ||
			 token==BotLex::lexOr ||
			 token==BotLex::lexAnd ||
			 token==BotLex::lexPlus ||
			 token==BotLex::lexMinus ||
			 token==BotLex::lexTimes ||
			 token==BotLex::lexDivide )
		{
			BotTree* ex2;
			if ( !Expression( ex2, errStr ) )
				return false;
			expr = new BotTree( Lex2BotParser(token), ex1, ex2 );

			token = lex->GetNextToken();
			if ( token==BotLex::lexLessThan ||
				 token==BotLex::lexGreaterThan ||
				 token==BotLex::lexGreaterEqualThan ||
				 token==BotLex::lexLessEqualThan ||
				 token==BotLex::lexEqual ||
				 token==BotLex::lexNotEqual ||
				 token==BotLex::lexOr ||
				 token==BotLex::lexAnd ||
				 token==BotLex::lexPlus ||
				 token==BotLex::lexMinus ||
				 token==BotLex::lexTimes ||
				 token==BotLex::lexDivide )
			{
				BotTree* ex2;
				if ( !BooleanExpr( ex2, errStr ) )
					return false;
				expr = new BotTree( Lex2BotParser(token), expr, ex2 );
			}
			else
			{
				lex->UngetToken();
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
};


bool BotParser::Expression( BotTree*& expr, TString& errStr )
{
	long token = lex->GetNextToken();
	switch (token)
	{
	case BotLex::lexRealNumber:
	{
		expr = new BotTree( vfloat, float(lex->yyReal) );
		return true;
	}
	case BotLex::lexIntNumber:
	{
		expr = new BotTree( vfloat, float(lex->yyInt) );
		return true;
	}
	case BotLex::lexAmmo:
	{
		expr = new BotTree( tammo );
		return true;
	}
	case BotLex::lexHealth:
	{
		expr = new BotTree( thealth );
		return true;
	}
	case BotLex::lexFuel:
	{
		expr = new BotTree( tfuel );
		return true;
	}
	case BotLex::lexShields:
	{
		expr = new BotTree( tshields );
		return true;
	}
	case BotLex::lexDistance:
	{
		BotTree* loc1;
		BotTree* loc2;
		token = lex->GetNextToken();
		if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
			return false;
		if ( !Location( loc1, errStr ) )
			return false;
		token = lex->GetNextToken();
		if ( !CheckToken( BotLex::lexComma, token, errStr ) )
			return false;
		if ( !Location( loc2, errStr ) )
			return false;
		token = lex->GetNextToken();
		if ( !CheckToken( BotLex::lexRight, token, errStr ) )
			return false;
		expr = new BotTree( tdistance, loc1, loc2 );
		return true;
	}
	case BotLex::lexRandom:
	{
		BotTree* ex;
		token = lex->GetNextToken();
		if ( !CheckToken( BotLex::lexLeft, token, errStr ) )
			return false;
		if ( !Expression( ex, errStr ) )
			return false;
		token = lex->GetNextToken();
		if ( !CheckToken( BotLex::lexRight, token, errStr ) )
			return false;
		expr = new BotTree( trandom, ex, NULL );
		return true;
	}
	default:
	{
		lex->UngetToken();
		if ( !Location( expr, errStr ) )
			return false;
		return true;
	}
	};
};


void BotParser::AddNewState( const TString& name, BotTree* BotTree )
{
	if ( name==initialStateName )
		initialState = BotTree;
	stateNames[numStates] = name;
	states[numStates++] = BotTree;
};


TString BotParser::ToString( size_t tag )
{
	switch ( tag )
	{
	case tstateId:
		return "stateId";
	case tsetState:
		return "setState";
	case tinitPos:
		return "initPos";
	case tprimaryTarget:
		return "primaryTarget";
	case tsecondaryTarget:
		return "secondaryTarget";
	case tnull:
		return "null";
	case tnearestFuel:
		return "nearestFuel";
	case tnearestAmmo:
		return "nearestAmmo";
	case tnearestHealth:
		return "nearestHealth";
	case tvar_i:
		return "i";
	case tturnTowards:
		return "turnTowards";
	case tturnTurretTowards:
		return "turnTurretTowards";
	case tmoveTo:
		return "moveTo";
	case tfireAt:
		return "fireAt";
	case tif:
		return "if";
	case telse:
		return "else";
	case tforeachi:
		return "foreachi";
	case tnot:
		return "not";
	case vfloat:
		return "float";
	case tdistance:
		return "distance";
	case trandom:
		return "random";
	case tme:
		return "me";
	case tlessthan:
		return "<";
	case tgreaterthan:
		return ">";
	case tgreaterequalthan:
		return ">=";
	case tlessequalthan:
		return "<=";
	case tequal:
		return "==";
	case tnotequal:
		return "!=";
	case tor:
		return "||";
	case tand:
		return "&&";
	case tplus:
		return "+";
	case tminus:
		return "-";
	case ttimes:
		return "*";
	case tdivide:
		return "\\";
	case tfuel:
		return "fuel";
	case tammo:
		return "ammo";
	case thealth:
		return "health";
	case tshields:
		return "shields";
	case tplayerId:
		return "playerId";

	default:
		return "???";
	}
};


void BotParser::Indent( TString& str, size_t indent )
{
	for ( size_t i=0; i<indent; i++ )
		str = str + "  ";
};


TString BotParser::TreeToString( BotTree* BotTree, size_t indent )
{
	TString retStr;

	if ( BotTree==NULL )
		return "";

	switch ( BotTree->Tag() )
	{
	case tstateId:
		{
			retStr = "[" + BotTree->Name() + "]\n";
			retStr = retStr + TreeToString( BotTree->Left() );
			break;
		}

	case tsetState:
		{
			Indent( retStr, indent );
			retStr = retStr + "state = [" + BotTree->Name() + "]\n";
			break;
		}

	case tinitPos:
		{
			if ( BotTree->Left()!=NULL )
			{
				Indent( retStr, indent );
				retStr = retStr + "InitPos = " + TreeToString( BotTree->Left() ) + "\n";
			}
			else
				retStr = "InitPos";
			break;
		}

	case tprimaryTarget:
		{
			if ( BotTree->Left()!=NULL )
			{
				Indent( retStr, indent );
				retStr = retStr + "PrimaryTarget = " + TreeToString( BotTree->Left() ) + "\n";
			}
			else
				retStr = "PrimaryTarget";
			break;
		}
	case tsecondaryTarget:
		{
			if ( BotTree->Left()!=NULL )
			{
				Indent( retStr, indent );
				retStr = retStr + "SecondaryTarget = " + TreeToString( BotTree->Left() ) + "\n";
			}
			else
				retStr = "SecondaryTarget";
			break;
		}
	case tnull:
		{
			retStr = "null";
			break;
		}
	case tnearestFuel:
		{
			Indent( retStr, indent );
			retStr = retStr + "NearestFuel()\n";
			break;
		}
	case tnearestAmmo:
		{
			Indent( retStr, indent );
			retStr = retStr + "NearestAmmo()\n";
			break;
		}
	case tnearestHealth:
		{
			Indent( retStr, indent );
			retStr = retStr + "NearestHealth()\n";
			break;
		}
	case tvar_i:
		{
			retStr = "i";
			break;
		}
	case tturnTowards:
		{
			Indent( retStr, indent );
			retStr = retStr + "TurnTowards( " + TreeToString(BotTree->Left()) + " )\n";
			break;
		}
	case tturnTurretTowards:
		{
			Indent( retStr, indent );
			retStr = retStr + "TurnTurretTowards( " + TreeToString(BotTree->Left()) + " )\n";
			break;
		}
	case tmoveTo:
		{
			Indent( retStr, indent );
			retStr = retStr + "MoveTo( " + TreeToString(BotTree->Left()) + " )\n";
			break;
		}
	case tfireAt:
		{
			Indent( retStr, indent );
			retStr = retStr + "FireAt( " + TreeToString(BotTree->Left()) + " )\n";
			break;
		}
	case tif:
		{
			Indent( retStr, indent );
			retStr = retStr + "if ( " + TreeToString(BotTree->Left(),indent) + " )\n";
			Indent( retStr, indent );
			retStr = retStr + "{\n";
			retStr = retStr + TreeToString(BotTree->Right(),indent+1);
			Indent( retStr, indent );
			retStr = retStr + "}\n";
			if ( BotTree->ElsePart()!=NULL )
			{
				retStr = retStr + "else\n";
				Indent( retStr, indent );
				retStr = retStr + "{\n";
				retStr = retStr + TreeToString(BotTree->ElsePart(),indent+1);
				Indent( retStr, indent );
				retStr = retStr + "}\n";
			}
			break;
		}
	case telse:
		{
			Indent( retStr, indent );
			retStr = retStr + "else";
			break;
		}
	case tforeachi:
		{
			Indent( retStr, indent );
			retStr = retStr + "for each in in others\n";
			Indent( retStr, indent );
			retStr = retStr + "{\n";
			retStr = retStr + TreeToString(BotTree->Left(),indent+1);
			Indent( retStr, indent );
			retStr = retStr + "}\n";
			break;
		}
	case tnot:
		{
			retStr = "!";
			break;
		}
	case vfloat:
		{
			retStr = FloatToString( 1,1, BotTree->Value() );
			break;
		}
	case tdistance:
		{
			retStr = "Distance( " + TreeToString(BotTree->Left()) + ", ";
			retStr = retStr + TreeToString(BotTree->Right()) + " )";
			break;
		}
	case trandom:
		{
			retStr = "Random( " + TreeToString(BotTree->Left()) + ")";
			break;
		}
	case tme:
		{
			retStr = "me";
			break;
		}
	case tlessthan:
		{
			retStr = TreeToString(BotTree->Left()) + " < ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tgreaterthan:
		{
			retStr = TreeToString(BotTree->Left()) + " > ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tgreaterequalthan:
		{
			retStr = TreeToString(BotTree->Left()) + " >= ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tlessequalthan:
		{
			retStr = TreeToString(BotTree->Left()) + " <= ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tequal:
		{
			retStr = TreeToString(BotTree->Left()) + " == ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tnotequal:
		{
			retStr = TreeToString(BotTree->Left()) + " != ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tor:
		{
			retStr = TreeToString(BotTree->Left()) + " || ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tand:
		{
			retStr = TreeToString(BotTree->Left()) + " && ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tplus:
		{
			retStr = TreeToString(BotTree->Left()) + " + ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tminus:
		{
			retStr = TreeToString(BotTree->Left()) + " - ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case ttimes:
		{
			retStr = TreeToString(BotTree->Left()) + " * ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tdivide:
		{
			retStr = TreeToString(BotTree->Left()) + " \\ ";
			retStr = retStr + TreeToString(BotTree->Right());
			break;
		}
	case tfuel:
		{
			retStr = "fuel";
			break;
		}
	case tammo:
		{
			retStr = "ammo";
			break;
		}
	case thealth:
		{
			retStr = "health";
			break;
		}
	case tshields:
		{
			retStr = "shields";
			break;
		}
	case tplayerId:
		{
			retStr = "playerId";
			break;
		}

	default:
		{
			retStr = "???\n";
			break;
		}
	}

	if ( BotTree->Next()!=NULL )
	{
		retStr = retStr + TreeToString( BotTree->Next(), indent );
	}

	return retStr;
};

//==========================================================================

BotTree::BotTree( void )
	: tag(tplayerId),
	  left(NULL),
	  next(NULL),
	  elsepart(NULL),
	  right(NULL)
{
	value = 0;
};

BotTree::BotTree( size_t _tag )
	: tag(_tag),
	  left(NULL),
	  next(NULL),
	  elsepart(NULL),
	  right(NULL)
{
	value = 0;
};

BotTree::BotTree( size_t _tag, float v )
	: tag(_tag),
	  left(NULL),
	  next(NULL),
	  elsepart(NULL),
	  right(NULL)
{
	value = v;
};

BotTree::BotTree( size_t _tag, BotTree* _left, BotTree* _right )
	: tag(_tag),
	  left( _left ),
	  next(NULL),
	  elsepart(NULL),
	  right( _right )
{
	value = 0;
};

BotTree::BotTree( size_t _tag, BotTree* _left, BotTree* _right, BotTree* _else )
	: tag(_tag),
	  left( _left ),
	  next(NULL),
	  elsepart(_else),
	  right( _right )
{
	value = 0;
};

BotTree::BotTree( size_t _tag, const TString& _name, BotTree* _left )
	: tag(_tag),
	  left( _left ),
	  next(NULL),
	  elsepart(NULL),
	  right( NULL )
{
	name = _name;
	value = 0;
};

BotTree::~BotTree( void )
{
	BotTree* temp;

	tag = 0;
	if ( left!=NULL )
	{
		temp = left;
		left = NULL;
		delete temp;
	}

	if ( right!=NULL )
	{
		temp = right;
		right = NULL;
		delete temp;
	}

	if ( elsepart!=NULL )
	{
		temp = elsepart;
		elsepart = NULL;
		delete temp;
	}

	if ( next!=NULL )
	{
		temp = next;
		next = NULL;
		delete temp;
	}
	value = 0;
};

size_t BotTree::Tag( void ) const
{
	return tag;
};

void BotTree::Tag( size_t t )
{
	tag = t;
};

float BotTree::Value( void ) const
{
	return value;
}

void BotTree::Value( float _value )
{
	value = _value;
}

BotTree* BotTree::Left( void ) const
{
	return left;
};

void BotTree::Left( BotTree* t )
{
	left = t;
};

BotTree* BotTree::Right( void ) const
{
	return right;
};

void BotTree::Right( BotTree* t )
{
	right = t;
};

BotTree* BotTree::Next( void ) const
{
	return next;
};

void BotTree::Next( BotTree* t )
{
	next = t;
};

BotTree* BotTree::ElsePart( void ) const
{
	return elsepart;
};

const TString& BotTree::Name( void ) const
{
	return name;
};

void BotTree::Name( const TString& _name )
{
	name = _name;
};

bool BotTree::Write( TPersist& file, TString& errStr )
{
	// write node info
	file.FileWrite( &tag, sizeof(size_t) );
	file.FileWrite( name.c_str(), name.length()+1 );
	file.FileWrite( &value, sizeof(float) );

	// recursively write all other parts to file
	bool present;
	if ( left!=NULL )
	{
		present = true;
		file.FileWrite( &present, sizeof(bool) );
		if ( !left->Write( file, errStr ) )
			return false;
	}
	else
	{
		present = false;
		file.FileWrite( &present, sizeof(bool) );
	}

	if ( right!=NULL )
	{
		present = true;
		file.FileWrite( &present, sizeof(bool) );
		if ( !right->Write( file, errStr ) )
			return false;
	}
	else
	{
		present = false;
		file.FileWrite( &present, sizeof(bool) );
	}

	if ( elsepart!=NULL )
	{
		present = true;
		file.FileWrite( &present, sizeof(bool) );
		if ( !elsepart->Write( file, errStr ) )
			return false;
	}
	else
	{
		present = false;
		file.FileWrite( &present, sizeof(bool) );
	}

	if ( next!=NULL )
	{
		present = true;
		file.FileWrite( &present, sizeof(bool) );
		if ( !next->Write( file, errStr ) )
			return false;
	}
	else
	{
		present = false;
		file.FileWrite( &present, sizeof(bool) );
	}
	return true;
};

bool BotTree::Read( TPersist& file, TString& errStr )
{
	// read node info
	if ( !file.FileRead( &tag, sizeof(size_t) ) )
	{
		errStr = "Error reading tag from node";
		return false;
	}

	if ( !file.ReadString( name ) )
	{
		errStr = "Error reading name from node";
		return false;
	}

	if ( !file.FileRead( &value, sizeof(float) ) )
	{
		errStr = "Error reading value from node";
		return false;
	}

	bool present;
	file.FileRead( &present, sizeof(bool) );
	if ( present )
	{
		left = new BotTree(0);
		if ( !left->Read( file, errStr ) )
			return false;
	}
	else
	{
		left = NULL;
	}

	file.FileRead( &present, sizeof(bool) );
	if ( present )
	{
		right = new BotTree(0);
		if ( !right->Read( file, errStr ) )
			return false;
	}
	else
	{
		right = NULL;
	}

	file.FileRead( &present, sizeof(bool) );
	if ( present )
	{
		elsepart = new BotTree(0);
		if ( !elsepart->Read( file, errStr ) )
			return false;
	}
	else
	{
		elsepart = NULL;
	}

	file.FileRead( &present, sizeof(bool) );
	if ( present )
	{
		next = new BotTree(0);
		if ( !next->Read( file, errStr ) )
			return false;
	}
	else
	{
		next = NULL;
	}

	return true;
};

void BotTree::Indent( size_t level )
{
	for ( size_t i=0; i<level; i++ )
		printf( " " );
};

void BotTree::PrintTree( void )
{
	TString tagStr = BotParser::TreeToString( this );
	printf( "%s\n", tagStr.c_str() );
};

//==========================================================================

