#pragma once
#include "Components/Component.h"
#include "Components/Mesh.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "memory"
#include "../reactphysics3d/reactphysics3d.h"
#include "Physics/PhysicHelper.h"

namespace amp
{
	/// This is an AMP default component
	/// 
	/// With this component you can create concave rigid-bodies
	/// Use Case: Different types of environment like a landscape, plane, ....
	/// @note In most cases you want to use this component to create static (not-movable) rigid-bodies
	/// @attention Rigid-bodies created with this component can **not** physically interact with each other, but they can physically interact with DynamicMesh components
	/// @attention Make sure to call at first one of the following functions before calling other functions of this component
	/// @see SetUpFromMesh(), SetUpWithCustomCollisionShape()
	class DynamicConcaveMesh : public Component
	{
	public:
		DynamicConcaveMesh() {};
	    ~DynamicConcaveMesh();
		DynamicConcaveMesh(const DynamicConcaveMesh&) = delete;
		DynamicConcaveMesh(DynamicConcaveMesh&&) = delete;
	
		/// Creates a rigid-body from a existing mesh in your scene
		///
		/// @note Make sure your mesh is in local coordinates in your center
		/// @note Make sure the mesh you are using is triangulated correctly, otherwise an error could occur
		/// @note You can use a triangulate or re-mesh modifier (blender 2.83) or similar before exporting the mesh, to prevent errors
		/// @note Do not scale the created Mesh or the rigid-body, because the collision shape will not be scaled (not supported feature in reactphysics)
		/// When you want to transform the connected mesh, transform instead this component by calling following functions
		/// @see applyForceToCenter(), applyForce(), applyTorque(), setTransform()
		/// @param createdMesh The Mesh connected to the rigid-body and the collision shape you want to use
		/// @param type Defines if your rigid-body can move, collide, interact or not
		void SetUpFromMesh(Mesh * createdMesh, PhysicType type);

		/// When you want to setup a collision shape from a more complex mesh, you can use a simpler version of the mesh as collision shape instead 
		/// 
		/// @note The created Mesh must already exist
		/// @note Make sure your collisionShape is in local coordinates in your center
		/// @note Make sure the collisionShape you are using is triangulated correctly, otherwise an error could occur
		/// @note You can use a triangulate or re-mesh modifier (blender 2.83) or similar before exporting the mesh, to prevent errors
		/// @note Make sure the simplerMesh (collisionShape) is fully covering the (actual rendered)createdMesh for best results
		/// @note Do not scale the created Mesh or the rigid-body, because the collision shape will not be scaled (not supported feature in reactphysics)
		/// @param createdMesh The actual mesh which becomes displayed and is connected to the rigid-body
		/// @param collisinShape The path to the collision shape you want to use. Example to set the path correct Animation::SetUp()
		/// @param type Defines if your rigid-body can move, collide, interact or not
		void SetUpWithCustomCollisionShape(Mesh* createdMesh, TEXT collisinShape, PhysicType type);

		
		/// Defines with which rigid-body your rigidbody can interact (collide) with
		/// 
		/// @param category This is a bit-mask. Each rigid-body can be just in **one** category. So flag just exactly **one** bit inside the mask. 
		/// @param category The default value is 0x0001, so all default rigid-bodies are in the first category
		/// @param canCollideWith This is also a bit-mask. But you can specify here as many categories as you want to collide with.
		/// @param canCollideWith Flagging the second and the third bit, would result in that this rigidbody can collide with all rigid-bodies in category 2 or 3.
		/// @param canCollideWith As long as all rigid-bodies in category 2 and 3 have a flag in their canCollideWith bit-mask of this rigidbody category. (e.g: 1). 
		/// @param canCollideWith The default value is 0xFFFF, so all default rigid-bodies can interact (collide) with each other. 
		/// @attention This function can just be called after your DynamicConcaveMesh is setup.
		/// @see SetUpFromMesh(), SetUpWithCustomCollisionShape()
		void SetCollisionFilter(unsigned short category, unsigned short canCollideWith);

		/// @return The physic material where you can set physical behavior properties
		rp3d::Material& getPhysicMaterial();

		/// Enables or disables gravity
		void setGravity(bool isGravity);

		/// @return True, when gravity enabled, else false.
		bool isGravity();


		/// Activates or deactivates the rigidbody
		///
		/// @note A deactivated rigidbody is ignored in the physic simulation
		void setActive(bool isActive);


		/// @return True, if rigid-body is active, else false
		bool isActive();

		/// Applies a force to the center of your rigid-body
		void applyForceToCenter(rp3d::Vector3 force);

		/// Applies a force to specific point inside you rigidbody
		void applyForce(rp3d::Vector3 force, rp3d::Vector3 point);

		/// Applies torque to your rigid-body
		void applyTorque(rp3d::Vector3 torque);

		/// Sets the transformation of your rigidbody and the connected mesh
		///
		/// @note This transformation is different from the transformation of your component
		/// @note You want to change this transformation and not the components transformation
		void setTransform(glm::mat4 transform);

		/// @return The transformation of your rigid-body and/btw. the transformation of the connected mesh
		glm::mat4 getTransform();

		/// @return The reactphysics collision-body
		rp3d::CollisionBody* getCollissionBody() {
			if (rigidbody)return rigidbody;
			else throw "init rigidbody first";
		}
	
	protected:
		virtual void update() override;


		virtual void OnActivate() override;


		virtual void OnDeactivate() override;

	private:
		Mesh* connectedMesh = nullptr;
		rp3d::TriangleVertexArray* triangleArray = nullptr;
		rp3d::TriangleMesh* triangleMesh = nullptr;
		rp3d::ConcaveMeshShape* concaveMesh = nullptr;
		rp3d::RigidBody* rigidbody = nullptr;
		rp3d::ProxyShape* proxyShape = nullptr;
		rp3d::Transform prevTrans = rp3d::Transform::identity();

	};
}

