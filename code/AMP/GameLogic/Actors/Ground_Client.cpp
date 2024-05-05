#include "pch.h"
#include "cstdlib"
#include "Ground_Client.h"


void Ground_Client::Update()
{

}

void Ground_Client::AfterWindowStartup()
{
	//becomes not unsubscribed when not active !!
	engine->events.subscribeEvent(processServerGroundInfo, &Ground_Client::ProcessServerGroundInfo, this);
	//Transparency Events
	engine->events.subscribeEvent(TEXT("startIconTouch"), &Ground_Client::OnSetTransparent, this);
	engine->events.subscribeEvent(TEXT("IconTouchReset"), &Ground_Client::OnUnsetTransparent, this);
	CalculateMaze();
}

void Ground_Client::CalculateMaze()
{
	TEXT objects[3] = { TEXT("3DModels/Ground/Cereal.obj"),TEXT("3DModels/Ground/car.obj"),TEXT("3DModels/Ground/Bus.obj") };

	int maxMeshes = (numberOfObstacles / 3);
	for (int i = 0; i < 3; ++i) {
		for (int k = 0; k < maxMeshes; ++k) {
			SpawnMesh(k + i * maxMeshes, objects[i], i);
		}
	}
}

void Ground_Client::SpawnMesh(int index, TEXT mesh, int meshType)
{
	M_obstacles[index]->SetShaderPath(transparency_shader);
	M_obstacles[index]->SetDrawCallback([&](auto s) {OnDrawTransparency(s); });
	M_obstacles[index]->SetBlend(true);
	M_obstacles[index]->Mesh_SetUpGL(mesh, textures[meshType]);
}

void Ground_Client::ProcessServerGroundInfo(GroundInfo info)
{
	for (int i = 0; i < info.groundTransformations.size(); ++i) {
		M_obstacles[i]->setTransform(info.groundTransformations[i]);
	}
}

void Ground_Client::OnDrawTransparency(unsigned int ShaderProgram)
{
	GLHelper::SetUniformFloat(ShaderProgram, "transparency", currentTransparency);
}

void Ground_Client::OnSetTransparent()
{
	currentTransparency = transparency_boxes;
}

void Ground_Client::OnUnsetTransparent()
{
	currentTransparency = 1.0f;
}
