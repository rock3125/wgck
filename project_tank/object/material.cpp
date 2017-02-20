#include <precomp_header.h>

#include <win32/win32.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>

//==========================================================================

TMaterial::TMaterial( void )
	: objectSize(0)
{
	int i;

	for ( i=0; i<3; i++ )
		rgb[i] = 0.0f;

	for ( i=0; i<4; i++ )
    {
    	rgba[i] = 0.0f;
        power[i] = 0.0f;
        emissRGB[i] = 0.0f;
        specRGB[i] = 0.0f;
    }
};


TMaterial::TMaterial( const TMaterial& material )
	: objectSize(0)
{
	operator=(material);
};


const TMaterial& TMaterial::operator=( const TMaterial& mat )
{
	size_t i;

	for ( i=0; i<3; i++ )
		rgb[i] = mat.rgb[i];

	for ( i=0; i<4; i++ )
    {
    	rgba[i] = mat.rgba[i];
        power[i] = mat.power[i];
        emissRGB[i] = mat.emissRGB[i];
        specRGB[i] = mat.specRGB[i];
    }

	materialname = mat.materialname;
	filename = mat.filename;
    texture = mat.texture;
	objectSize = mat.objectSize;

	return *this;
};


TMaterial::~TMaterial( void )
{
};



long TMaterial::ObjectSize( void ) const
{
	return objectSize;
};



void TMaterial::Print( void ) const
{
	ConsolePrint( "material: %s", MaterialName() );
	if ( FileName().length()>0 )
	    ConsolePrint( "          texture filename     : %s", FileName().c_str() );
    else
	    ConsolePrint( "          texture filename     : n/a" );
    ConsolePrint( "          material colour      : r=%2.2f, g=%2.2f, b=%2.2f, a=%2.2f",
    			 rgba[0], rgba[1], rgba[2], rgba[3] );
    ConsolePrint( "          material power       : %2.2f", power[0] );
    ConsolePrint( "          material emission    : r=%2.2f, g=%2.2f, b=%2.2f",
    			 emissRGB[0], emissRGB[1], emissRGB[2] );
    ConsolePrint( "          material specularity : r=%2.2f, g=%2.2f, b=%2.2f",
    			 specRGB[0], specRGB[1], specRGB[2] );
    ConsolePrint( "          object size          : %ld bytes", ObjectSize() );
};



bool TMaterial::SaveBinary( TPersist& file) const
{
	file.FileWrite( materialname.c_str(), materialname.length()+1 );
	file.FileWrite( filename.c_str(), filename.length()+1 );

    file.FileWrite( rgba, 4*sizeof(float) );
    file.FileWrite( power, sizeof(float) );
    file.FileWrite( emissRGB, 3*sizeof(float) );
    file.FileWrite( specRGB, 3*sizeof(float) );

	return true;
};



bool TMaterial::LoadBinary( TPersist& file, const TString& path, TString& errStr )
{
	objectSize = 0;

	TString buf;
	char ch;
    int index = 0;

    do
    {
		file.FileRead( &ch, 1 );
        buf = buf + TString(ch);
		index++;
    }
    while ( ch!=0 && index<255 );
    MaterialName( buf );
	objectSize += index;

	buf = "";
    index = 0;
    do
    {
		file.FileRead( &ch, 1 );
        buf = buf + TString(ch);
		index++;
    }
    while ( ch!=0 && index<255 );

    if ( buf.length()==0 )
    	FileName( "", "", errStr );
    else
	{
    	if ( !FileName( buf, path, errStr ) )
			return false;
		objectSize += index;
	}

    file.FileRead( rgba, 4*sizeof(float) );
    file.FileRead( power, sizeof(float) );
    file.FileRead( emissRGB, 3*sizeof(float) );
    file.FileRead( &specRGB, 3*sizeof(float) );
	objectSize += (11*sizeof(float));

	// copy RGB access
	size_t i;
	for ( i=0; i<3; i++ )
		rgb[i] = rgba[i];

	// texture?
	objectSize += sizeof(TTexture);
	if ( texture.Used() )
	{
		objectSize += texture.ObjectSize();
	}
	return true;
};



const TString& TMaterial::FileName( void ) const
{
	return filename;
};



bool TMaterial::FileName( const TString& fname, const TString& path, TString& errStr )
{
   	filename = fname;
	if ( fname.length()>0 )
		return Texture( fname, path, errStr );
	return true;
};



const TString& TMaterial::MaterialName( void ) const
{
	return materialname;
};



void TMaterial::MaterialName( const TString& mname )
{
   	materialname = mname;
};



void TMaterial::GetMaterialColour( float& _r, float& _g, float& _b, float& _a ) const
{
	_r = rgba[0];
	_g = rgba[1];
	_b = rgba[2];
	_a = rgba[3];
};



void TMaterial::SetMaterialColour( const float& _r, const float& _g,
								   const float& _b, const float& _a )
{
	rgba[0] = _r;
	rgba[1] = _g;
	rgba[2] = _b;
	rgba[3] = _a;

	rgb[0] = _r;
	rgb[1] = _g;
	rgb[2] = _b;
};



void TMaterial::GetMaterialPower( float& pow ) const
{
	pow = power[0];
};



void TMaterial::SetMaterialPower( const float& pow )
{
	power[0] = pow;
};



void TMaterial::GetMaterialEmission( float& r, float& g, float& b ) const
{
	r = emissRGB[0];
	g = emissRGB[1];
	b = emissRGB[2];
};



void TMaterial::SetMaterialEmission( const float& r, const float& g, const float& b )
{
	emissRGB[0] = r;
	emissRGB[1] = g;
	emissRGB[2] = b;
};



void TMaterial::GetMaterialSpecularity( float& r, float& g, float& b ) const
{
	r = specRGB[0];
	g = specRGB[1];
	b = specRGB[2];
};



void TMaterial::SetMaterialSpecularity( const float& r, const float& g, const float& b )
{
	specRGB[0] = r;
	specRGB[1] = g;
	specRGB[2] = b;
};



const float* TMaterial::ColourRGBA( void ) const
{
	return rgba;
};



const float* TMaterial::ColourRGB( void ) const
{
	return rgb;
};



const float* TMaterial::Power( void ) const
{
	return power;
};



const float* TMaterial::Specularity( void ) const
{
	return specRGB;
};



const float* TMaterial::Emission( void ) const
{
	return emissRGB;
};



const TTexture& TMaterial::Texture( void ) const
{
	return texture;
};


void TMaterial::Texture( const TTexture& _texture )
{
	texture = _texture;
}


bool TMaterial::Texture( const TString& fname, const TString& path, TString& errStr )
{
    if ( fname.length()==0 )
		texture.Used(false);
    else
    {
    	texture = TTexture();
        if ( !texture.LoadBinary(fname, path, errStr ) )
        {
        	return false;
        }
    }
	return true;
};


void TMaterial::PauseAnimation( bool tf )
{
	texture.PauseAnimation(tf);
};

//==========================================================================


