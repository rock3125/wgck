#include <precomp_header.h>
#include <object/camera.h>
#include <win32/win32.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================

TCamera::TCamera( void )
	: recording(NULL)
{
	type = 0;
	minRadius = 2.5f;
	maxRadius = 25.0f;
	lx = 0;
	ly = 0;
	lz = 0;

	sx = 0;
	sy = 0;
	sz = 0;
	
	vx = 0;
	vz = 0;
	py = 0;

	yAngle = 180;
	fixedCameraRotation = 0;
	xAngle = 0;
	radius = maxRadius * 0.5f;
	zAngle = 0;
};


TCamera::~TCamera( void )
{
};


size_t TCamera::Type( void ) const
{
	return type;
};


void TCamera::Type( size_t t )
{
	type = t;
	switch ( type )
	{
	case 5:
	case 6:
	case 0:
	case 1:
		{
			fixedCameraRotation = 180;
			break;
		}
	case 2:
		{
			fixedCameraRotation = -90;
			break;
		}
	case 3:
		{
			fixedCameraRotation = 90;
			break;
		}
	case 4:
		{
			fixedCameraRotation = 0;
			break;
		}
	}
};


void TCamera::SetVehicleCamera( TLandscape& landObj, 
								TTank& vehicle, 
								TParams& params,
								bool ctrlDown,
								float dxa, float dya,
								bool showMap )
{
	if ( type==0 || showMap )
	{

		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
	
			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

		float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		if ( showMap )
		{
			v1fcos = v1fcos * 0.5f;
			v1fsin = v1fsin * 0.5f;
		}

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;
		if ( -ly < vehicle.Y() )
			ly = -vehicle.Y();

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
		}

		float yangle = (-vehicle.Yangle()+fixedCameraRotation -vehicle.TurretAngle())*degToRad;
		float v1fcos = float(cos(yangle))*radius;
		float v1fsin = float(sin(yangle))*radius;

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;
		if ( -ly < vehicle.Y() )
			ly = -vehicle.Y();

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		yAngle = -vehicle.Yangle() + fixedCameraRotation - vehicle.TurretAngle();
	}
	else
	{
		float ang = -vehicle.Yangle() + fixedCameraRotation - vehicle.TurretAngle();
		float yangle = ang*degToRad;

		lx = -vehicle.X();
		ly = -vehicle.Y() - 1;
		lz = -vehicle.Z();

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = -vehicle.Xangle();
		xAngle = vehicle.Zangle() + vehicle.BarrelAngle();
		yAngle = ang;
	}
};


