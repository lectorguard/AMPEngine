#include "pch.h"
#include "../AssetLoader/GeneralAssetLoader.h"
#include "Animation.h"

amp::Animation::Animation() {

}

amp::Animation::~Animation()
{
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &TexCoordBuffer);
	glDeleteBuffers(1, &JointBuffer);
	glDeleteBuffers(1, &WeightBuffer);
	glDeleteTextures(1, &TextureBuffer);
	GLHelper::ErrorCheck();
}

void amp::Animation::SetUp(TEXT GBLPath, TEXT texPath /*= TEXT()*/) {	
	//Load Shader
	if (!engine)throw "Engine Pointer not set for Mesh Component";
	ShaderAttributes* shaderAttr = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(shaderPath.getHash())) {
		shaderAttr = (ShaderAttributes*)*result;
	}
	else
	{
		shaderAttr = engine->ampShader.getShaderProgramFromPath(shaderPath.getCharArray());
		engine->currentScene.AssetAttribMap.add(shaderPath.getHash(), shaderAttr);
	}
	this->ShaderProgram = shaderAttr->ShaderProgram;

	//Load Animation

	if (auto result = engine->currentScene.AssetAttribMap.tryFind(GBLPath.getHash())) {
		animAttr = (AnimationAttrib*)*result;
	}
	else
	{
		animAttr = engine->animLoader.loadAnimation(GBLPath.getCharArray(), texPath.getHash() == 0);
		engine->currentScene.AssetAttribMap.add(GBLPath.getHash(), animAttr);
	}

	TexAttributes* tex = nullptr;
	if (texPath.getHash() != 0)
	{
		if (auto result = engine->currentScene.AssetAttribMap.tryFind(texPath.getHash())) {
			tex = (TexAttributes*)*result;
		}
		else
		{
			tex = engine->assetLoader.loadTextureFromImage(texPath.getCharArray());
			engine->currentScene.AssetAttribMap.add(texPath.getHash(), tex);
		}
	}

	animAttr->mesh.matrix = glm::mat4(1.0);
	GLHelper::SetBuffer(VertexBuffer, animAttr->mesh.vertices.size(), animAttr->mesh.vertices.data());
	GLHelper::SetBuffer(TexCoordBuffer, animAttr->mesh.texCoords.size(), animAttr->mesh.texCoords.data());
	if (animAttr->mesh.joints.size() == 0)throw "Not an animation";
	GLHelper::SetBuffer(JointBuffer,animAttr->mesh.joints.size(),animAttr->mesh.joints.data());
	if (animAttr->mesh.weights.size() == 0)throw "Not an animation";
	GLHelper::SetBuffer(WeightBuffer, animAttr->mesh.weights.size(), animAttr->mesh.weights.data());

	//Anim Texture
	if (animAttr->texture.data.size() > 0 && !tex)TexID = GLHelper::SetAnimTexture(TextureBuffer, &animAttr->texture, ShaderProgram, "TextureSampler");
	else if (tex)TexID = GLHelper::SetTexture(TextureBuffer, tex, ShaderProgram, "TextureSampler");
	else throw "no texture error";
	VerticesLength = animAttr->mesh.indices.size();
}


void amp::Animation::update() {
	updateAnimation();
	draw();
}

void amp::Animation::draw()
{
	if (ShaderProgram == GL_NONE)throw "Animation has to be set Up before becoming updated";
	glUseProgram(ShaderProgram);

	GLHelper::SetUniformMatrix(ShaderProgram, "model", AppliedTransform * defaultRotation);
	GLHelper::SetUniformMatrix(ShaderProgram, "view", Camera::getView());
	GLHelper::SetUniformMatrix(ShaderProgram, "projection", Camera::getProjection());
	//Make sure all joint Matrices are given
	std::vector<float> jointArray;
	for (auto& mat : animAttr->mesh.jointMatrices)
	{
		float* arr = (float*)glm::value_ptr(mat);
		std::copy(&arr[0], &arr[16], std::back_inserter(jointArray));
	}
	GLHelper::SetUniformMatrixArray(ShaderProgram, "bone_matrix", MAX_NUMBER_JOINTS, jointArray);

	
	GLHelper::BindTexture(TextureBuffer, TexID);
	GLHelper::BindArrayBuffer(0, 3, VertexBuffer);
	GLHelper::BindArrayBuffer(1, 2, TexCoordBuffer);
	GLHelper::BindArrayBuffer(2, 4, JointBuffer,GL_UNSIGNED_INT);
	GLHelper::BindArrayBuffer(3, 4, WeightBuffer);
	
	glDrawArrays(GL_TRIANGLES, 0, VerticesLength);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	GLHelper::ErrorCheck();
}

