#include "Utilities/Timer/TimerGPUOGL.h"

namespace he
{
  namespace util
  {
    OGLTimer::OGLTimer() : m_timerName(""), m_showTimerWhenDying(true), m_openGLTime(0)
    {
      glGenQueries(1, &m_query);
    }

    OGLTimer::OGLTimer(std::string &timerName) : m_timerName(timerName), m_showTimerWhenDying(true)
    {
      glGenQueries(1, &m_query);
      glBeginQuery(GL_TIME_ELAPSED, m_query);
    }

    OGLTimer::~OGLTimer()
    {
      if(m_showTimerWhenDying)
      {
        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_openGLTime);
        glDeleteQueries(1, &m_query);
        m_openGLTime /= 1000;

        std::clog << m_timerName << std::endl;
        std::clog << "OpenGL Time: " << m_openGLTime << " m" << MICROSIGN << std::endl;
      }
    }

    GLuint OGLTimer::getTimeDifference()
    {
      glEndQuery(GL_TIME_ELAPSED);
      glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_openGLTime);
      glDeleteQueries(1, &m_query);
      m_openGLTime /= 1000;

      m_showTimerWhenDying = false;

      return m_openGLTime;
    }

    void OGLTimer::start()
    {
      glBeginQuery(GL_TIME_ELAPSED, m_query);
    }

    void OGLTimer::stop()
    {
      glEndQuery(GL_TIME_ELAPSED);
      glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_openGLTime);
      glDeleteQueries(1, &m_query);
      m_openGLTime /= 1000;
    }

    GLuint OGLTimer::getTime()
    {
      return m_openGLTime;
    }
  }
}