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

    dynamicsWorld->setGravity(btVector3(0,-10,0));

    btScalar mass = 1.0;

    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0),0.0);

    btCollisionShape* fallShape = new  btBoxShape(btVector3(1.0, 1.0, 1.0));
    btDefaultMotionState* mS = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),btVector3(0, 0, 0)));


    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);

    droidsPhysic = new PhysicsObject[2];
    droidsPhysic[0].Init(fallShape);
    droidsPhysic[1].Init(fallShape);

    dynamicsWorld->addRigidBody(droidsPhysic[0].rigidBody);
    dynamicsWorld->addRigidBody(droidsPhysic[1].rigidBody);

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
    mDroids[0].render(viewMatrix, projectionMatrix, glm::vec3(-2.0f, 1.0f, -10.0f));
    mDroids[0].move(glm::vec3(0.0f, 0.0f, 0.01f));
    droidsPhysic[0].SetPosition(mDroids[0].getPosition());
    mDroids[1].render(viewMatrix, projectionMatrix, glm::vec3(2.0f, 1.0f, -10.0f));
    mDroids[1].move(glm::vec3(0.0f, 0.0f, 0.01f));
    droidsPhysic[1].SetPosition(mDroids[1].getPosition());

    dynamicsWorld->stepSimulation(0.0166f,10);
    btTransform trans;
    droidsPhysic[1].rigidBody->getMotionState()->getWorldTransform(trans);
    std::cout << "physic position: " << trans.getOrigin().getZ() <<"   "<< trans.getOrigin().getX()<< std::endl;
    std::cout << "droid position: " << mDroids[1].getPosition().z << "   " << mDroids[1].getPosition().x << std::endl;


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
