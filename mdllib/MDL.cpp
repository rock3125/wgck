#include <precomp_header.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gl/gl.h>
#include <GL/glu.h>
#include <mdllib/MDL.h>

#include <object/binobject.h>

#pragma warning( disable : 4244 ) // double to float

const float Rad2Deg = 57.295779511273f;

////////////////////////////////////////////////////////////////////////

MDL::MDL( void )
	: m_pstudiohdr(NULL),
	  m_ptexturehdr(NULL),
	  m_pmodel(NULL)
{
	g_vright[0] = 50;
	g_vright[1] = 50;
	g_vright[2] = 0;		// needs to be set to viewer's right in order for chrome to work
	g_lambert = 1.5;
	bFilterTextures = true;

	modelSize = 0;

	scale = 0.0125f;

	m_origin[0] = 0;
	m_origin[1] = 0;
	m_origin[2] = 0;

	m_translate[0] = 0;
	m_translate[1] = 0;
	m_translate[2] = 0;

	m_angles[0] = 0;
	m_angles[1] = 0;
	m_angles[2] = 0;

	// number of textures
	g_texcount = 0;

	for ( int i=0; i<32; i++ )
	{
		m_panimhdr[i] = NULL;
	}

	prev = 0;

	minx = miny = minz = 0;
	maxx = maxy = maxz = 0;
};


MDL::MDL( const MDL& m )
{
	operator=(m);
};


MDL::~MDL( void )
{
	FreeModel();
};


void CopyVector3( vec3_t& v1, const vec3_t& v2 )
{
	size_t i;
	for ( i=0; i<3; i++ )
		v1[i] = v2[i];
};


const MDL& MDL::operator=( const MDL& m )
{
	size_t i;

	CopyVector3( m_origin, m.m_origin );
	CopyVector3( m_translate, m.m_translate );
	CopyVector3( m_angles, m.m_angles );

	m_sequence = m.m_sequence;
	m_frame = m.m_frame;
	m_bodynum = m.m_bodynum;
	m_skinnum = m.m_skinnum;

	for ( i=0; i<4; i++ )
		m_controller[i] = m.m_controller[i];
	for ( i=0; i<2; i++ )
		m_blending[i] = m.m_blending[i];
	m_mouth = m.m_mouth;
	m_owntexmodel = m.m_owntexmodel;

	modelSize = m.modelSize;
	if ( m_pstudiohdr!=NULL )
		delete m_pstudiohdr;
	if ( m.m_pstudiohdr!=NULL )
	{
		m_pstudiohdr = (studiohdr_t*)new byte[modelSize];
		memcpy( m_pstudiohdr, m.m_pstudiohdr, modelSize );
	}
	else
		m_pstudiohdr = NULL;

	m_ptexturehdr = m_pstudiohdr;
	
	for ( i=0; i<32; i++ )
	{
		if ( m_panimhdr[i]!=NULL )
			delete m_panimhdr[i];
		if ( m.m_panimhdr[i]!=NULL )
		{
			m_panimhdr[i] = new studiohdr_t;
			memcpy( m_panimhdr[i], m.m_panimhdr[i], sizeof(studiohdr_t) );
		}
		else
			m_panimhdr[i] = NULL;
	}

	for ( i=0; i<4; i++ )
		m_adj[i] = m.m_adj[i];

	for ( i=0; i<MAXSTUDIOVERTS; i++ )
	{
		CopyVector3( g_xformverts[i], m.g_xformverts[i] );
		CopyVector3( g_lightvalues[i], m.g_lightvalues[i] );
		g_chrome[i][0] = m.g_chrome[i][0];
		g_chrome[i][1] = m.g_chrome[i][1];
	}

	g_pxformverts = &g_xformverts[0];
	g_pvlightvalues = &g_lightvalues[0];

	CopyVector3( g_lightvec, m.g_lightvec );
	g_ambientlight = m.g_ambientlight;
	g_shadelight = m.g_shadelight;
	CopyVector3( g_lightcolor, m.g_lightcolor );

	g_smodels_total = m.g_smodels_total;

	for ( i=0; i<MAXSTUDIOBONES; i++ )
	{
		CopyVector3( g_blightvec[i], m.g_blightvec[i] );

		for ( size_t j=0; j<3; j++ )
		for ( size_t k=0; k<4; k++ )
			g_bonetransform[i][j][k] = m.g_bonetransform[i][j][k];

		g_chromeage[i] = m.g_chromeage[i];
		CopyVector3( g_chromeup[i], m.g_chromeup[i] );
		CopyVector3( g_chromeright[i], m.g_chromeright[i] );
	}

	CopyVector3( g_vright, m.g_vright );
	g_lambert = m.g_lambert;

	bFilterTextures = m.bFilterTextures;
	g_texcount = m.g_texcount;

	for ( i=0; i<MAXSTUDIOSKINS; i++ )
		textures[i] = m.textures[i];

	prev = m.prev;
	scale = m.scale;

	minx = m.minx;
	miny = m.miny;
	minz = m.minz;

	maxx = m.maxx;
	maxy = m.maxy;
	maxz = m.maxz;

	return *this;
};


void MDL::SetTransRot( float x, float y, float z,
					   float xa, float ya, float za )
{
	m_translate[0] = x; // - m_origin[0];
	m_translate[1] = y; // - m_origin[1];
	m_translate[2] = z; // - m_origin[2];

	m_angles[2] = xa;
	m_angles[0] = ya;
	m_angles[1] = za;
};


void MDL::GetOpenGLTransformation( float* matrix )
{
	glPushMatrix();

		glLoadIdentity();
		glTranslatef(m_translate[0],  m_translate[1],  m_translate[2]);

		glRotatef (m_angles[1],  0, 0, 1);
		glRotatef (m_angles[0],  0, 1, 0);
		glRotatef (m_angles[2],  1, 0, 0);

		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	
	glPopMatrix();
};


void MDL::SetOpenGLTransformation( void ) const
{
    glTranslatef(m_translate[0],  m_translate[1],  m_translate[2]);

    glRotatef (m_angles[1],  0, 0, 1);
    glRotatef (m_angles[0],  0, 1, 0);
    glRotatef (m_angles[2],  1, 0, 0);
};


