#include <precomp_header.h>

#include <win32/win32.h>
#include <object/geometry.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>
#include <object/landscape.h>

#include <object/createlandscape.h>

#define _ALTERNATESEA

//==========================================================================

float _bx1 = -500;
float _by1 = 0;
float _bz1 = -500;
float _bx2 = 500;
float _by2 = 75;
float _bz2 = 500;

//==========================================================================
// plane surrounding landscape at -1,-1 to 1,1

void CreateSurroundMesh( float waterLevel, 
						 size_t& vcount, float* vertices, 
						 size_t& scount, size_t* surfaces )
{
	float maxX = 500;
	float maxZ = 500;
	float by1 = 0;
	float by2 = 75;

	float size = 1000;

	//  add slices of sea
	size_t pv = vcount;
	float v1, v3;
	float v2 = ((float(waterLevel)/256.0f) * (by2-by1) + by1) * 0.5f;

	// object 1
	// 1.0;-5.0;0.000000;,
	// 5.0;-5.0;0.000000;,
	// 5.0;5.0;0.000000;,
	// 1.0;5.0;0.000000;;

	v1 =  1 * maxX;
	v3 = -5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 =  5 * maxX;
	v3 = -5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = 5 * maxX;
	v3 = 5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = 1 * maxX;
	v3 = 5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	// 3;0,1,2;,
	// 3;3,0,2;,
	// 3;2,1,0;,
	// 3;2,0,3;;

	// add 4 surfaces defined by these triangles
	AddSurface( surfaces, scount, pv + 0, pv + 1, pv + 2 );
	AddSurface( surfaces, scount, pv + 3, pv + 0, pv + 2 );
	AddSurface( surfaces, scount, pv + 2, pv + 1, pv + 0 );
	AddSurface( surfaces, scount, pv + 2, pv + 0, pv + 3 );

	// object 2
	// -5.0;-5.0;0.000000;,
	// -1.0;-5.0;0.000000;,
	// -1.0;5.0;0.000000;,
	// -5.0;5.0;0.000000;;
	pv = vcount;

	v1 = -5 * maxX;
	v3 = -5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = -1 * maxX;
	v3 = -5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = -1 * maxX;
	v3 =  5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = -5 * maxX;
	v3 =  5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	// 3;0,1,2;,
	// 3;3,0,2;,
	// 3;2,1,0;,
	// 3;2,0,3;;
	// add 4 surfaces defined by these triangles
	AddSurface( surfaces, scount, pv + 0, pv + 1, pv + 2 );
	AddSurface( surfaces, scount, pv + 3, pv + 0, pv + 2 );
	AddSurface( surfaces, scount, pv + 2, pv + 1, pv + 0 );
	AddSurface( surfaces, scount, pv + 2, pv + 0, pv + 3 );

	// object 3
	// -1.0;-5.0;0.000000;,
	// 1.0;-5.0;0.000000;,
	// 1.0;-1.0;0.000000;,
	// -1.0;-1.0;0.000000;;
	pv = vcount;

	v1 = -1 * maxX;
	v3 = -5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 =  1 * maxX;
	v3 = -5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 =  1 * maxX;
	v3 = -1 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = -1 * maxX;
	v3 = -1 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	// 3;0,1,2;,
	// 3;3,0,2;,
	// 3;2,1,0;,
	// 3;2,0,3;;
	// add 4 surfaces defined by these triangles
	AddSurface( surfaces, scount, pv + 0, pv + 1, pv + 2 );
	AddSurface( surfaces, scount, pv + 3, pv + 0, pv + 2 );
	AddSurface( surfaces, scount, pv + 2, pv + 1, pv + 0 );
	AddSurface( surfaces, scount, pv + 2, pv + 0, pv + 3 );

	// object 4
	// -1.0;1.0;0.000000;,
	// 1.0;1.0;0.000000;,
	// 1.0;5.0;0.000000;,
	// -1.0;5.0;0.000000;;
	pv = vcount;

	v1 = -1 * maxX;
	v3 =  1 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = 1 * maxX;
	v3 = 1 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = 1 * maxX;
	v3 = 5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = -1 * maxX;
	v3 =  5 * maxZ;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	// 3;0,1,2;,
	// 3;3,0,2;,
	// 3;2,1,0;,
	// 3;2,0,3;;
	// add 4 surfaces defined by these triangles
	AddSurface( surfaces, scount, pv + 0, pv + 1, pv + 2 );
	AddSurface( surfaces, scount, pv + 3, pv + 0, pv + 2 );
	AddSurface( surfaces, scount, pv + 2, pv + 1, pv + 0 );
	AddSurface( surfaces, scount, pv + 2, pv + 0, pv + 3 );
};

//==========================================================================

#if !defined(_SPEEDTEST)

