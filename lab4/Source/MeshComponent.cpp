#include "ITPEnginePCH.h"

IMPL_COMPONENT(MeshComponent, DrawComponent, 1024);

MeshComponent::MeshComponent(Actor& owner)
	:Super(owner)
	,mTextureIndex(0)
{

}

void MeshComponent::Draw(class Renderer& render)
{
	if (mMesh)
	{
		render.DrawMesh(mMesh->GetVertexArray(), mMesh->GetTexture(mTextureIndex),
			mOwner.GetWorldTransform(), mMesh->GetShaderType());
	}
}

void MeshComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	std::string mesh;
	if (GetStringFromJSON(properties, "mesh", mesh)) {
		SetMesh(mOwner.GetGame().GetAssetCache().Load<Mesh>(mesh));
	}

	int index;
	if (GetIntFromJSON(properties, "textureIndex", index)) {
		SetTextureIndex(index);
	}
}
