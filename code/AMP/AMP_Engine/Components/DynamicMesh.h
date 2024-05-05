#pragma once
#include "Components/Component.h"
#include "Components/Mesh.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "memory"
#include "../reactphysics3d/reactphysics3d.h"
#include "Physics/PhysicHelper.h"
#include "Text.h"

namespace amp{

	/// This is an AMP default component
	/// 
	/// With this component you can create convex rigid-bodies
	/// Use Case: Bottles, balls, chests, ...
	/// @note In most cases you want to use this component to create dynamic rigid-bodies which can interact with each other and also with DynamicConcaveMesh components
	/// @attention Make sure to call at first **one** of the following functions before calling other functions of this component (One per Component)
	/// @see void SetUpFromConvexMesh(), SetUpFromExtraCollissionShape(), SetUpBox(), SetUpSphere(), SetUpCapsule()
	class DynamicMesh : public Component
	{
	public:
		DynamicMesh() {};
		~DynamicMesh();
		DynamicMesh(const DynamicMesh&) = delete;
		DynamicMesh(DynamicMesh&&) = delete;

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
		/// @param mass The mass in kg changes the behavior of the simulated object
		void SetUpFromConvexMesh(Mesh* createdMesh,PhysicType type, float mass = 15.0f);
		
		/// When you want to setup a collision shape from a more complex mesh, you can use a simpler version of the mesh as collision shape instead 
		/// 
		/// @note The created Mesh must already exist
		/// @note Make sure your collisionShape is in local coordinates in your center
		/// @note Make sure the collisionShape you are using is triangulated correctly, otherwise an error could occur
		/// @note You can use a triangulate or re-mesh modifier (blender 2.83) or similar before exporting the mesh, to prevent errors
		/// @note Make sure the simplerMesh (collisionShape) is fully covering the (actual rendered)createdMesh for best results
		/// @note Do not scale the created Mesh or the rigid-body, because the collision shape will not be scaled (not supported feature in reactphysics)
		/// @param createdMesh The actual mesh which becomes displayed and is connected to the rigid-body
		/// @param collissionShapePath The path to the collision shape you want to use. Example to set the path correct Animation::SetUp()
		/// @param type Defines if your rigid-body can move, collide, interact or not
		/// @param mass The mass changes the behavior of the simulated rigid-body
		void SetUpFromExtraCollissionShape(Component* createdMesh, TEXT collissionShapePath, PhysicType type, float mass = 15.0f);

		/// Creates a rigid-body in shape of a box
		///
		/// @note You can scale the fitting mesh, but make sure the collision shape has the correct size
		/// @note Make sure your mesh is in local coordinates centered
		/// When you want to transform the connected mesh, transform instead this component by calling following functions
		/// @see applyForceToCenter(), applyForce(), applyTorque(), setTransform()
		/// @param fittingMesh The Mesh connected to the rigid-body, make sure the box fits your mesh
		/// @param type Defines if your rigid-body can move, collide, interact or not
		/// @param mass The mass in kg changes the behavior of the simulated object
		/// @param deltaToCenter When your Mesh is not at the center in local coordinates, you can set here a translation to center the mesh for correct physic simulation
		/// @param halfExtents The half size of x, y, z in meters
		void SetUpBox(Component* fittingMesh, PhysicType type,glm::vec3 halfExtents, float mass = 15.0f, glm::vec3 deltaToCenter = glm::vec3(0,0,0));
		
		/// Creates a rigid-body in shape of a sphere
		///
		/// @param fittingMesh The Mesh connected to the rigid-body, make sure the box fits your mesh
		/// @param type Defines if your rigid-body can move, collide, interact or not
		/// @param mass The mass in kg changes the behavior of the simulated object
		/// @param deltaToCenter When your Mesh is not at the center in local coordinates, you set here a translation to center the mesh for correct physic simulation
		/// @param radius The radius of the sphere in meters
		/// @see SetUpBox() for additional behavior information
		void SetUpSphere(Component* fittingMesh, PhysicType type, float radius, float mass = 15.0f, glm::vec3 deltaToCenter = glm::vec3(0,0,0));
		
		/// Creates a rigid-body in shape of a capsule
		///
		/// @param radius The radius of the cylinder
		/// @param height The height of the cylinder
		/// @param fittingMesh The Mesh connected to the rigid-body, make sure the box fits your mesh
		/// @param type Defines if your rigid-body can move, collide, interact or not
		/// @param mass The mass in kg changes the behavior of the simulated object
		/// @param deltaToCenter When your Mesh is not at the center in local coordinates, you set here a translation to center the mesh for correct physic simulation
		/// @param radius The radius of the sphere in meters
		/// @see SetUpBox() for additional behavior information
		void SetUpCapsule(Component* fittingMesh, PhysicType type, float radius, float height, float mass = 15.0f, glm::vec3 deltaToCenter = glm::vec3(0,0,0));
	
