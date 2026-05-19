#pragma once

class IScene {
   public:
    virtual void update(float deltaTime) = 0;
};