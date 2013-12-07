#ifndef OGLTIMER_H_
#define OGLTIMER_H_

#include <iostream>
#include <string>

#include <GL\glew.h>

#include "Hydrox/DLLExport.h"

#ifdef _WIN32
  #define MICROSIGN unsigned char (230)
#else
  #define MICROSIGN unsigned short (0xC2B5) 
#endif

namespace he
{
  class GRAPHICAPI OGLTimer
  {
  public:

    OGLTimer(std::string &timerName);
    ~OGLTimer();

    GLuint getTimeDifference();

  private:

    std::string m_timerName;

    GLuint m_query;
    GLuint m_openGLTime;

    bool m_showTimerWhenDying;
  };
}

#endif