float* MDL::Matrix( void )
{
	return matrix;
};


float* MDL::GetGunMatrix( void )
{
	mstudioattachment_t *pattachments = (mstudioattachment_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->attachmentindex);

	// copy matrix into openGL form for my stuff
	for ( size_t i=0; i<16; i++ )
	{
		matrix[(i>>2)+((i&3)<<2)] = g_bonetransform[pattachments[0].bone][0][i] * scale;
	}
	return matrix;
}


void MDL::DrawGun( TBinObject* gun )
{
	if ( gun!=NULL )
	{
		GetGunMatrix();

		glFrontFace( GL_CCW );
		glPushMatrix();
			gun->Draw( matrix );
		glPopMatrix();
		glFrontFace( GL_CW );
	}
};


void MDL::Draw( TBinObject* gun )
{
	glFrontFace( GL_CW );
	glEnable ( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
	DrawModel( false, false, false, 1, gun );
	glDisable( GL_TEXTURE_2D );
	glFrontFace( GL_CCW );
};


void MDL::Logic( float speedScale )
{
	SetBlending(0, 0.0);
	SetBlending(1, 0.0);

	float curr = (float)GetTickCount() / 1000.0f;

	AdvanceFrame ((curr - prev) * speedScale);
	prev = curr;

	CalculateBoundingBox();
};


bool MDL::Load( const TString& filename, TString& errStr )
{
	// allocate a texture number
	glGenTextures(MAXSTUDIOSKINS,textures);

	FreeModel();
	if ( LoadModel((char *)filename.c_str()) )
	{
		if ( !PostLoadModel((char *)filename.c_str()) )
		{
			errStr = "PostLoadModel failed\n";
//			glDeleteTextures( (MAXSTUDIOSKINS-g_texcount), &textures[g_texcount] );
			return false;
		}
	}
	else
	{
		errStr = "Error loading MDL model\n";
//		glDeleteTextures( (MAXSTUDIOSKINS-g_texcount), &textures[g_texcount] );
		return false;
	}
//	glDeleteTextures( (MAXSTUDIOSKINS-g_texcount), &textures[g_texcount] );
	return true;
}


TString MDL::ModelInfo( void ) const
{
	studiohdr_t *hdr = getStudioHeader();

	char str[2048];
	if ( hdr==NULL )
	{
		str[0] = 0;
	}
	else
	{
		sprintf (str,
			"Bones: %d\n"
			"Bone Controllers: %d\n"
			"Hit Boxes: %d\n"
			"Sequences: %d\n"
			"Sequence Groups: %d\n"
			"Textures: %d\n"
			"Skin Families: %d\n"
			"Bodyparts: %d\n"
			"Attachments: %d\n"
			"Transitions: %d\n",
			hdr->numbones,
			hdr->numbonecontrollers,
			hdr->numhitboxes,
			hdr->numseq,
			hdr->numseqgroups,
			hdr->numtextures,
			hdr->numskinfamilies,
			hdr->numbodyparts,
			hdr->numattachments,
			hdr->numtransitions );
	}
	return str;
}


void MDL::CalcBoneAdj( )
{
	int					i, j;
	float				value;
	mstudiobonecontroller_t *pbonecontroller;
	
	pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	for (j = 0; j < m_pstudiohdr->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;
		if (i <= 3)
		{
			// check for 360% wrapping
			if (pbonecontroller[j].type & STUDIO_RLOOP)
			{
				value = m_controller[i] * (360.0/256.0) + pbonecontroller[j].start;
			}
			else 
			{
				value = m_controller[i] / 255.0;
				if (value < 0) value = 0;
				if (value > 1.0) value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
			// Con_DPrintf( "%d %d %f : %f\n", m_controller[j], m_prevcontroller[j], value, dadt );
		}
		else
		{
			value = m_mouth / 64.0;
			if (value > 1.0) value = 1.0;
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			// Con_DPrintf("%d %f\n", mouthopen, value );
		}
		switch(pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			m_adj[j] = value * (Q_PI / 180.0);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			m_adj[j] = value;
			break;
		}
	}
}


void MDL::CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q )
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j+3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j+3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j+3]);
			k = frame;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k+1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k+2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid+2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
			angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
		}

		if (pbone->bonecontroller[j+3] != -1)
		{
			angle1[j] += m_adj[pbone->bonecontroller[j+3]];
			angle2[j] += m_adj[pbone->bonecontroller[j+3]];
		}
	}

	if (!VectorCompare( angle1, angle2 ))
	{
		AngleQuaternion( angle1, q1 );
		AngleQuaternion( angle2, q2 );
		QuaternionSlerp( q1, q2, s, q );
	}
	else
	{
		AngleQuaternion( angle1, q );
	}
}


void MDL::CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos )
{
	int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);
			
			k = frame;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k+1].value * (1.0 - s) + s * panimvalue[k+2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k+1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if (pbone->bonecontroller[j] != -1)
		{
			pos[j] += m_adj[pbone->bonecontroller[j]];
		}
	}
}


void MDL::CalcRotations ( vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f )
{
	int					i;
	int					frame;
	mstudiobone_t		*pbone;
	float				s;

	frame = (int)f;
	s = (f - frame);

	// add in programatic controllers
	CalcBoneAdj( );

	pbone		= (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);
	for (i = 0; i < m_pstudiohdr->numbones; i++, pbone++, panim++) 
	{
		CalcBoneQuaternion( frame, s, pbone, panim, q[i] );
		CalcBonePosition( frame, s, pbone, panim, pos[i] );
	}

	if (pseqdesc->motiontype & STUDIO_X)
		pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y)
		pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z)
		pos[pseqdesc->motionbone][2] = 0.0;
}


mstudioanim_t * MDL::GetAnim( mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	pseqgroup = (mstudioseqgroup_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t *)((byte *)m_pstudiohdr + pseqgroup->data + pseqdesc->animindex);
	}

	return (mstudioanim_t *)((byte *)m_panimhdr[pseqdesc->seqgroup] + pseqdesc->animindex);
}


void MDL::SlerpBones( vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s )
{
	int			i;
	vec4_t		q3;
	float		s1;

	if (s < 0) 
		s = 0;
	else if (s > 1.0) 
		s = 1.0;

	s1 = 1.0 - s;

	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		QuaternionSlerp( q1[i], q2[i], s, q3 );
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}


