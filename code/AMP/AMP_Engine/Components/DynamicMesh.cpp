#include "pch.h"
#include "DynamicMesh.h"



amp::DynamicMesh::~DynamicMesh()
{
	engine->physics.dynWorld.get()->destroyRigidBody(rigidBody);
	if (convexMeshShape)delete convexMeshShape;
	convexMeshShape = nullptr;
	if (boxShape)delete boxShape;
	boxShape = nullptr;
	if (sphereShape) delete sphereShape;
	sphereShape = nullptr;
	if (capsuleShape)delete capsuleShape;
	capsuleShape = nullptr;
	delete polyhedronMesh;
	polyhedronMesh = nullptr;
	delete polygonFaces;
	polygonFaces = nullptr;
	delete polygonVertexArray;
	polygonVertexArray = nullptr;
}

void amp::DynamicMesh::SetUpFromConvexMesh(Mesh* createdMesh,PhysicType type, float mass /*=15.0f*/)
{
	CreateRigidBody(createdMesh, type,glm::vec3(0.0f));
	MeshAttributes* meshAttr = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(createdMesh->MeshAttributesPath.getHash())) {
		meshAttr = (MeshAttributes*)*result;
	}
	else throw "Mesh has to be set up before the Dynamic Mesh";
	CreateCollisionShapeFromMesh(meshAttr, mass);
}

glm::vec3 amp::DynamicMesh::getLocation()
{
	if(rigidBody){
		auto loc = rigidBody->getTransform().getPosition();
		return glm::vec3(loc.x, loc.y, loc.z);
	}
	else throw "invalid rigidbody";
}

void amp::DynamicMesh::setLocation(glm::vec3 newLocation)
{
	if (rigidBody) {
		auto mat = rigidBody->getTransform();
		mat.setPosition(rp3d::Vector3(newLocation.x,newLocation.y,newLocation.z));
		rigidBody->setTransform(mat);
	}
	else throw "invalid rigidbody";
}

glm::quat amp::DynamicMesh::getRotation()
{
	if (rigidBody) {
		auto rot = rigidBody->getTransform().getOrientation();
		return glm::quat(rot.w,rot.x,rot.y,rot.z);
	}
	else throw "invalid rigidbody";
}

void amp::DynamicMesh::setRotation(glm::quat newRotation)
{
	if (rigidBody) {
		auto mat = rigidBody->getTransform();
		//Rotation must have length of 1
		assert(glm::length(newRotation) > 0.8f);
		mat.setOrientation(rp3d::Quaternion(newRotation.x,newRotation.y,newRotation.z,newRotation.w));
		rigidBody->setTransform(mat);
	}
	else throw "invalid rigidbody";
}

rp3d::CollisionBody* amp::DynamicMesh::getCollissionBody()
{
	if (rigidBody)return rigidBody;
	else throw "rigidbody must be initialized first";
}

void amp::DynamicMesh::SetUpFromExtraCollissionShape(Component* createdMesh, TEXT collissionShapePath, PhysicType type, float mass /*= 15.0f*/)
{
	if (!engine)throw "Engine Pointer not set for Dynamic Mesh Component";
	CreateRigidBody(createdMesh, type,glm::vec3(0.0f));
	MeshAttributes* meshAttr = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(collissionShapePath.getHash())) {
		meshAttr = (MeshAttributes*)*result;
	}
	else {
		meshAttr = engine->assetLoader.loadOBJMesh(collissionShapePath.getCharArray());
		engine->currentScene.AssetAttribMap.add(collissionShapePath.getHash(), meshAttr);
	}
	CreateCollisionShapeFromMesh(meshAttr, mass);
}

