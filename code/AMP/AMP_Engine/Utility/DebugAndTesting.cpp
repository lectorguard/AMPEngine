
#include "DebugAndTesting.h"
#include "Static/Camera.h"
#include "../glm/gtx/matrix_decompose.hpp"
#include "stdio.h"
#include "cmath"

void amp::DebugAndTesting::UpdateCameraTransform(Axis axis, float delta)
{
	glm::vec3 pos[3] = { glm::vec3(1,0,0),glm::vec3(0,1,0),glm::vec3(0,0,1) };
	glm::vec3 trans = pos[axis] * glm::vec3(delta);
	glm::vec3 rot = pos[(axis + 1) % 3] * glm::vec3(delta);

	switch (currentAct)
	{
	case Translate:
		amp::Camera::translateView(trans);
		break;
	case Rotate:
		if (axis == Axis::Y)rot = rot * glm::mat3(-1.0f);
		amp::Camera::rotateView(std::abs(delta), rot);
		break;
	case Scale:
		//Camera::setView(glm::scale(Camera::getView(), delta));
		break;
	default:
		break;
	}
	currentMatrix = amp::Camera::getWorldView();
}

void amp::DebugAndTesting::UpdateCurrentMatrix(Axis axis, float delta)
{
	if (isCamera)UpdateCameraTransform(axis, delta);
	else {
		assert(currentActor >= 0);
		if (currentComponent < 0)UpdateActorTransform(axis, -delta);
		else UpdateComponentTransform(axis, -delta);
	}
}

void amp::DebugAndTesting::UpdateActorTransform(Axis axis, float delta)
{
	glm::vec3 pos[3] = { glm::vec3(1,0,0),glm::vec3(0,1,0),glm::vec3(0,0,1) };
	glm::vec3 trans = pos[axis] * glm::vec3(delta);
	glm::vec3 rot = pos[(axis + 1) % 3] * glm::vec3(delta);
	auto* actor = engine->getSceneActor(currentActor);

	switch (currentAct)
	{
	case Translate:
		actor->TranslateActor(trans.x, trans.y, trans.z);
		break;
	case Rotate:
		actor->RotateActor(std::abs(delta), rot.x, rot.y, rot.z);
		break;
	case Scale:
		actor->ScaleActor(delta);
		break;
	default:
		break;
	}
	currentMatrix = actor->getTransform();
}

void amp::DebugAndTesting::UpdateComponentTransform(Axis axis, float delta)
{
	glm::vec3 pos[3] = { glm::vec3(1,0,0),glm::vec3(0,1,0),glm::vec3(0,0,1) };
	glm::vec3 trans = pos[axis] * glm::vec3(delta);
	glm::vec3 rot = pos[(axis + 1) % 3] * glm::vec3(delta);
	auto* comp = engine->getSceneActor(currentActor)->components[currentComponent];

	switch (currentAct)
	{
	case Translate:
		comp->translate(trans.x, trans.y, trans.z);
		break;
	case Rotate:
		comp->rotateEuler(std::abs(delta), rot.x, rot.y, rot.z);
		break;
	case Scale:
		comp->scale(glm::vec3(delta,delta,delta));
		break;
	default:
		break;
	}
	currentMatrix = comp->getTransform();
}

void amp::DebugAndTesting::SetCurrentMatrix()
{
	if (isCamera)currentMatrix = amp::Camera::getWorldView();
	else {
		assert(currentActor >= 0);
		if (currentComponent < 0)currentMatrix = engine->getSceneActor(currentActor)->getTransform();
		else currentMatrix = engine->getSceneActor(currentActor)->components[currentComponent]->getTransform();
	}
}

void amp::DebugAndTesting::OnActivate()
{
	engine->events.subscribeEvent(engine->postInit,&DebugAndTesting::PostInit,this);
}

void amp::DebugAndTesting::OnDeactivate()
{

}

void amp::DebugAndTesting::PostInit()
{
	DrawOutliner();
}