void MDL::AdvanceFrame( float dt )
{
	if (!m_pstudiohdr)
		return;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	if (dt > 0.1)
		dt = 0.1f;
	m_frame += dt * pseqdesc->fps;

	if (pseqdesc->numframes <= 1)
	{
		m_frame = 0;
	}
	else
	{
		// wrap
		m_frame -= (int)(m_frame / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
	}
}


int MDL::SetFrame( int nFrame )
{
	if (nFrame == -1)
		return m_frame;

	if (!m_pstudiohdr)
		return 0;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	m_frame = nFrame;

	if (pseqdesc->numframes <= 1)
	{
		m_frame = 0;
	}
	else
	{
		// wrap
		m_frame -= (int)(m_frame / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
	}

	return m_frame;
}


void MDL::SetUpBones( void )
{
	int					i;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static vec3_t		pos[MAXSTUDIOBONES];
	float				bonematrix[3][4];
	static vec4_t		q[MAXSTUDIOBONES];

	static vec3_t		pos2[MAXSTUDIOBONES];
	static vec4_t		q2[MAXSTUDIOBONES];
	static vec3_t		pos3[MAXSTUDIOBONES];
	static vec4_t		q3[MAXSTUDIOBONES];
	static vec3_t		pos4[MAXSTUDIOBONES];
	static vec4_t		q4[MAXSTUDIOBONES];


	if ( m_sequence>=m_pstudiohdr->numseq ) 
	{
		m_sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	panim = GetAnim( pseqdesc );
	CalcRotations( pos, q, pseqdesc, panim, m_frame );

	if ( pseqdesc->numblends>1 )
	{
		float s;

		panim += m_pstudiohdr->numbones;
		CalcRotations( pos2, q2, pseqdesc, panim, m_frame );
		s = m_blending[0] / 255.0;

		SlerpBones( q, pos, q2, pos2, s );

		if (pseqdesc->numblends == 4)
		{
			panim += m_pstudiohdr->numbones;
			CalcRotations( pos3, q3, pseqdesc, panim, m_frame );

			panim += m_pstudiohdr->numbones;
			CalcRotations( pos4, q4, pseqdesc, panim, m_frame );

			s = m_blending[0] / 255.0;
			SlerpBones( q3, pos3, q4, pos4, s );

			s = m_blending[1] / 255.0;
			SlerpBones( q, pos, q3, pos3, s );
		}
	}

	pbones = (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);

	for (i = 0; i < m_pstudiohdr->numbones; i++) 
	{
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1) 
		{
			memcpy(g_bonetransform[i], bonematrix, sizeof(float) * 12);
		} 
		else 
		{
			R_ConcatTransforms( g_bonetransform[pbones[i].parent], bonematrix, 
								g_bonetransform[i]);
		}
	}
}



/*
================
MDL::TransformFinalVert
================
*/
void MDL::Lighting (float *lv, int bone, int flags, vec3_t normal)
{
	float 	illum;
	float	lightcos;

	illum = g_ambientlight;

	if (flags & STUDIO_NF_FLATSHADE)
	{
		illum += g_shadelight * 0.8;
	} 
	else 
	{
		float r;
		lightcos = DotProduct (normal, g_blightvec[bone]); // -1 colinear, 1 opposite

		if (lightcos > 1.0)
			lightcos = 1;

		illum += g_shadelight;

		r = g_lambert;
		if (r <= 1.0) r = 1.0;

		lightcos = (lightcos + (r - 1.0)) / r; 		// do modified hemispherical lighting
		if (lightcos > 0.0) 
		{
			illum -= g_shadelight * lightcos; 
		}
		if (illum <= 0)
			illum = 0;
	}

	if (illum > 255) 
		illum = 255;
	*lv = illum / 255.0;	// Light from 0 to 1.0
}


void MDL::Chrome (int *pchrome, int bone, vec3_t normal)
{
	float n;

	if (g_chromeage[bone] != g_smodels_total)
	{
		// calculate vectors from the viewer to the bone. This roughly adjusts for position
		vec3_t chromeupvec;		// g_chrome t vector in world reference frame
		vec3_t chromerightvec;	// g_chrome s vector in world reference frame
		vec3_t tmp;				// vector pointing at bone in world reference frame
		VectorScale( m_origin, -1, tmp );
		tmp[0] += g_bonetransform[bone][0][3];
		tmp[1] += g_bonetransform[bone][1][3];
		tmp[2] += g_bonetransform[bone][2][3];
		VectorNormalize( tmp );
		CrossProduct( tmp, g_vright, chromeupvec );
		VectorNormalize( chromeupvec );
		CrossProduct( tmp, chromeupvec, chromerightvec );
		VectorNormalize( chromerightvec );

		VectorIRotate( chromeupvec, g_bonetransform[bone], g_chromeup[bone] );
		VectorIRotate( chromerightvec, g_bonetransform[bone], g_chromeright[bone] );

		g_chromeage[bone] = g_smodels_total;
	}

	// calc s coord
	n = DotProduct( normal, g_chromeright[bone] );
	pchrome[0] = (n + 1.0) * 32; // FIX: make this a float

	// calc t coord
	n = DotProduct( normal, g_chromeup[bone] );
	pchrome[1] = (n + 1.0) * 32; // FIX: make this a float
}


/*
================
MDL::SetupLighting
	set some global variables based on entity position
inputs:
outputs:
	g_ambientlight
	g_shadelight
================
*/
void MDL::SetupLighting ( )
{
	int i;
	g_ambientlight = 32;
	g_shadelight = 192;

	g_lightvec[0] = 0;
	g_lightvec[1] = 0;
	g_lightvec[2] = -1.0;

	g_lightcolor[0] = 1;
	g_lightcolor[1] = 1;
	g_lightcolor[2] = 1;

	// TODO: only do it for bones that actually have textures
	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		VectorIRotate( g_lightvec, g_bonetransform[i], g_blightvec[i] );
	}
}


/*
=================
MDL::SetupModel
	based on the body part, figure out which mesh it should be using.
inputs:
	currententity
outputs:
	pstudiomesh
	pmdl
=================
*/

void MDL::SetupModel ( int bodypart )
{
	int index;

	if (bodypart > m_pstudiohdr->numbodyparts)
	{
		// Con_DPrintf ("MDL::SetupModel: no such bodypart %d\n", bodypart);
		bodypart = 0;
	}

	mstudiobodyparts_t   *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + bodypart;

	index = m_bodynum / pbodypart->base;
	index = index % pbodypart->nummodels;

	m_pmodel = (mstudiomodel_t *)((byte *)m_pstudiohdr + pbodypart->modelindex) + index;
}


void drawBox (vec3_t *v)
{
	glBegin (GL_QUAD_STRIP);
	for (int i = 0; i < 10; i++)
		glVertex3fv (v[i & 7]);
	glEnd ();
	
	glBegin  (GL_QUAD_STRIP);
	glVertex3fv (v[6]);
	glVertex3fv (v[0]);
	glVertex3fv (v[4]);
	glVertex3fv (v[2]);
	glEnd ();

	glBegin  (GL_QUAD_STRIP);
	glVertex3fv (v[1]);
	glVertex3fv (v[7]);
	glVertex3fv (v[3]);
	glVertex3fv (v[5]);
	glEnd ();

}


void MDL::GetBoundingBox( float& _minx, float& _miny, float& _minz,
						  float& _maxx, float& _maxy, float& _maxz )
{
	_minx = minx * scale;
	_miny = miny * scale;
	_minz = minz * scale;
	_maxx = maxx * scale;
	_maxy = maxy * scale;
	_maxz = maxz * scale;
};

void MDL::CalculateBoundingBox( void )
{
	bool firstTime = true;
	for ( int i = 0; i < m_pstudiohdr->numhitboxes; i++)
	{
		mstudiobbox_t *pbboxes = (mstudiobbox_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->hitboxindex);
		vec3_t v[8], v2[8], bbmin, bbmax;

		VectorCopy (pbboxes[i].bbmin, bbmin);
		VectorCopy (pbboxes[i].bbmax, bbmax);

		v[0][0] = bbmin[0];
		v[0][1] = bbmax[1];
		v[0][2] = bbmin[2];

		v[1][0] = bbmin[0];
		v[1][1] = bbmin[1];
		v[1][2] = bbmin[2];

		v[2][0] = bbmax[0];
		v[2][1] = bbmax[1];
		v[2][2] = bbmin[2];

		v[3][0] = bbmax[0];
		v[3][1] = bbmin[1];
		v[3][2] = bbmin[2];

		v[4][0] = bbmax[0];
		v[4][1] = bbmax[1];
		v[4][2] = bbmax[2];

		v[5][0] = bbmax[0];
		v[5][1] = bbmin[1];
		v[5][2] = bbmax[2];

		v[6][0] = bbmin[0];
		v[6][1] = bbmax[1];
		v[6][2] = bbmax[2];

		v[7][0] = bbmin[0];
		v[7][1] = bbmin[1];
		v[7][2] = bbmax[2];

		VectorTransform(v[0], g_bonetransform[pbboxes[i].bone], v2[0]);
		VectorTransform(v[1], g_bonetransform[pbboxes[i].bone], v2[1]);
		VectorTransform(v[2], g_bonetransform[pbboxes[i].bone], v2[2]);
		VectorTransform(v[3], g_bonetransform[pbboxes[i].bone], v2[3]);
		VectorTransform(v[4], g_bonetransform[pbboxes[i].bone], v2[4]);
		VectorTransform(v[5], g_bonetransform[pbboxes[i].bone], v2[5]);
		VectorTransform(v[6], g_bonetransform[pbboxes[i].bone], v2[6]);
		VectorTransform(v[7], g_bonetransform[pbboxes[i].bone], v2[7]);

		if ( firstTime )
		{
			firstTime = false;
			minx = v[0][0];
			miny = v[0][1];
			minz = v[0][2];
			maxx = v[0][0];
			maxy = v[0][1];
			maxz = v[0][2];
		}
		for ( size_t j=0; j<8; j++ )
		{
			if ( v2[j][0] < minx )
				minx = v2[j][0];
			if ( v2[j][0] > maxx )
				maxx = v2[j][0];

			if ( v2[j][1] < miny )
				miny = v2[j][1];
			if ( v2[j][1] > maxy )
				maxy = v2[j][1];

			if ( v2[j][2] < minz )
				minz = v2[j][2];
			if ( v2[j][2] > maxz )
				maxz = v2[j][2];
		}
	}
}


void MDL::PrepareBones( void )
{
	if ( m_pstudiohdr==NULL )
		return;
	if ( m_pstudiohdr->numbodyparts==0 )
		return;

	SetUpBones();
	for (int i=0 ; i < m_pstudiohdr->numbodyparts ; i++) 
	{
		SetupModel( i );
	}
};

size_t MDL::NumBones( void ) const
{
	if ( m_pstudiohdr==NULL )
		return 0;
	if ( m_pstudiohdr->numbodyparts==0 )
		return 0;

	mstudiobone_t *pbones = (mstudiobone_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->boneindex);
	return size_t(m_pstudiohdr->numbones);
};

size_t MDL::GetBones( float* bones )
{
	if ( m_pstudiohdr==NULL )
		return 0;
	if ( m_pstudiohdr->numbodyparts==0 )
		return 0;

	size_t index = 0;
	mstudiobone_t *pbones = (mstudiobone_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->boneindex);
	for ( size_t i=0; i<size_t(m_pstudiohdr->numbones); i++ )
	{
		if ( bones!=NULL )
		{
			bones[index*3+0] = g_bonetransform[i][0][3]*scale;
			bones[index*3+1] = g_bonetransform[i][1][3]*scale;
			bones[index*3+2] = g_bonetransform[i][2][3]*scale;
		}
		index++;
	}
	return index;
};

void MDL::GetBone( size_t index, float* v )
{
	if ( m_pstudiohdr==NULL )
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		return;
	}
	if ( m_pstudiohdr->numbodyparts==0 )
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		return;
	}

	mstudiobone_t *pbones = (mstudiobone_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->boneindex);
	if ( index >= size_t(m_pstudiohdr->numbones) )
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		return;
	}

	v[0] = g_bonetransform[index][0][3]*scale;
	v[1] = g_bonetransform[index][1][3]*scale;
	v[2] = g_bonetransform[index][2][3]*scale;
};

