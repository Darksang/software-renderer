#ifndef _VECTOR3F_H
#define _VECTOR3F_H

struct Vector3f {

    float x, y, z;

    Vector3f() {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3f(int a, int b, int c) {
        x = a;
        y = b;
        z = c;
    }

    float & operator [](int i) {
		return ((&x)[i]);
	}

	const float & operator [](int i) const {
		return ((&x)[i]);
	}

    Vector3f & operator +=(const Vector3f & v) {
		x += v.x;
		y += v.y;
		z += v.z;

		return (*this);
	}

	Vector3f & operator -=(const Vector3f & v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return (*this);
	}

    Vector3f & operator *=(float s) {
		x *= s;
		y *= s;
		z *= s;

		return (*this);
	}

	Vector3f & operator /=(float s) {
		s = 1.0f / s;
		x *= s;
		y *= s;
		z *= s;

		return (*this);
	}
};

inline Vector3f operator +(const Vector3f & a, const Vector3f & b) {
		return (Vector3f(a.x + b.x, a.y + b.y, a.z + b.z));
}

inline Vector3f operator -(const Vector3f & a, const Vector3f & b) {
	return (Vector3f(a.x - b.x, a.y - b.y, a.z - b.z));
}

inline Vector3f operator *(const Vector3f & v, float s) {
	return (Vector3f(v.x * s, v.y * s, v.z * s));
}

inline Vector3f operator /(const Vector3f & v, float s) {
	s = 1.0f / s;
	return (Vector3f(v.x * s, v.y * s, v.z * s));
}

inline Vector3f operator -(const Vector3f & v) {
	return (Vector3f(-v.x, -v.y, -v.z));
}

inline float Dot(const Vector3f & a, const Vector3f & b) {
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

inline Vector3f Cross(const Vector3f & a, const Vector3f & b) {
	return (Vector3f(a.y * b.z - a.z * b.y,
					 a.z * b.x - a.x * b.z,
					 a.x * b.y - a.y * b.x));
}

inline float Magnitude(const Vector3f & v) {
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline Vector3f Normalize(const Vector3f & v) {
	return (v / Magnitude(v));
}

#endif