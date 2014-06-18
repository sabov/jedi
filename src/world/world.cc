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
glm::vec3 droidPosition = glm::vec3(-4.0f, 1.0f, -8.0f);
glm::vec3 droidPosition2 = glm::vec3(-4.0f, 1.0f, -3.0f);
glm::vec3 droidPosition3 = glm::vec3(4.0f, 1.0f, -3.0f);
bool LocalContactProcessedCallback(btManifoldPoint& cp, void* body0, void* body1);


World::World()
{
    debug() << "loading game world..." << endl;
    //
    // load visual assets:
    mWorldGeometry = VertexArrayObjectCreator("quicktest.obj").create();
    mWorldShader   = ShaderProgramFileManager::the()->get( ShaderProgramCreator("worldShader") );
    mWorldGeometry->setAttributeLocations( mWorldShader->getAttributeLocations() );

    mBunnyGeometry = VertexArrayObjectCreator("Bunny.obj").create();
    mBunnyShader   = ShaderProgramFileManager::the()->get( ShaderProgramCreator("Bunny") );
    mBunnyGeometry->setAttributeLocations( mBunnyShader->getAttributeLocations() );
    mBunnyTexture  = loadTexture2D( "clownfishBunny.png" );

    //initialize bullet ==============================================================

    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

     // The actual physics solver
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

     // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, broadphase, solver, collisionConfiguration);

    //dynamicsWorld->setGravity(btVector3(0,-10,0));


    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0),0.0);

    btCollisionShape* sphereShape = new btSphereShape(1);
    btCollisionShape* saberShape = new btCylinderShape(btVector3(0.2, 2, 0.2));

    btScalar mass = 1.0;
    btVector3 fallInertia(0,0,0);
    //sphereShape->calculateLocalInertia(mass,fallInertia);


    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    //dynamicsWorld->addRigidBody(groundRigidBody);


    mDroids[0].mDroidRenderFlag = true;
    mDroids[0].mPhysicObject.Init(sphereShape, droidPosition);
    mDroids[0].setPosition(droidPosition);

    mDroids[1].mDroidRenderFlag = true;
    mDroids[1].setPosition(droidPosition2);
    mDroids[1].mPhysicObject.Init(sphereShape, droidPosition2);

    mDroids[2].mDroidRenderFlag = true;
    mDroids[2].setPosition(droidPosition3);
    mDroids[2].mPhysicObject.Init(sphereShape, droidPosition3);

    mPlayer.mLightsaber.mPhysicObject.Init(saberShape, mPlayer.mLightsaber.getPosition());

    dynamicsWorld->addRigidBody(mDroids[0].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mDroids[1].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mDroids[2].mPhysicObject.rigidBody);
    dynamicsWorld->addRigidBody(mPlayer.mLightsaber.mPhysicObject.rigidBody);

    //=====================================================================================
    // load audio assets:
    mBeep = new SimpleSound( "audio/musiccensor.wav" );
}

World::~World()
{
    delete mBeep;
}

void World::setPlayerCamera( ACGL::HardwareSupport::SimpleRiftController *riftControl )
{
    mPlayer.setCamera( riftControl->getCamera() );
}

glm::vec3 World::getPlayerPosition()
{
    return mPlayer.getPosition();
}

void World::getPlayerOrientation( ALfloat *playerOrientation )
{
    glm::mat4 HMDView = glm::inverse( mPlayer.getHMDViewMatrix() );

    glm::vec4 orientation  = HMDView * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    glm::vec4 lookUpVector = HMDView * glm::vec4(0.0f, 1.0f,  0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        playerOrientation[i  ] = (float) orientation[i];
        playerOrientation[i+3] = (float) lookUpVector[i];
    }
}