/*
================
MDL::DrawModel
inputs:
	currententity
	r_entorigin
================
*/
void MDL::DrawModel( bool showBones, bool showAttachments,
					 bool showHitBoxes, float transparency,
					 TBinObject* gun )
{
	int i;

	if ( m_pstudiohdr==NULL )
		return;

	g_smodels_total++; // render data cache cookie

	g_pxformverts = &g_xformverts[0];
	g_pvlightvalues = &g_lightvalues[0];

	if ( m_pstudiohdr->numbodyparts==0 )
		return;

    glTranslatef(m_translate[0],  m_translate[1],  m_translate[2]);

    glRotatef (m_angles[1],  0, 0, 1);
    glRotatef (m_angles[0],  0, 1, 0);
    glRotatef (m_angles[2],  1, 0, 0);

	SetUpBones();

	SetupLighting();

	for (i=0 ; i < m_pstudiohdr->numbodyparts ; i++) 
	{
		SetupModel( i );
		if ( transparency > 0.0f)
			DrawPoints( transparency );
	}

	if ( gun!=NULL )
		DrawGun(gun);

	// draw bones
	if ( showBones )
	{
		mstudiobone_t *pbones = (mstudiobone_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->boneindex);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);

		for ( i=0; i<m_pstudiohdr->numbones; i++ )
		{
			if (pbones[i].parent >= 0)
			{
				glPointSize(3.0f);
				glColor3f(1, 0.7f, 0);
				glBegin(GL_LINES);
				glVertex3f(g_bonetransform[pbones[i].parent][0][3]*scale, 
							g_bonetransform[pbones[i].parent][1][3]*scale, 
							g_bonetransform[pbones[i].parent][2][3]*scale);
				glVertex3f(g_bonetransform[i][0][3]*scale, 
						   g_bonetransform[i][1][3]*scale, 
						   g_bonetransform[i][2][3]*scale);
				glEnd();

				glColor3f(0, 0, 0.8f);
				glBegin(GL_POINTS);
				if (pbones[pbones[i].parent].parent != -1)
					glVertex3f(g_bonetransform[pbones[i].parent][0][3]*scale, 
							   g_bonetransform[pbones[i].parent][1][3]*scale, 
							   g_bonetransform[pbones[i].parent][2][3]*scale);
				glVertex3f( g_bonetransform[i][0][3]*scale,
							g_bonetransform[i][1][3]*scale, 
							g_bonetransform[i][2][3]*scale);
				glEnd();
			}
			else
			{
				// draw parent bone node
				glPointSize(5.0f);
				glColor3f(0.8f, 0, 0);
				glBegin(GL_POINTS);
				glVertex3f( g_bonetransform[i][0][3]*scale,
							g_bonetransform[i][1][3]*scale, 
							g_bonetransform[i][2][3]*scale );
				glEnd();
			}
		}
		glPointSize(1.0f);
	}

	if ( showAttachments )
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		for ( i=0; i<m_pstudiohdr->numattachments; i++ )
		{
			mstudioattachment_t *pattachments = (mstudioattachment_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->attachmentindex);
			vec3_t v[4];
			VectorTransformScaled(pattachments[i].org, g_bonetransform[pattachments[i].bone], v[0], scale);
			VectorTransformScaled(pattachments[i].vectors[0], g_bonetransform[pattachments[i].bone], v[1], scale);
			VectorTransformScaled(pattachments[i].vectors[1], g_bonetransform[pattachments[i].bone], v[2], scale);
			VectorTransformScaled(pattachments[i].vectors[2], g_bonetransform[pattachments[i].bone], v[3], scale);

			glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex3fv(v[0]);
			glColor3f(1, 1, 1);
			glVertex3fv(v[1]);
			glColor3f(1, 0, 0);
			glVertex3fv(v[0]);
			glColor3f(1, 1, 1);
			glVertex3fv(v[2]);
			glColor3f(1, 0, 0);
			glVertex3fv(v[0]);
			glColor3f(1, 1, 1);
			glVertex3fv(v[3]);
			glEnd();

			glPointSize(5);
			glColor3f(0, 1, 0);
			glBegin(GL_POINTS);
			glVertex3fv(v[0]);
			glEnd();
			glPointSize(1);
		}
	}

	if ( showHitBoxes )
	{
		glDisable (GL_TEXTURE_2D);
		glDisable (GL_CULL_FACE);
		if ( transparency < 1.0f )
			glDisable (GL_DEPTH_TEST);
		else
			glEnable (GL_DEPTH_TEST);

		glColor4f (1, 0, 0, 0.5f);

		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (i = 0; i < m_pstudiohdr->numhitboxes; i++)
		{
			mstudiobbox_t *pbboxes = (mstudiobbox_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->hitboxindex);
			vec3_t v[8], v2[8], bbmin, bbmax;

			VectorCopy (pbboxes[i].bbmin, bbmin);
			VectorCopy (pbboxes[i].bbmax, bbmax);

			v[0][0] = bbmin[0];
			v[0][1] = bbmax[1];
			v[0][2] = bbmin[2];

			v[1][0] = bbmin[0];
			v[1][1] = bbmin[1];
			v[1][2] = bbmin[2];

			v[2][0] = bbmax[0];
			v[2][1] = bbmax[1];
			v[2][2] = bbmin[2];

			v[3][0] = bbmax[0];
			v[3][1] = bbmin[1];
			v[3][2] = bbmin[2];

			v[4][0] = bbmax[0];
			v[4][1] = bbmax[1];
			v[4][2] = bbmax[2];

			v[5][0] = bbmax[0];
			v[5][1] = bbmin[1];
			v[5][2] = bbmax[2];

			v[6][0] = bbmin[0];
			v[6][1] = bbmax[1];
			v[6][2] = bbmax[2];

			v[7][0] = bbmin[0];
			v[7][1] = bbmin[1];
			v[7][2] = bbmax[2];

			VectorTransform(v[0], g_bonetransform[pbboxes[i].bone], v2[0]);
			VectorTransform(v[1], g_bonetransform[pbboxes[i].bone], v2[1]);
			VectorTransform(v[2], g_bonetransform[pbboxes[i].bone], v2[2]);
			VectorTransform(v[3], g_bonetransform[pbboxes[i].bone], v2[3]);
			VectorTransform(v[4], g_bonetransform[pbboxes[i].bone], v2[4]);
			VectorTransform(v[5], g_bonetransform[pbboxes[i].bone], v2[5]);
			VectorTransform(v[6], g_bonetransform[pbboxes[i].bone], v2[6]);
			VectorTransform(v[7], g_bonetransform[pbboxes[i].bone], v2[7]);
			
			drawBox (v2);
		}
	}
}

