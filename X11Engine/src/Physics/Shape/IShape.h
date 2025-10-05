#ifndef I_SHAPE_H
#define I_SHAPE_H

class IShape {
   public:
    virtual void setTrigger() = 0;
    virtual void setPhysical() = 0;
};

#endif  // I_SHAPE_H