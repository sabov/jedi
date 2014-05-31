#include "PhysicsObject.hh"

PhysicsObject::PhysicsObject()
{

}


PhysicsObject::~PhysicsObject(void)
{
   delete [] rigidBody;
   delete [] colShape;
}

void PhysicsObject::Init(btCollisionShape* cS) {
   colShape = cS;
   btScalar mass = 1;

   btDefaultMotionState* mS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
   btVector3 fallInertia(0,0,0);
   colShape->calculateLocalInertia(mass,fallInertia);
   btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,mS,cS,fallInertia);
   rigidBody = new btRigidBody(rigidBodyCI);
}

bool PhysicsObject::SetPosition(glm::vec3 nPos)
{
   if (rigidBody != NULL) {
      btTransform nTrans;
      rigidBody->getMotionState()->getWorldTransform(nTrans);
      nTrans.setOrigin(btVector3(nPos.x, nPos.y, nPos.z));
      delete rigidBody->getMotionState();
      rigidBody->setMotionState(new btDefaultMotionState(nTrans));
      return true;
   }

   return false;
}

bool PhysicsObject::SetRotation(glm::vec4)
{
   return true;
}

bool PhysicsObject::SetVelocity(glm::vec3 nVelocity)
{
   rigidBody->setLinearVelocity(btVector3(nVelocity.x, nVelocity.y, nVelocity.z));
   return true;
}

bool PhysicsObject::SetMass(float nMass)
{
   rigidBody->setMassProps(nMass, btVector3(0,0,0));
   rigidBody->updateInertiaTensor();
}