void TCamera::SetVehicleCamera( TLandscape& landObj, 
								TPlane& vehicle, 
								TParams& params,
								bool ctrlDown,
								float dxa, float dya,
								bool showMap )
{
	if ( type==0 || showMap )
	{

		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
	
			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

		float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		if ( showMap )
		{
			v1fcos = v1fcos * 0.5f;
			v1fsin = v1fsin * 0.5f;
		}

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
		}

		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;
		float v1fcos = float(cos(yangle))*radius;
		float v1fsin = float(sin(yangle))*radius;

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
	else if ( type==5 )
	{
		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;

		lx = -vehicle.X();
		ly = -vehicle.Y() - 1;
		lz = -vehicle.Z();

		CheckCameraHeight( params, lx,ly,lz );

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = -vehicle.Xangle();
		xAngle = vehicle.Zangle()-vehicle.AngleAdjust();
		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
	else // below the plane
	{
		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;

		lx = -vehicle.X();
		ly = -vehicle.Y() - 1;
		lz = -vehicle.Z();

		vx = -lx;
		vz = -lz;

		sx = lx;
		sy = ly;
		sz = lz;

		zAngle = -vehicle.Xangle();
		xAngle = (vehicle.Zangle() - vehicle.AngleAdjust()) + 45;
		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
};


void TCamera::SetVehicleCamera( TLandscape& landObj, 
								TFlak& vehicle, 
								TParams& params,
								bool ctrlDown,
								float dxa, float dya,
								bool showMap )
{
	if ( type==0 || showMap )
	{

		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
	
			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

		float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;
		if ( showMap )
		{
			v1fcos = v1fcos * 0.5f;
			v1fsin = v1fsin * 0.5f;
		}

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
		}

		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;
		float v1fcos = float(cos(yangle))*radius;
		float v1fsin = float(sin(yangle))*radius;

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
	else
	{
		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;

		lx = -vehicle.X();
		ly = -vehicle.Y() - 1;
		lz = -vehicle.Z();

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		// vehicle.angleAdjust?
		xAngle = vehicle.Xangle() + vehicle.BarrelAngle();
		yAngle = -vehicle.Yangle() + fixedCameraRotation;
		zAngle = vehicle.Zangle();
	}
};


void TCamera::SetVehicleCamera( TLandscape& landObj, 
								TArtillery& vehicle, 
								TParams& params,
								bool ctrlDown,
								float dxa, float dya,
								bool showMap )
{
	if ( type==0 || showMap )
	{

		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
	
			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

		float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;
		if ( showMap )
		{
			v1fcos = v1fcos * 0.5f;
			v1fsin = v1fsin * 0.5f;
		}

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
		}

		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;
		float v1fcos = float(cos(yangle))*radius;
		float v1fsin = float(sin(yangle))*radius;

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
	else
	{
		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;

		lx = -vehicle.X();
		ly = -vehicle.Y() - 1;
		lz = -vehicle.Z();

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = -vehicle.Xangle();
		xAngle = vehicle.Zangle();
		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
};


void TCamera::SetVehicleCamera( TLandscape& landObj, 
								TMG42& vehicle, 
								TParams& params,
								bool ctrlDown,
								float dxa, float dya,
								bool showMap )
{
	if ( type==0 || showMap )
	{

		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
	
			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

		float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;
		if ( showMap )
		{
			v1fcos = v1fcos * 0.5f;
			v1fsin = v1fsin * 0.5f;
		}

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minRadius )
				radius = minRadius;
			if ( radius>maxRadius )
				radius = maxRadius;
		}

		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;
		float v1fcos = float(cos(yangle))*radius;
		float v1fsin = float(sin(yangle))*radius;

		lx = -vehicle.X() + v1fsin;
		lz = -vehicle.Z() - v1fcos;
		vx = vehicle.X() - v1fsin;
		vz = vehicle.Z() + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<vehicle.Y() )
			ly = vehicle.Y();
		ly = -ly - 3.0f;

		if ( -ly < (landObj.WaterLevel()+3) )
			ly = -landObj.WaterLevel()-3;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (vehicle.Y()+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		yAngle = -vehicle.Yangle() + fixedCameraRotation;
	}
	else
	{
		float yangle = (-vehicle.Yangle()+fixedCameraRotation)*degToRad;

		lx = -vehicle.X();
		ly = -vehicle.Y() - 1;
		lz = -vehicle.Z();

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		// vehicle.angleAdjust?
		xAngle = vehicle.Xangle() + vehicle.BarrelAngle();
		yAngle = -vehicle.Yangle() + fixedCameraRotation;
		zAngle = vehicle.Zangle();
	}
};


void TCamera::SetPersonCamera( TLandscape& landObj, const TCharacter& ch,
							   TParams& params, bool ctrlDown, 
							   float dxa, float dya, bool showMap )
{
	if ( type==0 || showMap )
	{
		float mx = ch.X();
		float my = ch.Y();
		float mz = ch.Z();

		float minr = minRadius * 0.5f;
		float maxr = maxRadius * 0.25f;
	
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minr )
				radius = minr;
			if ( radius>maxr )
				radius = maxr;

			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

	    float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;
		if ( showMap )
		{
			v1fcos = v1fcos * 0.5f;
			v1fsin = v1fsin * 0.5f;
		}

		lx = -mx + v1fsin;
		lz = -mz - v1fcos;
		vx = mx - v1fsin;
		vz = mz + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<my )
			ly = my;
		ly = -ly - 1.5f;
		if ( -ly < (landObj.WaterLevel()+1.5f) )
			ly = -landObj.WaterLevel()-1.5f;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (my+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		float mx = ch.X();
		float my = ch.Y();
		float mz = ch.Z();

		float minr = minRadius * 0.5f;
		float maxr = maxRadius * 0.25f;
	
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minr )
				radius = minr;
			if ( radius>maxr )
				radius = maxr;
		}

		float yangle = (ch.Yangle()+fixedCameraRotation)*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		lx = -mx + v1fsin;
		lz = -mz - v1fcos;
		vx = mx - v1fsin;
		vz = mz + v1fcos;

		// camera height on landscape
		py = ly;
		landObj.GetLORTriangle( vx,ly,vz );
		if ( ly<my )
			ly = my;
		ly = -ly - 1.5f;
		if ( -ly < (landObj.WaterLevel()+1.5f) )
			ly = -landObj.WaterLevel()-1.5f;

		CheckCameraHeight( params, lx,ly,lz );

		sx = vx;
		sy = ly;
		sz = vz;

		xAngle = -float(atan( (my+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		zAngle = 0;
		yAngle = ch.Yangle() + fixedCameraRotation;
	}
	else
	{
		float yangle = (ch.Yangle()+fixedCameraRotation)*degToRad;

		lx = -ch.X();
		ly = -ch.Y() - 1;
		lz = -ch.Z();

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = 0;
		yAngle = ch.Yangle() + fixedCameraRotation;
	}
};


void TCamera::SetCamera( float ox, float oy, float oz,
						 float oyangle,
					     float dxa, float dya )
{
	if ( type==0 )
	{
		bool ctrlDown = false;
		float mx = ox;
		float my = oy;
		float mz = oz;

		float minr = minRadius;
		float maxr = maxRadius * 10;
	
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minr )
				radius = minr;
			if ( radius>maxr )
				radius = maxr;

			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

	    float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		lx = -mx + v1fsin;
		lz = -mz - v1fcos;
		vx = mx - v1fsin;
		vz = mz + v1fcos;

		// camera height on landscape
		py = ly;
		ly = -my - 1.5f;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (my+ly) / radius )) * radToDeg;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else if ( type<5 )
	{
		bool ctrlDown = false;
		float mx = ox;
		float my = oy;
		float mz = oz;

		float minr = minRadius;
		float maxr = maxRadius * 10;
	
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minr )
				radius = minr;
			if ( radius>maxr )
				radius = maxr;
		}

		float yangle = (oyangle+fixedCameraRotation)*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		lx = -mx + v1fsin;
		lz = -mz - v1fcos;
		vx = mx - v1fsin;
		vz = mz + v1fcos;

		// camera height on landscape
		py = ly;
		ly = -my - 1.5f;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (my+ly) / radius )) * radToDeg;

		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		yAngle = oyangle + fixedCameraRotation;
	}
	else
	{
		lx = -ox;
		ly = -oy-1;
		lz = -oz;

		vx = -lx;
		vz = -lz;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = 0;
		yAngle = oyangle + 180;
	}
};


