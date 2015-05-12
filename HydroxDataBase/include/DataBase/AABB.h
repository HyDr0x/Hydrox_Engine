#ifndef AABB_H_
#define AABB_H_

#include "DataBase/DLLExport.h"

#include <vector>

#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace db
  {
    class GRAPHICAPI AABB
    {
    public:

      AABB();
      AABB(util::vec3f min, util::vec3f max);
      AABB(const std::vector<util::vec3f>& positions);
      ~AABB();

      util::vec3f getBBMax() const;
      util::vec3f getBBMin() const;

      bool collisionLine(util::vec3f point, util::vec3f direction) const;

    private:

      util::vec3f m_bbMax;
      util::vec3f m_bbMin;
    };
  }
}

#endif