void amp::DebugAndTesting::DrawOutliner()
{
	int outlineIndex = 0;
	std::vector<amp::TextRenderInformation> TextOutliner;
	for (int i = 0; i < engine->getSceneCount(); i++)
	{
		AddOutlineActor(i, outlineIndex++, TextOutliner);
		if (currentActor == i) {
			auto* actor = engine->getSceneActor(i);
			for (auto& comp : actor->components) {
				AddOutlineComponent(comp, outlineIndex++, TextOutliner);
			}
		}
	}
	multiBut = amp::MultiButton(TextOutliner);
	gest->AddButton(multiBut, [&]() {OnMultiButton(); });
}

void amp::DebugAndTesting::SetCurrentActor(int actorIndex)
{
	if (actorIndex < multiBut.getLength()) {
		currentActor = actorIndex;
		currentComponent = -1;
		ReDrawOutliner();
		multiBut.SetPressed(rend, actorIndex, amp::Pressed);
	}
}

void amp::DebugAndTesting::OnCamera()
{
	if (isCamera)return;
	//multiBut.SetPressed(rend, -1, amp::Default);
	SetCurrentActor(-1);
	rend->updateTextColType(Camera, amp::Pressed);
	isCamera = true;
	SetCurrentMatrix();
}

void amp::DebugAndTesting::OnMenu(bool isFirst)
{
	if (isFirst){
		img->SetPressed(menu);
		if (isOutlinerDisplayed) {
			OnCamera();
			rend->deleteTextSequence(multiBut.renderInformation);
			gest->RemoveButton(multiBut);
			isOutlinerDisplayed = false;
		}
		else {
			DrawOutliner();
			isOutlinerDisplayed = true;
		}
	}
	else img->SetUnpressed();
}

void amp::DebugAndTesting::SetCurrentComponent(int actorIndex, int compIndex, int pressedIndex)
{
	int NumOfComp = engine->getSceneActor(actorIndex)->components.size();
	if (actorIndex < multiBut.getLength() && compIndex < NumOfComp) {

		multiBut.SetPressed(rend, pressedIndex, amp::Pressed);
		currentActor = actorIndex;
		currentComponent = compIndex;
	}
}

void amp::DebugAndTesting::UpdateOutlinerSelection(int index)
{
	if (index < 0)return;
	if (currentActor < 0) {
		SetCurrentActor(index);
		return;
	}
	int numOfComp = engine->getSceneActor(currentActor)->components.size();
	int startComp = currentActor;
	int endComp = currentActor + numOfComp;
	if (index <= startComp) {
		SetCurrentActor(index);
	}
	else if (index <= endComp) {
		int CompIndex = index - currentActor - 1;
		SetCurrentComponent(currentActor, CompIndex, index);
	}
	else {
		SetCurrentActor(index - numOfComp);
	}
}

void amp::DebugAndTesting::AddOutlineComponent(amp::Component* comp, int outlineIndex, std::vector<amp::TextRenderInformation>& TextOutliner)
{
	if (!comp->GetName().getHash())throw "All Scene Components must hava a unique name, please update the TEXT for the component";
	auto CompText = comp->GetName();
	auto compInfo = amp::TextRenderInformation(CompText.getString(), posXComp, posY - padding - outlineIndex * size, size);
	rend->renderText(compInfo);
	TextOutliner.push_back(compInfo);
}

void amp::DebugAndTesting::AddOutlineActor(int i, int outlineIndex, std::vector<amp::TextRenderInformation>& TextOutliner)
{
	auto* actor = engine->getSceneActor(i);
	if (!actor->GetName().getHash())throw "All Actors must have a unique name ! !";
	auto actorText = actor->GetName().getString();
	auto actInfo = amp::TextRenderInformation(actorText, posX, posY - padding - outlineIndex * size, size);
	TextOutliner.push_back(actInfo);
	rend->renderText(actInfo);
}

void amp::DebugAndTesting::OnMultiButton()
{
	int index = gest->GetTouchedIndex(multiBut);
	if (isCamera) {
		isCamera = false;
		rend->updateTextColType(Camera, amp::Default);
	}
	UpdateOutlinerSelection(index);
	SetCurrentMatrix();
}

