#include "ITPEnginePCH.h"

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

	std::string mesh;
	if (GetStringFromJSON(properties, "mesh", mesh)) {
		SetMesh(mOwner.GetGame().GetAssetCache().Load<Mesh>(mesh));
	}

	int index;
	if (GetIntFromJSON(properties, "textureIndex", index)) {
		SetTextureIndex(index);
	}

	Vector3 color;
	if (GetVectorFromJSON(properties, "color", color)) {
		SetColor(color);
	}
}