/*
================

================
*/
void MDL::DrawPoints( float transparency )
{
	int					i, j;
	mstudiomesh_t		*pmesh;
	byte				*pvertbone;
	byte				*pnormbone;
	vec3_t				*pstudioverts;
	vec3_t				*pstudionorms;
	mstudiotexture_t	*ptexture;
	float 				*av;
	float				*lv;
	float				lv_tmp;
	short				*pskinref;

	pvertbone = ((byte *)m_pstudiohdr + m_pmodel->vertinfoindex);
	pnormbone = ((byte *)m_pstudiohdr + m_pmodel->norminfoindex);
	ptexture = (mstudiotexture_t *)((byte *)m_ptexturehdr + m_ptexturehdr->textureindex);

	pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex);

	pstudioverts = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->vertindex);
	pstudionorms = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->normindex);

	pskinref = (short *)((byte *)m_ptexturehdr + m_ptexturehdr->skinindex);
	if (m_skinnum != 0 && m_skinnum < m_ptexturehdr->numskinfamilies)
		pskinref += (m_skinnum * m_ptexturehdr->numskinref);

	for (i = 0; i < m_pmodel->numverts; i++)
	{
		//vec3_t tmp;
		//VectorScale (pstudioverts[i], 12, tmp);
		VectorTransform(pstudioverts[i], g_bonetransform[pvertbone[i]], g_pxformverts[i]);
	}

	if ( transparency < 1.0f)
	{
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	//
	// clip and draw all triangles
	//
	lv = (float *)g_pvlightvalues;
	for (j = 0; j < m_pmodel->nummesh; j++) 
	{
		int flags;
		flags = ptexture[pskinref[pmesh[j].skinref]].flags;
		for (i = 0; i < pmesh[j].numnorms; i++, lv += 3, pstudionorms++, pnormbone++)
		{
			Lighting (&lv_tmp, *pnormbone, flags, (float *)pstudionorms);

			// FIX: move this check out of the inner loop
			if (flags & STUDIO_NF_CHROME)
				Chrome( g_chrome[(float (*)[3])lv - g_pvlightvalues], *pnormbone, (float *)pstudionorms );

			lv[0] = lv_tmp * g_lightcolor[0];
			lv[1] = lv_tmp * g_lightcolor[1];
			lv[2] = lv_tmp * g_lightcolor[2];
		}
	}

	// glCullFace(GL_FRONT);

	for (j = 0; j < m_pmodel->nummesh; j++) 
	{
		float s, t;
		short		*ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

		s = 1.0/(float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0/(float)ptexture[pskinref[pmesh->skinref]].height;

		//glBindTexture( GL_TEXTURE_2D, ptexture[pskinref[pmesh->skinref]].index );
		glBindTexture( GL_TEXTURE_2D, textures[pskinref[pmesh->skinref]] );

		if (ptexture[pskinref[pmesh->skinref]].flags & STUDIO_NF_CHROME)
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					glBegin( GL_TRIANGLE_FAN );
					i = -i;
				}
				else
				{
					glBegin( GL_TRIANGLE_STRIP );
				}


				for( ; i > 0; i--, ptricmds += 4)
				{
					// FIX: put these in as integer coords, not floats
					glTexCoord2f(g_chrome[ptricmds[1]][0]*s, g_chrome[ptricmds[1]][1]*t);
					
					lv = g_pvlightvalues[ptricmds[1]];
					glColor4f( lv[0], lv[1], lv[2], transparency);

					av = g_pxformverts[ptricmds[0]];
					glVertex3f(av[0]*scale, av[1]*scale, av[2]*scale);
//					glVertex3f(av[0], av[1], av[2]);
				}
				glEnd( );
			}	
		} 
		else 
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					glBegin( GL_TRIANGLE_FAN );
					i = -i;
				}
				else
				{
					glBegin( GL_TRIANGLE_STRIP );
				}


				for( ; i > 0; i--, ptricmds += 4)
				{
					// FIX: put these in as integer coords, not floats
					glTexCoord2f(ptricmds[2]*s, ptricmds[3]*t);
					
					lv = g_pvlightvalues[ptricmds[1]];
					glColor4f( lv[0], lv[1], lv[2], transparency);

					av = g_pxformverts[ptricmds[0]];
					glVertex3f(av[0]*scale, av[1]*scale, av[2]*scale);
//					glVertex3f(av[0], av[1], av[2]);
				}
				glEnd( );
			}	
		}
	}
}



