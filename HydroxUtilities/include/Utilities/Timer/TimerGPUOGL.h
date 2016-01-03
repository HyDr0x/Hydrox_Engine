#ifndef OGLTIMER_H_
#define OGLTIMER_H_

#include <iostream>

#include <GL/glew.h>

#include "Utilities/DLLExport.h"

#ifdef _WIN32
  #define MICROSIGN unsigned char (230)
#else
  #define MICROSIGN unsigned short (0xC2B5) 
#endif

namespace he
{
  namespace util
  {
    class GRAPHICAPI OGLTimer
    {
    public:

      OGLTimer();
      ~OGLTimer();

      GLuint getTimeDifference();

      void start();
      void stop();

      GLuint getTime();

    private:

      GLuint m_query;
      GLuint m_openGLTime;
    };
  }
}

#endif
