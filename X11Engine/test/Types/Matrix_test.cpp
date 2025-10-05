#include "Matrix.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Vector4.h"

TEST(MatrixTest, IdentityMultiplication) {
    Matrix identity = Matrix::identity();
    Matrix translation = Matrix::identity();
    // translation.rows[3] = Vector4{1.0, 2.0, 3.0, 1.0};

    Matrix multiplied = identity * translation;

    ASSERT_EQ(multiplied, translation);
}

TEST(MatrixTest, TranslationScaleMultiplication) {
    Matrix scale = Matrix::identity();
    // scale.matrix[0][0] = 2;
    // scale.matrix[2][2] = 1.5;

    // Matrix translation = Matrix::identity();
    // translation.rows[3] = Vector4{1.0, 2.0, 3.0, 1.0};

    // Matrix multiplied = scale * translation;

    // Matrix expected = Matrix::identity();

    // expected.matrix[0][0] = 2;
    // expected.matrix[2][2] = 1.5;
    // expected.rows[3] = Vector4{1.0, 2.0, 3.0, 1.0};

    // ASSERT_EQ(multiplied, expected);
}