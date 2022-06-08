#pragma once
//#include <PxPhysicsAPI.h>
#include <DirectXMath.h>

struct vector3 {
	inline vector3() { memset(this, 0, sizeof(vector3)); }
	inline __vectorcall vector3(const DirectX::XMVECTOR& vec) { DirectX::XMStoreFloat3(&this->vec, vec); }
	inline __vectorcall vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

	inline float length() const { 
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}
	inline vector3 normalized() const { 
		return DirectX::XMVector3Normalize(*this);
	}
	inline vector3 negate() const {
		return DirectX::XMVectorNegate(*this);
	}

	inline vector3 __vectorcall operator+(const vector3& other) const {	return DirectX::XMVectorAdd(*this, other); }
	inline vector3 __vectorcall operator-(const vector3& other) const { return DirectX::XMVectorSubtract(*this, other); }

	inline vector3& __vectorcall operator+=(const vector3& other) { *this = DirectX::XMVectorAdd(*this, other); }
	inline vector3& __vectorcall operator-=(const vector3& other) { *this = DirectX::XMVectorSubtract(*this, other); }

	inline vector3 __vectorcall operator*(const float& value) const { return DirectX::XMVectorMultiply(*this, DirectX::XMVectorReplicate(value)); }
	inline vector3 __vectorcall operator/(const float& value) const { return DirectX::XMVectorDivide(*this, DirectX::XMVectorReplicate(value)); }

	inline vector3& __vectorcall operator*=(const float& value) { *this = DirectX::XMVectorMultiply(*this, DirectX::XMVectorReplicate(value)); }
	inline vector3& __vectorcall operator/=(const float& value) { *this = DirectX::XMVectorDivide(*this, DirectX::XMVectorReplicate(value)); }

	inline operator DirectX::XMVECTOR() const {	return DirectX::XMLoadFloat3(&this->vec); }
	//inline operator physx::PxVec3() const {	return physx::PxVec3(x, y, z); }

	inline bool __vectorcall operator==(const vector3& other) const { return DirectX::XMVector3Equal(*this, other); }
	inline bool __vectorcall operator!=(const vector3& other) const { return !DirectX::XMVector3Equal(*this, other); }
	
	union {
		struct { float x, y, z; };
		DirectX::XMFLOAT3 vec;
	};
};

inline float __vectorcall dot(const vector3& a, const vector3& b) {
	return DirectX::XMVector3Dot(a, b).m128_f32[0];
}

inline vector3 __vectorcall cross(const vector3& a, const vector3& b) {
	return DirectX::XMVector3Cross(a, b);
}
