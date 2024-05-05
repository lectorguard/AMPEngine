#pragma once
#include "../reactphysics3d/reactphysics3d.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

namespace amp{
	
	/// Defines the different physic types of the rigid-bodies you can create inside amp
	enum PhysicType { StaticBody, KinematicBody, DynamicBody };

	/// This enum can be used to define collision filters inside the physics system
	enum Filter {
		F_0 = 0x0001, F_1 = 0x0002, F_2 = 0x0004
	};

	/// Creates a reactphysics3D transform matrix from a glm transformation matrix
	///
	/// @param mat The glm transformation matrix
	/// @return The same matrix as reactphysic3D transform matrix
	/// @attention The scale part of the matrix will be lost in the rp3d matrix (rp3d does not support scale)
	static rp3d::Transform GetReactTrans(glm::mat4 mat)
	{
		rp3d::Transform transform;
		transform.setFromOpenGL(glm::value_ptr(mat));
		return transform;
	}

	/// Creates a glm transformation matrix from a reactphysics3D transform matrix 
	///
	/// @param mat The rp3d transformation matrix
	/// @return The same matrix as glm transform matrix, but the scale part will always be (1,1,1)
	/// @attention reactphysics3D transformation matrices do not support scales other than (1,1,1)
	static glm::mat4 GetGLMTransform(rp3d::Transform transform)
	{
		float matrix[16];
		transform.getOpenGLMatrix(matrix);
		return glm::make_mat4(matrix);
	}
}
