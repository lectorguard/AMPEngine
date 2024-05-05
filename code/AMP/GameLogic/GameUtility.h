#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "vector"
#include "../AssetLoader/Extern/json.hpp"
#include "Tasks/CharacterTouch.h"

static void unflag(unsigned char& flagContainer, unsigned char flagToRemove) {
	flagContainer &= ~(flagToRemove);
}

static bool isFlagged(unsigned char valueToTest, unsigned char against) {
	return (valueToTest & against) == against;
}

static bool isFlaggedIndex(unsigned char valueToTest, unsigned char index) {
	return (valueToTest & (1<<index)) == (1<<index);
}

enum DefendingStages {
	WaitingForHit, RunToBottle, PickUpBottle, GoBackToStart, Finished, NotActive
};

enum AttackingStages {
	NoAttack = 0, AttackingCenterBottle = 1,  AttackDefendingPlayer = 2
};

enum GameStagesOffline {
	Attacker = 0, Defender = 1
};

enum SoundFlags {
	ROLL = 1 << 0, HIT = 1 << 1, FALLBACK = 1 << 2, JUMP = 1 << 3
};

struct CurrentState {

	CurrentState() {};
	glm::mat4 DefenderTransform = glm::mat4(1.0f);
	glm::mat4 AttackerTransform = glm::mat4(1.0f);
	glm::mat4 ThrowingBottle = glm::mat4(1.0f);
	glm::mat4 TargetBottle = glm::mat4(1.0f);
	glm::mat4 b0 = glm::mat4(1.0f);
	glm::mat4 b1 = glm::mat4(1.0f);
	glm::mat4 b2 = glm::mat4(1.0f);
	glm::mat4 b3 = glm::mat4(1.0f);
	glm::mat4 b4 = glm::mat4(1.0f);
	glm::mat4 spotlight = glm::mat4(1.0f);
	
	float progressClient = 0.0f;
	float progressServer = 0.0f;
	GameStagesOffline currentClientStage = Attacker;
	AttackingStages attackStage = AttackingStages::NoAttack;
	DefendingStages defendStage = DefendingStages::NotActive;
	int currentAnimation = 0;
	uint8_t soundflags = 0u;
	uint8_t inputFlags = 0u;

	std::vector<uint8_t> bsify() {
		nlohmann::json j;
		j["0"] = toVector(DefenderTransform);
		j["1"] = toVector(AttackerTransform);
		j["2"] = toVector(ThrowingBottle);
		j["3"] = toVector(TargetBottle);
		j["4"] = toVector(b0);
		j["5"] = toVector(b1);
		j["6"] = toVector(b2);
		j["7"] = toVector(b3);
		j["8"] = toVector(b4);
		j["9"] = progressClient;
		j["10"] = progressServer;
		j["11"] = currentClientStage;
		j["12"] = attackStage;
		j["13"] = defendStage;
		j["14"] = inputFlags;
		j["15"] = toVector(spotlight);
		j["16"] = currentAnimation;
		j["17"] = soundflags;
		return nlohmann::json::to_bson(j);
	};

	CurrentState(std::vector<uint8_t> fromBson) {
		if (fromBson.size() == 0)throw "No valid Data";
		nlohmann::json j = nlohmann::json::from_bson(fromBson);
		DefenderTransform = toMatrix(j["0"]);
		AttackerTransform = toMatrix(j["1"]);
		ThrowingBottle =	toMatrix(j["2"]);
		TargetBottle =		toMatrix(j["3"]);
		b0 =				toMatrix(j["4"]);
		b1 =				toMatrix(j["5"]);
		b2 =				toMatrix(j["6"]);
		b3 =				toMatrix(j["7"]);
		b4 =				toMatrix(j["8"]);
		progressClient =	j["9"];
		progressServer =	j["10"];
		currentClientStage =j["11"];
		attackStage =		j["12"];
		defendStage =		j["13"];
		inputFlags =		j["14"];
		spotlight =			toMatrix(j["15"]);
		currentAnimation =  j["16"];
		soundflags =		j["17"];
	}

	std::vector<float> toVector(glm::mat4 matrix) {
		float* curr = glm::value_ptr(matrix);
		return std::vector<float>(curr, curr + 16);
	}

	glm::mat4 toMatrix(std::vector<float> matrix){
		return glm::make_mat4(matrix.data());
	}

};


#ifndef SET
#define SET(var,elem,index)if(var != elem)flags |= uint8_t(1<<index);var = elem
#endif

struct CurrentInput {
	Move defendingMove_0 = Move::NoMove;
	glm::vec3 attackThrowForce_1 = glm::vec3(0,0,0);
	int touchedIcons_2 = 0;
	uint8_t soundflags = 0u;

	CurrentInput() {};

	std::vector<uint8_t> bsify() {
		nlohmann::json j;
		j["0"] = defendingMove_0;
		j["1"] = toVector(attackThrowForce_1);
		j["2"] = touchedIcons_2;
		j["3"] = soundflags;
		return nlohmann::json::to_bson(j);
	}

	CurrentInput(std::vector<uint8_t> bson) {
		nlohmann::json j = nlohmann::json::from_bson(bson);
		SET(defendingMove_0, j["0"], 0);
		SET(attackThrowForce_1, toVec3(j["1"]), 1);
		SET(touchedIcons_2, j["2"], 2);
		soundflags = j["3"];
	}

	std::vector<float> toVector(glm::vec3 vec) {
		float* curr = glm::value_ptr(vec);
		return std::vector<float>(curr, curr + 3);
	}

	glm::vec3 toVec3(std::vector<float> vec) {
		return glm::make_vec3(vec.data());
	}

	uint8_t getFlags(){
		return flags;
	}
private:
	uint8_t flags = 0u;
};

#undef SET


struct GroundInfo {

	using ground_t = std::vector<std::vector<float>>;

	std::vector<glm::mat4> groundTransformations;
	std::vector<int> groundTypes;

	GroundInfo() {};

	GroundInfo(std::vector<glm::mat4> info, std::vector<int> types) {
		groundTransformations = info;
		groundTypes = types;
	}

	GroundInfo(std::vector<uint8_t> bson) {
		if (bson.size() == 0)throw "invalid bson";
		nlohmann::json j = nlohmann::json::from_bson(bson);
		groundTransformations = makeGroundInfo(j["0"]);
		std::vector<int> temp = j["1"];
		groundTypes = temp;
	}

	std::vector<uint8_t> bsify() {
		nlohmann::json j;
		j["0"] = getGroundInfo(groundTransformations);
		j["1"] = groundTypes;
		return nlohmann::json::to_bson(j);
	}

	std::vector<glm::mat4> makeGroundInfo(ground_t info) {
		std::vector<glm::mat4> res;
		for (auto& elem : info) {
			res.push_back(glm::make_mat4(elem.data()));
		}
		return res;
	}

	ground_t getGroundInfo(std::vector<glm::mat4> info) {
		ground_t res;
		for (auto& elem : info) {
			float* mat = glm::value_ptr(elem);
			res.push_back(std::vector<float>(mat, mat + 16));
		};
		return res;
	}
};