void amp::DynamicMesh::CreateCollisionShapeFromMesh(MeshAttributes* meshAttr, float mass)
{
	meshAttr->isUsedAtRuntime = true;
	int NumberOfFaces = meshAttr->originIndices.size() / meshAttr->originIndicesPerFace;
	auto* polygonFaces = new rp3d::PolygonVertexArray::PolygonFace[NumberOfFaces];
	auto* faces = polygonFaces;
	for (int f = 0; f < NumberOfFaces; f++) {

		// First vertex of the face in the indices array 
		faces->indexBase = f * meshAttr->originIndicesPerFace;

		// Number of polygonVertexArray in the face 
		faces->nbVertices = meshAttr->originIndicesPerFace;

		faces++;
	}
	int NumberOfVertices = meshAttr->originVertices.size() / 3;

	// Create the polygon vertex array 
	polygonVertexArray = new rp3d::PolygonVertexArray(NumberOfVertices, meshAttr->originVertices.data(), 3 * sizeof(float),
		meshAttr->originIndices.data(), sizeof(int), NumberOfFaces, polygonFaces,
		rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

	// Create the polyhedronMesh mesh 
	polyhedronMesh = new rp3d::PolyhedronMesh(polygonVertexArray);

	// Create the convex mesh collision shape 
	convexMeshShape = new rp3d::ConvexMeshShape(polyhedronMesh);

	proxyShape = rigidBody->addCollisionShape(convexMeshShape, rp3d::Transform::identity(), rp3d::decimal(mass));

	previousTransform = rp3d::Transform::identity();
}

void amp::DynamicMesh::SetCollisionFilter(unsigned short category, unsigned short canCollideWith)
{
	assert(proxyShape);
	proxyShape->setCollisionCategoryBits(category);
	proxyShape->setCollideWithMaskBits(canCollideWith);
}

void amp::DynamicMesh::CreateRigidBody(Component* createdMesh, PhysicType type, glm::vec3 deltaToCenter)
{
	if (!createdMesh)throw "Mesh doesnt exist anymore";
	connectedMesh = createdMesh;
	if (isSetUp)throw "You can setup a dynamic body just once";
	isSetUp = true;
	this->deltaToCenter = deltaToCenter;
	//ignoring scale and rotation
	auto MeshTrans = glm::translate(glm::mat4(1.0f), deltaToCenter) * createdMesh->getTransform();
	rigidBody = engine->physics.dynWorld.get()->createRigidBody(GetReactTrans(MeshTrans));
	rigidBody->setType(rp3d::BodyType(type));
}

void amp::DynamicMesh::SetUpBox(Component* fittingMesh, PhysicType type, glm::vec3 halfExtents, float mass /*= 15.0f*/, glm::vec3 deltaToCenter /*= glm::vec3(0,0,0)*/)
{
	CreateRigidBody(fittingMesh, type,deltaToCenter);
	float* vec = glm::value_ptr(halfExtents);
	boxShape = new rp3d::BoxShape(rp3d::Vector3(vec[0],vec[1],vec[2]));
	proxyShape = rigidBody->addCollisionShape(boxShape, rp3d::Transform::identity(), rp3d::decimal(mass));
	previousTransform = rp3d::Transform::identity();
}

void amp::DynamicMesh::SetUpSphere(Component* fittingMesh, PhysicType type, float radius, float mass /*= 15.0f*/, glm::vec3 deltaToCenter /*= glm::vec3(0,0,0)*/)
{
	CreateRigidBody(fittingMesh, type,deltaToCenter);
	sphereShape = new rp3d::SphereShape(radius);
	proxyShape = rigidBody->addCollisionShape(sphereShape, rp3d::Transform::identity(), rp3d::decimal(mass));
	previousTransform = rp3d::Transform::identity();
}

void amp::DynamicMesh::SetUpCapsule(Component* fittingMesh, PhysicType type, float radius, float height, float mass /*= 15.0f*/, glm::vec3 deltaToCenter /*= glm::vec3(0,0,0)*/)
{
	CreateRigidBody(fittingMesh, type,deltaToCenter);
	capsuleShape = new rp3d::CapsuleShape(radius, height);
	proxyShape = rigidBody->addCollisionShape(capsuleShape, rp3d::Transform::identity(), rp3d::decimal(mass));
	previousTransform = rp3d::Transform::identity();
}

rp3d::Material& amp::DynamicMesh::getPhysicMaterial()
{
	return rigidBody->getMaterial();
}


void amp::DynamicMesh::setPhysicMaterial(rp3d::Material mat)
{
	return rigidBody->setMaterial(mat);
}

void amp::DynamicMesh::setGravity(bool isGravity)
{
	rigidBody->enableGravity(isGravity);
}

bool amp::DynamicMesh::isGravity()
{
	return rigidBody->isGravityEnabled();
}

void amp::DynamicMesh::setActive(bool isActive)
{
	if(rigidBody)rigidBody->setIsActive(isActive);
}

bool amp::DynamicMesh::isActive()
{
	return rigidBody->isActive();
}

float amp::DynamicMesh::getDamping()
{
	if (rigidBody)
	{
		return rigidBody->getLinearDamping();
	}
	return 0;
}

void amp::DynamicMesh::setDamping(float factor)
{
	if (rigidBody) {
		rigidBody->setLinearDamping(factor);
	}
}

void amp::DynamicMesh::applyForceToCenter(glm::vec3 force)
{
	rigidBody->applyForceToCenterOfMass(rp3d::Vector3(force.x,force.y,force.z));
}

void amp::DynamicMesh::applyForce(rp3d::Vector3 force, rp3d::Vector3 point)
{
	rigidBody->applyForce(force, point);
}

void amp::DynamicMesh::applyTorque(rp3d::Vector3 torque)
{
	rigidBody->applyTorque(torque);
}

glm::mat4 amp::DynamicMesh::getTransform()
{
	return GetGLMTransform(rigidBody->getTransform());
}

void amp::DynamicMesh::setTransform(glm::mat4 transform, bool resetLinearVelocity, bool resetAngularVelocity)
{
	rigidBody->setTransform(GetReactTrans(transform));
	if (resetLinearVelocity)ResetLinearVelocity();
	if (resetAngularVelocity)ResetAngularVelocity();

}

void amp::DynamicMesh::ResetAngularVelocity(glm::vec3 velocity)
{
	rigidBody->setAngularVelocity(rp3d::Vector3(velocity.x,velocity.y,velocity.z));
}

void amp::DynamicMesh::ResetLinearVelocity(glm::vec3 velocity)
{
	if(rigidBody)rigidBody->setLinearVelocity(rp3d::Vector3(velocity.x, velocity.y, velocity.z));
}

glm::vec3 amp::DynamicMesh::getAngularVelocity()
{
	if (!rigidBody)throw "rigidbody has to be initialized first, call setUp rigidbody";
	auto vector = rigidBody->getAngularVelocity();
	return glm::vec3(vector.x, vector.y, vector.z);
}

glm::vec3 amp::DynamicMesh::getLinearVelocity()
{
	if (!rigidBody)throw "rigidbody has to be initialized first, call setUp rigidbody";
	auto vector = rigidBody->getLinearVelocity();
	return glm::vec3(vector.x, vector.y, vector.z);
}

void amp::DynamicMesh::update()
{
	if (!connectedMesh)throw "connected Mesh is invalid, maybe you forgot to init this component";
	if (!engine)throw "engine was not initialised";
	rp3d::Transform currTransform = rigidBody->getTransform();
 	// Compute the interpolated transform of the rigid body 
	rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(previousTransform, currTransform, engine->physics.factor);

	auto newTrans = GetGLMTransform(interpolatedTransform);
	newTrans = glm::translate(newTrans, -deltaToCenter);
	newTrans = glm::scale(newTrans, connectedMesh->getScale());
	connectedMesh->setTransform(newTrans);
	// rp3d discards the scale part, so the mesh becomes rescaled here
	//connectedMesh->setTransform(glm::scale(connectedMesh->getTransform(), connectedMesh->getScale()));

 	// Update the previous transform 
 	previousTransform = currTransform;
}

void amp::DynamicMesh::OnActivate()
{
	setActive(true);
}

void amp::DynamicMesh::OnDeactivate()
{
	setActive(false);
}