void MDL::UploadTexture(mstudiotexture_t *ptexture, byte *data, byte *pal, int name)
{
	// unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight;
	int		i, j;
	int		row1[256], row2[256], col1[256], col2[256];
	byte	*pix1, *pix2, *pix3, *pix4;
	byte	*tex, *out;

	// convert texture to power of 2
	int outwidth;
	for (outwidth = 1; outwidth < ptexture->width; outwidth <<= 1)
		;

	//outwidth >>= 1;
	if (outwidth > 256)
		outwidth = 256;

	int outheight;
	for (outheight = 1; outheight < ptexture->height; outheight <<= 1)
		;

	//outheight >>= 1;
	if (outheight > 256)
		outheight = 256;

	tex = out = new byte[ outwidth * outheight * 4];
	if (!out)
	{
		return;
	}
/*
	int k = 0;
	for (i = 0; i < ptexture->height; i++)
	{
		for (j = 0; j < ptexture->width; j++)
		{

			in[k++] = pal[data[i * ptexture->width + j] * 3 + 0];
			in[k++] = pal[data[i * ptexture->width + j] * 3 + 1];
			in[k++] = pal[data[i * ptexture->width + j] * 3 + 2];
			in[k++] = 0xff;;
		}
	}

	gluScaleImage (GL_RGBA, ptexture->width, ptexture->height, GL_UNSIGNED_BYTE, in, outwidth, outheight, GL_UNSIGNED_BYTE, out);
	free (in);
*/

	for (i = 0; i < outwidth; i++)
	{
		col1[i] = (int) ((i + 0.25) * (ptexture->width / (float)outwidth));
		col2[i] = (int) ((i + 0.75) * (ptexture->width / (float)outwidth));
	}

	for (i = 0; i < outheight; i++)
	{
		row1[i] = (int) ((i + 0.25) * (ptexture->height / (float)outheight)) * ptexture->width;
		row2[i] = (int) ((i + 0.75) * (ptexture->height / (float)outheight)) * ptexture->width;
	}

	// scale down and convert to 32bit RGB
	for (i=0 ; i<outheight ; i++)
	{
		for (j=0 ; j<outwidth ; j++, out += 4)
		{
			pix1 = &pal[data[row1[i] + col1[j]] * 3];
			pix2 = &pal[data[row1[i] + col2[j]] * 3];
			pix3 = &pal[data[row2[i] + col1[j]] * 3];
			pix4 = &pal[data[row2[i] + col2[j]] * 3];

			out[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
			out[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
			out[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
			out[3] = 0xFF;
		}
	}

	glBindTexture( GL_TEXTURE_2D, textures[name] ); //g_texnum );		
	glTexImage2D( GL_TEXTURE_2D, 0, 3/*??*/, outwidth, outheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex );
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, bFilterTextures ? GL_LINEAR:GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, bFilterTextures ? GL_LINEAR:GL_NEAREST);

	// ptexture->width = outwidth;
	// ptexture->height = outheight;
	//ptexture->index = name; //g_texnum;

	delete tex;
}



void MDL::FreeModel ()
{
	if (m_pstudiohdr)
		delete m_pstudiohdr;

	if (m_ptexturehdr && m_owntexmodel)
		delete m_ptexturehdr;

	m_pstudiohdr = m_ptexturehdr = 0;
	m_owntexmodel = false;

	int i;
	for (i = 0; i < 32; i++)
	{
		if (m_panimhdr[i]!=NULL)
		{
			delete m_panimhdr[i];
			m_panimhdr[i] = NULL;
		}
	}

	// remove all textures from openGL
//	glDeleteTextures( g_texcount, textures );
//	g_texcount = 0;
}



studiohdr_t *MDL::LoadModel( char *modelname )
{
	FILE *fp;
	byte *buffer;

	if (!modelname)
		return 0;

	// load the model
	if( (fp = fopen( modelname, "rb" )) == NULL)
		return 0;

	fseek( fp, 0, SEEK_END );
	modelSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	// check header
	TString header;
	size_t index = 0;
	char ch;
	do
	{
		fread(&ch,1,1,fp);
		if ( ch>0 )
			header = header + TString(ch);
		index++;
	}
	while ( ch!=0 && !feof(fp) );

	if ( header!="PDVCHAR01" )
	{
		fclose(fp);
		return 0;
	}

	// get character's name
	characterName = "";
	do
	{
		fread(&ch,1,1,fp);
		if ( ch>0 )
			characterName = characterName + TString(ch);
		index++;
	}
	while ( ch!=0 && !feof(fp) );

	if ( characterName.length()==0 )
	{
		fclose(fp);
		return 0;
	}

	buffer = new byte[(modelSize-index)+4];
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	if ( buffer==NULL )
	{
		fclose(fp);
		return 0;
	}
	fread( &buffer[4], modelSize-4, 1, fp );
	fclose( fp );

	byte				*pin;
	studiohdr_t			*phdr;
	mstudiotexture_t	*ptexture;

	pin = (byte *)buffer;
	phdr = (studiohdr_t *)pin;
	ptexture = (mstudiotexture_t *)(pin + phdr->textureindex);

	if (phdr->textureindex > 0 && phdr->numtextures <= MAXSTUDIOSKINS)
	{
		int n = phdr->numtextures;
		for (int i = 0; i < n; i++)
		{
			// strcpy( name, mod->name );
			// strcpy( name, ptexture[i].name );
			UploadTexture( &ptexture[i], pin + ptexture[i].index, pin + ptexture[i].width * ptexture[i].height + ptexture[i].index, g_texcount++ );
			PostCond( g_texcount < MAXSTUDIOSKINS );
		}
	}

	// UNDONE: free texture memory

	if (!m_pstudiohdr)
		m_pstudiohdr = (studiohdr_t *)buffer;

	return (studiohdr_t *)buffer;
}


bool MDL::PostLoadModel( char *modelname )
{
	// preload textures
	if (m_pstudiohdr->numtextures == 0)
	{
		char texturename[256];

		strcpy( texturename, modelname );
		strcpy( &texturename[strlen(texturename) - 4], "T.mdl" );

		m_ptexturehdr = LoadModel( texturename );
		if (!m_ptexturehdr)
		{
			FreeModel ();
			return false;
		}
		m_owntexmodel = true;
	}
	else
	{
		m_ptexturehdr = m_pstudiohdr;
		m_owntexmodel = false;
	}

	// preload animations
	if (m_pstudiohdr->numseqgroups > 1)
	{
		for (int i = 1; i < m_pstudiohdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy( seqgroupname, modelname );
			sprintf( &seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i );

			m_panimhdr[i] = LoadModel( seqgroupname );
			if (!m_panimhdr[i])
			{
				FreeModel ();
				return false;
			}
		}
	}

	SetSequence (0);
	SetController (0, 0.0f);
	SetController (1, 0.0f);
	SetController (2, 0.0f);
	SetController (3, 0.0f);
	SetMouth (0.0f);

	int n;
	for ( n=0; n<m_pstudiohdr->numbodyparts; n++ )
		SetBodygroup (n, 0);

	SetSkin (0);

	vec3_t mins,maxs;
	ExtractBbox(mins, maxs);
//	if (mins[2] < 5.0f)
//		m_origin[2] = -mins[2];
	m_origin[0] = (mins[0]+maxs[0]) * 0.5f;
	m_origin[1] = (mins[1]+maxs[1]) * 0.5f;
	m_origin[2] = (mins[2]+maxs[2]) * 0.5f;

	return true;
}



bool MDL::SaveModel ( char *modelname )
{
	if (!modelname)
		return false;

	if (!m_pstudiohdr)
		return false;

	FILE *file;
	
	file = fopen (modelname, "wb");
	if (!file)
		return false;

	fwrite (m_pstudiohdr, sizeof(byte), m_pstudiohdr->length, file);
	fclose (file);

	// write texture model
	if (m_owntexmodel && m_ptexturehdr)
	{
		char texturename[256];

		strcpy( texturename, modelname );
		strcpy( &texturename[strlen(texturename) - 4], "T.mdl" );

		file = fopen (texturename, "wb");
		if (file)
		{
			fwrite (m_ptexturehdr, sizeof(byte), m_ptexturehdr->length, file);
			fclose (file);
		}
	}

	// write seq groups
	if (m_pstudiohdr->numseqgroups > 1)
	{
		for (int i = 1; i < m_pstudiohdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy( seqgroupname, modelname );
			sprintf( &seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i );

			file = fopen (seqgroupname, "wb");
			if (file)
			{
				fwrite (m_panimhdr[i], sizeof(byte), m_panimhdr[i]->length, file);
				fclose (file);
			}
		}
	}

	return true;
}



////////////////////////////////////////////////////////////////////////

int MDL::GetSequence( void ) const
{
	return m_sequence;
}

int MDL::SetSequence( int iSequence )
{
	if (iSequence > m_pstudiohdr->numseq)
		return m_sequence;

	m_sequence = iSequence;
	m_frame = 0;

	return m_sequence;
}


void MDL::ExtractBbox( float *mins, float *maxs )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
	
	mins[0] = pseqdesc[ m_sequence ].bbmin[0];
	mins[1] = pseqdesc[ m_sequence ].bbmin[1];
	mins[2] = pseqdesc[ m_sequence ].bbmin[2];

	maxs[0] = pseqdesc[ m_sequence ].bbmax[0];
	maxs[1] = pseqdesc[ m_sequence ].bbmax[1];
	maxs[2] = pseqdesc[ m_sequence ].bbmax[2];
}



void MDL::GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}



