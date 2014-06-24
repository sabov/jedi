/********************************************************************************
 *
 * Class for Geometry-Buffer used in Defeerred Shading
 * (Note: move to CGEngine later
 *
 * *****************************************************************************/

#ifndef GBUFFER_HH
#define GBUFFER_HH

#include <GL/glew.h>

class GBuffer
{
public:

    enum GBUFFER_TEXTURE_TYPE {
            GBUFFER_TEXTURE_TYPE_POSITION,
            GBUFFER_TEXTURE_TYPE_DIFFUSE,
            GBUFFER_TEXTURE_TYPE_NORMAL,
            GBUFFER_NUM_TEXTURES
    };

    GBuffer();

    ~GBuffer();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void StartFrame();
    void BindForGeomPass();
    void BindForStencilPass();
    void BindForLightPass();
    void BindForFinalPass();

    GLuint getFinalTextureName() const {
        //return m_textures[GBUFFER_TEXTURE_TYPE_TEST];
        return m_finalTexture;
    }

private:

    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
    GLuint m_finalTexture;
};

#endif // GBUFFER_HH
