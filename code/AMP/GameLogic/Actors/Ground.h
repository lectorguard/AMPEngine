#pragma once
#include "Actor.h"
#include "Components/Mesh.h"
#include "Components/DynamicMesh.h"
#include "Components/DynamicConcaveMesh.h"
#include "PickUpActor.h"
#include "GameUtility.h"
#include "../AssetLoader/Extern/json.hpp"


class Ground : public amp::Actor
{
	
public:
	std::vector<amp::Mesh*> M_obstacles;
	std::vector<amp::DynamicConcaveMesh*> R_obstacles;
	//value must be devidable to the number of different mesh types
	const int numberOfObstacles = 12;
	//Translation that objects are on the ground 
	const float deltaYLoc = 14.0f;
	//Distance between obstacle rows
	const float rowDistance = 55.0f;

	TEXT sendServerGroundInfo = TEXT("sendServerGroundInfo");
	TEXT processServerGroundInfo = TEXT("processServerGroundInfo"); //One Param: GroundInfo info

	const float transparency_Boxes = 0.4f;

	Ground() {
		for (int i = 0; i < numberOfObstacles; ++i) {
			std::string R_Text = "R_obstacle_" + std::to_string(i);
			std::string M_Text = "M_obstacle_" + std::to_string(i);
			R_obstacles.push_back(AddComponent<amp::DynamicConcaveMesh>(TEXT(R_Text)));
			M_obstacles.push_back(AddComponent<amp::Mesh>(TEXT(M_Text)));
		}
	}
	virtual ~Ground() {};

	void Update() override;

	void AfterWindowStartup() override;

	void CalculateMaze();

	void CreateGroundInfo();

	int CalculateObject(std::vector<int>& maxMeshNumber);

	void SpawnMesh(int index, TEXT mesh, glm::vec3 location, int meshNumber, TEXT texture, int column);

	glm::vec3 ValidatePath(int column, glm::vec3 location, int meshNumber);

	void ServerSendGroundInfo();

	GroundInfo gInfo;

	std::vector<std::pair<int,glm::mat4>> sortedInfo;

	void OnDrawTransparency(unsigned int ShaderProgram);

	void OnSetTransparent();

	void OnUnsetTransparent();
private:
	const TEXT transparency_shader = TEXT("Shader/Transparent_Mesh_Shader.glsl");
	float currentTransparency = 1.0f;

	const TEXT collisionShapes[3] = {TEXT("3DModels/Ground/Cereal.obj"),TEXT("3DModels/Ground/carCollider.obj"),TEXT("3DModels/Ground/BusCollider.obj")};
	const float Heights[3] = {1,8,8};

	int currentBlockedLanes = 0;

};

