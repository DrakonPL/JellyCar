#ifndef _VECTOR2_H
#define _VECTOR2_H

#include <math.h>

class Vector2
{
public:
	float	X;
	float	Y;
	
	inline Vector2() : X(0), Y(0) { }
	inline Vector2(const float x, const float y) : X(x), Y(y) { }
	inline Vector2(const Vector2& vec) : X(vec.X), Y(vec.Y) { }
	
	inline Vector2& operator = ( const Vector2& vec )
	{
		X = vec.X;
		Y = vec.Y;
		return *this;
	}
	
	inline bool operator == (const Vector2& vec ) const
	{
		return ( X == vec.X && Y == vec.Y );
	}
	
	inline bool operator != (const Vector2& vec ) const
	{
		return ( X != vec.X || Y != vec.Y );
	}
	
	inline Vector2	operator + (const Vector2& right) const { return Vector2(X + right.X, Y + right.Y); }
	inline Vector2&	operator += (const Vector2& right) { X += right.X; Y += right.Y; return *this; }
	
	inline Vector2	operator - (const Vector2& right) const { return Vector2(X - right.X, Y - right.Y); }
	inline Vector2&	operator -= (const Vector2& right) { X -= right.X; Y -= right.Y; return *this; }
	
	// scaling!
	inline Vector2	operator * (const float right) const { return Vector2(X * right, Y * right); }
	inline Vector2&	operator *= (const float right) { X *= right; Y *= right; return *this; }
	
	inline Vector2	operator / (const float right) const
	{ 
		float inv = 1.0f / right;
		return Vector2( X * inv, Y * inv );
	}
	
	inline Vector2&	operator /= (const float right) { X /= right; Y /= right; return *this; }
	
	// per-element multiplication
	inline Vector2	operator * (const Vector2& right) const { return Vector2( X * right.X, Y * right.Y ); }
	inline Vector2&	operator *= (const Vector2& right) { X *= right.X; Y *= right.Y; return *this; }
	
	inline Vector2	operator / (const Vector2& right) const { return Vector2( X / right.X, Y / right.Y ); }
	inline Vector2&	operator /= (const Vector2& right) { X /= right.X; Y /= right.Y; return *this; }
	
	inline const Vector2& operator + () const { return *this; }
	inline Vector2 operator - () const { return Vector2(-X, -Y); }
	
	// helpers
	inline float		length() const { return (float)sqrt( (X*X) + (Y*Y) ); }
	
	inline float		lengthSquared() const { return ((X*X) + (Y*Y)); }
	
	inline float		normalise()
	{
		float l = sqrtf( X * X + Y * Y );
		
		if (l > 1e-08)
		{
			float invL = 1.0f / l;
			X *= invL;
			Y *= invL;
		}
		
		return l;
	}
	
	inline Vector2 normalisedCopy() const
	{
		Vector2 ret = *this;
		ret.normalise();
		return ret;
	}
	
	inline float dotProduct( const Vector2& vec ) const
	{
		return (X * vec.X) + (Y * vec.Y);
	}
	
	inline Vector2 getPerpendicular() const
	{
		return Vector2(-Y, X);
	}
	
	inline void makePerpendicular()
	{
		float tempX = X;
		X = -Y;
		Y = tempX;
	}
	
	inline float crossProduct( const Vector2& vec ) const
	{
		return (X * vec.Y) - (Y * vec.X);
	}
	
	inline Vector2 lerp( const Vector2& to, float i)
	{
		return (*this) + ((to - (*this)) * i);
	}
	
	inline Vector2 slerp( const Vector2& to, float i)
	{
		return (*this) + ((to - (*this)) * (0.5f + (sinf((3.141592654f * i) - 1.570796f) * 0.5f)));
	}	
	
	static const Vector2 Zero;
	static const Vector2 One;
};

#endif	// _VECTOR2_H
