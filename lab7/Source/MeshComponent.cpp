#include "ITPEnginePCH.h"
#include "Profiler.h"

IMPL_COMPONENT(MeshComponent, DrawComponent, 1024);

MeshComponent::MeshComponent(Actor& owner)
	:Super(owner)
	,mTextureIndex(0)
	,mColor(1.0f, 1.0f, 1.0f)
{

}

void MeshComponent::Draw(class Renderer& render)
{
	if (mMesh)
	{
		render.DrawMesh(mMesh->GetVertexArray(), mMesh->GetTextures(),
			mOwner.GetWorldTransform(), mColor, mMesh->GetShaderType());
	}
}

void MeshComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);
	PROFILE_SCOPE(MeshComponent_SetProperties);
	// TODO Lab 3m
	std::string meshName;
	if (GetStringFromJSON(properties, "mesh", meshName))
		SetMesh(GetOwner().GetGame().GetAssetCache().Load<Mesh>(meshName));
	int texIndex;
	if (GetIntFromJSON(properties, "textureIndex", texIndex))
		SetTextureIndex(texIndex);
	GetVectorFromJSON(properties, "color", mColor);
}