void amp::DebugAndTesting::ReDrawOutliner()
{
	rend->deleteTextSequence(multiBut.renderInformation);
	gest->RemoveButton(multiBut);
	DrawOutliner();
}

void amp::DebugAndTesting::Forward()
{
	UpdateCurrentMatrix(Axis::Z, StepValue);
	img->SetPressed(forw);
}

void amp::DebugAndTesting::Backward()
{
	UpdateCurrentMatrix(Axis::Z, -StepValue);
	img->SetPressed(backw);
}

void amp::DebugAndTesting::Left()
{
	UpdateCurrentMatrix(Axis::X, StepValue);
	img->SetPressed(left);
}

void amp::DebugAndTesting::Right()
{
	UpdateCurrentMatrix(Axis::X, -StepValue);
	img->SetPressed(right);
}

void amp::DebugAndTesting::Up()
{
	UpdateCurrentMatrix(Axis::Y, -StepValue);
	img->SetPressed(top);
}

void amp::DebugAndTesting::Down()
{
	UpdateCurrentMatrix(Axis::Y, StepValue);
	img->SetPressed(bot);
}

void amp::DebugAndTesting::UpdateLocRotScale() {
	amp::TextRenderInformation infos[3] = { translate,rotate,scale };
	for(int i = 0; i <3; ++i){
		if (i == currentAct) {
			infos[i].SetColType(amp::ColType::Pressed);
			rend->updateTextSettings(infos[i]);
		}
		else 
		{
			infos[i].SetColType(amp::ColType::Default);
			rend->updateTextSettings(infos[i]);
		}
	}
}

void amp::DebugAndTesting::UpdateSensitivity()
{
	//timeout
	float percent = img->updateHorizontalSlider(sensitivity, gest->GetX());
	float sensVal;
	if (percent <= 0.5f) {
		sensVal = percent * 0.02 + 0.01;
	}
	else {
		sensVal = (percent - 0.5f)* 0.1 + 0.02;
	}
	gest->SetButtonTimeOut(sensVal);
}

void amp::DebugAndTesting::UpdateStep()
{
	float percent = img->updateHorizontalSlider(step, gest->GetX());
	StepValue = (percent + 0.5f) * (percent + 0.5f) * (percent + 0.5f);
}


void amp::DebugAndTesting::SetTranslate()
{
	currentAct = Action::Translate;
	UpdateLocRotScale();
}

void amp::DebugAndTesting::SetRotate()
{
	currentAct = Action::Rotate;
	UpdateLocRotScale();
}


void amp::DebugAndTesting::SetScale()
{
	currentAct = Action::Scale;
	UpdateLocRotScale();
}

std::string amp::DebugAndTesting::GetDecimals(float f, int decimals)
{
	std::string retString = std::to_string(f);
	int delimeters = retString.find(".");
	if (!decimals)return retString.substr(0, delimeters);
	return retString.substr(0, delimeters + decimals + 1);
}

void amp::DebugAndTesting::Update()
{
	RotateActor(1, 0, 1, 0);
	std::string location;
	std::string rotation;
	std::string scale;
	GetLocRotScaleAsString(currentMatrix,location,rotation,scale);
	rend->renderText("LOC: "+ location, 0, 1850, 33,0.001f);
	rend->renderText("ROT: " + rotation, 0, 1800, 33, 0.001f);
	rend->renderText("SCA: " + scale, 0, 1750, 33, 0.001f);

	rend->renderText("Sens. :" + GetDecimals(gest->GetButtonTimeOut(),4), 650, 320, 33,0.01f); //Time out
	rend->renderText("Step :" + GetDecimals(StepValue,2), 650, 120, 33,0.01f); //Step

	rend->renderText(GetDecimals(1.0f / engine->getFrameTime(),0), 110, 1200, 66, 0.001f);
}