// remember next time:  z is really -z
TBinObject* CreateBinaryMesh( TString fileName, 
							  TString strip,
							  TString material,
							  size_t seaHeight, 
							  TString& errStr )
{
	size_t x,y,i;
	TTexture bmp;
	size_t _seaHeight = size_t(float(seaHeight) * 2.55f);

	bmp.DontDestroyRGB
		(true);
	if ( !bmp.LoadBinary( fileName, "data\\stories", errStr ) )
		return NULL;

	byte* rgb = const_cast<byte*>(bmp.Rgb());
	PostCond( rgb!=NULL );

	size_t w = bmp.Width();
	size_t h = bmp.Height();

	PreCond( w==h );

	// force 0 height around the edges
	for ( i=0; i<w; i++ )
	{
		rgb[i*3+0] = 0;
		rgb[i*3+1] = 0;
		rgb[i*3+2] = 0;

		rgb[(h-1)*w*3+i*3+0] = 0;
		rgb[(h-1)*w*3+i*3+1] = 0;
		rgb[(h-1)*w*3+i*3+2] = 0;
	}

	for ( i=0; i<h; i++ )
	{
		rgb[i*3*w+0] = 0;
		rgb[i*3*w+1] = 0;
		rgb[i*3*w+2] = 0;

		rgb[i*3*w+(w-1)*3+0] = 0;
		rgb[i*3*w+(w-1)*3+1] = 0;
		rgb[i*3*w+(w-1)*3+2] = 0;
	}

	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	TBinObject* bin = new TBinObject();
	TMaterial* heightMap = new TMaterial();
	TMaterial* colourMap = new TMaterial();
	TMaterial* seaMaterial = new TMaterial();

	heightMap->SetMaterialColour( 1,1,1,1 );
	heightMap->SetMaterialShininess( 4 );
	heightMap->SetMaterialEmission( 0.1f,0.1f,0.1f );
	heightMap->SetMaterialSpecularity( 0,0,0 );
	heightMap->MaterialName( "heightMap" );
	if ( !heightMap->FileName( material, "data\\textures", errStr ) )
		return NULL;
	bin->AddMaterial( *heightMap );

	colourMap->SetMaterialColour( 1,1,1,1 );
	colourMap->SetMaterialShininess( 4 );
	colourMap->SetMaterialEmission( 0.1f,0.1f,0.1f );
	colourMap->SetMaterialSpecularity( 0,0,0 );
	colourMap->MaterialName( "colourMap" );
	if ( !colourMap->FileName( strip, "data\\textures", errStr ) )
		return NULL;
	bin->AddMaterial( *colourMap );

	seaMaterial->SetMaterialColour( 0.307059f,0.345882f,0.802353f,0.45f );
	seaMaterial->SetMaterialShininess( 4 );
	seaMaterial->SetMaterialEmission( 0.034118f,0.038431f,0.051373f );
	seaMaterial->SetMaterialSpecularity( 0,0,0 );
	seaMaterial->MaterialName( "seaMaterial" );
	if ( !seaMaterial->FileName( "[wat00##.jpg,1,0,8,8]", "data\\textures", errStr ) )
		return NULL;
	bin->AddMaterial( *seaMaterial );

	TMesh* m1 = new TMesh();
	TMesh* m2 = new TMesh();

	m1->Name( "landscapeMesh" );
	m1->NumVertices( w*h+1 );
	float* v = m1->Vertices();

	// vertices: 	-2.501811;2.691057;2.584104;,
	size_t index = 0;
	for ( y=0; y<h; y++ )
	for ( x=0; x<w; x++ )
	{
		size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

		float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
		float v2 = (float(height) / 256) * (by2-by1) + by1;
		float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
		v[index*3+0] = v1;
		v[index*3+1] = v2;
		v[index*3+2] = -v3; // adjust for mesh loader
		index++;
	}
	v[index*3+0] = 3.141592f;
	v[index*3+1] = float(w);
	v[index*3+2] = float(h);

	m1->NumFaces( (w-1)*(h-1)*2 );
	size_t* s = m1->Surfaces();

	// surfaces: 	3;423,426,8;,
	index = 0;
	for ( y=0; y<(h-1); y++ )
	for ( x=0; x<(w-1); x++ )
	{
		size_t i1,i2,i3,i4;
		i1 = y*w+x;
		i2 = (y+1)*w+x;
		i3 = y*w+(x+1);
		i4 = (y+1)*w+(x+1);

		s[index*3+0] = i1;
		s[index*3+1] = i2;
		s[index*3+2] = i3;
		index++;

		s[index*3+0] = i2;
		s[index*3+1] = i4;
		s[index*3+2] = i3;
		index++;
	}

	m1->NumMaterialFaces( 1 );
	m1->NumMaterials( 2 );
	s = m1->MaterialFaces();
	s[0] = 0;
	m1->Materials( 0, 0, heightMap );
	m1->Materials( 1, 1, colourMap );

	m1->IsColourMapped( true );
	m1->SetColourMapSize( w, h );
	m1->CreateColourMap( colourMap->Texture(), heightMap->Texture() );
	m1->ColourMapSetWaterLevel( seaHeight, w, h );

	bin->AddMesh( *m1 );

	// add sea
	size_t vcount = 0;
	size_t scount = 0;
	size_t* surfaces = new size_t[w*h*12];
	float* vertices = new float[w*h*12];

	// create sea
	for ( y=0; y<(h-1); y++ )
	for ( x=0; x<(w-1); x++ )
	{
		size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

		if ( height<_seaHeight )
		{
			// add two triangles to make this part of the sea
			float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
			float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;

			float v2 = ((float(_seaHeight)/256.0f) * (by2-by1) + by1) * 0.5f;

			AddVertex( vertices, vcount, v1,v2,-v3 );

			v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,-v3 );

			v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,-v3 );

			v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,-v3 );

			// add two surfaces defined by these triangles
			AddSurface( surfaces, scount, vcount-4, vcount-1, vcount-3 );
			AddSurface( surfaces, scount, vcount-2, vcount-3, vcount-1 );
		}
	}

	// write sea mesh
	if ( vcount>0 && scount>0 )
	{
		m2->Name( "seaMesh" );
		m2->NumVertices( vcount );
		m2->Vertices( vertices );
		m2->NumFaces( scount );
		m2->Surfaces( surfaces );

		m2->NumMaterialFaces( 1 );
		m2->NumMaterials( 1 );
		s = m2->MaterialFaces();
		s[0] = 0;
		m2->Materials( 0, 2, seaMaterial );

		m2->NumNormals( 1 );
		v = m2->Normals();
		v[0] = 0;
		v[1] = 1;
		v[2] = 0;

		m2->NumNormalFaces( scount );
		s = m2->NormalFaces();

		index = 0;
		for ( i=0; i<scount; i++ )
		{
			s[index*3+0] = 1;
			s[index*3+1] = 1;
			s[index*3+2] = 1;
			index++;
		}
		
		m2->NumTextureCoords( vcount );
		v = m2->TextureCoords();

		index = 0;
		for ( i=0; i<(vcount/4); i++ )
		{
			v[index*2+0] = 0;
			v[index*2+1] = 0;
			index++;

			v[index*2+0] = 1;
			v[index*2+1] = 0;
			index++;

			v[index*2+0] = 1;
			v[index*2+1] = 1;
			index++;

			v[index*2+0] = 0;
			v[index*2+1] = 1;
			index++;
		}

		bin->AddMesh( *m2 );
	}

