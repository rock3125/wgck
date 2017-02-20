#include <precomp_header.h>

#include <object/geometry.h>
#include <object/graph.h>

//==========================================================================

TGraph::TGraph( void )
	: gsurfaces(NULL),
	  gvertices(NULL),
	  graph(NULL)
{
	gnumFaces = 0;
	recursionCounter = 0;
};


TGraph::TGraph( size_t _gnumFaces, size_t* _gsurfaces, 
			    size_t _gnumVertices, float* _gvertices )
	: gsurfaces(NULL),
	  gvertices(NULL),
	  graph(NULL)
{
	PreCond( _gnumFaces>0 );
	PreCond( _gsurfaces!=NULL );
	PreCond( _gvertices!=NULL );

	gnumFaces = _gnumFaces;
	gnumVertices = _gnumVertices;

	gsurfaces = new size_t[gnumFaces*3];
	gvertices = new float[gnumVertices*3];

	size_t i;
	for ( i=0; i<gnumFaces*3; i++ )
		gsurfaces[i] = _gsurfaces[i];
	for ( i=0; i<gnumVertices*3; i++ )
		gvertices[i] = _gvertices[i];
	
	graph = new int[gnumFaces*4];
	for ( i=0; i<gnumFaces; i++ )
	{
		graph[i*4+0] = -1;
		graph[i*4+1] = -1;
		graph[i*4+2] = -1;
		graph[i*4+3] = 0;
	}

	recursionCounter = 0;
};


TGraph::~TGraph( void )
{
	gnumFaces = 0;
	gnumVertices = 0;

	if ( gsurfaces!=NULL )
		delete gsurfaces;
	gsurfaces = NULL;

	if ( gvertices!=NULL )
		delete gvertices;
	gvertices = NULL;

	if ( graph!=NULL )
		delete graph;
	graph = NULL;
};


const TGraph& TGraph::operator=( const TGraph& g )
{
	gnumFaces = g.gnumFaces;
	gnumVertices = g.gnumVertices;

	if ( gsurfaces!=NULL )
		delete gsurfaces;
	if ( gvertices!=NULL )
		delete gvertices;
	gsurfaces = new size_t[gnumFaces*3];
	gvertices = new float[gnumVertices*3];

	size_t i;
	for ( i=0; i<gnumFaces*3; i++ )
		gsurfaces[i] = g.gsurfaces[i];
	for ( i=0; i<gnumVertices*3; i++ )
		gvertices[i] = g.gvertices[i];

	recursionCounter = g.recursionCounter;
	lorReference = g.lorReference;

	if ( graph!=NULL )
		delete graph;
	graph = NULL;

	if ( gnumFaces>0 )
	{
		graph = new int[4*gnumFaces];
		for ( size_t i=0; i<gnumFaces; i++ )
		{
			graph[i*4+0] = g.graph[i*4+0];
			graph[i*4+1] = g.graph[i*4+1];
			graph[i*4+2] = g.graph[i*4+2];
			graph[i*4+3] = g.graph[i*4+3];
		}
	}
	return *this;
};


TGraph::TGraph( const TGraph& g)
	: gsurfaces(NULL),
	  gvertices(NULL),
	  graph(NULL)
{
	gnumFaces = 0;
	operator=(g);
};


void TGraph::GetTriangleCenter( size_t surf,
								float& vx,
								float& vy,
								float& vz )
{
	vx = 0.0f;
	vy = 0.0f;
	vz = 0.0f;

	size_t s1 = gsurfaces[surf*3+0];
	vx = vx + gvertices[s1*3+0];
	vy = vy + gvertices[s1*3+1];
	vz = vz + gvertices[s1*3+2];

	s1 = gsurfaces[surf*3+1];
	vx = vx + gvertices[s1*3+0];
	vy = vy + gvertices[s1*3+1];
	vz = vz + gvertices[s1*3+2];

	s1 = gsurfaces[surf*3+2];
	vx = vx + gvertices[s1*3+0];
	vy = vy + gvertices[s1*3+1];
	vz = vz + gvertices[s1*3+2];

	vx = vx * 0.33333f;
	vy = vy * 0.33333f;
	vz = vz * 0.33333f;
};


