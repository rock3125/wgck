#ifndef __OBJECT_SMOKE_H__
#define __OBJECT_SMOKE_H__

//=============================================================

#define kMaxSmoke	200
#define kTimeToLive	50

//=============================================================

class _EXPORT TSmoke
{
	TSmoke( const TSmoke& );
	const TSmoke& operator=(const TSmoke&);
public:
	TSmoke( void );
	virtual ~TSmoke( void );

	static void Draw( void );
	static void Logic( void );
	static bool Add( float x, float y, float z, 
					 float dx, float dy, float dz );

protected:
	void Animate( void );
	void DrawSingle( void );
	void SetTTL( size_t _ttl );
	void SetPos( float x, float y, float z );
	void SetDirection( float dx, float dy, float dz );
	bool InUse( void );

private:
	float x,y,z;
	float dx,dy,dz;
	size_t ttl;
	float size;
	int cntr;
	unsigned char col;

	static TSmoke list[kMaxSmoke];
};

//=============================================================

#endif