//	delete []surfaces;
//	delete []vertices;

	return bin;
};

// remember next time:  z is really -z
TBinObject* CreateBinaryMesh( byte* rgb, size_t w, size_t h,
							  size_t seaHeight, 
							  const TString& strip,
							  const TString& material,
							  TString& errStr )
{
	size_t x,y,i;
	size_t _seaHeight = size_t(float(seaHeight) * 2.55f);

	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	TBinObject* bin = new TBinObject();
	TMaterial* heightMap = new TMaterial();
	TMaterial* colourMap = new TMaterial();
	TMaterial* seaMaterial = new TMaterial();

	heightMap->SetMaterialColour( 1,1,1,1 );
	heightMap->SetMaterialShininess( 4 );
	heightMap->SetMaterialEmission( 0.1f,0.1f,0.1f );
	heightMap->SetMaterialSpecularity( 0,0,0 );
	heightMap->MaterialName( "heightMap" );
	if ( !heightMap->FileName( material, "data\\textures", errStr ) )
		return NULL;
	bin->AddMaterial( *heightMap );

	colourMap->SetMaterialColour( 1,1,1,1 );
	colourMap->SetMaterialShininess( 4 );
	colourMap->SetMaterialEmission( 0.1f,0.1f,0.1f );
	colourMap->SetMaterialSpecularity( 0,0,0 );
	colourMap->MaterialName( "colourMap" );
	if ( !colourMap->FileName( strip, "data\\textures", errStr ) )
		return NULL;
	bin->AddMaterial( *colourMap );

	seaMaterial->SetMaterialColour( 0.307059f,0.325882f,0.852353f,0.55f );
	seaMaterial->SetMaterialShininess( 4 );
	seaMaterial->SetMaterialEmission( 0.034118f,0.038431f,0.051373f );
	seaMaterial->SetMaterialSpecularity( 0,0,0 );
	seaMaterial->MaterialName( "seaMaterial" );
	if ( !seaMaterial->FileName( "[wat00##.jpg,1,0,8,8]", "data\\textures", errStr ) )
		return NULL;
	bin->AddMaterial( *seaMaterial );

	TMesh* m1 = new TMesh();
	TMesh* m2 = new TMesh();

	m1->Name( "landscapeMesh" );
	m1->NumVertices( w*h+1 );
	float* v = m1->Vertices();

	// vertices: 	-2.501811;2.691057;2.584104;,
	size_t index = 0;
	for ( y=0; y<h; y++ )
	for ( x=0; x<w; x++ )
	{
		size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

		float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
		float v2 = (float(height) / 256) * (by2-by1) + by1;
		float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
		v[index*3+0] = v1;
		v[index*3+1] = v2;
		v[index*3+2] = -v3; // adjust for mesh loader
		index++;
	}
	v[index*3+0] = 3.141592f;
	v[index*3+1] = float(w);
	v[index*3+2] = float(h);

	m1->NumFaces( (w-1)*(h-1)*2 );
	size_t* s = m1->Surfaces();

	// surfaces: 	3;423,426,8;,
	index = 0;
	for ( y=0; y<(h-1); y++ )
	for ( x=0; x<(w-1); x++ )
	{
		size_t i1,i2,i3,i4;
		i1 = y*w+x;
		i2 = (y+1)*w+x;
		i3 = y*w+(x+1);
		i4 = (y+1)*w+(x+1);

		s[index*3+0] = i1;
		s[index*3+1] = i2;
		s[index*3+2] = i3;
		index++;

		s[index*3+0] = i2;
		s[index*3+1] = i4;
		s[index*3+2] = i3;
		index++;
	}

	m1->NumMaterialFaces( 1 );
	m1->NumMaterials( 2 );
	s = m1->MaterialFaces();
	s[0] = 0;
	m1->Materials( 0, 0, heightMap );
	m1->Materials( 1, 1, colourMap );

	m1->IsColourMapped( true );
	m1->SetColourMapSize( w, h );
	m1->CreateColourMap( colourMap->Texture(), heightMap->Texture() );
	m1->ColourMapSetWaterLevel( seaHeight, w, h );

	bin->AddMesh( *m1 );

	// add sea
	size_t vcount = 0;
	size_t scount = 0;
	size_t* surfaces = new size_t[w*h*12];
	float* vertices = new float[w*h*12];

	// create sea
	for ( y=0; y<(h-1); y++ )
	for ( x=0; x<(w-1); x++ )
	{
		size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

		if ( height<_seaHeight )
		{
			// add two triangles to make this part of the sea
			float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
			float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;

			float v2 = ((float(_seaHeight)/256.0f) * (by2-by1) + by1) * 0.5f;

			AddVertex( vertices, vcount, v1,v2,-v3 );

			v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,-v3 );

			v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,-v3 );

			v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
			v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
			AddVertex( vertices, vcount, v1,v2,-v3 );

			// add two surfaces defined by these triangles
			AddSurface( surfaces, scount, vcount-4, vcount-1, vcount-3 );
			AddSurface( surfaces, scount, vcount-2, vcount-3, vcount-1 );
		}
	}

	// write sea mesh
	if ( vcount>0 && scount>0 )
	{
		m2->Name( "seaMesh" );
		m2->NumVertices( vcount );
		m2->Vertices( vertices );
		m2->NumFaces( scount );
		m2->Surfaces( surfaces );

		m2->NumMaterialFaces( 1 );
		m2->NumMaterials( 1 );
		s = m2->MaterialFaces();
		s[0] = 0;
		m2->Materials( 0, 2, seaMaterial );

		m2->NumNormals( 1 );
		v = m2->Normals();
		v[0] = 0;
		v[1] = 1;
		v[2] = 0;

		m2->NumNormalFaces( scount );
		s = m2->NormalFaces();

		index = 0;
		for ( i=0; i<scount; i++ )
		{
			s[index*3+0] = 1;
			s[index*3+1] = 1;
			s[index*3+2] = 1;
			index++;
		}
		
		m2->NumTextureCoords( vcount );
		v = m2->TextureCoords();

		index = 0;
		for ( i=0; i<(vcount/4); i++ )
		{
			v[index*2+0] = 0;
			v[index*2+1] = 0;
			index++;

			v[index*2+0] = 1;
			v[index*2+1] = 0;
			index++;

			v[index*2+0] = 1;
			v[index*2+1] = 1;
			index++;

			v[index*2+0] = 0;
			v[index*2+1] = 1;
			index++;
		}

		bin->AddMesh( *m2 );
	}

