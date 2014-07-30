#include "world.hh"
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include "../audio/loadWavFile.hh"

using namespace std;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;
glm::vec3 droidPosition[4] = { glm::vec3(-3.0f, 1.5f, -5.0f), glm::vec3(0.0f, 1.5f, -5.0f), glm::vec3(3.0f, 1.5f, -5.0f), glm::vec3(-5.0f, 1.0f, -5.0f) };
glm::vec3 droidMoveDirection[4] = { glm::vec3(0.01f, 0.0f, 0.0166f), glm::vec3(0.00f, 0.0f, 0.015f), glm::vec3(-0.01f, 0.0f, 0.0166f), glm::vec3(-5.0f, 1.0f, -5.0f) };
bool LocalContactProcessedCallback(btManifoldPoint& cp, void* body0, void* body1);

World::World() {
    window_width = 1600;
    window_height = 900;
    m_ColorTexUnitLoc = 0;
    for (int i = 0; i < 3; ++i) {
        m_posTexLoc[i] = 0;
        m_colorTexLoc[i] = 0;
        m_normalTexLoc[i] = 0;
        m_screenSizeLoc[i] = 0;
        m_eyeWorldPosLoc[i] = 0;
    }

}

World::~World() {
    cout << "deleting world..." << endl;
    delete dynamicsWorld;
    delete droidsPhysic;
}

bool World::initializeWorld() {
    debug() << "loading game world..." << endl;

    mLevel.LoadMesh("geometry/L1/level.obj");

    CGEngine::LoadLightsFromFile("geometry/L1/level_lights.dae", mDirLights, mPointLights, mSpotLights);
    glm::mat4 t = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0));
    for (unsigned int i = 0; i < mPointLights.size(); ++i)
        mPointLights[i].Transform(t);
    for (unsigned int i = 0; i < mSpotLights.size(); ++i)
        mSpotLights[i].Transform(t);

    CGEngine::CDirectionalLight dir_light;
    dir_light.Initialize(CGEngine::Vec4(0.1, 1.0, -0.1, 0.0), CGEngine::Vec4(0.1), CGEngine::Vec4(0.25), CGEngine::Vec4(1.0));
    mDirLights.push_back(dir_light);

    mpProcessManager = GameLogic::CProcessManager::getInstance();

    initializeBullet();

    mDroids.resize(3);
    for (int i = 0 ; i < mDroids.size() ; ++i)
    {
        mDroids[i].initialize("geometry/Droid/droid1.obj", droidPosition[i]);
        dynamicsWorld->addRigidBody(mDroids[i].mPhysicObject.rigidBody);
//       mpProcessManager->attachProcess( mDroids[i].getMoveProcess() );
    }

      mDroids[1].setRenderFlag(false);
      mDroids[2].setRenderFlag(false);
//    mDroids.resize(2);
//    mDroids.end()->initialize("geometry/Droid/droid1.obj", glm::vec3(-5.0f, 1.0f, -5.0f));
//    mpProcessManager->attachProcess( mDroids.end()->getMoveProcess() );

    //createDroid(glm::vec3(-5.0f, 1.0f, -5.0f));

    m_BulletPhysicsProcess = PhysicsProcessPtr( new BulletPhysicsProcess( this ) );
    mpProcessManager->attachProcess( m_BulletPhysicsProcess );

    //Throw event
    GameLogic::CEventManager::getInstance()->VQueueEvent(GameLogic::EventDataPtr(new CEvtData_WorldInitialized(glfwGetTime())));
    return true;
}

void World::setPlayerCamera(ACGL::HardwareSupport::SimpleRiftController *riftControl) {
    mPlayer.setCamera(riftControl->getCamera());
}

glm::vec3 World::getPlayerPosition() {
    return mPlayer.getPosition();
}

void World::getPlayerOrientation(ALfloat *playerOrientation) {
    glm::mat4 HMDView = glm::inverse(mPlayer.getHMDViewMatrix());

    glm::vec4 orientation = HMDView * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    glm::vec4 lookUpVector = HMDView * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        playerOrientation[i] = (float) orientation[i];
        playerOrientation[i + 3] = (float) lookUpVector[i];
    }
}

