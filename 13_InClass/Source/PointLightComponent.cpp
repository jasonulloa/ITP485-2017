#include "ITPEnginePCH.h"

IMPL_COMPONENT(PointLightComponent, Component, MAX_POINT_LIGHTS);

PointLightComponent::PointLightComponent(Actor& owner)
	:Component(owner)
{
	// TODO Lab 3e
	mData.isEnabled = true;
	mData.position = owner.GetPosition();
}

void PointLightComponent::Register()
{
	Super::Register();
	mOwner.GetGame().GetRenderer().AddPointLight(ThisPtr());
}

void PointLightComponent::Unregister()
{
	Super::Unregister();
	mOwner.GetGame().GetRenderer().RemovePointLight(ThisPtr());
}

void PointLightComponent::OnUpdatedTransform()
{
	// TODO Lab 3e
	mData.position = mOwner.GetPosition();
}

void PointLightComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	// TODO Lab 3m
	GetVectorFromJSON(properties, "diffuseColor", mData.diffuseColor);
	GetFloatFromJSON(properties, "innerRadius", mData.innerRadius);
	GetFloatFromJSON(properties, "outerRadius", mData.outerRadius);
	GetVectorFromJSON(properties, "specularColor", mData.specularColor);
	GetFloatFromJSON(properties, "specularPower", mData.specularPower);
}

