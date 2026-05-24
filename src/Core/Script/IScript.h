#pragma once

class IScript {
   public:
    virtual ~IScript() = default;

    virtual void update(float delta_time) {}
    virtual void preRender() {}
};