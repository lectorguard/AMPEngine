#include "pch.h"
#include "cstdlib"
#include "../glm/gtx/fast_square_root.hpp"
#include "Static/Pixel.h"
#include "Sound/SoundPool.h"
#include "IconTouch.h"

void IconTouch::Init()
{
	engine->events.subscribeEvent(StartIconTouch, &IconTouch::Start, this);
	engine->events.subscribeEvent(endIconTouch, &IconTouch::End, this);
	engine->events.subscribeEvent(IconTouchReset, &IconTouch::Reset, this);

	icontTouchSoundIndex[0] = engine->sound.load(TEXT("Sound/S_sip1.wav"));
	icontTouchSoundIndex[1] = engine->sound.load(TEXT("Sound/S_sip2.wav"));
	icontTouchSoundIndex[2] = engine->sound.load(TEXT("Sound/S_sip3.wav"));
	icontTouchSoundIndex[3] = engine->sound.load(TEXT("Sound/S_sip4.wav"));

}

void IconTouch::Start()
{
	using namespace amp;
	for (int i = 0; i < NumberOfIcons; ++i) {
		
		int x = std::rand() % PIX::XP(P_particleBoxSizeX) + PIX::XP(PBL_minX);
		int y = std::rand() % PIX::YP(P_particleBoxSizeY) + PIX::YP(PBL_minY);
		int iconSize = PIX::CM(P_iconSizeUniformCM);
		images->RenderImage(0, glm::vec2(x, y), iconSize, iconSize);
		particles.push_back(BeerParticles(i, glm::vec2(x, y), getRandomDirection(), iconSize));
	}
	button = amp::Button(PIX::P(P_touchBoxLocation), PIX::XP(P_touchBoxSizeX), PIX::YP(P_touchBoxSizeY));
	touch->AddButton(button, [&]() {OnButtonPressed(); });
	AddUpdate();
}

void IconTouch::Update()
{
	for (auto& particle : particles) {
		UpdateLocation(particle);
		IconKillCheck(particle);
		images->UpdateImageInfo(std::make_pair(0, particle.index), particle.size, particle.size, particle.location);
	}
}

void IconTouch::OnButtonPressed()
{
	for (auto& particle : particles) {
		glm::vec2 currTouch = touch->GetTouch();
		if (glm::fastDistance(currTouch, particle.location) < amp::PIX::CM(P_iconSizeUniformCM)) {
			particle.isFlaggedKill = true;
			int soundindex = icontTouchSoundIndex[std::rand() % 4];
			engine->sound.play(soundindex, 1.0f);
			engine->events.publishEvent(onTouchIcon);
		}
	}
}

void IconTouch::End()
{
	RemoveUpdate();
	images->RemoveAllImages();
	touch->RemoveButton(button);
	particles.clear();
}

void IconTouch::Reset()
{
	ResetUpdate();
	touch->RemoveButton(button);
	images->RemoveAllImages();
	particles.clear();
}



void IconTouch::IconKillCheck(BeerParticles& particle)
{
	if (particle.isFlaggedKill) {
		particle.size -= amp::PIX::CM(P_IconReductionInCM);
		if (particle.size <= 0) {
			particle.size = amp::PIX::CM(P_iconSizeUniformCM);
			particle.isFlaggedKill = false;
			particle.location = getRandomInvisibleLocation();
		}
	}

}

glm::vec2 IconTouch::getRandomInvisibleLocation()
{
	using namespace amp;
	int lowerY = PIX::YP(P_touchBoxLocation.y + (P_touchBoxSizeY / 2.0f));
	int upperY = PIX::YP(PBL_maxY);
	int delta = upperY - lowerY;
	int x = std::rand() % PIX::XP(P_particleBoxSizeX) + PIX::XP(PBL_minX);
	int y = std::rand() % delta + lowerY;
	return glm::vec2(x, y);
}

glm::vec2 IconTouch::getRandomDirection()
{
	//Random Number between 0 and 1
	float randomNumber = ((double)std::rand() / (RAND_MAX));
	float radians = randomNumber * glm::pi<float>() * 2.0f;
	auto resultVector = glm::vec2(glm::cos(radians), glm::sin(radians));
	return glm::normalize(resultVector);
}

void IconTouch::UpdateLocation(BeerParticles& particle)
{
	//Integration
	IntegrateParticlePos(particle);
	if (particle.location.x < amp::PIX::XP(PBL_minX))ReflectParticle(particle, glm::vec2(1, 0));
	else if(particle.location.x > amp::PIX::XP(PBL_maxX))ReflectParticle(particle, glm::vec2(-1, 0));
	else if(particle.location.y < amp::PIX::YP(PBL_minY))ReflectParticle(particle, glm::vec2(0, 1));
	else if(particle.location.y > amp::PIX::YP(PBL_maxY))ReflectParticle(particle, glm::vec2(0, -1));
}

void IconTouch::ReflectParticle(BeerParticles& particle, glm::vec2 normal)
{
	particle.direction = glm::reflect(particle.direction, normal);
	IntegrateParticlePos(particle);
}

void IconTouch::IntegrateParticlePos(BeerParticles& particle)
{
	particle.direction = RandomRotate(particle.direction);

	float iconSpeed = amp::PIX::XP(P_iconSpeedX) * engine->getFrameTime();
	if (iconSpeed > maxPixelsPerIntegration)iconSpeed = maxPixelsPerIntegration;
	particle.location = particle.location + particle.direction * iconSpeed;
}

glm::vec2 IconTouch::RandomRotate(glm::vec2 input)
{
	float randomRot = std::rand() % (RandomDirectionDegrees * 2) - RandomDirectionDegrees;
	float cs = glm::cos(glm::radians(randomRot));
	float sn = glm::sin(glm::radians(randomRot));
	float px = input.x * cs - input.y * sn;
	float py = input.x * sn + input.y * cs;
	return glm::normalize(glm::vec2(px, py));
}
