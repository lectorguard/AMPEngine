#include "pch.h"
#include "Environment.h"

void Environment::AfterWindowStartup()
{
	M_ground->Mesh_SetUpGL(TEXT("3DModels/Ground_T_Fighter.obj"), TEXT("Textures/T_Fighter.png"));
	R_ground->SetUpFromMesh(M_ground, amp::PhysicType::StaticBody);

	M_background->Mesh_SetUpGL(TEXT("3DModels/T_Fighter.obj"), TEXT("Textures/T_Fighter.png"));
}

void Environment::Update()
{
}
