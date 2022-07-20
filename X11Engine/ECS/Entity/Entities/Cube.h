#pragma once
#include "Entity.h"

class Cube : public ECS::Entity<Cube>
{
public:
	Cube(EntityId id) : Entity(id) {}
};