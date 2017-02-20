#ifndef _WIN32_TIPS_H_
#define _WIN32_TIPS_H_

//==========================================================================

#define kMaxTips		30
#define kMaxTipLines	3

class _EXPORT TTips
{
public:
	TTips( void );
	~TTips( void );

	size_t	NumTips( void ) const;
	bool	LoadTips( const TString& fname, TString& errStr );

	TString	GetTip( size_t tip, size_t line ) const;
	size_t	GetTipSize( size_t tip ) const;

private:
	size_t	numTips;
	size_t	numLines[kMaxTips];
	TString	tips[kMaxTips][kMaxTipLines];
};

//==========================================================================

#endif