void amp::Animation::updateAnimation()
{
	if (!animAttr)throw "The animation must be initialized first !!";
	float delta = static_cast<float>(engine->getFrameTime());
	if(!pause)animationTime += delta * animRate;
	bool updated = false;
	auto& animation = animAttr->Animations[AnimationIndex];
	float time = std::fmod(animationTime, animation.endTime - animation.startTime);

	for (auto& channel : animAttr->Animations[AnimationIndex].Channels) {
		auto& animSampler = animation.Samplers[channel.Sampler_ID];
		if (animSampler.inputTime.size() > animSampler.output.size()/4)continue;
		for (std::size_t i = 0; i < animSampler.inputTime.size() - 1; ++i) {
			if ((time >= animSampler.inputTime[i]) && (time <= animSampler.inputTime[i + 1])) {
				//Progress between 2 keyframes in Percent
				float u = std::max(0.0f, time - animSampler.inputTime[i]) / (animSampler.inputTime[i + 1] - animSampler.inputTime[i]);
				if(u <= 1.0f){
					float x = animSampler.output[4 * i];
					float y = animSampler.output[4 * i + 1];
					float z = animSampler.output[4 * i + 2];
					float w = animSampler.output[4 * i + 3];
					glm::vec4 out(x, y, z, w);
					float xN = animSampler.output[4 * (i + 1)];
					float yN = animSampler.output[4 * (i + 1) + 1];
					float zN = animSampler.output[4 * (i + 1) + 2];
					float wN = animSampler.output[4 * (i + 1) + 3];
					glm::vec4 outN(xN, yN, zN, wN);
					switch (channel.pathType)
					{
					case TRANSLATION:
					{
						glm::vec4 trans = glm::mix(out, outN, u);
						animAttr->Nodes[channel.node_ID].translate = glm::vec3(trans);
						break;
					}						
					case ROTATION:
					{
						glm::quat q1;
						q1.x = x;
						q1.y = y;
						q1.z = z;
						q1.w = w;
						glm::quat q2;
						q2.x = xN;
						q2.y = yN;
						q2.z = zN;
						q2.w = wN;
						animAttr->Nodes[channel.node_ID].rotate = glm::normalize(glm::slerp(q1, q2, u));
						break;
					}	
					case SCALE:
					{
						glm::vec4 trans = glm::mix(out, outN, u);
						animAttr->Nodes[channel.node_ID].scale = glm::vec3(trans);
						break;
					}
					default:
						break;
					}
					updated = true;
				}
			}
		}
	}
	if (updated) {
		updateNode(animAttr->StartNode);
	}
}

void amp::Animation::updateNode(int id)
{
	auto& node = animAttr->Nodes[id];
	if (node.meshID > -1) {
		auto& mesh = animAttr->mesh;
		mesh.matrix = getNodeMatrix(node.ID);
		if (node.skinID > -1) {
			auto& skin = animAttr->skin;
			auto inverseTransform = glm::inverse(mesh.matrix);
			unsigned int numJoints = std::min(static_cast<unsigned int>(skin.joints.size()),MAX_NUMBER_JOINTS);
			for (std::size_t i = 0; i < numJoints; ++i) {
				auto jointMat = getNodeMatrix(skin.joints[i]) * skin.inverseBindMatrices[i];
				jointMat = inverseTransform * jointMat;
				mesh.jointMatrices[i] = jointMat;
			}
			
		}
	}
	for (auto child : node.childs)
	{
		updateNode(child);
	}
}

glm::mat4 amp::Animation::getNodeMatrix(int nodeID)
{
	auto& node = animAttr->Nodes[nodeID];
	auto matrix = LocalMatrix(node);
	
	for (auto id = node.parentID; id != -1; id = animAttr->Nodes[id].parentID)
	{
		matrix = LocalMatrix(animAttr->Nodes[id]) * matrix;
	}
	return matrix;
}

glm::mat4 amp::Animation::LocalMatrix(Node& node)
{
	return glm::translate(glm::mat4(1.0f), node.translate) * glm::mat4(node.rotate) * glm::scale(glm::mat4(1.0f), node.scale) * node.matrix;
}