float MDL::SetController( int iController, float flValue )
{
	if (!m_pstudiohdr)
		return 0.0f;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the index
	int i;
	for (i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= m_pstudiohdr->numbonecontrollers)
		return flValue;

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = (int) (255 * (flValue - pbonecontroller->start) /
	(pbonecontroller->end - pbonecontroller->start));

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;
	m_controller[iController] = setting;

	return setting * (1.0 / 255.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float MDL::SetMouth( float flValue )
{
	if (!m_pstudiohdr)
		return 0.0f;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the mouth
	for (int i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == 4)
			break;
	}

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = (int) (64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start));

	if (setting < 0) setting = 0;
	if (setting > 64) setting = 64;
	m_mouth = setting;

	return setting * (1.0 / 64.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float MDL::SetBlending( int iBlender, float flValue )
{
	mstudioseqdesc_t	*pseqdesc;

	if (!m_pstudiohdr)
		return 0.0f;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		// does the controller not wrap?
		if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
				flValue = flValue + 360;
		}
	}

	int setting = (int) (255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]));

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	m_blending[iBlender] = setting;

	return setting * (1.0 / 255.0) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
}



int MDL::SetBodygroup( int iGroup, int iValue )
{
	if (!m_pstudiohdr)
		return 0;

	if (iGroup > m_pstudiohdr->numbodyparts)
		return -1;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + iGroup;

	int iCurrent = (m_bodynum / pbodypart->base) % pbodypart->nummodels;

	if (iValue >= pbodypart->nummodels)
		return iCurrent;

	m_bodynum = (m_bodynum - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));

	return iValue;
}


