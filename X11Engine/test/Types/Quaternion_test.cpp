#include "Quaternion.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(QuaternionTest, FromEuler0) {
    Quaternion identity;
    // identity.w = 1.;
    // identity.x = 0.;
    // identity.y = 0.;
    // identity.z = 0.;

    ASSERT_EQ(Quaternion(0.f, 0.f, 0.f), identity);
}

TEST(QuaternionTest, FromRoll) {
    Quaternion roll;
    // roll.w = 0.;
    // roll.x = 1.;
    // roll.y = 0.;
    // roll.z = 0.;

    auto test = Quaternion(3.14f, 0.f, 0.f);

    // ASSERT_NEAR(roll.w, test.w, 0.001);
    // ASSERT_NEAR(roll.x, test.x, 0.001);
    // ASSERT_NEAR(roll.y, test.y, 0.001);
    // ASSERT_NEAR(roll.z, test.z, 0.001);
}

TEST(QuaternionTest, ToMatrix) {
    // Quaternion quat;
    // quat.w = 0.;
    // quat.x = 1.;
    // quat.y = 0.;
    // quat.z = 0.;

    // Matrix rotation = quat.toRotationMatrix();

    // Matrix expected = Matrix::identity();

    // expected.matrix[1][1] = -1;
    // expected.matrix[2][2] = -1;

    // ASSERT_EQ(rotation, expected);
}