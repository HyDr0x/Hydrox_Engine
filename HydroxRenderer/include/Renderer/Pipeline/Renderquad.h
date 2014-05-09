#ifndef RENDERQUAD_H_
#define RENDERQUAD_H_

#include <iostream>
#include <stdarg.h>
#include <assert.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/Texture2D.h"

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI Renderquad
    {
    public:

	    Renderquad();
	    ~Renderquad();

	    void setRenderTarget(int count, Texture2D *tex[]);
	    void setRenderTarget(int count, ...);

	    void render(bool direct) const;

    private:

	    Renderquad(const Renderquad&);
	    Renderquad& operator=(const Renderquad&);

	    unsigned int m_count;

	    GLuint m_vaoIndex;
	    GLuint m_vboindex;
      GLuint m_texIndex;
	    GLuint m_depthTex;
	    GLuint m_fboIndex;
	    GLuint m_width,m_height;
    };
	}
}

#endif