//	delete heightMap;
//	delete colourMap;
//	delete seaMaterial;

//	delete m1;
//	delete m2;
//	delete []surfaces;
//	delete []vertices;

	return bin;
};


void AddSeaToMesh( size_t x, size_t y, byte* rgb, size_t w, size_t h,
				   float waterLevel, 
				   size_t& vcount, float* vertices, 
				   size_t& scount, size_t* surfaces )
{
	size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	if ( height<waterLevel )
	{
		// add two triangles to make this part of the sea
		float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
		float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
		float v2 = ((float(waterLevel)/256.0f) * (by2-by1) + by1) * 0.5f;

		AddVertex( vertices, vcount, v1,v2,-v3 );

		v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
		v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
		AddVertex( vertices, vcount, v1,v2,-v3 );

		v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
		v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
		AddVertex( vertices, vcount, v1,v2,-v3 );

		v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
		v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
		AddVertex( vertices, vcount, v1,v2,-v3 );

		// add two surfaces defined by these triangles
		AddSurface( surfaces, scount, vcount-4, vcount-1, vcount-3 );
		AddSurface( surfaces, scount, vcount-2, vcount-3, vcount-1 );
	}
};


void AddSeaToMesh2( float x, float y, byte* rgb, size_t w, size_t h,
				    float waterLevel, 
				    size_t& vcount, float* vertices, 
				    size_t& scount, size_t* surfaces )
{
	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	// add two triangles to make this part of the sea
	float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
	float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
	float v2 = ((float(waterLevel)/256.0f) * (by2-by1) + by1) * 0.5f;

	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
	v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
	v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
	v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	// add two surfaces defined by these triangles
	AddSurface( surfaces, scount, vcount-4, vcount-1, vcount-3 );
	AddSurface( surfaces, scount, vcount-2, vcount-3, vcount-1 );
};


void AddLandToMesh( size_t x, size_t y, byte* rgb, size_t w, size_t h,
				    size_t& vcount, float* vertices, 
				    size_t& scount, size_t* surfaces )
{
	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	size_t height = rgb[(((h-1)-y)*w+((w-1)-x))*3];

	// band of zero around the field
	if ( x==0 || x==1 || y==0 || y==1 )
		height = 0;

	float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
	float v2 = (float(height) / 256) * (by2-by1) + by1;
	float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	size_t i1,i2,i3,i4;
	i1 = y*w+x;
	i2 = (y+1)*w+x;
	i3 = y*w+(x+1);
	i4 = (y+1)*w+(x+1);
	AddSurface( surfaces, scount, i1,i2,i3 );
	AddSurface( surfaces, scount, i2,i4,i3 );
};


