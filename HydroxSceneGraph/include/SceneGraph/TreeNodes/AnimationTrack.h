#ifndef ANIMATIONTRACK_H_
#define ANIMATIONTRACK_H_

#include <fstream>
#include <vector>
#include <string>

#include <Utilities/Math/Math.hpp>

#include "SceneGraph/DLLExport.h"

namespace he
{
  namespace sg
  {    struct GRAPHICAPI AnimationTrack//an animation track, each track represents all the keys for one animation in one node
    {
      std::string m_animationName;
      float m_duration;
      float m_animationTicksPerSecond;

      std::vector<util::Vector<float, 3>> m_positions;
      std::vector<float> m_positionsTime;

      std::vector<util::Quaternion<float>> m_rotations;
      std::vector<float> m_rotationsTime;

      std::vector<float> m_scales;
      std::vector<float> m_scalesTime;

      unsigned int getSize() const
      {
        unsigned int size = 0;
        size += m_positions.size() * sizeof(util::Vector<float, 3>);
        size += m_positionsTime.size() * sizeof(float);

        size += m_rotations.size() * sizeof(util::Quaternion<float>);
        size += m_rotationsTime.size() * sizeof(float);

        size += m_scales.size() * sizeof(float);
        size += m_scalesTime.size() * sizeof(float);

        return size;
      }
    };

    GRAPHICAPI std::ofstream& operator<<(std::ofstream& stream, const AnimationTrack& animationTrack);
    GRAPHICAPI std::ifstream& operator>>(std::ifstream& stream, AnimationTrack& animationTrack);
  }
}

#endif
