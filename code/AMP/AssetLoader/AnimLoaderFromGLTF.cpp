#include "pch.h"
#include "AnimLoaderFromGLTF.h"

amp::AnimLoaderFromGLTF::AnimLoaderFromGLTF()
{
}

AnimationAttrib* amp::AnimLoaderFromGLTF::loadAnimation(const char* path, bool loadTex/*=false*/)
{
	if (manager == nullptr)throw "Asset Manager inside MeshLoaderFromObj is not set";
	
	tinygltf::Model model;
	loadGLB(model, path);
	

	AnimationAttrib* anim = new AnimationAttrib();
	for (auto& mat : anim->mesh.jointMatrices) {
		mat = glm::identity<glm::mat4>();
	}

	
	const auto& scene = model.scenes[model.defaultScene];
	anim->StartNode = scene.nodes[0];
	for (int i = 0; i < model.nodes.size(); ++i)anim->Nodes.push_back(Node());
	for (std::size_t i = 0; i < scene.nodes.size(); ++i) {
		LoadNodes(model, model.nodes[scene.nodes[i]], anim, scene.nodes[i]);
	}
	//set Mesh Matrix
	for (auto& node : anim->Nodes) {
		if (node.meshID > -1)anim->mesh.matrix = node.matrix;
	}

	SetIndices(model, anim);
	SetAttributes(model, anim);

	if(loadTex)LoadTexture(model, anim);

	for (int i = 0; i < model.animations.size(); ++i)
	{
		anim->Animations.push_back(Anim());
		SetAnimInputAndOutputValues(model, anim,i);
		SetAnimChannels(model, anim,i);
	}

	SetSkin(model, anim);
	anim->isUsedAtRuntime = true;
	return anim;
}

void amp::AnimLoaderFromGLTF::LoadNodes(tinygltf::Model& model,tinygltf::Node& gltf_Node, AnimationAttrib* anim, int ID, int parentID /*=-1*/)
{
	Node node;
	node.childs = gltf_Node.children;
	node.meshID = gltf_Node.mesh;
	node.skinID = gltf_Node.skin;
	node.name = gltf_Node.name;
	node.ID = ID;
	node.parentID = parentID;

	if (gltf_Node.matrix.size() == 16) {
		node.matrix = glm::make_mat4x4(gltf_Node.matrix.data());
	}
	if (gltf_Node.translation.size() == 3) {
		node.translate = glm::make_vec3(gltf_Node.translation.data());
	}
	if (gltf_Node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(gltf_Node.rotation.data());
		node.rotate = glm::mat4(q);
	}
	if (gltf_Node.scale.size() == 3) {
		node.scale = glm::make_vec3(gltf_Node.scale.data());
	}
	anim->Nodes[ID] = node;
	for(auto& currNode : gltf_Node.children)
	{
		LoadNodes(model, model.nodes[currNode], anim,currNode,ID);
	}
	//anim->Nodes.push_back(node);
}

void amp::AnimLoaderFromGLTF::loadGLB(tinygltf::Model& model, const char* path)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;
	bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
	if (!warn.empty())throw std::runtime_error("Warning happend when loading anim " + warn);
	if (!err.empty())throw std::runtime_error("Error happend when loading anim " + err);
	if (!ret) throw "Loading was unsuccessful";
}

void amp::AnimLoaderFromGLTF::SetSkin(tinygltf::Model& model, AnimationAttrib* anim)
{
	if (model.skins.size() == 0)return;
	if (model.skins.size() > 1)throw "At least one skin necessary";//Just one skin used at the moment
	const auto& gltfSkin = model.skins[0];
	Skin skin;
	skin.joints = gltfSkin.joints;
	skin.SkeletonID = gltfSkin.skeleton;

	if (gltfSkin.inverseBindMatrices > -1) {
		const auto& accessor = model.accessors[gltfSkin.inverseBindMatrices];
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];

		const unsigned char* dataAdress = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		const auto stride = accessor.ByteStride(bufferView);
		const auto count = accessor.count;

		if (accessor.type != TINYGLTF_TYPE_MAT4)throw "Invalid accessor type";
		skin.inverseBindMatrices.reserve(count);
		if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
			for (std::size_t i = 0; i < count; ++i)
			{
				auto mat = glm::make_mat4(reinterpret_cast<const float*>(dataAdress + i * stride));
				skin.inverseBindMatrices.emplace_back(mat);
			}
		}
		else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
			for (std::size_t i = 0; i < count; ++i)
			{
				float fmat[16];
				const double* dmat = reinterpret_cast<const double*>(dataAdress + i * stride);
				for (std::size_t i = 0; i < 16; ++i)fmat[i] = static_cast<float>(dmat[i]);
				skin.inverseBindMatrices.emplace_back(glm::make_mat4(fmat));
			}
		}
		else throw "accessor component type is invalid";
	}
	anim->skin = skin;
}