void TCamera::SetCamera( float ox, float oy, float oz,
						 float oyangle,
					     float dxa, float dya, float scale )
{
	if ( type==0 )
	{
		bool ctrlDown = false;
		float mx = ox * scale;
		float my = oy * scale;
		float mz = oz * scale;

		float minr = minRadius * scale;
		float maxr = maxRadius * 10 * scale;
	
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minr )
				radius = minr;
			if ( radius>maxr )
				radius = maxr;

			yAngle += dya;
			if ( yAngle>360.0f ) yAngle -= 360.0f;
			if ( yAngle<0.0f ) yAngle += 360.0f;
		}

	    float yangle = yAngle*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		lx = -mx + v1fsin;
		lz = -mz - v1fcos;
		vx = mx - v1fsin;
		vz = mz + v1fcos;

		// camera height on landscape
	//	int lor, objlor;
		py = ly;
		ly = -my - 1.5f;

		sx = vx;
		sy = ly;
		sz = vz;

		zAngle = 0;
		xAngle = -float(atan( (my+ly) / radius )) * radToDeg;
	//	if ( xAngle > 40 )
	//		xAngle = 40;
		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;
	}
	else
	{
		bool ctrlDown = false;
		float mx = ox * scale;
		float my = oy * scale;
		float mz = oz * scale;

		float minr = minRadius * scale;
		float maxr = maxRadius * 10 * scale;
	
		if ( !ctrlDown )
		{
			radius += dxa;
			if ( radius<minr )
				radius = minr;
			if ( radius>maxr )
				radius = maxr;
		}

		float yangle = (oyangle+fixedCameraRotation)*degToRad;
		float v1fcos = (float)cos(yangle)*radius;
		float v1fsin = (float)sin(yangle)*radius;

		lx = -mx + v1fsin;
		lz = -mz - v1fcos;
		vx = mx - v1fsin;
		vz = mz + v1fcos;

		// camera height on landscape
		py = ly;
		ly = -my - 1.5f;

		zAngle = 0;
		xAngle = -float(atan( (my+ly) / radius )) * radToDeg;

		if ( xAngle < -2 ) // limit the sky movement downwards
			xAngle = -2;

		sx = vx;
		sy = ly;
		sz = vz;

		yAngle = oyangle + fixedCameraRotation;
	}
};


