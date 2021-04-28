#include "ScenePrimitive.h"
#include "Mesh2.h"

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

Ry::StaticMeshPrimitive::StaticMeshPrimitive(Mesh2* Mesh)
{
	this->StaticMesh = Mesh;
}

Ry::VertexArray2* Ry::StaticMeshPrimitive::GetVertexArray()
{
	return StaticMesh->GetVertexArray();
}

Ry::Mesh2* Ry::StaticMeshPrimitive::GetMesh()
{
	return StaticMesh;
}

Ry::ScenePrimitive* Ry::CreatePrimitive(Mesh2* SM)
{
	return new StaticMeshPrimitive(SM);
}
