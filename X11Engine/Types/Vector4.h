#pragma once
#include <immintrin.h>
#include <DirectXMath.h>

struct vector4 {
	inline vector4() { memset(this, 0, sizeof(vector4)); }
	inline vector4(const DirectX::XMVECTOR& vec) { DirectX::XMStoreFloat4(&this->vec, vec); }
	inline vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

	inline float length() const {
		return DirectX::XMVector4Length(*this).m128_f32[0];
	}
	inline vector4 normalized() const {
		return DirectX::XMVector4Normalize(*this);
	}
	inline vector4 negate() const {
		return DirectX::XMVectorNegate(*this);
	}

	inline vector4 __vectorcall operator+(const vector4& other) const { return DirectX::XMVectorAdd(*this, other); }
	inline vector4 __vectorcall operator-(const vector4& other) const { return DirectX::XMVectorSubtract(*this, other); }

	inline vector4& __vectorcall operator+=(const vector4& other) { *this = DirectX::XMVectorAdd(*this, other); }
	inline vector4& __vectorcall operator-=(const vector4& other) { *this = DirectX::XMVectorSubtract(*this, other); }

	inline vector4 __vectorcall operator*(const float& value) const { return DirectX::XMVectorMultiply(*this, DirectX::XMVectorReplicate(value)); }
	inline vector4 __vectorcall operator/(const float& value) const { return DirectX::XMVectorDivide(*this, DirectX::XMVectorReplicate(value)); }

	inline vector4& __vectorcall operator*=(const float& value) { *this = DirectX::XMVectorMultiply(*this, DirectX::XMVectorReplicate(value)); }
	inline vector4& __vectorcall operator/=(const float& value) { *this = DirectX::XMVectorDivide(*this, DirectX::XMVectorReplicate(value)); }

	inline operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(&this->vec); }

	inline bool __vectorcall operator==(const vector4& other) const { return DirectX::XMVector4Equal(*this, other); }
	inline bool __vectorcall operator!=(const vector4& other) const { return !DirectX::XMVector4Equal(*this, other); }

	union {
		struct { float x, y, z, w; };
		DirectX::XMFLOAT4 vec;
	};
};

inline float __vectorcall dot(const vector4& a, const vector4& b) { return DirectX::XMVector4Dot(a, b).m128_f32[0]; }