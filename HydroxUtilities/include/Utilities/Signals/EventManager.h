#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <map>

#include "Utilities/DLLExport.h"

#include "Utilities/Miscellaneous/SingletonBehaviour.hpp"
#include "Utilities/Signals/Signal.h"

//const member functions or functions with variadic arguments are not supported!

namespace he
{
  namespace util
  {
    class GRAPHICAPI EventManager : public SingletonBehaviour
    {
    public:

      enum Events
      {
        OnAddStringTexture2D,
        OnAddSprite,
        OnAddBillboardNode,
        OnAddGeometryNode,
        OnAddSkinnedGeometryNode,
        OnAddLightNode,
        OnAddShadowLightNode,
        OnAddParticleNode,
        OnRemoveStringTexture2D,
        OnRemoveSprite,
        OnRemoveBillboardNode,
        OnRemoveGeometryNode,
        OnRemoveSkinnedGeometryNode,
        OnRemoveLightNode,
        OnRemoveShadowLightNode,
        OnRemoveParticleNode,
        OnAddLODNode,
        OnAddTransformNode,
        OnAddAnimatedTransformNode,
        OnRemoveLODNode,
        OnRemoveTransformNode,
        OnRemoveAnimatedTransformNode,

        EVENTSTARTCOUNTER
      };

      ~EventManager()
      {
        m_sigMap.clear();
      }

      template<typename Signature> void addNewSignal(unsigned int eventID)
      {
        m_sigMap[eventID] = dynamic_cast<signal_save*>(new Signal<Signature>);
      }

      void deleteSignal(unsigned int eventID)
      {
        delete m_sigMap[eventID];
        m_sigMap.erase(eventID);
      }

      template<typename Signature> void addSlotToSignal(Signature fptr, unsigned int eventID)
      {
        dynamic_cast<Signal<Signature>*>(m_sigMap[eventID])->connect(fptr);
      }

      template<class C, typename SignalSignature, typename FuncSignature> void addSlotToSignal(C *obj, FuncSignature fptr, unsigned int eventID)
      {
        dynamic_cast<Signal<SignalSignature>*>(m_sigMap[eventID])->connect<C>(obj,fptr);
      }

      template<typename Signature> void deleteSlotFromSignal(Signature fptr, unsigned int eventID)
      {
        dynamic_cast<Signal<Signature>*>(m_sigMap[eventID])->disconnect(fptr);
      }

      template<class C, typename SignalSignature, typename FuncSignature> void deleteSlotFromSignal(C *obj, FuncSignature fptr, unsigned int eventID)
      {
        dynamic_cast<Signal<SignalSignature>*>(m_sigMap[eventID])->disconnect(obj,fptr);
      }

      template<typename Signature> Signal<Signature>* raiseSignal(unsigned int eventID)
      {
        return dynamic_cast<Signal<Signature>*>(m_sigMap[eventID]);
      }

      template<typename Signature> unsigned int getSlotNumber(unsigned int eventID) const
      {
        return dynamic_cast<Signal<Signature>*>(m_sigMap[eventID])->getSlotNumber();
      }

    private:

      std::map<unsigned int, signal_save*> m_sigMap;
    };
  }
}

#endif
