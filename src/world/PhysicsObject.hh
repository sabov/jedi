#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/common.hpp>

class PhysicsObject
{
public:
   PhysicsObject(void);
   ~PhysicsObject(void);

   btRigidBody *rigidBody;
   btCollisionShape *colShape;

   void Init(btCollisionShape*, glm::vec3);
   glm::vec3 GetPosition();
   bool SetPosition(glm::vec3);
   bool SetRotation(glm::vec4);
   bool SetVelocity(glm::vec3);
   bool SetMass(float);

};
