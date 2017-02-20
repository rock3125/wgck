#include <precomp_header.h>

#include <lib3ds/material.h>
#include <lib3ds/mesh.h>

#include <parser/imp3ds.h>

//==========================================================================

void MatrixMult( Lib3dsMatrix& matrix, 
				 float* v1, float* res )
{
	float a,b,c;
	float x = v1[0];
	float y = v1[1];
	float z = v1[2];
	a = x*matrix[0][0] + y*matrix[0][1] + z*matrix[0][2] + matrix[0][3];
	b = x*matrix[1][0] + y*matrix[1][1] + z*matrix[1][2] + matrix[1][3];
	c = x*matrix[2][0] + y*matrix[2][1] + z*matrix[2][2] + matrix[2][3];
	res[0] = a;
	res[1] = b;
	res[2] = c;
}

//==========================================================================

TBinObject* ToBin( const TString& orgFile, Lib3dsFile* obj3ds )
{
	TBinObject* obj = new TBinObject();
	size_t j;

	Lib3dsMaterial* mt = obj3ds->materials;
	size_t numMaterials = 0;
	while ( mt!=NULL )
	{
		TMaterial mat;
		
		mat.MaterialName( mt->name );
		mat.SetMaterialSpecularity( mt->specular[0],
									mt->specular[1],
									mt->specular[2],
									mt->specular[3] );
		mat.SetMaterialEmission( mt->diffuse[0],
								 mt->diffuse[1],
								 mt->diffuse[2],
								 mt->diffuse[3] );
		mat.SetMaterialShininess( mt->shininess );
		mat.SetMaterialColour( 1,1,1,1 );

		TString str = mt->texture1_map.name;
		TString path;
		TString errStr;
		size_t len = orgFile.length();
		while ( orgFile[len]!='\\' && len>0 ) len--;
		if ( orgFile[len]=='\\' )
		{
			path = orgFile;
			path[len] = 0;
		}
		mat.FileName( str, path, errStr );
		obj->AddMaterial( mat );
		numMaterials++;

		mt = mt->next;
	}

	Lib3dsMesh* ms = obj3ds->meshes;
	while ( ms!=NULL )
	{
		TMesh mesh;

		mesh.Name( ms->name );
		mesh.NumMaterials( numMaterials );

		mesh.NumVertices( ms->points );
		float* vert = mesh.Vertices();
		Lib3dsPoint* p = ms->pointL;
		for ( j=0; j<ms->points; j++ )
		{
			float res[4];
			float v[4];
			v[0] = p->pos[0];
			v[1] = p->pos[1];
			v[2] = p->pos[2];
			p = &p[1];
			MatrixMult( ms->matrix, v, res );
			vert[j*3+0] = res[0];
			vert[j*3+1] = res[1];
			vert[j*3+2] = res[2];
		}

		mesh.NumTextureCoords( ms->texels );
		float* uv = mesh.TextureCoords();
		for ( j=0; j<ms->texels; j++ )
		{
			uv[j*2+0] = ms->texelL[j][0];
			uv[j*2+1] = ms->texelL[j][1];
		}

		size_t numFaces = ms->faces;

		mesh.NumFaces( numFaces );
		mesh.NumMaterialFaces( numFaces );
		mesh.NumNormals( numFaces*3 );
		mesh.NumNormalFaces( numFaces*3 );

		float*  norm = mesh.Normals();
		size_t* face = mesh.Surfaces();
		size_t* nface = mesh.NormalFaces();
		size_t* mface = mesh.MaterialFaces();

		for ( j=0; j<numFaces; j++ )
		{
		    Lib3dsFace f = ms->faceL[j];

			size_t s1 = f.points[0];
			size_t s2 = f.points[1];
			size_t s3 = f.points[2];

			face[j*3+0] = s3;
			face[j*3+1] = s2;
			face[j*3+2] = s1;

			nface[j*3+0] = s1;
			nface[j*3+1] = s2;
			nface[j*3+2] = s3;

			norm[s1*3+0] = f.normal[0];
			norm[s1*3+1] = f.normal[1];
			norm[s1*3+2] = f.normal[2];

			norm[s2*3+0] = f.normal[0];
			norm[s2*3+1] = f.normal[1];
			norm[s2*3+2] = f.normal[2];

			norm[s3*3+0] = f.normal[0];
			norm[s3*3+1] = f.normal[1];
			norm[s3*3+2] = f.normal[2];

			size_t matIndex = 0;
			bool found = false;
			for ( size_t i=0; i<numMaterials && !found; i++ )
			{
				TMaterial* mat = obj->GetMaterial(i);
				if ( mat->MaterialName()==f.material )
				{
					found = true;
					matIndex = i;
					mesh.Materials(i,i,mat);
				}
			}
			PostCond( found );
			mface[j] = matIndex;
		}
		obj->AddMesh( mesh );
		ms = ms->next;
	}
	return obj;
};

