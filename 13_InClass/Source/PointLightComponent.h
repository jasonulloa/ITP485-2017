#pragma once
#include "Component.h"
#include "PointLightData.h"

class PointLightComponent : public Component
{
	DECL_COMPONENT(PointLightComponent, Component);
public:
	PointLightComponent(Actor& owner);

	void Register() override;
	void Unregister() override;

	void OnUpdatedTransform() override;

	const PointLightData& GetData() const { return mData; }

	void SetProperties(const rapidjson::Value& properties) override;

	PointLightData mData;
};

DECL_PTR(PointLightComponent);
