#pragma once

struct PointLight
{
	float constant;
	float lin;
	float quadratic;

	vector3 ambient;
	vector3 diffuse;
	vector3 specular;
};

struct DirLight
{
	vector3 direction;

	vector3 ambient;
	vector3 diffuse;
	vector3 specular;
};