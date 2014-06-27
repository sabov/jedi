#include "world.hh"
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <cmath>

using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

bool World::InitDS()
{
    if ( !m_GBuffer.Init( window_width, window_height ) )
        return false;
    final_texture = m_GBuffer.getFinalTextureName();

    m_GeometryPassShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("geometry_pass") );
    m_GeometryPassShader->use();
    m_ColorTexUnitLoc = m_GeometryPassShader->getUniformLocation("gColorMap");
    m_GeometryPassShader->setUniform(m_ColorTexUnitLoc, 0);

    m_PointLightPassShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("point_light_pass") );
    m_PointLightPassShader->use();
    m_posTexLoc = m_PointLightPassShader->getUniformLocation("gPositionMap");
    m_colorTexLoc = m_PointLightPassShader->getUniformLocation("gColorMap");
    m_normalTexLoc = m_PointLightPassShader->getUniformLocation("gNormalMap");
    m_eyeWorldPosLoc = m_PointLightPassShader->getUniformLocation("g_EyeWorldPos");
    m_screenSizeLoc = m_PointLightPassShader->getUniformLocation("gScreenSize");
    m_PointLightPassShader->setUniform(m_posTexLoc, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    m_PointLightPassShader->setUniform(m_colorTexLoc, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    m_PointLightPassShader->setUniform(m_normalTexLoc, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    m_PointLightPassShader->setUniform(m_screenSizeLoc, glm::vec2( (float)window_width, (float)window_height) );

    m_NullShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("null_technique") );

    m_bSphere.LoadMesh("geometry/sphere.obj", CGEngine::CGE_TRIANGULATE);

    use_direct_lighting = false;
    return true;
}

void World::DSGeometryPass()
{
    m_GeometryPassShader->use();

    m_GBuffer.BindForGeomPass();

    // Only the geometry pass updates the depth buffer
    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate(CGEngine::Vec3(0.0, -1.0, 0.0));

    glm::mat4 modelMatrix = mMatrixStack.getCompleteTransform();
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();


    m_GeometryPassShader->setUniform( "uModelMatrix", modelMatrix );
    m_GeometryPassShader->setUniform( "uViewMatrix",  viewMatrix );
    m_GeometryPassShader->setUniform( "uProjectionMatrix", mPlayer.getProjectionMatrix() );
    m_GeometryPassShader->setUniform( "uNormalMatrix",     glm::inverseTranspose(glm::mat3(viewMatrix)*glm::mat3(modelMatrix)) );

    //
    // draw geometry
    //
    mLevel.VOnDraw();

    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate(CGEngine::Vec3(0.0,1.0,-4.0));
    mMatrixStack.Rotate( glm::radians( mpRotProcess->rotAngle ), CGEngine::Vec3(0.0,1.0,0.0) );

    modelMatrix = mMatrixStack.getCompleteTransform();
    viewMatrix = mPlayer.getHMDViewMatrix();

    //m_GeometryPassShader->use();
    m_GeometryPassShader->setUniform( "uModelMatrix", modelMatrix );
    m_GeometryPassShader->setUniform( "uViewMatrix",  viewMatrix );
    m_GeometryPassShader->setUniform( "uProjectionMatrix", mPlayer.getProjectionMatrix() );
    m_GeometryPassShader->setUniform( "uNormalMatrix",     glm::inverseTranspose(glm::mat3(viewMatrix)*glm::mat3(modelMatrix)) );

    mDice.VOnDraw();

    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    //glDepthMask(GL_FALSE);

}

void World::DSStencilPass(unsigned int _PointLightIndex)
{
    m_NullShader->use();

    // Disable color/depth write and enable stencil
    //m_GBuffer.BindForStencilPass();
/*
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);

    glClear(GL_STENCIL_BUFFER_BIT);

    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    float BBoxScale = CalcPointLightBSphere( mPointLights[_PointLightIndex] );
    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate( mPointLights[_PointLightIndex].getPositionV3() );   //Translate to Light Postion
    mMatrixStack.Translate(CGEngine::Vec3(0.0,-1.0,0.0));   //Translate to fit to world
    mMatrixStack.UniformScale( BBoxScale );

    glm::mat4 modelMatrix = mMatrixStack.getCompleteTransform();
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();

    m_NullShader->setUniform( "uModelMatrix", modelMatrix );
    m_NullShader->setUniform( "uViewMatrix",  viewMatrix );
    m_NullShader->setUniform( "uProjectionMatrix", mPlayer.getProjectionMatrix() );

    m_bSphere.VOnDraw();
    */
}

void World::DSPointLightPass(unsigned int _PointLightIndex)
{
    m_PointLightPassShader->use();
    m_GBuffer.BindForLightPass();

    m_PointLightPassShader->setUniform( m_eyeWorldPosLoc, mPlayer.getPosition() );

    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    float BBoxScale = CalcPointLightBSphere( mPointLights[_PointLightIndex] );
    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate( mPointLights[_PointLightIndex].getPositionV3() );   //Translate to Light Postion
    mMatrixStack.Translate(CGEngine::Vec3(0.0,-1.0,0.0));   //Translate to fit to world
    mMatrixStack.UniformScale( BBoxScale );

    glm::mat4 modelMatrix = mMatrixStack.getCompleteTransform();
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();

    m_PointLightPassShader->setUniform( "uModelMatrix", modelMatrix );
    m_PointLightPassShader->setUniform( "gLightTransform", viewMatrix );
    m_PointLightPassShader->setUniform( "uViewMatrix",  viewMatrix );
    m_PointLightPassShader->setUniform( "uProjectionMatrix", mPlayer.getProjectionMatrix() );

    CGEngine::CShader s(m_PointLightPassShader);
    mPointLights[_PointLightIndex].VSendToShader(s);

    m_bSphere.VOnDraw();

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);
}

void World::DSFinalPass()
{
    m_GBuffer.BindForFinalPass();
    glBlitFramebuffer(0, 0, (GLint)window_width, (GLint)window_height,
                      0, 0, (GLint)window_width, (GLint)window_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

float World::CalcPointLightBSphere(const CGEngine::CPositionalLight &Light)
{
    float intensity = 1.0;
    float cnst = Light.GetConstAttenuation();
    float lin = Light.GetLinAttenuation();
    float exp = Light.GetExpAttenuation();
    float MaxChannel = fmax(fmax(Light.getDiffuseColor()[0], Light.getDiffuseColor()[1]), Light.getDiffuseColor()[2]);

    float ret = ( -lin + sqrt( lin*lin - 4.0*exp*( cnst - 256.0*MaxChannel*intensity ) ) );

    return ret;
}