void World::render()
{
    //
    // setup shaders
    //
    mWorldShader->use();

    //
    // set uniforms
    //
    glm::mat4 modelMatrix = glm::scale( glm::vec3( 1.0f,1.0f,1.0f) );
    mWorldShader->setUniform( "uModelMatrix", modelMatrix );

    // get the view from a camera that is attached to the SimpleRiftController:
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();
    mWorldShader->setUniform( "uViewMatrix", viewMatrix );

    // get the projection from the SimpleRiftController directly:
    glm::mat4 projectionMatrix = mPlayer.getProjectionMatrix();
    mWorldShader->setUniform( "uProjectionMatrix", projectionMatrix );
    mWorldShader->setUniform( "uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix)*glm::mat3(modelMatrix)) );

    //
    // draw geometry
    //
    mWorldGeometry->bind();
    mWorldGeometry->draw();

    //
    // Textured example:
    //
    mBunnyShader->use();

    // the bunny gets a different model matrix:
    modelMatrix = glm::scale( glm::vec3( 0.25f ) );
    glm::mat4 translateMatrix = glm::translate( glm::mat4(), glm::vec3(8.0f, 0.0f, -10.5f ) );
    modelMatrix = translateMatrix * modelMatrix;

    mBunnyShader->setUniform( "uModelMatrix", modelMatrix );
    mBunnyShader->setUniform( "uViewMatrix",  viewMatrix );
    mBunnyShader->setUniform( "uProjectionMatrix", projectionMatrix );
    mBunnyShader->setUniform( "uNormalMatrix",     glm::inverseTranspose(glm::mat3(viewMatrix)*glm::mat3(modelMatrix)) );

    // At least 16 texture units can be used, but multiple texture can also be placed in one
    // texture array and thus only occupy one texture unit!
    GLint textureUnit = 0;
    mBunnyTexture->bind( textureUnit );
    mBunnyShader->setUniform( "uTexture", textureUnit );
    //
    // draw geometry
    //
    mBunnyGeometry->bind();
    mBunnyGeometry->draw();

    mPlayer.mLightsaber.render(viewMatrix, projectionMatrix);
    mPlayer.mLightsaber.mPhysicObject.SetPosition(mPlayer.mLightsaber.getPosition());

    if (mDroids[0].mDroidRenderFlag){
        mDroids[0].render(viewMatrix, projectionMatrix);
        //mDroids[0].setPosition(mDroids[0].mPhysicObject.GetPosition());
    }

    if (mDroids[1].mDroidRenderFlag){
        mDroids[1].render(viewMatrix, projectionMatrix);
        //mDroids[1].setPosition(mDroids[1].mPhysicObject.GetPosition());
        mDroids[1].move(glm::vec3(0.01f, 0.0f, 0.0f));
    }

    if (mDroids[2].mDroidRenderFlag){
        mDroids[2].render(viewMatrix, projectionMatrix);
        //mDroids[2].setPosition(mDroids[2].mPhysicObject.GetPosition());
    }



    dynamicsWorld->stepSimulation(0.0166f,10);

    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i=0;i<numManifolds;i++)
        {
            btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            btRigidBody *obj1 = btRigidBody::upcast((btCollisionObject*)contactManifold->getBody0());
            btRigidBody *obj2 = btRigidBody::upcast((btCollisionObject*)contactManifold->getBody1());

            //checking for lightsaber collision
            if (obj1 == mPlayer.mLightsaber.mPhysicObject.rigidBody || obj2 == mPlayer.mLightsaber.mPhysicObject.rigidBody){
                if (obj1 == mPlayer.mLightsaber.mPhysicObject.rigidBody){
                    if ( mDroids[i].mPhysicObject.rigidBody == obj2){
                        //mDroids[i].;
                    }

                }
                else{
                    for (int i = 0; i < 3; i++){
                        if ( mDroids[i].mPhysicObject.rigidBody == obj1){
                            mDroids[i].mDroidRenderFlag = false;
                        }
                    }
                }
            }

        }
}

void World::movePlayer( glm::vec3 direction )
{
    mPlayer.move( direction );

    // simple game logic:
    // don't walk below the floor (y = 0)
    glm::vec3 playerPos = mPlayer.getPosition();
    if (playerPos.y < 0.0f) {
        playerPos.y = 0.0f;
        mPlayer.setPosition( playerPos );
    }
    mPlayer.mLightsaber.setPosition(mPlayer.getPosition());
    //mDroid.setPosition(mPlayer.getPosition());
}

void World::rotatePlayer( float dYaw )
{
    float yaw   = 0.0f;
    float pitch = 0.0f;
    float dPitch = 0.0f;
    glm::mat3 R = mPlayer.getRotationMatrix3();

    // get roll / pitch / yaw from the current rotation matrix:
    float yaw1 = asin(-R[2][0]);
    float yaw2 = M_PI - asin(-R[2][0]);

    float pitch1  = (cos(yaw1) > 0)? atan2(R[2][1], R[2][2]): atan2(-R[2][1], -R[2][2]);
    float pitch2  = (cos(yaw2) > 0)? atan2(R[2][1], R[2][2]): atan2(-R[2][1], -R[2][2]);

    float roll1   = (cos(yaw1) > 0)? atan2(R[1][0], R[0][0]): atan2(-R[1][0], -R[0][0]);
    float roll2   = (cos(yaw2) > 0)? atan2(R[1][0], R[0][0]): atan2(-R[1][0], -R[0][0]);

    // we assume no roll at all, in that case one of the roll variants will be 0.0
    // if not, use the smaller one -> minimize the camera "jump" as this will destroy
    // information
    if ( std::abs(roll1) <= std::abs(roll2) ) {
        yaw   = -yaw1;
        pitch = -pitch1;
    } else {
        yaw   = -yaw2;
        pitch = -pitch2;
    }

    // add rotation diffs given:
    yaw = yaw + dYaw;
    pitch = glm::clamp( pitch + dPitch, -0.5f * (float)M_PI, 0.5f*(float)M_PI );

    // create rotation matices, seperated so we have full control over the order:
    glm::mat4 newRotY = glm::yawPitchRoll( yaw, 0.0f, 0.0f );
    glm::mat4 newRotX = glm::yawPitchRoll( 0.0f, pitch, 0.0f );

    // multiplication order is important to prevent roll:
    mPlayer.setRotationMatrix( newRotX * newRotY );
}

void World::duckPlayer( float duckingValue )
{
    mPlayer.duck( duckingValue );
}

void World::useForcePlayer()
{
    mPlayer.useForce();
    if (!mBeep->isPlaying()){
        mBeep->play();
    }
}
