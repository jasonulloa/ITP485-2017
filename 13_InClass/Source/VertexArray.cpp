#include "ITPEnginePCH.h"

VertexArrayPtr VertexArray::Create(GraphicsDriver& graphics, InputLayoutCache& inputCache, 
	const void* verts, size_t vertCount, size_t vertSize, const std::string& inputLayoutName,
	const void* indices, size_t indexCount, size_t indexSize)
{
	return std::make_shared<VertexArray>(graphics, inputCache, 
		verts, vertCount, vertSize, inputLayoutName, 
		indices, indexCount, indexSize);
}

VertexArray::VertexArray(GraphicsDriver& graphics, InputLayoutCache& inputCache,
	const void* verts, size_t vertCount, size_t vertSize, const std::string& inputLayoutName,
	const void* indices, size_t indexCount, size_t indexSize)
	: mGraphics(graphics)
	, mVertexCount(vertCount)
	, mVertexSize(vertSize)
	, mIndexCount(indexCount)
{
	// TODO Lab 2d
	mInputLayout = inputCache.GetLayout(inputLayoutName);
	mVertexBuffer = mGraphics.CreateGraphicsBuffer(verts, vertCount*vertSize, EBF_VertexBuffer, ECPUAF_Neither, EGBU_Default);
	mIndexBuffer = mGraphics.CreateGraphicsBuffer(indices, indexCount*indexSize, EBF_IndexBuffer, ECPUAF_Neither, EGBU_Default);
}

VertexArray::~VertexArray()
{

}

void VertexArray::SetActive()
{
	// TODO Lab 2d
	mGraphics.SetInputLayout(mInputLayout);
	mGraphics.SetVertexBuffer(mVertexBuffer, mVertexSize);
	mGraphics.SetIndexBuffer(mIndexBuffer);
}
