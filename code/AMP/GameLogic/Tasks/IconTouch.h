#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "Components/TouchGesture.h"
#include "../glm/gtc/constants.hpp"
#include "Components/Image.h"

struct BeerParticles {

	BeerParticles(int i, glm::vec2 loc, glm::vec2 dir, int particleSize) : index(i), location(loc), direction(dir), size(particleSize) {};
	int index;
	glm::vec2 location;
	glm::vec2 direction;
	int size = 100.0;
	bool isAlive = true;
	bool isFlaggedKill = false;
};

class IconTouch : public amp::Task
{
	amp::Image * images = nullptr;
	amp::TouchGesture* touch = nullptr;

	public:
	IconTouch(amp::Image* image, amp::TouchGesture* touch) {
		this->images = image;
		this->touch = touch;
	};
	virtual ~IconTouch() {};

	IconTouch(const IconTouch&) = delete;
	IconTouch(IconTouch&&) = delete;

	//EVENTS
	//INPUTS
	TEXT IconTouchReset = TEXT("IconTouchReset");		//No Params: RESET has always 0 params
	TEXT StartIconTouch = TEXT("startIconTouch");		//No Params
	TEXT endIconTouch = TEXT("endIconTouch");			//No Params
	//OUTPUT
	TEXT onTouchIcon = TEXT("onTouchIcon");				//NoParams


	//Frame Particles in Percent
	const glm::vec2 P_particleBoxLocation = glm::vec2(0.5f,0.99f);
	const float P_particleBoxSizeX = 1.33f;
	const float P_particleBoxSizeY = 0.625;
	

	//Frame Touch In PERCENT
	const glm::vec2 P_touchBoxLocation = glm::vec2(0.5f, 0.84f);
	const float P_touchBoxSizeX = 1.0f;
	const float P_touchBoxSizeY = 0.322f;

	//Single Icon
	const int RandomDirectionDegrees = 5;
	const int NumberOfIcons = 33;
	//In PERCENT
	const float P_iconSizeUniformCM = 1.0f;
	const float P_IconReductionInCM = 0.05f;
	//Icon Speed dependent on Screen Size X
	const float P_iconSpeedX = 0.14f;
	const float maxPixelsPerIntegration = 12.0f;
	

	virtual void Init() override;

	virtual void Start() override;

	virtual void Update() override;

	virtual void End() override;


	virtual void Reset() override;

	void OnButtonPressed();

	void IconKillCheck(BeerParticles& particle);

private:
	glm::vec2 getRandomInvisibleLocation();

	glm::vec2 getRandomDirection();

	std::vector<BeerParticles> particles;

	void UpdateLocation(BeerParticles& particle);

	void ReflectParticle(BeerParticles& particle, glm::vec2 normal);

	void IntegrateParticlePos(BeerParticles& particle);

	glm::vec2 RandomRotate(glm::vec2 input);

	//IN PERCENT
	float PBL_minX = P_particleBoxLocation.x - (P_particleBoxSizeX / 2.0f);
	float PBL_maxX = P_particleBoxLocation.x + (P_particleBoxSizeX / 2.0f);
	float PBL_minY = P_particleBoxLocation.y - (P_particleBoxSizeY / 2.0f);
	float PBL_maxY = P_particleBoxLocation.y + (P_particleBoxSizeY / 2.0f);
	
	amp::Button button;

	int icontTouchSoundIndex[4] = { 0 };
};