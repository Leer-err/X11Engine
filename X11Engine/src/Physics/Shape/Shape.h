#ifndef SHAPE_H
#define SHAPE_H

#include "IShape.h"

namespace physx {
class PxShape;
};

class Shape : public IShape {
   public:
    Shape(physx::PxShape* shape);
    Shape(const Shape& shape);
    ~Shape();

    void setTrigger() override;
    void setPhysical() override;

    physx::PxShape* get() const;

   private:
    physx::PxShape* shape;
};

#endif  // SHAPE_H