bool TGraph::InsideSurface( float x, float &y, float z, size_t surface )
{
	size_t s1 = gsurfaces[surface*3+0];
	size_t s2 = gsurfaces[surface*3+1];
	size_t s3 = gsurfaces[surface*3+2];

	float vx1 = gvertices[s1*3+0];
	float vy1 = gvertices[s1*3+1];
	float vz1 = gvertices[s1*3+2];

	float vx2 = gvertices[s2*3+0];
	float vy2 = gvertices[s2*3+1];
	float vz2 = gvertices[s2*3+2];

	float vx3 = gvertices[s3*3+0];
	float vy3 = gvertices[s3*3+1];
	float vz3 = gvertices[s3*3+2];

	float maxx = vx1;
	float minx = vx1;
	if ( vx2>maxx )
		maxx = vx2;
	if ( vx3>maxx )
		maxx = vx3;
	if ( vx2<minx ) 
		minx = vx2;
	if ( vx3<minx )
		minx = vx3;

	float maxz = vz1;
	float minz = vz1;
	if ( vz2>maxz )
		maxz = vz2;
	if ( vz3>maxz )
		maxz = vz3;
	if ( vz2<minz ) 
		minz = vz2;
	if ( vz3<minz )
		minz = vz3;

	if ( minx<=x && x<=maxx )
	if ( minz<=z && z<=maxz )
	if ( PointInTriangle( x,y,z, vx1,vy1,vz1, vx2,vy2,vz2, vx3,vy3,vz3 ) )
	{
		lorReference = surface;
		return true;
	}
	return false;
};


bool TGraph::GetExhaustiveSearchTriangle( float x, float& y, float z )
{
	lorReference = -1;
	for ( size_t i=0; i<gnumFaces; i++ )
	{
		if ( InsideSurface( x,y,z, i ) )
			return true;
	}
	return false;
};


bool TGraph::GetLORTriangle( float x, float& y, float z,
							 int surface, int& lorRef )
{
	lorReference = -1;
	recursionCounter++;
	float prevy = y;
	if ( GetLORTriangle( x,y,z, surface, 0 ) )
	{
		lorRef = lorReference;
		return true;
	}
	else
	{
		y = prevy;
		lorRef = -1;
		return false;
	}
};


bool TGraph::GetLORTriangle(  float x, float &y, float z,
							  int surface, size_t rdepth )
{
	// initial value - do exhaustive search
	if ( surface==-1 )
		return GetExhaustiveSearchTriangle( x,y,z );

	if ( rdepth>10 )
		return false;

	if ( InsideSurface( x,y,z, surface ) )
	{
		return true;
	}
	else
	{
		bool found = false;
		int* lor = &graph[surface*4];
		if ( lor[4]!=recursionCounter )
		{
			// heuristically search the closest triangles
			float cx,cy,cz;
			float d1 = 999999999.0f;
			float d2 = 999999999.0f;
			float d3 = 999999999.0f;

			if ( lor[0]!=-1 )
			{
				GetTriangleCenter(lor[0],cx,cy,cz);
				d1 = ( (cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z) );
			}
			if ( lor[1]!=-1 )
			{
				GetTriangleCenter(lor[1],cx,cy,cz);
				d2 = ( (cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z) );
			}
			if ( lor[2]!=-1 )
			{
				GetTriangleCenter(lor[2],cx,cy,cz);
				d3 = ( (cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z) );
			}
			lor[3] = recursionCounter;

			if ( d1 < d2 && d1 < d3 )
			{
				if ( lor[0]!=-1 )
					found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
				if ( d2 < d3 && !found )
				{
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
				}
				else if ( !found )
				{
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
				}
			}
			else if ( d2 < d1 && d2 < d3 )
			{
				if ( lor[1]!=-1 )
					found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
				if ( d1 < d3 && !found )
				{
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
				}
				else if ( !found )
				{
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
				}
			}
			else if ( d3 < d2 && d3 < d1 )
			{
				if ( lor[2]!=-1 )
					found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
				if ( d1 < d2 && !found )
				{
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
				}
				else if ( !found )
				{
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
				}
			}
		}
		return found;
	}
};


