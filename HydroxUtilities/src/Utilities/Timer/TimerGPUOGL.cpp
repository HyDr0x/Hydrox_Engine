#include "Utilities/Timer/TimerGPUOGL.h"

namespace he
{
	namespace util
	{
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

        std::cout << m_timerName << std::endl;
        std::cout << "OpenGL Time: " << m_openGLTime << " m" << MICROSIGN << std::endl;
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
	}
}