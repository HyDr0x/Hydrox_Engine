#ifndef IDENTIFICATOR_H_
#define IDENTIFICATOR_H_

#include <list>

namespace he
{
  template<typename CLASS> class Identificator
  {
  public:

    typedef unsigned int IDType;

	  virtual ~Identificator() = 0
    {
      if(m_ID == m_availableBorder - 1)
      {
		    m_availableBorder--;
      }
	    else
      {
		    m_List.push_front(m_ID);
      }
    }

	  IDType getID() const
    {
	    return m_ID;
    }

  protected:

	  Identificator()
    {
	    if(m_List.empty())
	    {
		    m_ID = m_availableBorder;
		    m_availableBorder++;
	    }
	    else
	    {
		    m_ID = *m_List.begin();
		    m_List.pop_front();
	    }
    }

  private:

	  IDType m_ID;

	  static IDType m_availableBorder;
	  static std::list<IDType> m_List;
  };

  template<typename CLASS> typename Identificator<CLASS>::IDType Identificator<CLASS>::m_availableBorder = 0;
  template<typename CLASS> std::list<typename Identificator<CLASS>::IDType> Identificator<CLASS>::m_List;
}

#endif
