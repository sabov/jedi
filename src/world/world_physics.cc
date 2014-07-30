#include "world.hh"

void World::initializeBullet() {
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // The actual physics solver
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

     // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, broadphase, solver, collisionConfiguration);
    //dynamicsWorld->setGravity(btVector3(0,-1,0));

    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1.0);

    /*
    btScalar mass = 1.0;
    btVector3 fallInertia(0,1,0);
    */

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -10, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    //btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    //dynamicsWorld->addRigidBody(groundRigidBody);

    dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mDroids[1].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mDroids[2].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mPlayer.mLightsaber.mPhysicObject.rigidBody);
}

void World::updatePhysics()
{
    dynamicsWorld->stepSimulation(0.0166f,10);
    //cout << "first Droid Position " <<mDroids[2].mPhysicObject.GetPosition().x << " " <<mDroids[2].mPhysicObject.GetPosition().y<< " " << mDroids[2].mPhysicObject.GetPosition().z <<endl;
    //cout << "lighsaber position   " <<mPlayer.mLightsaber.mPhysicObject.GetPosition().x << " " << mPlayer.mLightsaber.mPhysicObject.GetPosition().y << " " <<mPlayer.mLightsaber.mPhysicObject.GetPosition().z << endl;

    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for ( int i = 0; i < numManifolds; i++ )
    {
        btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btRigidBody *obj1 = btRigidBody::upcast((btCollisionObject*)contactManifold->getBody0());
        btRigidBody *obj2 = btRigidBody::upcast((btCollisionObject*)contactManifold->getBody1());

        //cout << mPlayer.mLightsaber.mPhysicObject.rigidBody->getCompanionId() << endl;
        //checking for lightsaber collision
        if (obj1 == mPlayer.mLightsaber.mPhysicObject.rigidBody || obj2 == mPlayer.mLightsaber.mPhysicObject.rigidBody)
        {
            if (obj1 == mPlayer.mLightsaber.mPhysicObject.rigidBody)
            {
                if ( mDroids[i].mPhysicObject.rigidBody == obj2){
                    //cout << "collision ooooooooo droid"<< i << endl;
                    //mDroids[i].mDroidRenderFlag = false;
                    mpProcessManager->attachProcess( mDroids[i].getDestrucionProcess() );
                }
            }
            else
            {
                for ( int j = 0; j < 3; j++ )
                {
                    if ( mDroids[j].mPhysicObject.rigidBody == obj1 )
                    {
                        cout << "collision with droid"<< j << endl;
                        dynamicsWorld->removeRigidBody(mDroids[j].mPhysicObject.rigidBody);
                        delete mDroids[j].mPhysicObject.rigidBody->getMotionState();
                        mDroids[j].setRenderFlag(false);
                        mDroids[j].setAnimationFlag(true);
                        //mDroids[i].animate();
                        mpProcessManager->attachProcess( mDroids[j].getDestrucionProcess() );
                        GameLogic::CEventManager::getInstance()->VQueueEvent( GameLogic::EventDataPtr( new CEvtData_CollisionLightSaber( glfwGetTime() ) ) );
                    }
                }
            }
        }
    }
}

void World::BulletPhysicsProcess::VOnInitialize()
{
    m_Kill          = false ;
    m_Active        = true  ;
    m_InitialUpdate = true  ;
    m_Paused        = false ;
}

void World::BulletPhysicsProcess::VKill()
{
    m_Kill            = true ;
    m_Active          = false ;
    m_InitialUpdate   = false ;
}

void World::BulletPhysicsProcess::VOnUpdate(const int elapsedTime)
{
    m_World->updatePhysics();
}