void amp::AnimLoaderFromGLTF::LoadTexture(tinygltf::Model& model, AnimationAttrib* anim)
{
	Texture tex;
	const auto& gltfTex = model.textures[0];
	{
		if (model.images.size() == 0)return; // NO existing Texture
		//maybe you need 1
		const auto& image = model.images[gltfTex.source];

		tex.width = image.width;
		tex.height = image.height;
		tex.components = image.component;
		tex.data = image.image;
	}

	{
		if (gltfTex.sampler > -1) {
			const auto& sampler = model.samplers[gltfTex.sampler];
			tex.MinFilter = sampler.minFilter;
			tex.MagFilter = sampler.magFilter;
			tex.wrapR = sampler.wrapR;
			tex.wrapS = sampler.wrapS;
			tex.wrapT = sampler.wrapT;
		}
	}

	anim->texture = tex;
}

void amp::AnimLoaderFromGLTF::SetAnimChannels(tinygltf::Model& model, AnimationAttrib* anim, int i)
{
	for (auto& chan : model.animations[i].channels) {
		Channel CurrChannel;
		CurrChannel.pathType = GetPathType(chan.target_path);
		CurrChannel.node_ID = chan.target_node;
		CurrChannel.Sampler_ID = chan.sampler;
		anim->Animations[i].Channels.push_back(CurrChannel);
	}
}

void amp::AnimLoaderFromGLTF::SetAnimInputAndOutputValues(tinygltf::Model& model, AnimationAttrib* anim, int i)
{
	if (model.animations.size() < 1)throw "No Animations";
	anim->Animations[i].name = model.animations[i].name;
	for (auto& sampler : model.animations[i].samplers) {
		Sampler AnimSampler;
		AnimSampler.interpType = getInterpType(sampler.interpolation);
		{
			const auto& accessor = model.accessors[sampler.input];
			const auto& bufferView = model.bufferViews[accessor.bufferView];
			const auto& buffer = model.buffers[bufferView.buffer];

			const unsigned char* dataAdress = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
			const auto stride = accessor.ByteStride(bufferView);
			const auto count = accessor.count;

			if (accessor.type != TINYGLTF_TYPE_SCALAR)throw "Invalid accessor type";
			AnimSampler.inputTime.reserve(count);
			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
				FillWithScalar<float>(AnimSampler.inputTime, count, [&](int i) {return dataAdress + i * stride; });
			}
			else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
				FillWithScalar<double>(AnimSampler.inputTime, count, [&](int i) {return dataAdress + i * stride; });
			}
			else throw "Invalid accessor component type";
		}

		//Probably last element is always startTime and first element is always endTime
		for (auto& time : AnimSampler.inputTime)
		{
			if (time < anim->Animations[i].startTime) {
				anim->Animations[i].startTime = time;
			}
			if (time > anim->Animations[i].endTime) {
				anim->Animations[i].endTime = time;
			}
		}

		//same for output
		{
			const auto& accessor = model.accessors[sampler.output];
			const auto& bufferView = model.bufferViews[accessor.bufferView];
			const auto& buffer = model.buffers[bufferView.buffer];

			const unsigned char* dataAdress = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
			const auto stride = accessor.ByteStride(bufferView);
			const auto count = accessor.count;

			AnimSampler.output.reserve(count * 4);
			if (accessor.type == TINYGLTF_TYPE_VEC3) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					FillWithVectors<float>(AnimSampler.output, 3, count,
						[&](int i, int k) {return dataAdress + k * sizeof(float) + i * stride; },
						[&]() {AnimSampler.output.emplace_back(0.0f); });
				}
				else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
					FillWithVectors<double>(AnimSampler.output, 3, count,
						[&](int i, int k) {return dataAdress + k * sizeof(double) + i * stride; },
						[&]() {AnimSampler.output.emplace_back(0.0f); });
				}
				else throw "accessor component type is invalid";
			}
			else if (accessor.type == TINYGLTF_TYPE_VEC4) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					FillWithVectors<float>(AnimSampler.output, 4, count,
						[&](int i, int k) {return dataAdress + k * sizeof(float) + i * stride; });
				}
				else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
					FillWithVectors<double>(AnimSampler.output, 4, count,
						[&](int i, int k) {return dataAdress + k * sizeof(double) + i * stride; });
				}
				else throw "accessor component type is invalid";
			}
			else throw "accessor type is invalid";
		}
		anim->Animations[i].Samplers.push_back(AnimSampler);
	}
}

