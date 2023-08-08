#include "vector.h"

#include <cmath>

Vector::Vector(const Point& p)
	: x(p.x), y(p.y), z(p.z)
{
}

Vector Vector::operator-() const
{
	return Vector(-x, -y, -z);
}

Vector Vector::operator+(const Vector& v) const
{
	return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::Normalized() const
{
	return (*this) / Length();
}

Vector Vector::operator*(float s) const
{
	return Vector(x * s, y * s, z * s);
}

Vector Vector::operator/(float s) const
{
	return Vector(x / s, y / s, z / s);
}

float Vector::Length() const
{
	return sqrt(x*x + y*y + z*z);
}

float Vector::LengthSqr() const
{
	return (x*x + y*y + z*z);
}

Vector operator-(Point a, Point b)
{
	Vector v;

	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;

	return v;
}

Point Point::operator+(const Vector& v) const
{
	return Point(x + v.x, y + v.y, z + v.z);
}

Point Point::operator-(const Vector& v) const
{
	return Point(x - v.x, y - v.y, z - v.z);
}
