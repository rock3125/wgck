#ifndef _BOTS_GRAMMAR_
#define _BOTS_GRAMMAR_

//==========================================================================

#include <botCompiler/botparser.h>

//==========================================================================

#define kMaxStates 25

//==========================================================================

enum tags	{
				tstateId = 1,
				tsetState,
				tinitPos,
				tprimaryTarget,
				tsecondaryTarget,
				tnull,
				tnearestFuel,
				tnearestAmmo,
				tnearestHealth,
				tvar_i,
				tplayerId,
				tturnTowards,
				tturnTurretTowards,
				tmoveTo,
				tfireAt,
				tif,
				telse,
				tforeachi,
				tnot,
				vfloat,
				tdistance,
				trandom,
				tme,
				tlessthan,
				tgreaterthan,
				tgreaterequalthan,
				tlessequalthan,
				tequal,
				tnotequal,
				tor,
				tand,
				tplus,
				tminus,
				ttimes,
				tdivide,
				tfuel,
				tammo,
				thealth,
				tshields
			};

//==========================================================================

class BotTree
{
public:
	BotTree( void ); // special case for tplayerId
	BotTree( size_t _tag );
	BotTree( size_t _tag, float v );
	BotTree( size_t _tag, BotTree* _left, BotTree* _right );
	BotTree( size_t _tag, BotTree* _left, BotTree* _right, BotTree* _else );
	BotTree( size_t _tag, const TString& str, BotTree* _left );
	~BotTree( void );

	bool	Write( TPersist& file, TString& errStr );
	bool	Read( TPersist& file, TString& errStr );

	void	Indent( size_t level );
	void	PrintTree( void );

	size_t	Tag( void ) const;
	void	Tag( size_t );

	float	Value( void ) const;
	void	Value( float _value );

	BotTree*	ElsePart( void ) const;

	BotTree*	Left( void ) const;
	void		Left( BotTree* );

	BotTree*	Right( void ) const;
	void		Right( BotTree* );

	BotTree*	Next( void ) const;
	void		Next( BotTree* );

	const TString&	Name( void ) const;
	void			Name( const TString& name );

private:
	size_t		tag;
	TString		name;
	float		value;

	BotTree*	left;
	BotTree*	right;
	BotTree*	elsepart;
	BotTree*	next;
};

//==========================================================================

class BotParser
{
	const BotParser& operator=(const BotParser&);
	BotParser( const BotParser& );
public:
	BotParser( void );
	BotParser( BotLex& _lex ); // constructor takes lexical tokens
	~BotParser( void );

	// parse the tokens in the lexical analyser
	// grammar BotParser
	bool Parse( TString& errStr );

	// write/read byte code to/from file
	bool SaveBot( const TString& fname, TString& errStr );
	bool LoadBot( const TString& fname, TString& errStr );

	// get a state from a name, or NULL if not found
	BotTree* GetState( const TString& ) const;

	// print BotTrees (numStates)
	void PrintTree( void ) const;

	// convert individual tag to string
	static TString ToString( size_t tag );

	// convert complete BotTree to a string
	static TString TreeToString( BotTree* BotTree, size_t indent=0 );

	// initial state get
	BotTree* InitialState( void ) const;

private:
	bool CheckToken( long token1, long token2, TString& errStr );
	bool Float( float& f, TString& errStr );
	bool Int( int& i, TString& errStr );
	bool String( TString& str, TString& errStr );

	bool ParseInit( TString& errStr );
	bool GetState( TString& state, BotTree*& stateBotTree, 
				   TString& errStr );
	bool Statement( BotTree*& statement, TString& errStr );
	bool Location( BotTree*& location, TString& errStr );
	bool BooleanExpr( BotTree*& expr, TString& errStr );
	bool Expression( BotTree*& expr, TString& errStr );

	void AddNewState( const TString& name, BotTree* BotTree );
	size_t Lex2BotParser( long token );

	static void Indent( TString& str, size_t i );

private:
	BotLex*		lex;

	TString		signature;

	float		version;
	TString		botName;
	TString		initialStateName;
	BotTree*	initialState;

	bool		error;

	TString		stateNames[kMaxStates];
	BotTree*	states[kMaxStates];
	size_t		numStates;
};

//==========================================================================

#endif
