#pragma once

#include "ScenePrimitive.gen.h"
#include "Transform.h"

namespace Ry
{
	class Mesh2;

	class VertexArray2;

	class SCENEGRAPH_MODULE ScenePrimitive
	{
	public:

		ScenePrimitive(bool bStatic = false);
		virtual ~ScenePrimitive() = default;

		virtual VertexArray2* GetVertexArray() = 0;

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

		StaticMeshPrimitive(Mesh2* Mesh);
		virtual ~StaticMeshPrimitive() = default;

		VertexArray2* GetVertexArray() override;
		Mesh2* GetMesh();

	private:

		Mesh2* StaticMesh;

	};

	SCENEGRAPH_MODULE ScenePrimitive* CreatePrimitive(Mesh2* SM);


}
