#ifndef __OBJECT_SMOKE_H__
#define __OBJECT_SMOKE_H__

//=============================================================

#define kMaxSmoke	125
#define kTimeToLive	50

//=============================================================

class TSmoke
{
	TSmoke( const TSmoke& );
	const TSmoke& operator=(const TSmoke&);
public:
	TSmoke( void );
	virtual ~TSmoke( void );

	void Animate( void );
	void Draw( void );
	void SetTTL( size_t _ttl );
	void SetPos( float x, float y, float z );
	void SetDirection( float dx, float dz );
	bool InUse( void );

	static void DrawSmoke( void );
	static void AnimateSmoke( void );
	static bool AddNewParticle( float x, float y, float z, 
								float dx, float dy );
private:
	float x,y,z;
	float dx,dz;
	size_t ttl;
	float size;
	int cntr;
	unsigned char col;

	static TSmoke list[kMaxSmoke];
};

//=============================================================

#endif