void amp::AnimLoaderFromGLTF::SetAttributes(tinygltf::Model& model, AnimationAttrib* anim)
{
	auto& primitive = model.meshes[0].primitives[0];
	for (const auto& attribute : primitive.attributes)
	{
		const auto& accessor = model.accessors[attribute.second];
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];

		const auto dataAdress = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		const auto stride = accessor.ByteStride(bufferView);

		if (attribute.first == "JOINTS_0")
		{
			if (accessor.type != TINYGLTF_TYPE_VEC4)throw "Incompatible accessor type";
			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT || accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			{
				for (std::size_t i = 0; i < anim->mesh.indices.size(); ++i)
				{
					auto face = anim->mesh.indices[i];
					FillWithScalar<const short>(anim->mesh.joints,4,[&](int k) {return dataAdress + k * sizeof(short) + face * stride; });
				}
			}
			else throw "Incompatible accessor component type";
		}
		else
		{
			std::vector<float> currData;
			int VectorSize = NumberOfElementsInType(attribute.first, accessor.type);
			if (VectorSize == -1)continue;
			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			{
				for (std::size_t i = 0; i < anim->mesh.indices.size(); ++i)
				{
					auto face = anim->mesh.indices[i];
					FillWithScalar<const float>(currData, VectorSize, [&](int k) {return dataAdress + k * sizeof(float) + face * stride; });
				}
			}
			else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
			{
				for (std::size_t i = 0; i < anim->mesh.indices.size(); ++i)
				{
					auto face = anim->mesh.indices[i];
					FillWithScalar<const double>(currData, VectorSize, [&](int k) {return dataAdress + k * sizeof(double) + face * stride; });
				}
			}
			else throw "Incompatible component Vec 3 type";

			SetByType(anim, attribute.first, currData);
		}
	}
}

void amp::AnimLoaderFromGLTF::SetIndices(tinygltf::Model& model, AnimationAttrib* anim)
{
	if (model.meshes.size() < 1)throw "Mesh is invalid";
	if (model.meshes[0].primitives.size() < 1)throw "Just one primitive is allowed";

	//Just triangle mode is supported (primitive.mode must be 4)
	auto& primitive = model.meshes[0].primitives[0];
	//load indices
	anim->materialID = primitive.material;
	

	const auto& accessor = model.accessors[primitive.indices];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	const auto& buffer = model.buffers[bufferView.buffer];

	const auto dataAdress = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
	const auto stride = accessor.ByteStride(bufferView);
	const auto count = accessor.count;

	anim->mesh.indices.reserve(count);
	if (accessor.type != TINYGLTF_TYPE_SCALAR) throw std::runtime_error("Undefined component type");
	if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_BYTE || accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
	{
		FillWithScalar<const char>(anim->mesh.indices, count, [&](int i) {return dataAdress + i * stride; });
	}
	else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT || accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
	{
		FillWithScalar<const short>(anim->mesh.indices, count, [&](int i) {return dataAdress + i * stride; });
	}
	else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_INT || accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
	{
		FillWithScalar<const int>(anim->mesh.indices, count, [&](int i) {return dataAdress + i * stride; });
	}
	else { throw std::runtime_error("Undefined component type"); };

	if (primitive.mode != TINYGLTF_MODE_TRIANGLES)throw "Indices primitive mode must be triangle";
	if (anim->mesh.indices.size() == 0)throw "Indices could not be loaded";
}

// asset manager inside of tinygltf is set to extern


PathType amp::AnimLoaderFromGLTF::GetPathType(std::string type)
{
	if (type == "rotation")return PathType::ROTATION;
	else if (type == "translation")return PathType::TRANSLATION;
	else if (type == "scale")return PathType::SCALE;
	else if (type == "weights")return PathType::WEIGHTS;
	else throw "Unknown Path type";
}

int amp::AnimLoaderFromGLTF::NumberOfElementsInType(std::string type, int accessorType)
{
	if ((type == "POSITION" || type == "NORMAL") && accessorType == TINYGLTF_TYPE_VEC3)return 3;
	else if (type == "TEXCOORD_0" && accessorType == TINYGLTF_TYPE_VEC2)return 2;
	else if (type == "WEIGHTS_0" && accessorType == TINYGLTF_TYPE_VEC4)return 4;
	//else throw "Incompatible Type";
	return -1;
}

void amp::AnimLoaderFromGLTF::SetByType(AnimationAttrib* ptr, std::string type, std::vector<float>& data)
{
	if (type == "POSITION") { ptr->mesh.vertices = data; return; }
	else if (type == "NORMAL") { ptr->mesh.normals = data; return; }
	else if (type == "TEXCOORD_0") { ptr->mesh.texCoords = data; return; }
	else if (type == "WEIGHTS_0") { ptr->mesh.weights = data; return; }
	else throw "Incompatible type";
}

InterpType amp::AnimLoaderFromGLTF::getInterpType(std::string type)
{
	if (type == "LINEAR")return InterpType::LINEAR;
	else if (type == "STEP")return InterpType::STEP;
	else if (type == "CUBICSPLINE")return InterpType::CUBICSPLINE;
	throw "type does not exist";
}

namespace tinygltf {
	AAssetManager* asset_manager = nullptr;
}

void amp::AnimLoaderFromGLTF::setAssetManager(AAssetManager* manager)
{
	this->manager = manager;
	tinygltf::asset_manager = manager;
}
