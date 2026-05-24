#include "Matrix.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/matrix.hpp>

#include "Quaternion.h"
#include "Vector3.h"


using namespace glm;

static const mat4& get(const Matrix& mat) {
    return reinterpret_cast<const mat4&>(mat);
}
static mat4& get(Matrix& mat) { return reinterpret_cast<mat4&>(mat); }

static const quat& get(const Quaternion& vec) {
    return reinterpret_cast<const quat&>(vec);
}
static quat& get(Quaternion& vec) { return reinterpret_cast<quat&>(vec); }

static const vec3& get(const Vector3& vec) {
    return reinterpret_cast<const vec3&>(vec);
}
static vec3& get(Vector3& vec) { return reinterpret_cast<vec3&>(vec); }

Matrix Matrix::identity() {
    Matrix result;
    get(result) = glm::identity<mat4>();
    return result;
}

Matrix Matrix::projection(float fov, float aspect_ratio, float far,
                          float near) {
    Matrix result;
    get(result) = glm::perspective(fov, aspect_ratio, near, far);
    return result;
}

Matrix Matrix::translation(const Vector3& pos) {
    Matrix result;
    get(result) = translate(glm::identity<mat4>(), get(pos));
    return result;
}

Matrix Matrix::rotation(float roll, float pitch, float yaw) {
    auto q = Quaternion(pitch, yaw, roll);

    return rotation(q);
}

Matrix Matrix::rotation(const Quaternion& q) {
    Matrix result;
    get(result) = glm::mat4_cast(get(q));
    return result;
}

Matrix Matrix::scale(const Vector3& scale) {
    Matrix result;
    get(result) = glm::scale(glm::identity<mat4>(), get(scale));
    return result;
}

Matrix Matrix::view(const Vector3& position, const Vector3& forward,
                    const Vector3& up) {
    Matrix result;

    auto look_point = position + forward;
    get(result) = lookAt(get(position), get(look_point), get(up));
    return result;
}

Matrix Matrix::transpose() const {
    Matrix result;
    get(result) = glm::transpose(get(*this));
    return result;
}

Matrix Matrix::inverse() const {
    Matrix result;
    get(result) = glm::inverse(get(*this));
    return result;
}

Vector3 Matrix::getTranslation() const {
    Vector3 position;
    Vector3 scale;
    Quaternion orientation;

    decompose(position, scale, orientation);

    return position;
}

Quaternion Matrix::getRotation() const {
    Vector3 position;
    Vector3 scale;
    Quaternion orientation;

    decompose(position, scale, orientation);

    return orientation;
}

Vector3 Matrix::getScale() const {
    Vector3 position;
    Vector3 scale;
    Quaternion orientation;

    decompose(position, scale, orientation);

    return scale;
}

void Matrix::decompose(Vector3& position, Vector3& scale,
                       Quaternion& orientation) const {
    glm::vec3 skew;
    glm::vec4 perspective;

    bool decomposed = glm::decompose(get(*this), get(scale), get(orientation),
                                     get(position), skew, perspective);
}

Matrix operator*(const Matrix& a, const Matrix& b) {
    Matrix result;
    get(result) = get(a) * get(b);
    return result;
}