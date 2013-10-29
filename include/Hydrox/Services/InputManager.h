#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window.hpp>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Services/Io_service.h"

class GRAPHICAPI InputManager : public Io_service
{
public:

	InputManager();
	~InputManager();

	void update();
	
	bool keyDown(sf::Keyboard::Key Key) const;
	bool keyHit(sf::Keyboard::Key Key);

	bool mouseDown(sf::Mouse::Button Button) const;
	bool mouseHit(sf::Mouse::Button Button);

	void setMousePos(Vec<unsigned int,2> pos);
	Vec<unsigned int,2> getMousePos() const;

private:

	InputManager(const InputManager&){}

	sf::Clock m_clock;
	sf::Uint32 m_keyPressed[sf::Keyboard::KeyCount];
	sf::Uint32 m_mousePressed[sf::Mouse::ButtonCount];

	sf::Keyboard m_Keyboard;
	sf::Mouse m_Mouse;

	static const sf::Uint32 HITMAXTIME=1;
};

#endif