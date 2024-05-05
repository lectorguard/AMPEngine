#include "pch.h"
#include "cstdlib"
#include "algorithm"
#include "Ground.h"


void Ground::Update()
{

}

void Ground::AfterWindowStartup()
{
	engine->events.subscribeEvent(sendServerGroundInfo, &Ground::ServerSendGroundInfo, this);
	//Transparency Events
	engine->events.subscribeEvent(TEXT("startIconTouch"), &Ground::OnSetTransparent, this);
	engine->events.subscribeEvent(TEXT("IconTouchReset"), &Ground::OnUnsetTransparent, this);
	CalculateMaze();
}

void Ground::CalculateMaze()
{
	std::pair<TEXT, int> objects[3] = { std::pair(TEXT("3DModels/Ground/Cereal.obj"), 1),
									std::pair(TEXT("3DModels/Ground/car.obj"), 3),
									std::pair(TEXT("3DModels/Ground/Bus.obj"), 3) };
	TEXT textures[3] = { TEXT("Textures/Ground/cereal.png"), TEXT("Textures/Ground/car.png"), TEXT("Textures/Ground/bus.png") };

	int maxMeshes = (numberOfObstacles / 3);
	std::vector<int> maxMeshNumber = { maxMeshes, maxMeshes, maxMeshes};

	int createdObstacles = 0;
	int rowCounter = 0;
	int HalfNumberLanes = (G_LaneInfo::NumberOfLanesAssets - 1) / 2;
	// Z = 0 is origin
	float startZ = G_LaneInfo::laneDistanceAssets * HalfNumberLanes * -1.0f;
	while (createdObstacles < numberOfObstacles)
	{
		int column = 0;
		while (column < G_LaneInfo::NumberOfLanesAssets)
		{
			//Objects + 1 (the extra one is for no object)
			int object = CalculateObject(maxMeshNumber);
			if (object == 3)++column;
			else if (column + objects[object].second < G_LaneInfo::NumberOfLanesAssets + 2) {
				int centerOfObject = std::round(objects[object].second / 2.0f) - 1;
				float Zloc = startZ + (column + centerOfObject) * G_LaneInfo::laneDistanceAssets;
				float XLoc = G_LaneInfo::location.x + rowDistance * rowCounter;
				column += objects[object].second;
				if(XLoc >= 0.0f)SpawnMesh(createdObstacles++, objects[object].first, glm::vec3(0, 5000, 0), object, textures[object], column);
				else SpawnMesh(createdObstacles++, objects[object].first, glm::vec3(XLoc, Heights[object], Zloc), object, textures[object], column);
			}
			else {
				//Spawn mesh under the ground to make sure the number of objects and types stay the same
				SpawnMesh(createdObstacles++, objects[object].first, glm::vec3(0, 5000, 0), object, textures[object],column);
			}
			if (createdObstacles >= numberOfObstacles)break;
		}
		++rowCounter;
	}
	CreateGroundInfo();
}

void Ground::CreateGroundInfo()
{
	std::sort(sortedInfo.begin(), sortedInfo.end(), [](auto left, auto right) {return left.first < right.first; });
	for (auto& elem : sortedInfo) {
		gInfo.groundTypes.push_back(elem.first);
		gInfo.groundTransformations.push_back(elem.second);
	}
	sortedInfo.clear();
}

int Ground::CalculateObject(std::vector<int>& maxMeshNumber)
{
	bool invalid = true;
	int currRes = 0;
	while (invalid)
	{
		currRes = std::rand() % 4;
		if (currRes == 3)invalid = false;
		else if (maxMeshNumber[currRes] > 0) {
			--maxMeshNumber[currRes];
			invalid = false;
		}
	}
	return currRes;
}

void Ground::SpawnMesh(int index, TEXT mesh, glm::vec3 location, int meshNumber, TEXT texture, int column)
{
	location = ValidatePath(column, location, meshNumber);

	M_obstacles[index]->SetShaderPath(transparency_shader);
	M_obstacles[index]->SetDrawCallback([&](auto s) {OnDrawTransparency(s); });
	M_obstacles[index]->SetBlend(true);
	M_obstacles[index]->Mesh_SetUpGL(mesh, texture);
	M_obstacles[index]->translate(location.x,location.y,location.z);
	R_obstacles[index]->SetUpWithCustomCollisionShape(M_obstacles[index], collisionShapes[meshNumber], amp::PhysicType::StaticBody);
	R_obstacles[index]->SetCollisionFilter(amp::Filter::F_2,amp::Filter::F_0);
	sortedInfo.push_back(std::make_pair(meshNumber, M_obstacles[index]->getTransform()));
}

glm::vec3 Ground::ValidatePath(int column, glm::vec3 location, int meshNumber)
{
	if (column >= G_LaneInfo::NumberOfLanesAssets) {
		if (currentBlockedLanes >= G_LaneInfo::NumberOfLanesPlayer && meshNumber == 2)location = glm::vec3(0, 5000.0f, 0);
		currentBlockedLanes = 0;
	}
	else currentBlockedLanes += meshNumber==2? 3:meshNumber;	
	return location;
}

void Ground::ServerSendGroundInfo()
{
	engine->events.publishEvent(TEXT("setGroundInfo"), gInfo);
}

void Ground::OnDrawTransparency(unsigned int ShaderProgram)
{
	GLHelper::SetUniformFloat(ShaderProgram, "transparency", currentTransparency);
}

void Ground::OnSetTransparent()
{
	currentTransparency = transparency_Boxes;
}

void Ground::OnUnsetTransparent()
{
	currentTransparency = 1.0f;
}
