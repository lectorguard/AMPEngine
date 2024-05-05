#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "Components/DynamicConcaveMesh.h"
#include "Components/Mesh.h"

class Environment : public amp::Actor
{
public:
	amp::Mesh* M_ground = nullptr;
	amp::Mesh* M_background = nullptr; 
	amp::DynamicConcaveMesh* R_ground = nullptr;

	Environment(const Environment&) = delete;
	Environment(Environment&&) = delete;
	Environment(amp::Type type):Actor(type) {
		M_background = AddComponent<amp::Mesh>(TEXT("M_Background"));
		R_ground = AddComponent<amp::DynamicConcaveMesh>(TEXT("R_Ground"));
		M_ground = AddComponent<amp::Mesh>(TEXT("M_Ground"));
	};
	~Environment(){};

	void AfterWindowStartup() override;

	void Update() override;
};

