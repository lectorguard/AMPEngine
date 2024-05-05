#pragma once
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include "GLES3/gl3platform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "Component.h"
#include "../AssetLoader/AnimLoaderFromGLTF.h"
#include "Text.h"
#include "Static/Camera.h"
#include "Graphics/Shader.h"
#include "AMP_Engine.h"
#include "Graphics/GLHelper.h"
#include <cmath>


namespace amp
{
	/// This is an AMP default component
	///
	/// This component can be used to load and play gltf animations 
	class Animation : public Component
	{
	public:
		Animation();
		~Animation();
		Animation(Animation&&) = delete;
		Animation(const Animation&) = delete;

		/// Sets up a gltf animation
		/// 
		/// This function should be called inside the Actor::AfterWindowStartup() method
		/// You should call this function before calling any other other function inside this component
		/// @param GBLPath Relative path to your .glb file beginning after assets
		/// @param texPath Relative path to your .png, .jpg, ... file beginning after assets
		/// @param texPath The gltf format also support textures, so when leaving this parameter out, the gltf texture is used
		/// @note You can add your assets inside the Android/GamePackaging/assets folder, everything in this folder will be copied inside the app 
		/// ### Example function call inside the AfterWindowStartup method inside your actor
		/// ```cpp
		/// myAnimationComponent->SetUp(TEXT("Animation/myAwesomeAnimation.glb"), TEXT("Textures/myAwesomeAnimationTexture.png"));
		/// ```
		void SetUp(TEXT GBLPath, TEXT texPath = TEXT());

		/// Pause or un-pause the animation 
		///
		/// @note The animation is played and looped by default
		void SetPause(bool IsPause) { pause = IsPause; };

		/// Starts the currently selected animation from the beginning
		void ResetAnimation() { animationTime = 0.0f; };

		/// Slows down or fasten up the play rate of the animation
		///
		/// @param rate Values > 1.0f fasten up the animation and values < 1.0f slows the animation down
		void SetSpeed(float rate = 1.0f) { animRate = rate; };

		//TODO : Interpolate different Animations, maybe also at different Start index 
		/// Select the played animation
		///
		/// @param index With this value you can change the animation, the dafault value is 0
		/// @note The first animation will be automatically played after calling SetUp()
		/// @attention When the index >= Number of Animations an error is thrown
		void SetAnimation(int index) { AnimationIndex = index; };

		/// @return The animation index
		int GetAnimation() { return AnimationIndex; };

		/// You display your animation also with a custom shader, as an inspiration you can look at the default shader (assets/Shader/AnimationShader.glsl)
		void SetShaderPath(TEXT path) { shaderPath = path; };

		/// This function sets a default rotation for the animation
		///
		/// @param rotation Rotation as euler matrix
		/// @note This is helpful when your animation has a unpractical default rotation
		/// @note So after setting this variable, you can treat the animation and also the component as it would be "correctly" rotated
		void SetDefaultRotation(glm::mat4 rotation) { defaultRotation = rotation; };
	
	protected:
		void update() override;

	private:
		void draw();
		void updateAnimation();
		void updateNode(int id);
		glm::mat4 getNodeMatrix(int nodeID);
		glm::mat4 LocalMatrix(Node& node);

		TEXT shaderPath = TEXT("Shader/AnimationShader.glsl");
		AnimationAttrib* animAttr = nullptr;
		int VerticesLength = 0;
		float animationTime = 0.0f;
		bool pause = false;
		float animRate = 1.0f;
		int AnimationIndex = 0;

		unsigned int ShaderProgram = GL_NONE;
		unsigned int VertexBuffer = GL_NONE;
		unsigned int TexCoordBuffer = GL_NONE;
		unsigned int TexID = GL_NONE;
		unsigned int TextureBuffer = GL_NONE;
		//vec 4 vector
		unsigned int JointBuffer = GL_NONE;
		unsigned int WeightBuffer = GL_NONE;

		glm::mat4 defaultRotation = glm::mat4(1.0f);
	};
}

