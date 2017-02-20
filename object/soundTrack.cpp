#include <precomp_header.h>

#include <object/soundTrack.h>

//==========================================================================

SoundTrack::SoundTrack( void )
{
	count = 0;

	TPersist file(fileRead);
	if ( file.FileOpen( "data\\music\\soundtracks.bin" ) )
	{
		TString c;
		file.ReadLine( c );
		c = c.GetItem('=',1);
		if ( c.length() > 0 )
		{
			count = atoi( c.c_str() );
			size_t index = 0;
			for ( size_t i=0; i<count; i++ )
			{
				file.ReadLine( c );
				TString p1,p2;
				p1 = c.GetItem('=',0);
				p2 = c.GetItem('=',1);
				if ( p1.length()>0 && p2.length()>0 )
				{
					filename[index] = p1;
					title[index] = p2;
					index++;
				}
			}
			count = index;
		}
	}
};

SoundTrack::~SoundTrack( void )
{
	count = 0;
};

size_t SoundTrack::Count( void ) const
{
	return count;
};

void SoundTrack::Get( size_t index, TString& _filename, TString& _title )
{
	PreCond( index < count );

	_filename = filename[index];
	_title = title[index];
};

//==========================================================================

