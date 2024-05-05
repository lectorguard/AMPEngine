#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "Static/TimeManager.h"

namespace amp
{
	/// This is the AMP camera
	/// 
	/// There is just one camera inside amp, which is static
	/// You can change the camera everywhere inside your code
	/// The camera controls what is displayed on your device screen
	/// @note The camera contains the projecton matrix and the view matrix
	/// @note Projection matrix: Defines the field of view, the aspect ratio(screen width/height), the near clipping plane and the far clipping plane
	/// @note View matrix: Defines the camera transformation, This is done by its position, the up, the right and the forward vector of the camera
	struct Camera
	{
		/// @return The projection matrix of the camera
		static glm::mat4 getProjection() {
			return projection;
		}
	

		/// @param newProjection Can be set easily by calling glm::perspective(fieldOfView, aspectRation, nearClipping, farClipping)
		static void setProjection(glm::mat4 newProjection) {
			projection = newProjection;
		}
	
		/// Resets the current View Matrix
		/// 
		/// @note Reseted view is at origin and aligned to the open gl coordinate system
		static void resetView() {
			location = glm::vec3(0, 0, 0);
			up = glm::vec3(0, 1, 0);
			forward = glm::vec3(0, 0, -1);
		}
	
		/// @param newLocation Sets the new location of the camera
		static void setViewLocation(glm::vec3 newLocation) {
			location = newLocation;
		}
	
		/// @return The current location of the camera
		static glm::vec3 getLocation() {
			return location;
		}
	
		/// Translates the camera position
		/// 
		/// @param translation This is the delta location between you desired new location and the current location
		static void translateView(glm::vec3 translation) {
			location = location + translation;
		}
	
		/// Sets the rotation of the camera
		/// 
		/// @param newUp The new Up vector of the camera
		/// @param newForward The new forward vector of the camera
		/// @note The right vector is calculated with the cross product between newUp and newForward
		static void setViewRotation(glm::vec3 newUp, glm::vec3 newForward) {
			assert(glm::dot(newUp, newForward) == 0.0f);
			up = newUp;
			forward = newForward;
			up = glm::normalize(up);
			forward = glm::normalize(forward);
		}
	
		/// Rotates the camera
		/// 
		/// @param rotation The rotation is done by rotating the up, the forward and the left vector by the quaternion
		static void rotateView(glm::quat rotation) {
			up = glm::mat3_cast(rotation) * up;
			forward = glm::mat3_cast(rotation) * forward;
			float dotProduct = glm::dot(up, forward);
			assert(dotProduct <= 0.009f);
			up = glm::normalize(up);
			forward = glm::normalize(forward);
		}
	
		/// Rotates the camera
		/// 
		/// @param degrees The degrees you want to rotate the camera with
		/// @param axis The rotation axis vector, you want to rotate the camera around
		/// @note The rotation is done by rotating the up, the forward and the left vector by the calculated euler rotation matrix
		static void rotateView(float degrees, glm::vec3 axis) {
			glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0), glm::radians(degrees), axis);
			up = glm::mat3(rotMatrix) * up;
			forward = glm::mat3(rotMatrix) * forward;
			up = glm::normalize(up);
			forward = glm::normalize(forward);
		}
	
		/// @return The view matrix
		static glm::mat4 getView() {
			glm::mat4 resultMatrix = glm::lookAt(location, location + forward, up);
			return resultMatrix;
		}
	
		/// @return The view matrix in world space, can be used to attach actors or components relative to the camera position
		static glm::mat4 getWorldView() {
			return glm::inverse(getView());
		}
	
		/// @return The forward vector of the camera
		static glm::vec3 getForwardView() {
			return glm::normalize(forward);
		}
	
		/// @return The up vector of the camera
		static glm::vec3 getUpView() {
			return glm::normalize(up);
		}
	
		/// @return The right vector of the camera
		static glm::vec3 getRightView() {
			return glm::normalize(glm::cross(forward, up));
		}
	
	
	private:
		//default values are inside the cpp files
		static glm::vec3 forward;
		static glm::vec3 up;
		static glm::vec3 location;
	
		static glm::mat4 projection;
		friend class AMP_Engine;
	};
}