void amp::DebugAndTesting::GetLocRotScaleAsString(glm::mat4 matrix, std::string& locS, std::string& rotS, std::string& scaleS)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, scale, rotation, translation, skew, perspective);
	locS = "X:" + GetDecimals(translation.x, 2) + " Y:" + GetDecimals(translation.y, 2) + " Z:" + GetDecimals(translation.z, 2);
	rotation = glm::conjugate(rotation);
	rotS = "W:" + GetDecimals(rotation.w, 2) + " X:" + GetDecimals(-rotation.x, 2) + " Y:" + GetDecimals(-rotation.y, 2) + " Z:" + GetDecimals(-rotation.z,2);
	scaleS = "X:" + GetDecimals(scale.x, 2) + " Y:" + GetDecimals(scale.y, 2) + " Z:" + GetDecimals(scale.z, 2);
}

void amp::DebugAndTesting::OnButton()
{
	rend->renderText(std::to_string(1 / engine->getFrameTime()), 110, 1200, 66, 1.0f);
}


void amp::DebugAndTesting::AfterWindowStartup()
{
	rend->setDefaultColor(1.0f, 0.0f, 0.0f);
	//rend->setAnimation(true);
	rend->setUpText();
	gest->SetUp();
	img->SetUp({ TEXT("Textures/EditorButtons/bot.png"), TEXT("Textures/EditorButtons/left.png"),
			TEXT("Textures/EditorButtons/forward.png"),TEXT("Textures/EditorButtons/right.png"),TEXT("Textures/EditorButtons/in.png"),
			TEXT("Textures/EditorButtons/out.png"),TEXT("Textures/Slider/slider.png"),TEXT("Textures/Slider/BoxForSlider.png"),
			TEXT("Textures/EditorButtons/menu.png")});
	bot	   = img->RenderImage(0, glm::vec2(200, 100), 100, 100);
	left   = img->RenderImage(1, glm::vec2(150, 200), 100, 100);
	top	   = img->RenderImage(2, glm::vec2(200, 300), 100, 100);
	right  = img->RenderImage(3, glm::vec2(250, 200), 100, 100);
	forw   = img->RenderImage(4, glm::vec2(450, 300), 100, 200);
	backw  = img->RenderImage(5, glm::vec2(450, 100), 100, 200);
	menu   = img->RenderImage(8, glm::vec2(1140, 1700), 120, 120);


	
	sensitivity = img->RenderHorizontalSlider(6,7, glm::vec2(900, 260), 500, 100);
	gest->AddButton(amp::Button(sensitivity), [&]() {UpdateSensitivity(); });

	step = img->RenderHorizontalSlider(6, 7, glm::vec2(900, 60), 500, 100);
	gest->AddButton(amp::Button(step), [&]() {UpdateStep(); });

	gest->AddButton(amp::Button(bot),  [&]() {Down(); });
	gest->AddButton(amp::Button(left), [&]() {Left(); });
	gest->AddButton(amp::Button(top),  [&]() {Up();   });
	gest->AddButton(amp::Button(right),[&]() {Right(); });
	gest->AddButton(amp::Button(forw), [&]() {Forward(); });
	gest->AddButton(amp::Button(backw),[&]() {Backward(); });
	gest->SetButtonTimeOut(0.02f);
	gest->AddButton(amp::SimpleButton(menu), [&](bool b) {OnMenu(b); });

	translate	= amp::TextRenderInformation(TEXT("T"), 10, 400, 100,amp::Pressed); 
	rotate		= amp::TextRenderInformation(TEXT("R"), 150, 400, 100);
	scale		= amp::TextRenderInformation(TEXT("S"), 290, 400, 100);
	Camera		= amp::TextRenderInformation(TEXT("CAMERA"), 650, 350, 66, amp::Pressed);

	rend->renderText(translate);
	rend->renderText(rotate);
	rend->renderText(scale);
	rend->renderText(Camera);

	gest->AddButton(amp::Button(translate), [&]() {SetTranslate(); });
	gest->AddButton(amp::Button(rotate), [&]() {SetRotate(); });
	gest->AddButton(amp::Button(scale), [&]() {SetScale(); });
	gest->AddButton(amp::Button(Camera), [&]() {OnCamera(); });
	currentMatrix = amp::Camera::getWorldView();
}