// check if we're not going through a vehicle at the camera position
// if so - put the camera up a bit
void TCamera::CheckCameraHeight( TParams& params, float lx, float& ly, float lz )
{
	size_t i;

	for ( i=0; i<params.numTanks; i++ )
	{
		TTank& item = params.tanks[i];
		if ( item.Strength()>0 )
		if ( item.InsideVehicle( -lx, -ly, -lz ) )
		{
			ly -= item.SizeY();
			return;
		}
	}

	for ( i=0; i<params.numPlanes; i++ )
	{
		TPlane& item = params.planes[i];
		if ( item.Strength()>0 )
		if ( item.InsideVehicle( -lx, -ly, -lz ) )
		{
			ly -= item.SizeY();
			return;
		}
	}

	for ( i=0; i<params.numArtillery; i++ )
	{
		TArtillery& item = params.artillery[i];
		if ( item.Strength()>0 )
		if ( item.InsideVehicle( -lx, -ly, -lz ) )
		{
			ly -= item.SizeY();
			return;
		}
	}

	for ( i=0; i<params.numFlak; i++ )
	{
		TFlak& item = params.flak[i];
		if ( item.Strength()>0 )
		if ( item.InsideVehicle( -lx, -ly, -lz ) )
		{
			ly -= item.SizeY();
			return;
		}
	}

	for ( i=0; i<params.numMG42s; i++ )
	{
		TMG42& item = params.mg42[i];
		if ( item.Strength()>0 )
		if ( item.InsideVehicle( -lx, -ly, -lz ) )
		{
			ly -= item.SizeY();
			return;
		}
	}
};


void TCamera::Record( size_t selectedCamera )
{
	PreCond( recording!=NULL );
	recording->Add( lx, ly, lz, xAngle, yAngle, selectedCamera );
};


void TCamera::Playback( size_t& selectedCamera )
{
	PreCond( recording!=NULL );
	recording->Get( lx, ly, lz, xAngle, yAngle, selectedCamera );
};

float TCamera::CameraX( void ) const
{
	return lx;
};


float TCamera::CameraY( void ) const
{
	return ly;
};


float TCamera::CameraZ( void ) const
{
	return lz;
};


float TCamera::ListenX( void ) const
{
	return sx;
};


float TCamera::ListenY( void ) const
{
	return -sy;
};


float TCamera::ListenZ( void ) const
{
	return sz;
};


float TCamera::XAngle( void ) const
{
	return xAngle;
};


float TCamera::YAngle( void ) const
{
	return yAngle;
};


float TCamera::ZAngle( void ) const
{
	return zAngle;
};


void TCamera::SetupRecording( const TString& filename )
{
	if ( recording!=NULL )
		delete recording;
	recording = new TCameraRecord();
	PostCond( recording!=NULL );

	recordingFname = filename;
};


bool TCamera::StopRecording( TString& errStr )
{
	PreCond( recording!=NULL );
	return recording->SaveBinary( recordingFname, errStr );
};


bool TCamera::SetupPlayback( const TString& filename, TString& errStr )
{
	if ( recording!=NULL )
		delete recording;
	recording = new TCameraRecord();
	PostCond( recording!=NULL );

	return recording->LoadBinary( filename, errStr );
};


void TCamera::StopPlayback( void )
{
};


float TCamera::FixedCameraRotation( void ) const
{
	return fixedCameraRotation;
};


float TCamera::CameraAngle( void ) const
{
	switch ( type )
	{
		case 0:
		case 1:
		case 5:
		case 6:
		{
			return -yAngle + fixedCameraRotation;
		};
		case 2:
		{
			return -yAngle + fixedCameraRotation - 90;
		};
		case 3:
		{
			return -yAngle + fixedCameraRotation + 90;
		};
		case 4:
		{
			return -yAngle + fixedCameraRotation - 180;
		};
	}
	return 0;
}

//==========================================================================
