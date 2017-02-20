#ifndef _BOTS_PARSER_
#define _BOTS_PARSER_

//==========================================================================
// lexical class
class BotLex
{
public:
	BotLex( void );
    ~BotLex( void );

    void 				SetString( const TString& parseString ); // set the string to be parsed
	bool 				Error( void );				// has there been an error?
    const TString&		ErrorString( void ) const; 	// get error string

    // statics
	static TString		Trim( const TString& );
	static TString		ToString( long token ); // token back to string

private:
	// token and parse help
	long 	GetNextToken( void );	// parse next token
	long 	GetNextToken( long& _prevToken );	// parse next token
	void 	UngetToken( void );		// rewind 1 token (can only be used once!)
	bool 	WhiteSpace( char ch );	// is a character a whitespace char?
	bool 	Error( long token );		// is a token an error token?
	bool 	CheckToken( long token1, long token2 ); // make sure two tokens match

public:
	enum Symbols
    {
        lexEOF = 0,
		lexMinus,
        lexAssign,
		lexSquareLeft,
		lexSquareRight,
		lexCurlyLeft,
		lexCurlyRight,
		lexLeft,
		lexRight,
		lexPeriod,
		lexExclamation,
        lexSemicolon,
        lexComma,
		lexLessThan,
		lexGreaterThan,
		lexLessEqualThan,
		lexGreaterEqualThan,
		lexEqual,
		lexNotEqual,
		lexOr,
		lexAnd,
		lexRealNumber,
		lexIntNumber,
		lexStringLiteral,
        lexConstant,
		lexPlus,
		lexTimes,
		lexDivide,

		// pre-defined constants
		lexInit,
		lexType,
		lexVersion,
		lexName,
		lexTankBot,
		lexOthers,
		lexMe,
		lexState,
		lexInitPos,
		lexFuel,
		lexShields,
		lexAmmo,
		lexHealth,
		lexDestination,
		lexPrimaryTarget,
		lexSecondaryTarget,
		lexNULL,
		lexI,

		// pre-defined functions
		lexDistance,
		lexRandom,
		lexTurnTowards,
		lexTurnTurretTowards,
		lexMoveTo,
		lexFireAt,
		lexNearestFuel,
		lexNearestAmmo,
		lexNearestHealth,

		// language constructs
		lexIf,
		lexElse,
		lexForEachI,

        lexIgnore, // symbol to be ignored
        
        lexLast // last - for ID purposes
    };

    enum Errors
    {
		lexErrorUnterminatedString = 1000,
		lexErrorMalformedReal,
		lexErrorMalformedScientificReal,
		lexErrorUnknownSymbol,
        lexErrorUnexpectedToken
    };

private:
	friend class BotParser;

	TString		parse;
    TString		yyString;
    TString		errorString;
    TString		path;
    double		yyReal;
    long		yyInt;

    long		index;
    long		lineCntr;
    long		charCntr;
    long		prevIndex;
    long		prevToken;
	long		currToken;
	long		bufferSize; // strlen size of parse

    // error handling
   	long		errorIndex;
	long		errorToken;
    bool		error;
};

//==========================================================================

#endif