//
//	Given two vertices, find another surface to orgSurface that has these vertices
//	or return -1 if not.  Static version - takes outside surfaces as parameters
//
int TGraph::FindNeighbour( size_t orgSurface, size_t vert1, size_t vert2 )
{
	size_t i = 0;
	while ( i<gnumFaces )
	{
		if ( i!=orgSurface )
		{
			size_t v1 = gsurfaces[i*3+0];
			size_t v2 = gsurfaces[i*3+1];
			size_t v3 = gsurfaces[i*3+2];
			if ( ( v1==vert1 || v2==vert1 || v3==vert1 ) &&
				 ( v1==vert2 || v2==vert2 || v3==vert2 ) )
				return i;
		}
		i++;
	};

	// if we couldn't find it the simple way - exhaustively search
	// the vertices to see if two points aren't connected after all
	float vx1 = gvertices[vert1*3+0];
	float vy1 = gvertices[vert1*3+1];
	float vz1 = gvertices[vert1*3+2];

	float vx2 = gvertices[vert2*3+0];
	float vy2 = gvertices[vert2*3+1];
	float vz2 = gvertices[vert2*3+2];

	i = 0;
	while ( i<gnumFaces )
	{
		if ( i!=orgSurface )
		{
			size_t s1 = gsurfaces[i*3+0];
			size_t s2 = gsurfaces[i*3+1];
			size_t s3 = gsurfaces[i*3+2];

			float sx1 = gvertices[s1*3+0];
			float sy1 = gvertices[s1*3+1];
			float sz1 = gvertices[s1*3+2];

			float sx2 = gvertices[s2*3+0];
			float sy2 = gvertices[s2*3+1];
			float sz2 = gvertices[s2*3+2];
		
			float sx3 = gvertices[s3*3+0];
			float sy3 = gvertices[s3*3+1];
			float sz3 = gvertices[s3*3+2];

			bool s1ok = false;
			if ( (vx1==sx1 && vy1==sy1 && vz1==sz1) ||
				 (vx1==sx2 && vy1==sy2 && vz1==sz2) ||
				 (vx1==sx3 && vy1==sy3 && vz1==sz3) )
				s1ok = true;

			bool s2ok = false;
			if ( (vx2==sx1 && vy2==sy1 && vz2==sz1) ||
				 (vx2==sx2 && vy2==sy2 && vz2==sz2) ||
				 (vx2==sx3 && vy2==sy3 && vz2==sz3) )
				s2ok = true;

			if ( s1ok && s2ok )
				return i;
		}
		i++;
	};
	return -1;
};


// Create a surface graph
void TGraph::CreateSurfaceGraph( void )
{
	size_t i;

	PreCond( gvertices!=NULL && gsurfaces!=NULL && gnumFaces>0 );

	if ( graph!=NULL )
		delete graph;
	graph = new int[gnumFaces*4];

	for ( i=0; i<gnumFaces; i++ )
	{
		graph[i*4+0] = -1;
		graph[i*4+1] = -1;
		graph[i*4+2] = -1;
		graph[i*4+3] = 0;
	}

	ResetPercentage();
	for ( i=0; i<gnumFaces; i++ )
	{
		DisplayPercentage(i,gnumFaces);

		size_t s1 = gsurfaces[i*3+0];
		size_t s2 = gsurfaces[i*3+1];
		size_t s3 = gsurfaces[i*3+2];

		int surf = FindNeighbour(i,s1,s2);
		graph[i*4+0] = surf;

		surf = FindNeighbour(i,s2,s3);
		graph[i*4+1] = surf;

		surf = FindNeighbour(i,s3,s1);
		graph[i*4+2] = surf;

		graph[i*4+3] = 0;
	};
	FinishPercentage();
};


bool TGraph::SaveBinary( TPersist& file )
{
	size_t i;

	// graph
	if ( graph==NULL )
	{
		i = 0;
		file.FileWrite( &i, sizeof(size_t) );
	}
	else
	{
		i = gnumFaces;
		file.FileWrite( &i, sizeof(size_t) );

		// copy graph into smaller memory structure, discarding
		// recursion counter
		int* temp = new int[gnumFaces*3];
		for ( size_t j=0; j<gnumFaces; j++ )
		{
			temp[j*3+0] = graph[j*4+0];
			temp[j*3+1] = graph[j*4+1];
			temp[j*3+2] = graph[j*4+2];
		}
		file.FileWrite( temp, sizeof(int)*3*gnumFaces );
		delete temp;
	}
	return true;
};


bool TGraph::LoadBinary( TPersist& file, TString& errStr )
{
	if ( graph!=NULL )
		delete graph;
	graph = NULL;

	// graph
	file.FileRead( &gnumFaces, sizeof(size_t) );
	if ( gnumFaces>0)
	{
		// get graph into smaller memory structure
		int* temp = new int[gnumFaces*3];
		file.FileRead( temp, sizeof(int)*3*gnumFaces );
		// copy graph into its new structure
		graph = new int[gnumFaces*4];
		for ( size_t j=0; j<gnumFaces; j++ )
		{
			graph[j*4+0] = temp[j*3+0];
			graph[j*4+1] = temp[j*3+1];
			graph[j*4+2] = temp[j*3+2];
			graph[j*4+3] = 0;
		}
		delete temp;
	}
	return true;
};


bool TGraph::IsValidGraph( void ) const
{
	return (gnumFaces>0);
};

//==========================================================================

