#include <precomp_header.h>

#include <common/font.h>

const size_t currentVersion = 1;

//==========================================================================
//	Font class implementation
//

Font::Font( void )
{
	signature = "PDVFNT02";
	
	cellWidth = 0;
	cellHeight = 0;
	textureId = 0;

	for ( size_t i=0; i<256; i++ )
		widths[i] = 0;
};

Font::~Font( void )
{
	if ( textureId>0 )
	{
		glDeleteTextures( 1, &textureId );
	}
};


//
//	Load a font and put it in a class
//
bool Font::Load( const TString& fname, TString& errStr )
{
	TPersist file(fileRead);

	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	TString str;
	file.ReadString( str );
	if ( str!=signature )
	{
		errStr = "Incorrect file signature (? " + str + ")\n";
		return false;
	}

	// check file-type
	size_t version;
	file.FileRead( &version, sizeof(size_t) );
	if ( version > currentVersion )
	{
		errStr = "Incorrect font file version (too new) (v=" + Int2Str(version) + ")\n";
		return false;
	}

	// cellwidth (max)
	file.FileRead( &cellWidth, sizeof(size_t) );

	// cellheight
	file.FileRead( &cellHeight, sizeof(size_t) );

	// all widths
	file.FileRead( widths, sizeof(size_t) * 256 );

	// rgba map based on this info
	size_t size = cellWidth*16*4 * cellHeight * 16;
	byte* rgba = new byte[size];
	PostCond( rgba!=NULL );

	file.FileRead( rgba, size );

	// generate new texture id
	glGenTextures(1,&textureId);

	glBindTexture(GL_TEXTURE_2D, textureId );
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 16*cellWidth, 16*cellHeight, 
				  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

	delete []rgba;

	return true;
};


size_t Font::GetWidth( const TString& text )
{
	size_t i;
	char* p = (char*)text.c_str();
	size_t fsize = 0;
	size_t ssize = text.length();
	for ( i=0; i<ssize; i++ )
	{
		size_t ch = (unsigned char)(p[i]);

		if ( ch=='\t' ) 
		{
			fsize += widths[32] * 4;
		}
		else if ( ch!=10 && ch!=13 )
		{
			fsize += widths[ch];
		}
	}
	return fsize;
}


size_t Font::GetCharWidth( char ch )
{
	if ( ch==13 || ch==10 )
		return 0;

	if ( ch=='\t' ) 
	{
		return widths[32] * 4;
	}
	else
	{
		return widths[size_t(ch)];
	}
}


void Font::Write( float x, float y, float z, const TString& text )
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, textureId );

	size_t i = 0;
	size_t ssize  = text.length();

	float xpos = x;
    char* p = (char*)text.c_str();
	while (i<ssize)
	{
		size_t ch = (unsigned char)(p[i]);

		FontWriteLetter( ch, float(int(xpos)), float(int(y)), z );
		xpos += (float)widths[ch];
		i++;
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}


void Font::WriteReverse( float x, float y, float z, const TString& text )
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, textureId );

	size_t i = 0;
	size_t ssize  = text.length();

	float xpos = x;
    char* p = (char*)text.c_str();
	while (i<ssize)
	{
		size_t ch = (unsigned char)(p[i]);

		if ( ch=='\n' || ch==10 || ch==13 )
		{
			xpos = x;
			y += cellHeight;
		}
		else
		{
			FontWriteLetterReverse( ch, float(int(xpos)), float(int(y)), z );
			xpos += (float)widths[ch];
		}
		i++;
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}


void Font::Write( float x, float y, float z, float wdth, const TString& text )
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, textureId );

	size_t i = 0;
	size_t ssize  = text.length();

	float xpos = x;
    char* p = (char*)text.c_str();
	while ( i < ssize && xpos < wdth )
	{
		size_t ch = (unsigned char)(p[i]);

		FontWriteLetter( ch, float(int(xpos)), float(int(y)), z );
		xpos += (float)widths[ch];
		i++;
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}


void Font::FontWriteLetter( size_t ch, float x, float y, float z )
{
	// convert ch to a (sx,sy) - (ex,ey) for a uv map
	int cx = ch % 16;
	int cy = ch / 16;

	float sx = float(cx)*cellWidth;
	float sy = float(cy)*cellHeight;
	float ex = sx + widths[ch];
	float ey = sy + cellHeight;

	// fixed sizes for 256 character alphabets
	float cwdth = 1 / float(cellWidth*16);
	float chght = 1 / float(cellHeight*16);
	sx = sx * cwdth;
	sy = sy * chght;
	ex = ex * cwdth;
	ey = ey * chght;

	glNormal3f(0,0,1);
	glBegin(GL_QUADS);
		glTexCoord2f(ex,ey);
		glVertex3f(x+widths[ch],y,z);

		glTexCoord2f(ex,sy);
		glVertex3f(x+widths[ch],y+cellHeight,z);

		glTexCoord2f(sx,sy);
		glVertex3f(x,y+cellHeight,z);

		glTexCoord2f(sx,ey);
		glVertex3f(x,y,z);
	glEnd();
}


void Font::FontWriteLetterReverse( size_t ch, float x, float y, float z )
{
	// convert ch to a (sx,sy) - (ex,ey) for a uv map
	int cx = ch % 16;
	int cy = ch / 16;

	float sx = float(cx)*cellWidth;
	float sy = float(cy)*cellHeight;
	float ex = sx + widths[ch];
	float ey = sy + cellHeight;

	// fixed sizes for 256 character alphabets
	sx = sx / (cellWidth*16);
	sy = sy / (cellHeight*16);
	ex = ex / (cellWidth*16);
	ey = ey / (cellHeight*16);

	glNormal3f(0,0,1);
	glBegin(GL_QUADS);
		glTexCoord2f(sx,sy);
		glVertex3f(x,y,	z);

		glTexCoord2f(sx,ey);
		glVertex3f(x,y+cellHeight,z);

		glTexCoord2f(ex,ey);
		glVertex3f(x+widths[ch],y+cellHeight,z);

		glTexCoord2f(ex,sy);
		glVertex3f(x+widths[ch],y,z);
	glEnd();
}


//==========================================================================
