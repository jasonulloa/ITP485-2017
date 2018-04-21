#include "ITPEnginePCH.h"
#include <SDL/SDL_log.h>

Animation::Animation(class Game& game)
	:Asset(game)
{

}

bool Animation::Load(const char* fileName, class AssetCache* cache)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Animation %s", fileName);
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Animation %s is not valid json", fileName);
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itpanim" ||
		ver != 2)
	{
		SDL_Log("Animation %s unknown format", fileName);
		return false;
	}

	const rapidjson::Value& sequence = doc["sequence"];
	if (!sequence.IsObject())
	{
		SDL_Log("Animation %s doesn't have a sequence.", fileName);
		return false;
	}

	const rapidjson::Value& frames = sequence["frames"];
	const rapidjson::Value& length = sequence["length"];
	const rapidjson::Value& bonecount = sequence["bonecount"];

	if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint())
	{
		SDL_Log("Sequence %s has invalid frames, length, or bone count.", fileName);
		return false;
	}

	mNumFrames = frames.GetUint();
	mLength = length.GetDouble();
	mNumBones = bonecount.GetUint();

	mTracks.resize(mNumBones);

	const rapidjson::Value& tracks = sequence["tracks"];

	if (!tracks.IsArray())
	{
		SDL_Log("Sequence %s missing a tracks array.", fileName);
		return false;
	}

	for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
	{
		if (!tracks[i].IsObject())
		{
			SDL_Log("Animation %s: Track element %d is invalid.", fileName, i);
			return false;
		}

		size_t boneIndex = tracks[i]["bone"].GetUint();

		const rapidjson::Value& transforms = tracks[i]["transforms"];
		if (!transforms.IsArray())
		{
			SDL_Log("Animation %s: Track element %d is missing transforms.", fileName, i);
			return false;
		}

		BoneTransform temp;

		if (transforms.Size() != mNumFrames)
		{
			SDL_Log("Animation %s: Track element %d has fewer frames than expected.", fileName, i);
			return false;
		}

		for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
		{
			const rapidjson::Value& rot = transforms[j]["rot"];
			const rapidjson::Value& trans = transforms[j]["trans"];

			if (!rot.IsArray() || !trans.IsArray())
			{
				SDL_Log("Skeleton %s: Bone %d is invalid.", fileName, i);
				return false;
			}

			temp.mRotation.x = rot[0].GetDouble();
			temp.mRotation.y = rot[1].GetDouble();
			temp.mRotation.z = rot[2].GetDouble();
			temp.mRotation.w = rot[3].GetDouble();

			temp.mTranslation.x = trans[0].GetDouble();
			temp.mTranslation.y = trans[1].GetDouble();
			temp.mTranslation.z = trans[2].GetDouble();

			mTracks[boneIndex].emplace_back(temp);
		}
	}

	return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, SkeletonPtr inSkeleton, float inTime)
{
	int fromKey = 0;
	int toKey = 0;
	float lerp = 0.0f;
	if (inTime > mLength)
	{
		fromKey = mNumFrames - 1;
		toKey = fromKey;
	}
	else if (inTime > 0.0f)
	{
		float timePerFrame = mLength / (mNumFrames - 1);
		float frac = inTime / timePerFrame;
		fromKey = (int)frac;
		toKey = fromKey + 1;
		lerp = frac - fromKey;
	}

	int numBones = inSkeleton->GetNumBones();
	outPoses.resize(numBones);
	
	for (int i = 0; i < numBones; i++) {
		const Skeleton::Bone& bone = inSkeleton->GetBone(i);
		
		if (mTracks[i].size() > 0)
		{
			BoneTransform xform = Interpolate(mTracks[i][fromKey], mTracks[i][toKey], lerp);
			outPoses[i] = xform.ToMatrix();
		}
		else {
			outPoses[i] = Matrix4::Identity;
		}

		if (bone.mParent >= 0) {
			outPoses[i] *= outPoses[bone.mParent];
		}
	}
}
