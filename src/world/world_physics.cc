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

    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1.0);


    btDefaultMotionState* groundMotionState = new btDefaultMotionState(
            btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -10, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));

    //dynamicsWorld->addRigidBody(mPlayer.mLightsaber.mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mPlayer.mPhysicObject.rigidBody);

}

void World::updatePhysics()
{
    dynamicsWorld->stepSimulation(0.0166f,10);

    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for ( int i = 0; i < numManifolds; i++ )
        {
            btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            btRigidBody *obj1 = btRigidBody::upcast((btCollisionObject*)contactManifold->getBody0());
            btRigidBody *obj2 = btRigidBody::upcast((btCollisionObject*)contactManifold->getBody1());

            //checking for lightsaber collision
            if (obj1 == mPlayer.mLightsaber.mPhysicObject.rigidBody || obj2 == mPlayer.mLightsaber.mPhysicObject.rigidBody)
            {
                if (obj1 == mPlayer.mLightsaber.mPhysicObject.rigidBody){
                    for ( unsigned int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj2 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            mDroids[j].setRenderFlag(false);
                            mDroids[j].setAnimationFlag(true);
                            mpProcessManager->attachProcess( mDroids[j].getDestrucionProcess() );
                            GameLogic::CEventManager::getInstance()->VQueueEvent( GameLogic::EventDataPtr( new CEvtData_CollisionLightSaber( glfwGetTime() ) ) );

                            if (j == (mDroids.size() -1) ){
                                mDroids[0].reInit();
                                if (!mDroids[0].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
                                    mDroids[0].rigidflag = true;
                                }
                            } else{
                                mDroids[j+1].reInit();
                                if (!mDroids[j+1].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[j+1].mPhysicObject.rigidBody);
                                    mDroids[j+1].rigidflag = true;
                                }
                            }
                        }
                    }
                }
                else{
                    for ( unsigned int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj1 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            mDroids[j].setRenderFlag(false);
                            mDroids[j].setAnimationFlag(true);
                            mpProcessManager->attachProcess( mDroids[j].getDestrucionProcess() );
                            GameLogic::CEventManager::getInstance()->VQueueEvent( GameLogic::EventDataPtr( new CEvtData_CollisionLightSaber( glfwGetTime() ) ) );

                            if (j == (mDroids.size() -1) ){
                                mDroids[0].reInit();
                                if (!mDroids[0].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
                                    mDroids[0].rigidflag = true;
                                }
                            } else{
                                mDroids[j+1].reInit();
                                if (!mDroids[j+1].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[j+1].mPhysicObject.rigidBody);
                                    mDroids[j+1].rigidflag = true;
                                }
                            }
                        }
                    }
                }
            }

            //==================User collisions======================
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++

            if (obj1 == mPlayer.mPhysicObject.rigidBody || obj2 == mPlayer.mPhysicObject.rigidBody){
                if (obj1 == mPlayer.mPhysicObject.rigidBody){
                    for ( unsigned int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj2 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            mDroids[j].setRenderFlag(false);
                            // mDroids[j].setAnimationFlag(true);
                            mpProcessManager->attachProcess( mDroids[j].getDestrucionProcess() );
                            GameLogic::CEventManager::getInstance()->VQueueEvent( GameLogic::EventDataPtr( new CEvtData_CollisionPlayer( glfwGetTime() ) ) );

                            if (j == (mDroids.size() -1) ){
                               mDroids[0].reInit();
                                  if (!mDroids[0].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
                                    mDroids[0].rigidflag = true;
                                }
                            } else{
                                mDroids[j+1].reInit();
                                if (!mDroids[j+1].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[j+1].mPhysicObject.rigidBody);
                                    mDroids[j+1].rigidflag = true;
                                }
                            }
                        }
                    }
                }
                else{
                    for ( unsigned int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj1 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            mDroids[j].setRenderFlag(false);
                            //mDroids[j].setAnimationFlag(true);
                            mpProcessManager->attachProcess( mDroids[j].getDestrucionProcess() );
                            GameLogic::CEventManager::getInstance()->VQueueEvent( GameLogic::EventDataPtr( new CEvtData_CollisionPlayer( glfwGetTime() ) ) );

                            if (j == (mDroids.size() -1) ){
                                mDroids[0].reInit();
                                if (!mDroids[0].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
                                    mDroids[0].rigidflag = true;
                                }
                            } else{
                                mDroids[j+1].reInit();
                                if (!mDroids[j+1].rigidflag){
                                    dynamicsWorld->addRigidBody(mDroids[j+1].mPhysicObject.rigidBody);
                                    mDroids[j+1].rigidflag = true;
                                }
                            }
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
