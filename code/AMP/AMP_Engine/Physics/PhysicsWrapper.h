#pragma once
#include "../reactphysics3d/reactphysics3d.h"
#include "../reactphysics3d/collision/ContactManifold.h"
#include "../reactphysics3d/constraint/ContactPoint.h"
#include "memory"
#include "Event/EventSystem.h"
#include "Static/TimeManager.h"


namespace amp
{
	class AMP_Engine;

	/// This is the AMP wrapper around the reactphysics3D physics library
	/// 
	/// This class is used to create, update and to remove rp3d dynamic worlds
	class PhysicsWrapper
	{
	public:
	
		using unique_Dyn = std::unique_ptr<rp3d::DynamicsWorld>;
		using unique_Col = std::unique_ptr<rp3d::CollisionWorld>;
	
		PhysicsWrapper();
		PhysicsWrapper(const PhysicsWrapper& p) = delete;
		PhysicsWrapper(PhysicsWrapper&& p) = delete;
	
		/// Creates the dynamic world for rp3d rigid-bodies
		void createDynamicWorld();

		/// Destroys the dynamic world for rp3d rigid-bodies
		void destroyDynamicWorld();

		/// Updates the dynamic world for rp3d rigid-bodies
		///
		/// This function is called once per frame, when dynamic world is alive
		void integrateDynamicPhysics();
		

		/// @return The time interpolation factor of the last frame
		double getFactor() { return factor; };

		/// Sets the gravity of the dynamic world in m/s^2
		void SetGravity(float x, float y, float z);

		/// Sets the engine reference
		void SetEngine(AMP_Engine* engine);
	
	private:
		rp3d::Vector3 gravity = rp3d::Vector3(0.0f, -9.81f, 0.0f);
		double factor = 0.0;
		unique_Dyn dynWorld = nullptr;
		int64_t previousFrameTime = 0;
		int64_t accumulator = 0;
		const float timeStep = 1.0 / 60.0;
		const int64_t timeStepInNanos = (1.0 / (float)60.0) * 1000000000;
		std::unique_ptr<rp3d::EventListener> collissionDetector = nullptr;
		AMP_Engine* engine = nullptr;

		friend class DynamicMesh;
		friend class DynamicConcaveMesh;
	};
}

