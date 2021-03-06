//================================================================================
// @ Vector3.cpp
// 
// Description: Definitions for the Vector3 class
//
// -------------------------------------------------------------------------------
//
// Authors: James M. Van Verth, Lars M. Bishop
// Retrieved From: Essential Mathematics for Games and Interactive Applications SE
// On Date: 2013
//
// Modified by: Frank Hart
// Date last modified: 2013
//
//================================================================================

#include "Vector3.h"
#include "CommonMath.h"


//--------------------------------------------------------------------------------
//		STATIC MEMBERS
//--------------------------------------------------------------------------------

Vector3 Vector3::xAxis(1.0f, 0.0f, 0.0f);
Vector3 Vector3::yAxis(0.0f, 1.0f, 0.0f);
Vector3 Vector3::zAxis(0.0f, 0.0f, 1.0f);
Vector3 Vector3::origin(0.0f, 0.0f, 0.0f);
Vector3 Vector3::unity(1.0f, 1.0f, 1.0f);


//--------------------------------------------------------------------------------
//	@	Vector3::Vector3()
//--------------------------------------------------------------------------------
//		Assignment operator
//--------------------------------------------------------------------------------
Vector3& Vector3::operator= (const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;

}	//End: Vector3::operator=()


//--------------------------------------------------------------------------------
//	@	Vector3::operator==()
//--------------------------------------------------------------------------------
//		Comparison operator
//--------------------------------------------------------------------------------
bool Vector3::operator== (const Vector3& other) const
{
	if (DgAreEqual(other.x,x)
		&& DgAreEqual(other.y,y)
		&& DgAreEqual(other.z,z))
		return true;

	return false;

}	//End: Vector3::operator==()


//--------------------------------------------------------------------------------
//	@	Vector3::operator!=()
//--------------------------------------------------------------------------------
//		Comparison operator
//--------------------------------------------------------------------------------
bool Vector3::operator!= (const Vector3& other) const
{
	if (DgAreEqual(other.x,x)
		&& DgAreEqual(other.y,y)
		&& DgAreEqual(other.z,z))
		return false;

	return true;

}	//End: Vector3::operator!=()


//--------------------------------------------------------------------------------
//	@	operator<<()
//--------------------------------------------------------------------------------
//		Output 
//--------------------------------------------------------------------------------
DgWriter& operator<<(DgWriter& out, const Vector3& source)
{
	out << '<' << source.x << ' ' << source.y << ' ' << source.z << '>';

	return out;
    
}   // End of operator<<()


//--------------------------------------------------------------------------------
//	@	operator>>()
//--------------------------------------------------------------------------------
//		Build vector from in stream
//--------------------------------------------------------------------------------
DgReader& operator>> (DgReader& in, Vector3& dest)
{
    if ((in >> dest.x >> dest.y >> dest.z).fail())
    {
        std::cerr << "@operator>>(Vector3) -> Bad read." << std::endl;
    }

    return in;

}	//End: operator>>()


//--------------------------------------------------------------------------------
//	@	Vector3::IsZero()
//--------------------------------------------------------------------------------
//		Check for zero vector
//--------------------------------------------------------------------------------
bool Vector3::IsZero () const
{
	return ::IsZero(x*x + y*y + z*z);

}	//End: Vector3::IsZero()



//--------------------------------------------------------------------------------
//	@	Vector3::IsUnit()
//--------------------------------------------------------------------------------
//		Checks for unit vector
//--------------------------------------------------------------------------------
bool Vector3::IsUnit() const
{
	return ::IsZero(1.0f - x*x + y*y + z*z);

}	//End: Vector3::IsUnit()


//--------------------------------------------------------------------------------
//	@	Vector3::Clean()
//--------------------------------------------------------------------------------
//		Sets elements close to zero to zero
//--------------------------------------------------------------------------------
void Vector3::Clean()
{
	if (::IsZero(x))
		x = 0.0f;
	if (::IsZero(y))
		y = 0.0f;
	if (::IsZero(z))
		z = 0.0f;

}	//End: Vector3::Clean()


//--------------------------------------------------------------------------------
//	@	Vector3::Normalize()
//--------------------------------------------------------------------------------
//		Set to a unit vector
//--------------------------------------------------------------------------------
void Vector3::Normalize()
{
	float lengthsq = x*x + y*y + z*z;

	if (::IsZero(lengthsq))
	{
		Zero();
	}
	else
	{
		float factor = DgInvSqrt(lengthsq);
		x *= factor;
		y *= factor;
		z *= factor;
	}

}	//End: Vector3::Normalize()


//--------------------------------------------------------------------------------
//	@	Vector3::operator+()
//--------------------------------------------------------------------------------
//		Add vector to self and return
//--------------------------------------------------------------------------------
Vector3 Vector3::operator+ (const Vector3& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);

}	//End: Vector3::operator+()


