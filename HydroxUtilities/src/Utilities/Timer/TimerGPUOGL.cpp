#include "Utilities/Timer/TimerGPUOGL.h"

namespace he
{
  namespace util
  {
    OGLTimer::OGLTimer() : m_openGLTime(0)
    {
      glGenQueries(1, &m_query);
    }

    OGLTimer::~OGLTimer()
    {
      glDeleteQueries(1, &m_query);
    }

    GLuint OGLTimer::getTimeDifference()
    {
      glEndQuery(GL_TIME_ELAPSED);
      glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_openGLTime);
      glDeleteQueries(1, &m_query);
      m_openGLTime /= 1000;

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
      m_openGLTime /= 1000;
    }

    GLuint OGLTimer::getTime()
    {
      return m_openGLTime;
    }
  }
}