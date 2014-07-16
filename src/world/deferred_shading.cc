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

    m_GeometryPassShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("geometry_pass") );
    m_GeometryPassShader->use();
    m_ColorTexUnitLoc = m_GeometryPassShader->getUniformLocation("gColorMap");
    m_GeometryPassShader->setUniform(m_ColorTexUnitLoc, 0);

    m_PointLightPassShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("point_light_pass") );
    m_PointLightPassShader->use();
    m_posTexLoc[0]      = m_PointLightPassShader->getUniformLocation("gPositionMap");
    m_colorTexLoc[0]    = m_PointLightPassShader->getUniformLocation("gColorMap");
    m_normalTexLoc[0]   = m_PointLightPassShader->getUniformLocation("gNormalMap");
    m_eyeWorldPosLoc[0] = m_PointLightPassShader->getUniformLocation("g_EyeWorldPos");
    m_screenSizeLoc[0]  = m_PointLightPassShader->getUniformLocation("gScreenSize");
    m_PointLightPassShader->setUniform(m_posTexLoc[0],      GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    m_PointLightPassShader->setUniform(m_colorTexLoc[0],    GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    m_PointLightPassShader->setUniform(m_normalTexLoc[0],   GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    m_PointLightPassShader->setUniform(m_screenSizeLoc[0],  glm::vec2( (float)window_width, (float)window_height) );

    m_DirLightPassShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("dir_light_pass") );
    m_DirLightPassShader->use();
    m_posTexLoc[1]      = m_DirLightPassShader->getUniformLocation("gPositionMap");
    m_colorTexLoc[1]    = m_DirLightPassShader->getUniformLocation("gColorMap");
    m_normalTexLoc[1]   = m_DirLightPassShader->getUniformLocation("gNormalMap");
    m_eyeWorldPosLoc[1] = m_DirLightPassShader->getUniformLocation("g_EyeWorldPos");
    m_screenSizeLoc[1]  = m_DirLightPassShader->getUniformLocation("gScreenSize");
    m_DirLightPassShader->setUniform(m_posTexLoc[1],        GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    m_DirLightPassShader->setUniform(m_colorTexLoc[1],      GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    m_DirLightPassShader->setUniform(m_normalTexLoc[1],     GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    m_DirLightPassShader->setUniform(m_screenSizeLoc[1],    glm::vec2( (float)window_width, (float)window_height) );

    m_SpotLightPassShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("spot_light_pass") );
    m_SpotLightPassShader->use();
    m_posTexLoc[2]      = m_SpotLightPassShader->getUniformLocation("gPositionMap");
    m_colorTexLoc[2]    = m_SpotLightPassShader->getUniformLocation("gColorMap");
    m_normalTexLoc[2]   = m_SpotLightPassShader->getUniformLocation("gNormalMap");
    m_eyeWorldPosLoc[2] = m_SpotLightPassShader->getUniformLocation("g_EyeWorldPos");
    m_screenSizeLoc[2]  = m_SpotLightPassShader->getUniformLocation("gScreenSize");
    m_SpotLightPassShader->setUniform(m_posTexLoc[2],       GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    m_SpotLightPassShader->setUniform(m_colorTexLoc[2],     GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    m_SpotLightPassShader->setUniform(m_normalTexLoc[2],    GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    m_SpotLightPassShader->setUniform(m_screenSizeLoc[2],   glm::vec2( (float)window_width, (float)window_height) );

    m_NullShader = ShaderProgramFileManager::the()->get( ShaderProgramCreator("null_technique") );

    m_Sphere.LoadMesh("geometry/sphere.obj", CGEngine::CGE_TRIANGULATE);
    m_Cone.LoadMesh("geometry/spotlightcone.obj",  CGEngine::CGE_TRIANGULATE);
    m_Quad.LoadMesh("");

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

    geometryRender();

    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    glDepthMask(GL_FALSE);

}

void World::DSStencilPass(unsigned int _PointLightIndex)
{
    m_NullShader->use();

    // Disable color/depth write and enable stencil
    m_GBuffer.BindForStencilPass();

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

    m_Sphere.VOnDraw();
}

void World::DSPointLightPass(unsigned int _PointLightIndex)
{
    m_PointLightPassShader->use();
    m_GBuffer.BindForLightPass();

    m_PointLightPassShader->setUniform( m_eyeWorldPosLoc[0], mPlayer.getPosition() );

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

    m_Sphere.VOnDraw();

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);
}

void  World::DSDirectionalLightPass()
{
    m_GBuffer.BindForLightPass();

    m_DirLightPassShader->use();

    m_DirLightPassShader->setUniform( m_eyeWorldPosLoc[1], mPlayer.getPosition() );
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();
    m_DirLightPassShader->setUniform( "gLightTransform", viewMatrix );

    glm::mat4 id(1.0);
    m_DirLightPassShader->setUniform( "uModelMatrix", id );
    m_DirLightPassShader->setUniform( "uViewMatrix",  id );
    m_DirLightPassShader->setUniform( "uProjectionMatrix", id );

    CGEngine::CShader s(m_DirLightPassShader);
    mDirLights[0].VSendToShader(s);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    m_Quad.VOnDraw();

    glDisable(GL_BLEND);
}

void World::DSSpotStencilPass(unsigned int _SpotLightIndex)
{
    m_NullShader->use();

    // Disable color/depth write and enable stencil
    m_GBuffer.BindForStencilPass();

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);

    glClear(GL_STENCIL_BUFFER_BIT);

    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate(CGEngine::Vec3(0.0,-1.0,0.0));   //Translate to fit to world

    glm::mat4 modelMatrix = mMatrixStack.getCompleteTransform();
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();

    m_NullShader->setUniform( "uModelMatrix", modelMatrix );
    m_NullShader->setUniform( "uViewMatrix",  viewMatrix );
    m_NullShader->setUniform( "uProjectionMatrix", mPlayer.getProjectionMatrix() );

    m_Cone.VOnDraw();

}

void World::DSSpotLightPass(unsigned int _SpotLightIndex)
{
    m_SpotLightPassShader->use();
    m_GBuffer.BindForLightPass();

    m_SpotLightPassShader->setUniform( m_eyeWorldPosLoc[2], mPlayer.getPosition() );

    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    mMatrixStack.LoadIdentity();
    mMatrixStack.Translate(CGEngine::Vec3(0.0,-1.0,0.0));   //Translate to fit to world

    glm::mat4 modelMatrix = mMatrixStack.getCompleteTransform();
    glm::mat4 viewMatrix = mPlayer.getHMDViewMatrix();

    m_SpotLightPassShader->setUniform( "uModelMatrix", modelMatrix );
    m_SpotLightPassShader->setUniform( "gLightTransform", viewMatrix );
    m_SpotLightPassShader->setUniform( "uViewMatrix",  viewMatrix );
    m_SpotLightPassShader->setUniform( "uProjectionMatrix", mPlayer.getProjectionMatrix() );

    CGEngine::CShader s(m_SpotLightPassShader);
    mSpotLights[_SpotLightIndex].VSendToShader(s);

    m_Cone.VOnDraw();

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
