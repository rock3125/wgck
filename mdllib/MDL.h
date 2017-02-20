#ifndef _MDL_MDL_H_
#define _MDL_MDL_H_

#ifndef byte
typedef unsigned char byte;
#endif // byte


#include <mdllib/mathlib.h>
#include <mdllib/studio.h>

class TBinObject;

class MDL
{
public:
	MDL( void );
	virtual ~MDL( void );

	MDL( const MDL& );
	const MDL& operator=( const MDL& );

	// set animation sequence number
	int SetSequence( int iSequence );
	int	GetSequence( void ) const;

	// draw this figure
	void Draw( TBinObject* gun );

	// Logic calculations for moving frames using a speed scale (normally==1)
	void Logic( float speedScale=1.0f );

	// load an MDL model from file, return t|f if success, and errStr on failure
	bool Load( const TString& filename, TString& errStr );

	// get a description of the model in a string
	TString ModelInfo( void ) const;

	void SetTransRot( float x, float y, float z,
					  float xa, float ya, float za );

	void	GetOpenGLTransformation( float* matrix );
	void	SetOpenGLTransformation( void ) const;
	float*	Matrix( void );

	TString Name( void ) const;

	void GetBoundingBox( float& minx, float& miny, float& minz,
						 float& maxx, float& maxy, float& maxz );

	void CalculateBoundingBox( void );

	// return number of bones and vertices in pairs
	size_t NumBones( void ) const;
	size_t GetBones( float* bones );
	void PrepareBones( void );
	void GetBone( size_t index, float* v );

	// get-set scale
	float	Scale( void ) const;
	void	Scale( float _scale );

private:
	studiohdr_t*			getStudioHeader () const { return m_pstudiohdr; }
	studiohdr_t*			getTextureHeader () const { return m_ptexturehdr; }
	studiohdr_t*			getAnimHeader (int i) const { return m_panimhdr[i]; }

	void					UploadTexture( mstudiotexture_t *ptexture, byte *data, byte *pal, int name );
	void					FreeModel();
	studiohdr_t*			LoadModel( char *modelname );
	bool					PostLoadModel ( char *modelname );
	bool					SaveModel ( char *modelname );
	void					DrawModel( bool showBones, bool showAttachments,
									   bool showHitBoxes, float transparency,
									   TBinObject* gun );

	float*					GetGunMatrix( void ); // get gun position transformation matrix

	void					AdvanceFrame( float dt );
	int						SetFrame (int nFrame);

	void					ExtractBbox( float *mins, float *maxs );

	void					GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed );

	float					SetController( int iController, float flValue );
	float					SetMouth( float flValue );
	float					SetBlending( int iBlender, float flValue );
	int						SetBodygroup( int iGroup, int iValue );
	int						SetSkin( int iValue );

	void					scaleMeshes (float scale);
	void					scaleBones (float scale);

	void					DrawGun( TBinObject* gun );

private:
	TString					characterName;

	// entity settings
	vec3_t					m_origin;
	vec3_t					m_translate;	
	vec3_t					m_angles;	
	int						m_sequence;			// sequence index
	float					m_frame;			// frame
	int						m_bodynum;			// bodypart selection	
	int						m_skinnum;			// skin group selection
	byte					m_controller[4];	// bone controllers
	byte					m_blending[2];		// animation blending
	byte					m_mouth;			// mouth position
	bool					m_owntexmodel;		// do we have a modelT.mdl ?

	// internal data
	studiohdr_t				*m_pstudiohdr;
	mstudiomodel_t			*m_pmodel;
	long					modelSize;

	studiohdr_t				*m_ptexturehdr;
	studiohdr_t				*m_panimhdr[32];

	vec4_t					m_adj;				// FIX: non persistant, make static

	void					CalcBoneAdj( void );
	void					CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q );
	void					CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos );
	void					CalcRotations ( vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f );
	mstudioanim_t			*GetAnim( mstudioseqdesc_t *pseqdesc );
	void					SlerpBones( vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s );
	void					SetUpBones ( void );

	void					DrawPoints( float transparency );

	void					Lighting (float *lv, int bone, int flags, vec3_t normal);
	void					Chrome (int *chrome, int bone, vec3_t normal);

	void					SetupLighting( void );

	void					SetupModel ( int bodypart );

	vec3_t					g_xformverts[MAXSTUDIOVERTS];	// transformed vertices
	vec3_t					g_lightvalues[MAXSTUDIOVERTS];	// light surface normals
	vec3_t*					g_pxformverts;
	vec3_t*					g_pvlightvalues;

	vec3_t					g_lightvec;						// light vector in model reference frame
	vec3_t					g_blightvec[MAXSTUDIOBONES];	// light vectors in bone reference frames
	int						g_ambientlight;					// ambient world light
	float					g_shadelight;					// direct world light
	vec3_t					g_lightcolor;

	int						g_smodels_total;				// cookie

	float					g_bonetransform[MAXSTUDIOBONES][3][4];	// bone transformation matrix

	int						g_chrome[MAXSTUDIOVERTS][2];	// texture coords for surface normals
	int						g_chromeage[MAXSTUDIOBONES];	// last time chrome vectors were updated
	vec3_t					g_chromeup[MAXSTUDIOBONES];		// chrome vector "up" in bone reference frames
	vec3_t					g_chromeright[MAXSTUDIOBONES];	// chrome vector "right" in bone reference frames

	vec3_t					g_vright;						// needs to be set to viewer's right in order for chrome to work
	float					g_lambert;

	bool					bFilterTextures;
	size_t					g_texcount;
	size_t					textures[MAXSTUDIOSKINS];		// reserved textures

	float					prev; // keep track of timing
	float					scale; // character scaling
	float					matrix[16]; // openGL matrix for gun

	float					minx,miny,minz;
	float					maxx,maxy,maxz;
};

#endif