		/// @return The location of the rigid-body and the connect
		glm::vec3 getLocation() override;

		/// Sets the location of the rigid-body and the connected mesh
		void setLocation(glm::vec3 newLocation);

		/// @return The rotation of the rigid-body and the connected mesh
		glm::quat getRotation();

		/// Sets the roatation of the rigid-body and the connected mesh
		void setRotation(glm::quat newRotation);

		/// @return The reactphysics collision body
		rp3d::CollisionBody* getCollissionBody();

		/// @return The physic material where you can set physical behavior properties
		rp3d::Material& getPhysicMaterial();

		/// Sets the reactphysics physics material, which changes the rigid-body behavior
		void setPhysicMaterial(rp3d::Material mat);

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

		/// Set the linear damping factor. This is the ratio of the linear velocity that the body will lose every at seconds of simulation
		void setDamping(float factor);

		/// @return The linear damping factor. This is the ratio of the linear velocity that the body will lose every at seconds of simulation
		float getDamping();

		/// Applies a force to the center of your rigid-body
		void applyForceToCenter(glm::vec3 force);

		/// Applies a force to specific point inside you rigidbody
		void applyForce(rp3d::Vector3 force, rp3d::Vector3 point);

		/// Applies torque to your rigid-body
		void applyTorque(rp3d::Vector3 torque);

		/// Sets the transformation of your rigidbody and the connected mesh
		///
		/// @note This transformation is different from the transformation of your component
		/// @note You want to change this transformation and not the components transformation
		void setTransform(glm::mat4 transform, bool resetLinearVelocity = true, bool resetAngularVelocity = true);

		/// @return The transformation of your rigid-body and/btw. the transformation of the connected mesh
		glm::mat4 getTransform();

		/// @param velocity The angular velocity vector of the body
		void ResetAngularVelocity(glm::vec3 velocity = glm::vec3(0,0,0));

		/// @param velocity The linear velocity(current force) vector of the body
		void ResetLinearVelocity(glm::vec3 velocity = glm::vec3(0,0,0));

		/// @return The angular velocity vector of the body
		glm::vec3 getAngularVelocity();

		/// @return The linear velocity(current force) vector of the body
		glm::vec3 getLinearVelocity();

		/// Defines with which rigid-body your rigidbody can interact (collide) with
		/// 
		/// @param category This is a bit-mask. Each rigid-body can be just in **one** category. So flag just exactly **one** bit inside the mask. 
		/// @param category The default value is 0x0001, so all default rigid-bodies are in the first category
		/// @param canCollideWith This is also a bit-mask. But you can specify here as many categories as you want to collide with.
		/// @param canCollideWith Flagging the second and the third bit, would result in that this rigidbody can collide with all rigid-bodies in category 2 or 3.
		/// @param canCollideWith As long as all rigid-bodies in category 2 and 3 have a flag in their canCollideWith bit-mask of this rigidbody category. (e.g: 1). 
		/// @param canCollideWith The default value is 0xFFFF, so all default rigid-bodies can interact (collide) with each other. 
		/// @attention This function can just be called after your DynamicMesh is setup.
		void SetCollisionFilter(unsigned short category, unsigned short canCollideWith);

	protected:
		virtual void update() override;

		virtual void OnActivate() override;


		virtual void OnDeactivate() override;

	private:
		void CreateCollisionShapeFromMesh(MeshAttributes* meshAttr, float mass);

		void CreateRigidBody(Component* createdMesh, PhysicType type, glm::vec3 deltaToCenter);
		rp3d::RigidBody* rigidBody = nullptr;
		//have to be self organized
		rp3d::PolygonVertexArray* polygonVertexArray = nullptr;
		rp3d::PolygonVertexArray::PolygonFace* polygonFaces = nullptr;
		rp3d::PolyhedronMesh* polyhedronMesh = nullptr;
		rp3d::ConvexMeshShape* convexMeshShape = nullptr;
		rp3d::BoxShape* boxShape = nullptr;
		rp3d::SphereShape* sphereShape = nullptr;
		rp3d::CapsuleShape* capsuleShape = nullptr;
		rp3d::ProxyShape* proxyShape = nullptr;
		Component* connectedMesh = nullptr;
		rp3d::Transform previousTransform;
		glm::vec3 deltaToCenter  = glm::vec3(0.0f,0.0f,0.0f);
		bool isSetUp = false;
	};
}



