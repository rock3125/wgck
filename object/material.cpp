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
		rgb[i] = 0.7f;

	for ( i=0; i<4; i++ )
    {
    	rgba[i] = 0.7f;
        power[i] = 12.0f;
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
	WriteString( "material: %s", MaterialName() );
	if ( FileName().length()>0 )
	    WriteString( "          texture filename     : %s", FileName().c_str() );
    else
	    WriteString( "          texture filename     : n/a" );
    WriteString( "          material colour      : r=%2.2f, g=%2.2f, b=%2.2f, a=%2.2f",
    			 rgba[0], rgba[1], rgba[2], rgba[3] );
    WriteString( "          material power       : %2.2f", power[0] );
    WriteString( "          material emission    : r=%2.2f, g=%2.2f, b=%2.2f",
    			 emissRGB[0], emissRGB[1], emissRGB[2] );
    WriteString( "          material specularity : r=%2.2f, g=%2.2f, b=%2.2f",
    			 specRGB[0], specRGB[1], specRGB[2] );
    WriteString( "          object size          : %ld bytes", ObjectSize() );
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
	file.ReadString(buf);
    MaterialName( buf );

	objectSize += buf.length()+1;

	// read material filename
	file.ReadString(buf);

	// then - first - setup colours
    file.FileRead( rgba, 4*sizeof(float) );
    file.FileRead( power, sizeof(float) );
    file.FileRead( emissRGB, 3*sizeof(float) );
    file.FileRead( &specRGB, 3*sizeof(float) );
	objectSize += (11*sizeof(float));

	// load material?
    if ( buf.length()==0 )
	{
    	FileName( "", "", errStr );
	}
    else
	{
    	if ( !FileName( buf, path, errStr ) )
			return false;
		objectSize += buf.length()+1;
	}

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
	{
//		WriteString( "%s\n", fname.c_str() );
		return Texture( fname, path, errStr );
	}
	return true;
};


void TMaterial::FileName( const TString& fname )
{
	filename = fname;
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



void TMaterial::GetMaterialShininess( float& pow ) const
{
	pow = power[0];
};



void TMaterial::SetMaterialShininess( const float& pow )
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


void TMaterial::SetMaterialEmission( const float& r, const float& g, 
									 const float& b, const float& a )
{
	emissRGB[0] = r;
	emissRGB[1] = g;
	emissRGB[2] = b;
	emissRGB[3] = a;
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


void TMaterial::SetMaterialSpecularity( const float& r, const float& g, 
									    const float& b, const float& a )
{
	specRGB[0] = r;
	specRGB[1] = g;
	specRGB[2] = b;
	specRGB[3] = a;
};


const float* TMaterial::ColourRGBA( void ) const
{
	return rgba;
};



const float* TMaterial::ColourRGB( void ) const
{
	return rgb;
};



const float* TMaterial::Shininess( void ) const
{
	return power;
};



const float* TMaterial::Specularity( void ) const
{
	return emissRGB;
};


const float* TMaterial::Emission( void ) const
{
	return specRGB;
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
		bool dontDelete = false;
		TString lcaseName = materialname.lcase();
		if ( lcaseName=="colourmap" )
			dontDelete = true;

    	texture = TTexture();
		texture.DontDestroyRGB( dontDelete );
		if ( rgba[3]>0.0f && rgba[3]<1.0f && fname.GetItem(',',1).length()==0 )
		{
			size_t a = size_t( rgba[3] * 255.0f );
			return texture.LoadBinary( fname, path, errStr, unsigned char(a), true );
		}
		else
		{
	        return texture.LoadBinary( fname, path, errStr, true );
		}
    }
	return true;
};


void TMaterial::PauseAnimation( bool tf )
{
	texture.PauseAnimation(tf);
};


bool TMaterial::SaveText( TPersist& file, TString& errStr )
{
	TString temp;

	//Material Bridge_1Mesh {
	//	0.223529;0.350196;0.853137;1.000000;;
	//	204.000000;
	//	0.011765;0.018431;0.044902;;
	//	0.000000;0.000000;0.000000;;
	//	TextureFilename {
	//		"br3.jpg";
	//	}
	//}

	temp = "Material " + MaterialName() + " {\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "\t" + FloatToString(1,6,rgba[0]) + ";";
	temp = temp + FloatToString(1,6,rgba[1]) + ";";
	temp = temp + FloatToString(1,6,rgba[2]) + ";";
	temp = temp + FloatToString(1,6,rgba[3]) + ";;\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "\t" + FloatToString(1,6,power[0]) + ";\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "\t" + FloatToString(1,6,emissRGB[0]) + ";";
	temp = temp + FloatToString(1,6,emissRGB[1]) + ";";
	temp = temp + FloatToString(1,6,emissRGB[2]) + ";;\n";
	file.FileWrite( temp.c_str(), temp.length() );

	temp = "\t" + FloatToString(1,6,specRGB[0]) + ";";
	temp = temp + FloatToString(1,6,specRGB[1]) + ";";
	temp = temp + FloatToString(1,6,specRGB[2]) + ";;\n";
	file.FileWrite( temp.c_str(), temp.length() );

	if ( FileName().length()>0 )
	{
		temp = "\tTextureFilename {\n";
		temp = temp + "\t\"" + FileName() + "\";\n";
		temp = temp + "\t}\n";
		file.FileWrite( temp.c_str(), temp.length() );
	}

	temp = "}\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

	return true;
};

bool TMaterial::Reload( TString& errStr )
{
	if ( FileName().length() > 0 )
	{
		return texture.Reload(errStr);
	}
	return true;
};

//==========================================================================


