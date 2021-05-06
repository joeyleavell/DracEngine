#include "ScenePrimitive.h"
#include "Mesh.h"

Ry::ScenePrimitive::ScenePrimitive(bool bStatic)
{
	this->bStatic = bStatic;
	this->ParentPrimitive = nullptr;
}

void Ry::ScenePrimitive::SetParent(ScenePrimitive* Parent)
{
	this->ParentPrimitive = Parent;
}

Ry::ScenePrimitive* Ry::ScenePrimitive::GetParent()
{
	return ParentPrimitive;
}

bool Ry::ScenePrimitive::IsStatic() const
{
	return bStatic;
}

Ry::Transform& Ry::ScenePrimitive::GetRelativeTransform()
{
	return RelativeTransform;
}

Ry::Transform Ry::ScenePrimitive::GetWorldTransform()
{
	if(ParentPrimitive)
	{
		return ParentPrimitive->GetWorldTransform() * RelativeTransform;
	}
	else
	{
		return RelativeTransform;
	}
}

Ry::StaticMeshPrimitive::StaticMeshPrimitive(Mesh* Mesh)
{
	this->StaticMesh = Mesh;
}

Ry::VertexArray* Ry::StaticMeshPrimitive::GetVertexArray()
{
	return StaticMesh->GetVertexArray();
}

Ry::Mesh* Ry::StaticMeshPrimitive::GetMesh()
{
	return StaticMesh;
}

Ry::ScenePrimitive* Ry::CreatePrimitive(Mesh* SM)
{
	return new StaticMeshPrimitive(SM);
}
