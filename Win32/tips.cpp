#include <precomp_header.h>

#include <win32/tips.h>

//==========================================================================

TTips::TTips( void )
{
	numTips = 0;
};

TTips::~TTips( void )
{
	numTips = 0;
}

size_t TTips::NumTips( void ) const
{
	return numTips;
};

TString TTips::GetTip( size_t tip, size_t line ) const
{
	PreCond( tip < numTips );
	PreCond( line < numLines[tip] );
	return tips[tip][line];
};

size_t TTips::GetTipSize( size_t tip ) const
{
	PreCond( tip < numTips );
	return numLines[tip];
};

bool TTips::LoadTips( const TString& fname, TString& errStr )
{
	TPersist file(fileRead);

	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	numTips = 0;
	size_t _numLines = 0;
	bool eof = false;
	while ( numTips < kMaxTips && !eof )
	{
		TString str;
		eof = !file.ReadLine( str );
		if ( str=="<tip>" )
		{
			_numLines = 0;
			numTips++;
		}
		else
		{
			if ( numTips>0 && _numLines<kMaxTipLines )
			{
				tips[numTips-1][_numLines++] = str;
				numLines[numTips-1] = _numLines;
			}
		}
	};

	file.FileClose();

	return true;
};

//==========================================================================

