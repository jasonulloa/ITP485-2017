#include "ITPEnginePCH.h"

LevelLoader::LevelLoader(Game& game)
	:mGame(game)
{
	SetupSpawnMaps();
}

void LevelLoader::SetupSpawnMaps()
{
	// Actor spawn map
	mActorSpawnMap.emplace("Actor", &Actor::SpawnWithProperties);
	mActorSpawnMap.emplace("KillVolume", &KillVolume::SpawnWithProperties);
	mActorSpawnMap.emplace("Player", &Player::SpawnWithProperties);

	// Component spawn map
	mCompSpawnMap.emplace("BoxComponent", 
		ComponentInfo(BoxComponent::StaticType(), &BoxComponent::CreateWithProperties));
	mCompSpawnMap.emplace("CameraComponent",
		ComponentInfo(CameraComponent::StaticType(), &CameraComponent::CreateWithProperties));
	mCompSpawnMap.emplace("CharacterMoveComponent",
		ComponentInfo(CharacterMoveComponent::StaticType(), &CharacterMoveComponent::CreateWithProperties));
	mCompSpawnMap.emplace("MeshComponent", 
		ComponentInfo(MeshComponent::StaticType(), &MeshComponent::CreateWithProperties));
	mCompSpawnMap.emplace("PointLightComponent",
		ComponentInfo(PointLightComponent::StaticType(), &PointLightComponent::CreateWithProperties));
	mCompSpawnMap.emplace("SkeletalMeshComponent",
		ComponentInfo(SkeletalMeshComponent::StaticType(), &SkeletalMeshComponent::CreateWithProperties));
}

void LevelLoader::Load(const char* fileName)
{
	std::ifstream file(fileName);

	if (!file.is_open())
	{
		SDL_Log("Level file %s not found", fileName);
		return;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Level file %s is not valid JSON", fileName);
		DbgAssert(false, "Level file is not valid JSON!");
		return;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 1)
	{
		SDL_Log("Level %s is not a know level file format", fileName);
		return;
	}

	// TODO Lab 3l
	// Step 1: Setup any overall world properties
	const rapidjson::Value &world = doc["world"];
	if (world.IsObject())
	{
		Vector3 ambient;
		if (GetVectorFromJSON(world, "ambientLight", ambient))
			mGame.GetRenderer().SetAmbientLight(ambient);
	}

	// TODO Lab 3l
	// Step 2: Setup the actors (and each of their components)
	const rapidjson::Value &actors = doc["actors"];
	if (actors.IsArray())
	{
		for (rapidjson::SizeType actorIndex = 0; actorIndex < actors.Size(); ++actorIndex)
		{
			const rapidjson::Value &actor = actors[actorIndex];
			ActorSpawnFunc spawnFcn = mActorSpawnMap[actor["type"].GetString()];
			ActorPtr actorPtr = spawnFcn(mGame, actor["properties"]);
			const rapidjson::Value &updatedComp = actor["updatedComponents"];
			if (updatedComp.IsArray())
			{
				for (rapidjson::SizeType compIndex = 0; compIndex < updatedComp.Size(); ++compIndex)
				{
					const rapidjson::Value &comp = updatedComp[compIndex];
					const ComponentInfo &info = mCompSpawnMap.find(comp["type"].GetString())->second;
					ComponentPtr compPtr = actorPtr->GetComponentFromType(info.mType);
					if (nullptr != compPtr)
					{
						compPtr->SetProperties(comp["properties"]);
					}
				}
			}
			const rapidjson::Value &newComp = actor["newComponents"];
			if (newComp.IsArray())
			{
				for (rapidjson::SizeType compIndex = 0; compIndex < newComp.Size(); ++compIndex)
				{
					const rapidjson::Value &comp = newComp[compIndex];
					const ComponentInfo &info = mCompSpawnMap.find(comp["type"].GetString())->second;
					Component::UpdateType updateType = Component::PreTick;
					if ("PostTick" == comp["update"].GetString())
						updateType = Component::PostTick;
					info.mFunc(*actorPtr, updateType, comp["properties"]);
					ComponentPtr compPtr = actorPtr->GetComponentFromType(info.mType);
					if (nullptr != compPtr)
					{
						compPtr->SetProperties(comp["properties"]);
					}
				}
			}
			actorPtr->BeginPlay();
		}
	}
}

// Global helper functions
bool GetFloatFromJSON(const rapidjson::Value& inObject, const char* inProperty, float& outFloat)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsDouble())
	{
		return false;
	}

	outFloat = property.GetDouble();
	return true;
}

bool GetIntFromJSON(const rapidjson::Value& inObject, const char* inProperty, int& outInt)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsInt())
	{
		return false;
	}

	outInt = property.GetInt();
	return true;
}

bool GetStringFromJSON(const rapidjson::Value& inObject, const char* inProperty, std::string& outStr)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsString())
	{
		return false;
	}

	outStr = property.GetString();
	return true;
}

bool GetBoolFromJSON(const rapidjson::Value& inObject, const char* inProperty, bool& outBool)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsBool())
	{
		return false;
	}

	outBool = property.GetBool();
	return true;
}

bool GetVectorFromJSON(const rapidjson::Value& inObject, const char* inProperty, Vector3& outVector)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsArray() || property.Size() != 3)
	{
		return false;
	}

	for (rapidjson::SizeType i = 0; i < 3; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outVector.x = property[0].GetDouble();
	outVector.y = property[1].GetDouble();
	outVector.z = property[2].GetDouble();

	return true;
}

bool GetQuaternionFromJSON(const rapidjson::Value& inObject, const char* inProperty, Quaternion& outQuat)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;

	for (rapidjson::SizeType i = 0; i < 4; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outQuat.x = property[0].GetDouble();
	outQuat.y = property[1].GetDouble();
	outQuat.z = property[2].GetDouble();
	outQuat.w = property[3].GetDouble();

	return true;
}
