#include "ITPEnginePCH.h"

IMPL_COMPONENT(PointLightComponent, Component, MAX_POINT_LIGHTS);

PointLightComponent::PointLightComponent(Actor& owner)
	:Component(owner)
{
	mData.enabled = true;
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
	mData.position = mOwner.GetPosition();
}

void PointLightComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	Vector3 diffuse;
	if (GetVectorFromJSON(properties, "diffuseColor", diffuse)) {
		mData.diffuseColor = diffuse;
	}

	float inner;
	if (GetFloatFromJSON(properties, "innerRadius", inner)) {
		mData.innerRadius = inner;
	}

	float outer;
	if (GetFloatFromJSON(properties, "outerRadius", outer)) {
		mData.outerRadius = outer;
	}

	Vector3 specular;
	if (GetVectorFromJSON(properties, "specularColor", specular)) {
		mData.specularColor = specular;
	}

	float power;
	if (GetFloatFromJSON(properties, "specularPower", power)) {
		mData.specularPower = power;
	}
}

