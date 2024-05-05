#pragma once
#include "Actor.h"
#include "Components/Mesh.h"
#include "Components/DynamicMesh.h"
#include "Components/DynamicConcaveMesh.h"
#include "PickUpActor.h"
#include "GameUtility.h"
#include "../AssetLoader/Extern/json.hpp"



class Ground_Client : public amp::Actor
{

public:
	std::vector<amp::Mesh*> M_obstacles;
	std::vector<amp::DynamicConcaveMesh*> R_obstacles;
	//value must be devidable to the number of different mesh types
	const int numberOfObstacles = 12;
	//Translation that objects are on the Ground_Client 
	const float deltaYLoc = 14.0f;
	//Distance between obstacle rows
	const float rowDistance = 55.0f;

	TEXT processServerGroundInfo = TEXT("processServerGroundInfo"); //One Param: Ground_ClientInfo info

	const float transparency_boxes = 0.4f;

	Ground_Client() {
		for (int i = 0; i < numberOfObstacles; ++i) {
			std::string M_Text = "M_obstacle_" + std::to_string(i);
			M_obstacles.push_back(AddComponent<amp::Mesh>(TEXT(M_Text)));
		}
	}
	virtual ~Ground_Client() {};

	void Update() override;

	void AfterWindowStartup() override;

	void CalculateMaze();

	void SpawnMesh(int index, TEXT mesh, int meshType);

	void ProcessServerGroundInfo(GroundInfo info);

	void OnDrawTransparency(unsigned int ShaderProgram);

	void OnSetTransparent();

	void OnUnsetTransparent();
private:
	const TEXT transparency_shader = TEXT("Shader/Transparent_Mesh_Shader.glsl");
	float currentTransparency = 1.0f;

	const TEXT collisionShapes[3] = { TEXT("3DModels/Ground/Cereal.obj"),TEXT("3DModels/Ground/carCollider.obj"),TEXT("3DModels/Ground/BusCollider.obj") };
	const TEXT textures[3] = { TEXT("Textures/Ground/cereal.png"), TEXT("Textures/Ground/car.png"), TEXT("Textures/Ground/bus.png") };
};

