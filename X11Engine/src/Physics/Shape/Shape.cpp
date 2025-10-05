#include "Shape.h"

#include "PxShape.h"

Shape::Shape(physx::PxShape* shape) : shape(shape) {
    shape->acquireReference();
}

Shape::Shape(const Shape& shape) {
    this->shape = shape.shape;

    this->shape->acquireReference();
}

Shape::~Shape() {
    if (shape == nullptr) return;
    shape->release();
}

void Shape::setPhysical() {
    shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
    shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
}

void Shape::setTrigger() {
    shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
    shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
}

physx::PxShape* Shape::get() const { return shape; }