void AddLandToMesh2( size_t x, size_t y, byte* rgb, size_t w, size_t h,
				     size_t& vcount, float* vertices, 
				     size_t& scount, size_t* surfaces )
{
	float w1 = float(w) / 128;
	float h1 = float(h) / 128;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	bx1 *= w1;
	bz1 *= h1;
	bx2 *= w1;
	bz2 *= h1;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	// add two triangles to make this part of the land
	float v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
	float v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
	float v2 = 0;

	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
	v3 = (float(y) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = (float(x+1) / float(w)) * (bx2-bx1) + bx1;
	v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	v1 = (float(x) / float(w)) * (bx2-bx1) + bx1;
	v3 = (float(y+1) / float(h)) * (bz2-bz1) + bz1;
	AddVertex( vertices, vcount, v1,v2,-v3 );

	// add two surfaces defined by these triangles
	AddSurface( surfaces, scount, vcount-4, vcount-1, vcount-3 );
	AddSurface( surfaces, scount, vcount-2, vcount-3, vcount-1 );
};

//==========================================================================

TLandscape* CreateLandscapeObject( const TDefFile& def, TString& errStr, int )
{
	return CreateLandscapeObject( def, errStr );
}

TLandscape* CreateLandscapeObject( const TDefFile& def, TString& errStr )
{
	size_t i,x,y;

	TLandscape* land;
	TBinObject* obj;

	// load RGB
	TTexture bmp;
	bmp.DontDestroyRGB(true);
	if ( !bmp.LoadBinary( def.BMPName(), "data\\stories", errStr ) )
	{
		return NULL;
	}
	byte* rgb = const_cast<byte*>(bmp.Rgb());
	PostCond( rgb!=NULL );
	size_t w = bmp.Width();
	size_t h = bmp.Height();
	PreCond( w==h );

	// force 0 height around the edges
	for ( i=0; i<w; i++ )
	{
		rgb[i*3+0] = 0;
		rgb[i*3+1] = 0;
		rgb[i*3+2] = 0;

		rgb[(h-1)*w*3+i*3+0] = 0;
		rgb[(h-1)*w*3+i*3+1] = 0;
		rgb[(h-1)*w*3+i*3+2] = 0;
	}

	for ( i=0; i<h; i++ )
	{
		rgb[i*3*w+0] = 0;
		rgb[i*3*w+1] = 0;
		rgb[i*3*w+2] = 0;

		rgb[i*3*w+(w-1)*3+0] = 0;
		rgb[i*3*w+(w-1)*3+1] = 0;
		rgb[i*3*w+(w-1)*3+2] = 0;
	}

	float waterLevel = 0;
	obj = CreateBinaryMesh( rgb, w,h, size_t(def.WaterLevel()), 
							def.BMPStrip(), def.Material(), errStr );
	if ( obj==NULL )
	{
		return NULL;
	}

	obj->CalculateBoundaries();

	land = new TLandscape();
	PostCond( land!=NULL );

	land->divx = 64;
	land->divz = 64;

	float bx1 = _bx1;
	float bx2 = _bx2;
	float by1 = _by1;
	float by2 = _by2;
	float bz1 = _bz1;
	float bz2 = _bz2;

	float divSizeX = (bx2-bx1) / float(land->divx);
	float divSizeY = (bz2-bz1) / float(land->divz);

	land->Initialised(true);
	land->Def( def );

	land->landscapeminX = bx1;
	land->landscapeminZ = bz1;

	land->divSizeX = divSizeX;
	land->divSizeY = divSizeY;
	land->MinX(bx1);
	land->MinY(by1);
	land->MinZ(bz1);
	land->MaxX(bx2);
	land->MaxY(by2);
	land->MaxZ(bz2);
	land->CenterX(obj->CenterX());
	land->CenterY(obj->CenterY());
	land->CenterZ(obj->CenterZ());

	land->divRadiusSize = float(sqrt(divSizeX*divSizeX+divSizeY*divSizeY));

	// add 3 materials from obj to land
    TMaterialList* ml2 = obj->MaterialList();
    while ( ml2!=NULL )
    {
		land->AddMaterial( ml2->material );
    	ml2 = ml2->next;
    }
	obj->MaterialList(NULL); // make sure obj can't destroy them

	float _seaHeight = float(def.WaterLevel()) * 2.55f;

	// zero slice for drawing blanks outside the map
	TMesh* zeroSeaSlice = new TMesh();
	TMesh* m = zeroSeaSlice;

#ifdef _ALTERNATESEA
	// add sea slice to this mesh
	{
		size_t vcount = 0;
		size_t scount = 0;
		size_t surfaces[200];
		float  vertices[200];

		CreateSurroundMesh( _seaHeight, vcount, vertices, scount, surfaces );

		PreCond( scount>0 );

		// setup new mesh
		m->NumVertices( vcount );
		m->Vertices( vertices );
		m->NumFaces( scount );
		m->Surfaces( surfaces );

		// one material with a face for each surface
		m->NumMaterials( 1 );
		m->Materials( 0, 2, land->GetMaterial(2) );
		m->NumMaterialFaces( scount );
		size_t matFaces[16];
		for ( i=0; i<16; i++ )
			matFaces[i] = 0;
		m->MaterialFaces( matFaces );

		// one normal, with a normal face for each vertex
		float normal[3];
		normal[0] = 0;
		normal[1] = 1;
		normal[2] = 0;
		m->NumNormals( 1 );
		m->Normals( normal );
		m->NumNormalFaces( vcount );
		size_t normalFaces[100];
		for ( i=0; i<100; i++ )
			normalFaces[i] = 0;
		m->NormalFaces( normalFaces );

		m->NumTextureCoords( vcount );
		float* v = m->TextureCoords();

	// 1.0;-5.0;0.000000;,
	// 5.0;-5.0;0.000000;,
	// 5.0;5.0;0.000000;,
	// 1.0;5.0;0.000000;;

		size_t uvindex = 0;
		float scale = 0.1f; // 1.5625f;
		for ( i=0; i<(vcount/4); i++ )
		{
			float v1 = vertices[uvindex*3+0];
			float v3 = vertices[uvindex*3+2];
			v[uvindex*2+0] = v1 * scale;
			v[uvindex*2+1] = v3 * scale;
			uvindex++;

			v1 = vertices[uvindex*3+0];
			v3 = vertices[uvindex*3+2];
			v[uvindex*2+0] = v1 * scale;
			v[uvindex*2+1] = v3 * scale;
			uvindex++;

			v1 = vertices[uvindex*3+0];
			v3 = vertices[uvindex*3+2];
			v[uvindex*2+0] = v1 * scale;
			v[uvindex*2+1] = v3 * scale;
			uvindex++;

			v1 = vertices[uvindex*3+0];
			v3 = vertices[uvindex*3+2];
			v[uvindex*2+0] = v1 * scale;
			v[uvindex*2+1] = v3 * scale;
			uvindex++;
		}
	}
#else
	// add sea slice to this mesh
	{
		size_t vcount = 0;
		size_t scount = 0;
		size_t surfaces[200];
		float  vertices[200];

		float f = 0.99f;
		AddSeaToMesh2( 0, 0, rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );
		AddSeaToMesh2( f, 0, rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );
		AddSeaToMesh2( 0, f, rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );
		AddSeaToMesh2( f, f, rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );

		PreCond( scount>0 );

		// setup new mesh
		m->NumVertices( vcount );
		m->Vertices( vertices );
		m->NumFaces( scount );
		m->Surfaces( surfaces );

		// one material with a face for each surface
		m->NumMaterials( 1 );
		m->Materials( 0, 2, land->GetMaterial(2) );
		m->NumMaterialFaces( scount );
		size_t matFaces[16];
		for ( i=0; i<16; i++ )
			matFaces[i] = 0;
		m->MaterialFaces( matFaces );

		// one normal, with a normal face for each vertex
		float normal[3];
		normal[0] = 0;
		normal[1] = 1;
		normal[2] = 0;
		m->NumNormals( 1 );
		m->Normals( normal );
		m->NumNormalFaces( vcount );
		size_t normalFaces[100];
		for ( i=0; i<100; i++ )
			normalFaces[i] = 0;
		m->NormalFaces( normalFaces );

		m->NumTextureCoords( vcount );
		float* v = m->TextureCoords();

		size_t uvindex = 0;
		for ( i=0; i<(vcount/4); i++ )
		{
			v[uvindex*2+0] = 0;
			v[uvindex*2+1] = 0;
			uvindex++;

			v[uvindex*2+0] = 1;
			v[uvindex*2+1] = 0;
			uvindex++;

			v[uvindex*2+0] = 1;
			v[uvindex*2+1] = 1;
			uvindex++;

			v[uvindex*2+0] = 0;
			v[uvindex*2+1] = 1;
			uvindex++;
		}
	}
#endif
	land->zeroSeaSlice = zeroSeaSlice;

	// zero slice for drawing blanks outside the map
	TMesh* zeroSurfaceSlice = new TMesh();
	m = zeroSurfaceSlice;

#ifdef _ALTERNATESEA
	// add land slice to this mesh
	{
		size_t vcount = 0;
		size_t scount = 0;
		size_t surfaces[200];
		float  vertices[200];

		CreateSurroundMesh( 0, vcount, vertices, scount, surfaces );
		PreCond( scount>0 );

		// setup new mesh
		m->NumVertices( vcount );
		m->Vertices( vertices );
		m->NumFaces( scount );
		m->Surfaces( surfaces );

		// one material with a face for each surface
		m->NumMaterials( 1 );
		m->Materials( 0, 0, land->GetMaterial(0) );
		m->NumMaterialFaces( scount );
		size_t matFaces[16];
		for ( i=0; i<16; i++ )
			matFaces[i] = 0;
		m->MaterialFaces( matFaces );

		// one normal, with a normal face for each vertex
		float normal[3];
		normal[0] = 0;
		normal[1] = 1;
		normal[2] = 0;
		m->NumNormals( 1 );
		m->Normals( normal );
		m->NumNormalFaces( vcount );
		size_t normalFaces[100];
		for ( i=0; i<100; i++ )
			normalFaces[i] = 0;
		m->NormalFaces( normalFaces );

		m->NumTextureCoords( vcount );
		float* v = m->TextureCoords();

		size_t uvindex = 0;
		for ( i=0; i<(vcount/4); i++ )
		{
			v[uvindex*2+0] = 0;
			v[uvindex*2+1] = 0;
			uvindex++;

			v[uvindex*2+0] = 1;
			v[uvindex*2+1] = 0;
			uvindex++;

			v[uvindex*2+0] = 1;
			v[uvindex*2+1] = 1;
			uvindex++;

			v[uvindex*2+0] = 0;
			v[uvindex*2+1] = 1;
			uvindex++;
		}
	}
#else
	// add land slice to this mesh
	{
		size_t vcount = 0;
		size_t scount = 0;
		size_t surfaces[200];
		float  vertices[200];

		AddLandToMesh2( 0, 0, rgb,w,h, vcount, vertices, scount, surfaces );
		AddLandToMesh2( 1, 0, rgb,w,h, vcount, vertices, scount, surfaces );
		AddLandToMesh2( 0, 1, rgb,w,h, vcount, vertices, scount, surfaces );
		AddLandToMesh2( 1, 1, rgb,w,h, vcount, vertices, scount, surfaces );

		PreCond( scount>0 );

		// setup new mesh
		m->NumVertices( vcount );
		m->Vertices( vertices );
		m->NumFaces( scount );
		m->Surfaces( surfaces );

		// one material with a face for each surface
		m->NumMaterials( 1 );
		m->Materials( 0, 0, land->GetMaterial(0) );
		m->NumMaterialFaces( scount );
		size_t matFaces[16];
		for ( i=0; i<16; i++ )
			matFaces[i] = 0;
		m->MaterialFaces( matFaces );

		// one normal, with a normal face for each vertex
		float normal[3];
		normal[0] = 0;
		normal[1] = 1;
		normal[2] = 0;
		m->NumNormals( 1 );
		m->Normals( normal );
		m->NumNormalFaces( vcount );
		size_t normalFaces[100];
		for ( i=0; i<100; i++ )
			normalFaces[i] = 0;
		m->NormalFaces( normalFaces );

		m->NumTextureCoords( vcount );
		float* v = m->TextureCoords();

		size_t uvindex = 0;
		for ( i=0; i<(vcount/4); i++ )
		{
			v[uvindex*2+0] = 0;
			v[uvindex*2+1] = 0;
			uvindex++;

			v[uvindex*2+0] = 1;
			v[uvindex*2+1] = 0;
			uvindex++;

			v[uvindex*2+0] = 1;
			v[uvindex*2+1] = 1;
			uvindex++;

			v[uvindex*2+0] = 0;
			v[uvindex*2+1] = 1;
			uvindex++;
		}
	}
#endif

	land->zeroSurfaceSlice = zeroSurfaceSlice;

	//
	// create actual divisions
	// which are the water divided into mesh segments
	//
	PreCond( land->divisions==NULL );
	land->divisions = new TMesh*[land->divx*land->divz];
	PostCond( land->divisions!=NULL );

	for ( y=0; y<land->divz; y++ )
	for ( x=0; x<land->divx; x++ )
	{
		land->divisions[y*land->divx+x] = NULL;
	}
	
	for ( y=0; y<land->divz; y++ )
	for ( x=0; x<land->divx; x++ )
    {
		size_t vcount = 0;
		size_t scount = 0;
		size_t surfaces[100];
		float  vertices[100];

		AddSeaToMesh( (x*2+0), (y*2+0), rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );
		AddSeaToMesh( (x*2+1), (y*2+0), rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );
		AddSeaToMesh( (x*2+0), (y*2+1), rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );
		AddSeaToMesh( (x*2+1), (y*2+1), rgb,w,h, _seaHeight, vcount, vertices, scount, surfaces );

		if ( scount>0 )
		{
			// take a random vertex from this slice
			float vx = vertices[0];
			float vz = vertices[2];

			size_t landx = x;
			size_t landy = (land->divz-1) - y;
			size_t index = landy * land->divx + landx;

			PreCond( land->divisions[index]==NULL );

			TMesh* m = new TMesh();

			// setup new mesh
			m->NumVertices( vcount );
			m->Vertices( vertices );
			m->NumFaces( scount );
			m->Surfaces( surfaces );

			// get waterlevel from vertex
			waterLevel = vertices[1];

			// one material with a face for each surface
			m->NumMaterials( 1 );
			m->Materials( 0, 2, land->GetMaterial(2) );
			m->NumMaterialFaces( scount );
			size_t matFaces[16];
			for ( i=0; i<16; i++ )
				matFaces[i] = 0;
			m->MaterialFaces( matFaces );

			// one normal, with a normal face for each vertex
			float normal[3];
			normal[0] = 0;
			normal[1] = 1;
			normal[2] = 0;
			m->NumNormals( 1 );
			m->Normals( normal );
			m->NumNormalFaces( vcount );
			size_t normalFaces[100];
			for ( i=0; i<100; i++ )
				normalFaces[i] = 0;
			m->NormalFaces( normalFaces );

			m->NumTextureCoords( vcount );
			float* v = m->TextureCoords();

			size_t uvindex = 0;
			for ( i=0; i<(vcount/4); i++ )
			{
				v[uvindex*2+0] = 0;
				v[uvindex*2+1] = 0;
				uvindex++;

				v[uvindex*2+0] = 1;
				v[uvindex*2+1] = 0;
				uvindex++;

				v[uvindex*2+0] = 1;
				v[uvindex*2+1] = 1;
				uvindex++;

				v[uvindex*2+0] = 0;
				v[uvindex*2+1] = 1;
				uvindex++;
			}
			// set mesh
			land->divisions[index] = m;
		}
    }

	if ( land->landscapeMesh!=NULL )
		delete land->landscapeMesh;
	land->landscapeMesh = new TMesh();

	*land->landscapeMesh = *obj->GetMesh(0);
	TMesh* landscapeMesh = land->landscapeMesh;

	// create small map from rgb
	size_t sw = w / 2;
	size_t sh = h / 2;
	byte* smallRGB = new byte[sh*sw*3];
	for ( y=0; y<sh; y++ )
	for ( x=0; x<sw; x++ )
	{
		byte r;
		float hr1 = float(rgb[(y*2+0)*w*3+(x*2+0)*3+0]);
		float hr2 = float(rgb[(y*2+0)*w*3+(x*2+1)*3+0]);
		float hr3 = float(rgb[(y*2+1)*w*3+(x*2+0)*3+0]);
		float hr4 = float(rgb[(y*2+1)*w*3+(x*2+1)*3+0]);
		r = byte( (hr1+hr2+hr3+hr4)*0.25f );

		byte g;
		float hg1 = float(rgb[(y*2+0)*w*3+(x*2+0)*3+1]);
		float hg2 = float(rgb[(y*2+0)*w*3+(x*2+1)*3+1]);
		float hg3 = float(rgb[(y*2+1)*w*3+(x*2+0)*3+1]);
		float hg4 = float(rgb[(y*2+1)*w*3+(x*2+1)*3+1]);
		g = byte( (hg1+hg2+hg3+hg4)*0.25f );

		byte b;
		float hb1 = float(rgb[(y*2+0)*w*3+(x*2+0)*3+2]);
		float hb2 = float(rgb[(y*2+0)*w*3+(x*2+1)*3+2]);
		float hb3 = float(rgb[(y*2+1)*w*3+(x*2+0)*3+2]);
		float hb4 = float(rgb[(y*2+1)*w*3+(x*2+1)*3+2]);
		b = byte( (hb1+hb2+hb3+hb4)*0.25f );

		smallRGB[y*3*sw+x*3+0] = r;
		smallRGB[y*3*sw+x*3+1] = g;
		smallRGB[y*3*sw+x*3+2] = b;
	}

	TBinObject* smallMap = CreateBinaryMesh( smallRGB,sw,sh, size_t(def.WaterLevel()), 
											 def.BMPStrip(), def.Material(), errStr );

	// release memory used
	delete []smallRGB;

	if ( smallMap==NULL )
	{
		return NULL;
	}
	land->smallMap = smallMap;
	land->owner = true; // take ownership of the small map


	// setup main divisions and internal references to main mesh
	land->gvertices = landscapeMesh->Vertices();
	land->gnumVertices = landscapeMesh->NumVertices();
	land->gsurfaces = landscapeMesh->Surfaces();
	land->gnumSurfaces = landscapeMesh->NumFaces();

	// load landscape surface divisions
	land->surfaceDivisionCounts = new size_t[land->divx*land->divz];
	land->surfaceDivisions = new size_t*[land->divx*land->divz];
	land->surfaceMaterialFaces = new size_t*[land->divx*land->divz];

	for ( y=0; y<land->divz; y++ )
	for ( x=0; x<land->divx; x++ )
	{
		land->surfaceDivisions[y*land->divx+x] = NULL;
	};

	for ( y=0; y<land->divz; y++ )
	for ( x=0; x<land->divx; x++ )
    {
		size_t vcount = 0;
		size_t scount = 0;
		size_t surfaces[100];
		float  vertices[100];

		AddLandToMesh( (x*2+0), (y*2+0), rgb,w,h, vcount, vertices, scount, surfaces );
		AddLandToMesh( (x*2+1), (y*2+0), rgb,w,h, vcount, vertices, scount, surfaces );
		AddLandToMesh( (x*2+0), (y*2+1), rgb,w,h, vcount, vertices, scount, surfaces );
		AddLandToMesh( (x*2+1), (y*2+1), rgb,w,h, vcount, vertices, scount, surfaces );

		// take the middle vertex from this slice
		float minx, maxx, minz,maxz;
		minx = vertices[0];
		minz = vertices[2];
		maxx = minx;
		maxz = minz;
		for ( i=1; i<vcount; i++ )
		{
			if ( vertices[i*3+0] < minx )
				minx = vertices[i*3+0];
			if ( vertices[i*3+2] < minz )
				minz = vertices[i*3+2];
			if ( vertices[i*3+0] > maxx )
				maxx = vertices[i*3+0];
			if ( vertices[i*3+2] > maxz )
				maxz = vertices[i*3+2];
		}
		float vx = (minx+maxx) * 0.5f;
		float vz = (minz+maxz) * 0.5f;

		size_t landx = size_t((vx-bx1) / divSizeX);
		size_t landy = size_t((vz-bz1) / divSizeY);
		size_t index = landy*land->divx+landx;

		// and do the honours
		land->surfaceDivisionCounts[index] = scount;

		PreCond( land->surfaceDivisions[index]==NULL );

		size_t* surf = new size_t[scount*3];
		land->surfaceDivisions[index] = surf;
		for ( i=0; i<scount*3; i++ )
			surf[i] = surfaces[i];

		size_t* matsurf = new size_t[scount];
		land->surfaceMaterialFaces[index] = matsurf;
		for ( i=0; i<scount; i++ )
			matsurf[i] = 0;
	}

	// get the center of slice(0) for translating the piece
	// into off-world positions
	PostCond( land->surfaceDivisionCounts[0]>0 );
	float* v = landscapeMesh->Vertices();
	size_t s = land->surfaceDivisions[0][0];
	land->zeroMinx = v[s*3+0];
	land->zeroMinz = v[s*3+2];
	for ( i=1; i<land->surfaceDivisionCounts[0]; i++ )
	{
		size_t s = land->surfaceDivisions[0][i];
		if ( v[s*3+0] < land->zeroMinx )
			land->zeroMinx = v[s*3+0];
		if ( v[s*3+2] < land->zeroMinz )
			land->zeroMinz = v[s*3+2];
	}

	// set landscape water level (converted * 2.55f)
	land->waterLevel = waterLevel;

	// colour mapped?
	if ( landscapeMesh->IsColourMapped() )
	{
		land->landscapeMesh->ColourMapSetWaterLevel( size_t(def.WaterLevel()), w, h  );
		float r,g,b;
		land->landscapeMesh->GetSeaSurfaceColour( r,g,b );
		land->SetSeaSurfaceColour( r,g,b );
	}

	// set player info
	land->playerInfo = const_cast<TDefFile&>(def).GetPlayerInfo();

	land->landscapeObjectDivisions = new TLandscapeObjectDraw*[land->divx*land->divz];
	for ( y=0; y<land->divz; y++ )
	for ( x=0; x<land->divx; x++ )
	{
		land->landscapeObjectDivisions[y*land->divx+x] = NULL;
	}

	// must set renderdepth for circle
	// calculate min and max render depths
	float depth = float(land->divx + land->divz) * 0.5f;
	size_t max = size_t( depth / 4.0f );
	if ( max<10 )
		max = 10;
	land->RenderDepth( max );

	// remove object
	delete obj;

	// return created landscape
	return land;
};

//==========================================================================

#endif

