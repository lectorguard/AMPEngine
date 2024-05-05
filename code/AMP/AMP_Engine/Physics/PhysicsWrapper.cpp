#include "pch.h"
#include "AMP_Engine.h"
#include "PhysicsWrapper.h"
#include "utility"




struct CollissionDetector : public rp3d::EventListener
{
	amp::AMP_Engine* engine = nullptr;
public:
	CollissionDetector(amp::AMP_Engine* engine) {
		this->engine = engine;
	};

	virtual ~CollissionDetector() = default;

	virtual void newContact(const rp3d::CollisionCallback::CollisionCallbackInfo& collisionInfo) override {
		if (engine) {
			rp3d::Vector3 normal = collisionInfo.contactManifoldElements->getContactManifold()->getContactPoints()->getNormal();
			rp3d::Vector3 pointFirst = collisionInfo.contactManifoldElements->getContactManifold()->getContactPoints()->getLocalPointOnShape1();
			rp3d::Vector3 pointSecond = collisionInfo.contactManifoldElements->getContactManifold()->getContactPoints()->getLocalPointOnShape2();
			std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*> collissionBodies(collisionInfo.body1, collisionInfo.body2);
			std::pair<glm::vec3, glm::vec3> contactPoints(glm::vec3(pointFirst.x, pointFirst.y, pointFirst.z), glm::vec3(pointSecond.x, pointSecond.y, pointSecond.z));
			engine->events.publishEvent(engine->onCollission,collissionBodies,contactPoints, glm::vec3(normal.x,normal.y,normal.z));
		}
	}
};


void amp::PhysicsWrapper::integrateDynamicPhysics()
{
	if (!dynWorld.get())throw "A Dynamic World has to be created before the integration step";
	// Get the current system time 
	int64_t currentFrameTime = TIME::currentTimeInNanos();

	// Compute the time difference between the two frames 
	int64_t deltaTime = currentFrameTime - previousFrameTime;

	// Update the previous time 
	previousFrameTime = currentFrameTime;

	deltaTime = std::min(deltaTime, (int64_t)(10 * timeStepInNanos));

	// Add the time difference in the accumulator 
	accumulator += deltaTime;

	// While there is enough accumulated time to take 
	// one or several physics steps 
	int simulationCounter = 0;
	while (accumulator >= timeStepInNanos) {

		// Update the Dynamics world with a constant time step 
		dynWorld.get()->update(timeStep);

		// Decrease the accumulated time 
		accumulator -= timeStepInNanos;
		++simulationCounter;
	}

	// Compute the time interpolation factor 
	factor = accumulator / (double)timeStepInNanos;
}

void amp::PhysicsWrapper::SetGravity(float x, float y, float z)
{
	gravity = rp3d::Vector3(x, y, z);
	if (dynWorld) {
		dynWorld.get()->setGravity(gravity);
	}
}

void amp::PhysicsWrapper::SetEngine(AMP_Engine* engine)
{
	this->engine = engine;
}

amp::PhysicsWrapper::PhysicsWrapper()
{
}

void amp::PhysicsWrapper::createDynamicWorld()
{
	dynWorld = std::make_unique<rp3d::DynamicsWorld>(gravity);
	dynWorld.get()->setNbIterationsVelocitySolver(30);
	dynWorld.get()->setNbIterationsPositionSolver(15);
	collissionDetector = std::make_unique<CollissionDetector>(engine);
	dynWorld.get()->setEventListener(collissionDetector.get());
}

void amp::PhysicsWrapper::destroyDynamicWorld()
{
	dynWorld.reset(nullptr);
	collissionDetector.reset(nullptr);
}