//--------------------------------------------------------------------------------
//	@	Vector3::operator+=()
//--------------------------------------------------------------------------------
//		Add vector to self, store in self
//--------------------------------------------------------------------------------
Vector3& operator+= (Vector3& self, const Vector3& other)
{
	self.x += other.x;
	self.y += other.y;
	self.z += other.z;

	return self;

}	//End: Vector3::operator+=()


//--------------------------------------------------------------------------------
//	@	Vector3::operator-()
//--------------------------------------------------------------------------------
//		Subtract vector from self and return
//--------------------------------------------------------------------------------
Vector3 Vector3::operator- (const Vector3& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);

}	//End: Vector3::operator-()


//--------------------------------------------------------------------------------
//	@	Vector3::operator-=()
//--------------------------------------------------------------------------------
//		Subtract vector to self, store in self
//--------------------------------------------------------------------------------
Vector3& operator-= (Vector3& self, const Vector3& other)
{
	self.x -= other.x;
	self.y -= other.y;
	self.z -= other.z;

	return self;

}	//End: Vector3::operator-=()


//--------------------------------------------------------------------------------
//	@	Vector3::operator-()
//--------------------------------------------------------------------------------
//		Negate self and return
//--------------------------------------------------------------------------------
Vector3 Vector3::operator- () const
{
	return Vector3(-x, -y, -z);

}	//End: Vector3::operator-()


//--------------------------------------------------------------------------------
//	@	Vector3::operator+()
//--------------------------------------------------------------------------------
//		Scalar multiplication
//--------------------------------------------------------------------------------
Vector3 Vector3::operator* (float val)
{
	return Vector3(val*x, val*y, val*z);

}	//End: Vector3::operator*()


//--------------------------------------------------------------------------------
//	@	Vector3::operator*()
//--------------------------------------------------------------------------------
//		Scalar multiplication
//--------------------------------------------------------------------------------
Vector3 operator* (float val, const Vector3& v)
{
	return Vector3(val*v.x, val*v.y, val*v.z);

}	//End: operator*()


//--------------------------------------------------------------------------------
//	@	Vector3::operator*=()
//--------------------------------------------------------------------------------
//		Scalar multiplication by self
//--------------------------------------------------------------------------------
Vector3& Vector3::operator*= (float val)
{
	x *= val;
	y *= val;
	z *= val;

	return *this;

}	//End: Vector3::operator*=()


//--------------------------------------------------------------------------------
//	@	Vector3::operator/()
//--------------------------------------------------------------------------------
//		Scalar division
//--------------------------------------------------------------------------------
Vector3 Vector3::operator/ (float val)
{
	return Vector3(x/val, y/val, z/val);

}	//End: Vector3::operator/()


//--------------------------------------------------------------------------------
//	@	Vector3::operator/=()
//--------------------------------------------------------------------------------
//		Scalar multiplication by self
//--------------------------------------------------------------------------------
Vector3& Vector3::operator/= (float val)
{
	x /= val;
	y /= val;
	z /= val;

	return *this;

}	//End: Vector3::operator/=()


//--------------------------------------------------------------------------------
//	@	Dot()
//--------------------------------------------------------------------------------
//		Dot product
//--------------------------------------------------------------------------------
float Dot (const Vector3& v1, const Vector3& v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);

}	//End: Dot()


//--------------------------------------------------------------------------------
//	@	Cross()
//--------------------------------------------------------------------------------
//		Cross product
//--------------------------------------------------------------------------------
Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	return Vector3(	v1.y*v2.z - v1.z*v2.y,
					v1.z*v2.x - v1.x*v2.z,
					v1.x*v2.y - v1.y*v2.x);

}	//End: Cross()


//--------------------------------------------------------------------------------
//	@	DistVector3Vector3
//--------------------------------------------------------------------------------
//		Point distance
//--------------------------------------------------------------------------------
float DistVector3Vector3(const Vector3& p1, const Vector3& p2)
{
	float x = p1.x - p2.x;
	float y = p1.y - p2.y;
	float z = p1.z - p2.z;

	return ::DgSqrt(x*x + y*y + z*z);

}	//End: DistVector3Vector3()


//--------------------------------------------------------------------------------
//	@	SqDistVector3Vector3()
//--------------------------------------------------------------------------------
//		Point distance squared
//--------------------------------------------------------------------------------
float SqDistVector3Vector3(const Vector3& p1, const Vector3& p2)
{
	float x = p1.x - p2.x;
	float y = p1.y - p2.y;
	float z = p1.z - p2.z;

	return (x*x + y*y + z*z);

}	//End: SqDistVector3Vector3()


//--------------------------------------------------------------------------------
//	@	Vector3::Length()
//--------------------------------------------------------------------------------
//		Vector Length
//--------------------------------------------------------------------------------
float Vector3::Length() const
{
	return DgSqrt(x*x + y*y + z*z);

}	//End: Vector::Length()


//--------------------------------------------------------------------------------
//	@	Vector3::LengthSquared()
//--------------------------------------------------------------------------------
//		Vector Length squared
//--------------------------------------------------------------------------------
float Vector3::LengthSquared() const
{
	return (x*x + y*y + z*z);

}	//End: Vector::Length()