int MDL::SetSkin( int iValue )
{
	if (!m_pstudiohdr)
		return 0;

	if (iValue >= m_pstudiohdr->numskinfamilies)
	{
		return m_skinnum;
	}

	m_skinnum = iValue;

	return iValue;
}



void MDL::scaleMeshes (float scale)
{
	if (!m_pstudiohdr)
		return;

	int i, j, k;

	// scale verts
	int tmp = m_bodynum;
	for (i = 0; i < m_pstudiohdr->numbodyparts; i++)
	{
		mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + i;
		for (j = 0; j < pbodypart->nummodels; j++)
		{
			SetBodygroup (i, j);
			SetupModel (i);

			vec3_t *pstudioverts = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->vertindex);

			for (k = 0; k < m_pmodel->numverts; k++)
				VectorScale (pstudioverts[k], scale, pstudioverts[k]);
		}
	}

	m_bodynum = tmp;

	// scale complex hitboxes
	mstudiobbox_t *pbboxes = (mstudiobbox_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->hitboxindex);
	for (i = 0; i < m_pstudiohdr->numhitboxes; i++)
	{
		VectorScale (pbboxes[i].bbmin, scale, pbboxes[i].bbmin);
		VectorScale (pbboxes[i].bbmax, scale, pbboxes[i].bbmax);
	}

	// scale bounding boxes
	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
	for (i = 0; i < m_pstudiohdr->numseq; i++)
	{
		VectorScale (pseqdesc[i].bbmin, scale, pseqdesc[i].bbmin);
		VectorScale (pseqdesc[i].bbmax, scale, pseqdesc[i].bbmax);
	}

	// maybe scale exeposition, pivots, attachments
}



void MDL::scaleBones (float scale)
{
	if (!m_pstudiohdr)
		return;

	mstudiobone_t *pbones = (mstudiobone_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->boneindex);
	for (int i = 0; i < m_pstudiohdr->numbones; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pbones[i].value[j] *= scale;
			pbones[i].scale[j] *= scale;
		}
	}	
}


TString MDL::Name( void ) const
{
	return characterName;
}


float MDL::Scale( void ) const
{
	return scale;
};


void MDL::Scale( float _scale )
{
	scale = _scale;
};

