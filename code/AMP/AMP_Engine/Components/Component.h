#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "functional"
#include "Text.h"
#include "list"

namespace amp
{

	class AMP_Engine;

	/// Component inside the AMP engine
	/// 
	/// @note You can inherit from this class to create a custom component
	/// @note You can create components inside your actors constructor 
	/// @note You can find a component creation example inside the Actor class
	/// @note Be aware that pointers to your component become invalidated after your actor is deleted
	/// @note When creating a custom component which loads an asset you can use the amp assetAttribMap to manage this asset 
	/// @see Attributes for further information about asset management
	/// ### Example CustomComponent
	/// ```cpp
	///	class CustomComponent : public amp::Component
	///	{
	///	public:
	///		CustomComponent(...) {};
	///		CustomComponent(CustomComponent&&) = delete;
	///		CustomComponent(const CustomComponent&) = delete;
	///		virtual ~Mesh();
	///
	///		void CustomComponentSetUp(...);
	///	protected:
	///		//You will get automatically an update callback as long the actor you are attached to is alive
	///		void update() override;
	///}
	/// ```
	class Component
	{
	public:
		using  callback_t = typename std::function<void()>;
		using type = typename std::list<callback_t>;
		using callbacksIter = typename type::iterator;
	
		Component(){};
		virtual ~Component(){};

		Component(Component&&) = delete;
		Component(const Component&) = delete;

		/// @name Transformation Methods
		/// @brief Method for transforming your component 
		/// @{
		/// Translates the component from the current transformation
		void translate(float x, float y, float z) {
			Transform = glm::translate(Transform, glm::vec3(x,y,z));
		}
		/// Translates the component from the current transformation
		void translate(glm::vec3 deltaLocation) {
			Transform = glm::translate(Transform, deltaLocation);
		}
		/// Translates the component from identity matrix
		void translateFromIdentity(glm::vec3 delta){
			Transform = glm::translate(glm::mat4(1.0f), delta) * Transform;
		}
		/// Rotates the component from the current transform
		void rotateEuler(float angle,float x, float y, float z) {
			Transform = glm::rotate(Transform, glm::radians(angle), glm::vec3(x, y, z));
		}
		/// Rotates the component from the current transform
		void rotateQuat(float w, float x, float y, float z) {
			Transform = Transform * glm::toMat4(glm::quat(w, x, y, z));
		}
		/// Rotates the component from the current transform
		void rotateQuat(glm::quat q) {
			Transform = Transform * glm::toMat4(q);
		}
		/// Scales the component from the current transform
		void scale(glm::vec3 newScale) {
			Transform = glm::scale(Transform, newScale);
			internScale = internScale * newScale;
		}
		/// @return the location of the component
		virtual glm::vec3 getLocation() {
			return glm::vec3(Transform[3][0], Transform[3][1], Transform[3][2]);
		}
		/// @return the current transformation
		glm::mat4 getTransform() { return Transform; };
		/// Sets btw. replaces the current transformation
		void setTransform(glm::mat4 t) { Transform = t; }
		/// @return the current scale
		glm::vec3 getScale(){
			return internScale;
		}
		/// @}

		/// @return the name of this component
		TEXT GetName() { return name; };
		
	protected:
		/// Here is the actual world transformation saved after multiplying the different relative transformations
		glm::mat4 AppliedTransform = glm::mat4(1.0f);

		/// This function must be overridden and will give you automatically an update per frame callback
		virtual void update() = 0;

		/// When the Actor where this component is attached to becomes activated, this function is called
		///
		/// @note This function is **not** called at the beginning when the component is created
		/// @note This function is just called when the activity state changes from deactivated to activated
		virtual void OnActivate(){};

		/// When the Actor where this component is attached to becomes deactivated, this function is called
		///
		/// @note This function is just called when the activity state changes from activated(default) to deactivated
		virtual void OnDeactivate() {};

		/// AMP engine ref
		amp::AMP_Engine* engine = nullptr;
	private:
		void applyTransform(glm::mat4 actorEntityTransform) {
			AppliedTransform = actorEntityTransform * Transform;
		};

		void initComponent(amp::AMP_Engine* engine) {
			this->engine = engine;
			OnActivate();
		}
		void setName(TEXT name) {
			this->name = name;
		}
		glm::mat4 Transform = glm::mat4(1.0f);
		// For Physiscs system, no scale is applied
		glm::vec3 internScale = glm::vec3(1.0f, 1.0f, 1.0f);
		TEXT name;
		friend class Scene;
		friend class Actor;
	};
}


