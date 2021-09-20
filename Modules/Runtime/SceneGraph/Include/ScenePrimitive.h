#pragma once

#include "ScenePrimitive.gen.h"
#include "Transform.h"

namespace Ry
{
	class Mesh;
	class VertexArray;

	class SCENEGRAPH_MODULE ScenePrimitive
	{
	public:

		ScenePrimitive(bool bStatic = false);
		virtual ~ScenePrimitive() = default;

		virtual VertexArray* GetVertexArray() = 0;

		bool IsStatic() const;
		ScenePrimitive* GetParent();
		Transform GetWorldTransform();
		Transform& GetRelativeTransform();

		void SetParent(ScenePrimitive* Parent);

	private:

		bool bStatic;

		Transform RelativeTransform;
		ScenePrimitive* ParentPrimitive;
		
	};

	class SCENEGRAPH_MODULE StaticMeshPrimitive : public ScenePrimitive
	{
	public:

		StaticMeshPrimitive(Mesh* Mesh);
		virtual ~StaticMeshPrimitive() = default;

		VertexArray* GetVertexArray() override;
		Mesh* GetMesh();

	private:

		Mesh* StaticMesh;

	};

	SCENEGRAPH_MODULE ScenePrimitive* CreatePrimitive(Mesh* SM);


}
