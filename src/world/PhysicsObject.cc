#include "PhysicsObject.hh"
#include <iostream>

PhysicsObject::PhysicsObject() {
}


PhysicsObject::~PhysicsObject(void) {
    std::cout << "deleting physics object..." << std::endl;
    delete rigidBody;
    // we do not delete colShape as it is deleted from its corresponding game object
}

void PhysicsObject::Init(btCollisionShape* cS, glm::vec3 startPosition) {
   colShape = cS;
   btScalar mass = 0.1;

   btDefaultMotionState* mS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(startPosition.x,startPosition.y,startPosition.z)));
   btVector3 fallInertia(0,0,0);
   colShape->calculateLocalInertia(mass,fallInertia);
   btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,mS,cS,fallInertia);
   rigidBody = new btRigidBody(rigidBodyCI);
}

glm::vec3 PhysicsObject::GetPosition(){
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        float x = trans.getOrigin().getX();
        float y = trans.getOrigin().getY();
        float z = trans.getOrigin().getZ();
        return glm::vec3(x,y,z);
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
