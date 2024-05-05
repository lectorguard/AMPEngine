#include "TaskController.h"

void amp::Task::AddUpdate()
{
	controller->AddUpdate(static_cast<Task*>(this));
	isUpdateActive = true;
}

void amp::Task::AddSharedUpdate()
{
	if (SharedCounter == 0)AddUpdate();
	++SharedCounter;
}

bool amp::Task::IsUpdateActive()
{
	return isUpdateActive;
}

void amp::Task::RemoveUpdate()
{
	controller->RemoveUpdate(static_cast<Task*>(this));
	isUpdateActive = false;
}

void amp::Task::RemoveSharedUpdate()
{
	--SharedCounter;
	if (SharedCounter == 0)RemoveUpdate();
	if (SharedCounter < 0)throw "Every Add Call needs also a Remove Call !!!";
}

void amp::Task::ResetUpdate()
{
	SharedCounter = 0;
	if (isUpdateActive)RemoveUpdate();
	isUpdateActive = false;
}
