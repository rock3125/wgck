#ifndef __DXPARSER_VECTOR_H_
#define __DXPARSER_VECTOR_H_

//==========================================================================

class _EXPORT TVector
{
public:
	TVector( void );
    ~TVector( void );

	TVector( const TVector& );
	const TVector& operator=( const TVector& );

    float	X( void ) const;
    void	X( float _x );

    float	Y( void ) const;
    void	Y( float _y );

    float	Z( void ) const;
    void	Z( float _z );

    float	Xangle( void ) const;
    void	Xangle( float _xangle );

    float	Yangle( void ) const;
    void	Yangle( float _yangle );

    float	Zangle( void ) const;
    void	Zangle( float _zangle );

private:
	float x;
    float y;
    float z;
    float xangle;
    float yangle;
    float zangle;
};

//==========================================================================

#endif

