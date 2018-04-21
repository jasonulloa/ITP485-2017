#include "ITPEnginePCH.h"
#include "Job.h"

Job::Job() {

}

Job::~Job() {

}

AnimJob::AnimJob(MatrixPalette* palette, SkeletonPtr skeleton, AnimationPtr anim, float animTime) {
	mPalette = palette;
	mSkeleton = skeleton;
	mAnimation = anim;
	mAnimationTime = animTime;
}

AnimJob::~AnimJob() {

}

void AnimJob::DoIt() {
	std::vector<Matrix4> invPoses, globalPoses;
	invPoses = mSkeleton->GetGlobalInvBindPoses();
	mAnimation->GetGlobalPoseAtTime(globalPoses, mSkeleton, mAnimationTime);
	for (unsigned int i = 0; i < invPoses.size(); i++) {
		mPalette->mPalette[i] = invPoses[i] * globalPoses[i];
	}
}