#include "SceneGraph/TreeNodes/AnimationTrack.h"

namespace he
{
	namespace sg
	{
    std::ofstream& operator<<(std::ofstream& stream, const AnimationTrack& animationTrack)
    {
      stream << animationTrack.m_animationName << std::endl;
      stream << animationTrack.m_duration << std::endl;
      stream << animationTrack.m_animationTicksPerSecond << std::endl;

      stream << animationTrack.m_positions.size() << std::endl;
      for(unsigned int i = 0; i < animationTrack.m_positions.size(); i++)
      {
        stream << animationTrack.m_positions[i] << std::endl;
        stream << animationTrack.m_positionsTime[i] << std::endl;
      }

      stream << animationTrack.m_rotations.size() << std::endl;
      for(unsigned int i = 0; i < animationTrack.m_rotations.size(); i++)
      {
        stream << animationTrack.m_rotations[i] << std::endl;
        stream << animationTrack.m_rotationsTime[i] << std::endl;
      }

      stream << animationTrack.m_scales.size() << std::endl;
      for(unsigned int i = 0; i < animationTrack.m_scales.size(); i++)
      {
        stream << animationTrack.m_scales[i] << std::endl;
        stream << animationTrack.m_scalesTime[i] << std::endl;
      }

      stream << std::endl;

      return stream;
    }

    std::ifstream& operator>>(std::ifstream& stream, AnimationTrack& animationTrack)
    {
      stream >> animationTrack.m_animationName;
      stream >> animationTrack.m_duration;
      stream >> animationTrack.m_animationTicksPerSecond;

      unsigned int positionSize;
      stream >> positionSize;
      animationTrack.m_positions.resize(positionSize);
      animationTrack.m_positionsTime.resize(positionSize);
      for(unsigned int i = 0; i < animationTrack.m_positions.size(); i++)
      {
        stream >> animationTrack.m_positions[i];
        stream >> animationTrack.m_positionsTime[i];
      }

      unsigned int rotationSize;
      stream >> rotationSize;
      animationTrack.m_rotations.resize(rotationSize);
      animationTrack.m_rotationsTime.resize(rotationSize);
      for(unsigned int i = 0; i < animationTrack.m_rotations.size(); i++)
      {
        stream >> animationTrack.m_rotations[i];
        stream >> animationTrack.m_rotationsTime[i];
      }

      unsigned int scalesSize;
      stream >> scalesSize;
      animationTrack.m_scales.resize(scalesSize);
      animationTrack.m_scalesTime.resize(scalesSize);
      for(unsigned int i = 0; i < animationTrack.m_scales.size(); i++)
      {
        stream >> animationTrack.m_scales[i];
        stream >> animationTrack.m_scalesTime[i];
      }

      return stream;
    }
	}
}