void World::geometryRender()
{
    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate(CGEngine::Vec3(0.0, -1.0, 0.0));

    glm::mat4 modelMatrix = mMatrixStack.getCompleteTransform();
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();
    glm::mat4 projectionMatrix = mPlayer.getProjectionMatrix();

    m_GeometryPassShader->setUniform("uModelMatrix", modelMatrix);
    m_GeometryPassShader->setUniform("uViewMatrix", viewMatrix);
    m_GeometryPassShader->setUniform("uProjectionMatrix", projectionMatrix);
    m_GeometryPassShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    //
    // draw geometry
    //
    mLevel.VOnDraw();

    mPlayer.mPhysicObject.SetPosition(mPlayer.getPosition());
//    cout <<  "x:   " << mPlayer.mPhysicObject.GetPosition().x <<"y:   " << mPlayer.mPhysicObject.GetPosition().y<< "z:   " << mPlayer.mPhysicObject.GetPosition().z<<endl;
    for (int i = 0; i < mDroids.size() ; ++i) {
        m_GeometryPassShader->setUniform("uModelMatrix", mDroids[i].getModelMatrix());
        m_GeometryPassShader->setUniform("uViewMatrix", viewMatrix);
        m_GeometryPassShader->setUniform("uProjectionMatrix", projectionMatrix);
        m_GeometryPassShader->setUniform("uNormalMatrix",
        glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(mDroids[i].getModelMatrix())));
        mDroids[i].baseRender(droidMoveDirection[i]);
    }



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
                    for ( int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj2 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            cout << "collision with droid"<< j << endl;
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
                    for ( int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj1 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            cout << "collision with droid"<< j << endl;
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
                    for ( int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj2 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            cout << "player collision with droid"<< j << endl;
                            mDroids[j].setRenderFlag(false);
                            // mDroids[j].setAnimationFlag(true);
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
                    for ( int j = 0; j < mDroids.size(); j++ ){
                        if ( mDroids[j].mPhysicObject.rigidBody == obj1 )
                        {
                            dynamicsWorld->removeCollisionObject(mDroids[j].mPhysicObject.rigidBody);
                            mDroids[j].rigidflag = false;
                            cout << "player collision with droid"<< j << endl;
                            mDroids[j].setRenderFlag(false);
                            //mDroids[j].setAnimationFlag(true);
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
        }

    }



void World::movePlayer(const glm::vec3 &direction) {
    mPlayer.move(direction);

    // simple game logic:
    // don't walk below the floor (y = 0)
    glm::vec3 playerPos = mPlayer.getPosition();
    if (playerPos.y < 0.0f) {
        playerPos.y = 0.0f;
        mPlayer.setPosition(playerPos);
    }

    mPlayer.mLightsaber.setPlayerPosition(playerPos);
    mPlayer.mLightsaber.move(direction);
}

void World::rotatePlayer(float dYaw) {
    float yaw = 0.0f;
    float pitch = 0.0f;
    float dPitch = 0.0f;
    glm::mat3 R = mPlayer.getRotationMatrix3();

    // get roll / pitch / yaw from the current rotation matrix:
    float yaw1 = asin(-R[2][0]);
    float yaw2 = M_PI - asin(-R[2][0]);

    float pitch1 = (cos(yaw1) > 0) ? atan2(R[2][1], R[2][2]) : atan2(-R[2][1], -R[2][2]);
    float pitch2 = (cos(yaw2) > 0) ? atan2(R[2][1], R[2][2]) : atan2(-R[2][1], -R[2][2]);

    float roll1 = (cos(yaw1) > 0) ? atan2(R[1][0], R[0][0]) : atan2(-R[1][0], -R[0][0]);
    float roll2 = (cos(yaw2) > 0) ? atan2(R[1][0], R[0][0]) : atan2(-R[1][0], -R[0][0]);

    // we assume no roll at all, in that case one of the roll variants will be 0.0
    // if not, use the smaller one -> minimize the camera "jump" as this will destroy
    // information
    if (std::abs(roll1) <= std::abs(roll2)) {
        yaw = -yaw1;
        pitch = -pitch1;
    } else {
        yaw = -yaw2;
        pitch = -pitch2;
    }

    // add rotation diffs given:
    yaw = yaw + dYaw;
    pitch = glm::clamp(pitch + dPitch, -0.5f * (float) M_PI, 0.5f * (float) M_PI);

    // create rotation matices, seperated so we have full control over the order:
    glm::mat4 newRotY = glm::yawPitchRoll(yaw, 0.0f, 0.0f);
    glm::mat4 newRotX = glm::yawPitchRoll(0.0f, pitch, 0.0f);

    // multiplication order is important to prevent roll:
    mPlayer.setRotationMatrix(newRotX * newRotY);
}

void World::duckPlayer(float duckingValue) {
    mPlayer.duck(duckingValue);
}

void World::useForcePlayer() {
    mPlayer.useForce();
}

void World::moveLightsaber(const glm::vec3 &direction) {
    mPlayer.mLightsaber.move(direction);
}

void World::toggleLightsaber() {
    mPlayer.mLightsaber.toggle();
    GameLogic::CEventManager::getInstance()->VQueueEvent(GameLogic::EventDataPtr(new CEvtData_ToggleSword(glfwGetTime())));
}
void World::rotateLightsaber(float yaw, float pitch, float roll) {
    mPlayer.mLightsaber.rotate(yaw, pitch, roll);
}

void World::setRotationMatrixLightsaber(const glm::mat4 &rotation) {
    mPlayer.mLightsaber.setRotationMatrix(rotation);
}

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

    btScalar mass = 1.0;
    btVector3 fallInertia(0,1,0);

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(
            btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -10, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    //dynamicsWorld->addRigidBody(groundRigidBody);

    //dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
    //dynamicsWorld->addRigidBody(mDroids[1].mPhysicObject.rigidBody);
    //dynamicsWorld->addRigidBody(mDroids[2].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mPlayer.mLightsaber.mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mPlayer.mPhysicObject.rigidBody);

}

void World::update(int time) {
    mpProcessManager->updateProcesses(time);
}

void World::setWidthHeight(unsigned int _w, unsigned int _h) {
    window_width = _w;
    window_height = _h;
}

void World::DSRender() {
    m_GBuffer.StartFrame();

    DSGeometryPass();

    // We need stencil to be enabled in the stencil pass to get the stencil buffer
    // updated and we also need it in the light pass because we render the light
    // only if the stencil passes.

    glEnable(GL_STENCIL_TEST);

    glDisable(GL_CULL_FACE);

    unsigned int num_lights1 = mPointLights.size();

    for (unsigned int i = 0; i < num_lights1; i++) {
        DSStencilPass(i);
        DSPointLightPass(i);
    }

    DSSpotStencilPass(0);
    DSSpotLightPass(0);

    // The directional light does not need a stencil test because its volume
    // is unlimited and the final pass simply copies the texture.
    glDisable(GL_STENCIL_TEST);

    DSDirectionalLightPass();

    //glEnable(GL_BLEND);
    //glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_ONE, GL_ONE);

    mPlayer.mLightsaber.render(mPlayer.getHMDViewMatrix(), mPlayer.getProjectionMatrix());

    glDisable(GL_BLEND);

    DSFinalPass();
}

void World::createDroid(glm::vec3 position){
    int i = mDroids.size();
    //mDroids.resize(i + 1);

    //mDroids[i].initialize("geometry/Droid/droid1.obj", position);
    //mpProcessManager->attachProcess( mDroids[i].getMoveProcess() );

    //mDroid->initialize("geometry/Droid/droid1.obj", position);
    //mDroids.push_back(*mDroid);
    //mpProcessManager->attachProcess( mDroids.back().getMoveProcess() );
}
