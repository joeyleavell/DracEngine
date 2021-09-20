
#include "Physics.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

namespace Ry
{

	SharedPtr<RigidBody> MakeBoxBody(Transform Initial, float Mass, BoundingBox Box)
	{
		SharedPtr<RigidBody> NewBody = MakeShared<>(new RigidBody);

		// Scale the bounding box such that the bounds match the scaled bounds
		Box *= Initial.scale;
		
		// float BBWidth = Box.MaxX - Box.MaxX;
		// float BBHeight = Box.MaxY - Box.MaxY;
		// float BBDepth = Box.MaxZ - Box.MaxZ;
		// float HalfWidth = BBWidth / 2;
		// float HalfHeight = BBHeight / 2;
		// float HalfDepth = BBDepth / 2;
		// float OX = (Box.MaxX + Box.MinX) / 2;
		// float OY = (Box.MaxY + Box.MinY) / 2;
		// float OZ = (Box.MaxZ + Box.MinZ) / 2;

		// Make the shape
		btTriangleMesh* TriMesh = new btTriangleMesh;

		btVector3 FrontBottomLeft = btVector3(Box.MinX, Box.MinY, Box.MinZ);
		btVector3 FrontTopLeft = btVector3(Box.MinX, Box.MaxY, Box.MinZ);
		btVector3 FrontTopRight = btVector3(Box.MaxX, Box.MaxY, Box.MinZ);
		btVector3 FrontBottomRight = btVector3(Box.MaxX, Box.MinY, Box.MinZ);
		btVector3 BackBottomLeft = btVector3(Box.MaxX, Box.MinY, Box.MaxZ);
		btVector3 BackTopLeft = btVector3(Box.MaxX, Box.MaxY, Box.MaxZ);
		btVector3 BackTopRight = btVector3(Box.MinX, Box.MaxY, Box.MaxZ);
		btVector3 BackBottomRight = btVector3(Box.MinX, Box.MinY, Box.MaxZ);

		// Build the cube bounding box
		TriMesh->addTriangle(FrontBottomLeft, FrontTopLeft, FrontTopRight);
		TriMesh->addTriangle(FrontBottomLeft, FrontBottomRight, FrontTopRight);

		TriMesh->addTriangle(BackBottomLeft, BackTopLeft, BackTopRight);
		TriMesh->addTriangle(BackBottomLeft, BackBottomRight, BackTopRight);

		TriMesh->addTriangle(FrontBottomLeft, BackBottomRight, BackTopRight);
		TriMesh->addTriangle(FrontBottomLeft, FrontTopLeft, BackTopRight);

		TriMesh->addTriangle(FrontBottomRight, BackBottomLeft, BackTopLeft);
		TriMesh->addTriangle(FrontBottomRight, FrontTopRight, BackTopLeft);

		TriMesh->addTriangle(FrontBottomLeft, BackBottomRight, BackBottomLeft);
		TriMesh->addTriangle(FrontBottomLeft, FrontBottomRight, BackBottomLeft);

		TriMesh->addTriangle(FrontTopLeft, BackTopRight, BackTopLeft);
		TriMesh->addTriangle(FrontTopLeft, FrontTopRight, BackTopLeft);

		btConvexTriangleMeshShape* TriMeshShape = new btConvexTriangleMeshShape(TriMesh, true);

	//	btBoxShape* BoxShape = new btBoxShape(btVector3(HalfWidth, HalfHeight, HalfDepth));
		
		NewBody->Shape = TriMeshShape;

		TriMeshShape->setMargin(0.01f);

		bool Dynamic = Mass > 0.0001;

		// Initialize the body transform
		btTransform PhysicsTransform;
		PhysicsTransform.setIdentity();
		PhysicsTransform.setOrigin(btVector3(Initial.position.x, Initial.position.y, Initial.position.z));
		PhysicsTransform.setRotation(btQuaternion(Initial.rotation.z, Initial.rotation.y, Initial.rotation.x));

		btVector3 LocalInertia = btVector3(0.0f, 0.0f, 0.0f);
		if (Dynamic)
		{
			NewBody->Shape->calculateLocalInertia(btScalar(Mass), LocalInertia);
		}

		btDefaultMotionState* MotionState = new btDefaultMotionState(PhysicsTransform);
		btRigidBody::btRigidBodyConstructionInfo ConstructInfo(btScalar(Mass), MotionState, NewBody->Shape, LocalInertia);
		NewBody->Body = new btRigidBody(ConstructInfo);

//		NewBody->Body->applyTorqueImpulse(btVector3(10.0f, 0.0f, 0.0f));

		return NewBody;
	}
}