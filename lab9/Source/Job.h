#pragma once

class Job {
public:
	Job();
	~Job();

	virtual void DoIt() = 0;
};

class AnimJob : public Job {
public:
	AnimJob(MatrixPalette* palette, SkeletonPtr skeleton, AnimationPtr anim, float animTime);
	~AnimJob();

	void DoIt() override;

private:
	MatrixPalette* mPalette;
	SkeletonPtr mSkeleton;
	AnimationPtr mAnimation;
	float mAnimationTime;
};