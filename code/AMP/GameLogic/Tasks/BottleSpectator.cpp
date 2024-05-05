#include "pch.h"
#include "BottleSpectator.h"

void BottleSpectator::Init()
{
	engine->events.subscribeEvent(addSpectatingMesh,&BottleSpectator::AddMeshSpectator,this);
	engine->events.subscribeEvent(stopBottleSpectator, &BottleSpectator::End, this);
	engine->events.subscribeEvent(BottleSpectatorReset, &BottleSpectator::Reset, this);
}

void BottleSpectator::AddMeshSpectator(amp::DynamicMesh* mesh, bool isBottle)
{
	assert(mesh && "Invalid mesh pointer");
	switch (meshCounter)
	{
	case 0:
		meshes[0] = mesh;
		if (isBottle)bottleIndex = 0;
		else characterIndex = 0;
		break;
	case 1:
		meshes[1] = mesh;
		assert(meshes[0] != meshes[1]);
		if (isBottle)bottleIndex = 1;
		else characterIndex = 1;
		AddUpdate();
		break;
	default:
		throw "mesh counter must be 0 or 1 !!!!!, you can add just 2 elements at maximum";
		break;
	}
	++meshCounter;
}

void BottleSpectator::Update()
{
	assert(meshes[0] && meshes[1]);
	auto loc1 = meshes[0]->getLocation();
	loc1.y = 0;
	auto loc2 = meshes[1]->getLocation();
	loc2.y = 0;
	engine->events.publishEvent(onBottleLoc, meshes[bottleIndex]->getLocation());
	if (glm::length(loc2 - loc1) < maxDistance) {
		End();
		engine->events.publishEvent(onMeshesClose);
	}
	else if (meshes[characterIndex]->getLocation().x > meshes[bottleIndex]->getLocation().x + maxDistanceBehindBottle) {
		engine->events.publishEvent(onCharacterBehineBottle);
	}
}	

void BottleSpectator::End()
{
	RemoveUpdate();
	meshes[0] = nullptr;
	meshes[1] = nullptr;
	meshCounter = 0;
}

void BottleSpectator::Reset()
{
	meshCounter = 0;
	bottleIndex = 0;
	characterIndex = 0;
	meshes[0] = nullptr;
	meshes[1] = nullptr;
	ResetUpdate();
}
