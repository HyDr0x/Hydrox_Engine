#include "Hydrox/Services/InputManager.h"


InputManager::InputManager()
{
	for(int i=0;i!=sf::Keyboard::KeyCount;i++)
		m_keyPressed[i]=0;
}

InputManager::~InputManager()
{
}

void InputManager::update()
{
	for(int i=0;i!=sf::Keyboard::KeyCount;i++)
		if(m_keyPressed[i]>0 && !m_Keyboard.isKeyPressed((sf::Keyboard::Key)i))
			m_keyPressed[i]--;

	for(int i=0;i!=sf::Mouse::ButtonCount;i++)
		if(m_mousePressed[i]>0 && !m_Mouse.isButtonPressed((sf::Mouse::Button)i))
			m_mousePressed[i]--;
}

bool InputManager::keyDown(sf::Keyboard::Key Key) const
{
	return m_Keyboard.isKeyPressed(Key);
}

bool InputManager::keyHit(sf::Keyboard::Key Key)
{
	if(m_keyPressed[Key]==0 && m_Keyboard.isKeyPressed(Key))
	{
		m_keyPressed[Key]=HITMAXTIME;
		return true;
	}
	else
		return false;
}

bool InputManager::mouseDown(sf::Mouse::Button Button) const
{
	return m_Mouse.isButtonPressed(Button);
}

bool InputManager::mouseHit(sf::Mouse::Button Button)
{
	if(m_mousePressed[Button]==0 && m_Mouse.isButtonPressed(Button))
	{
		m_mousePressed[Button]=HITMAXTIME;
		return true;
	}
	else
		return false;
}

void InputManager::setMousePos(Vec<unsigned int,2> pos)
{
	m_Mouse.setPosition(sf::Vector2i(pos[0],pos[1]));
}

Vec<unsigned int,2> InputManager::getMousePos() const
{
	return Vec<unsigned int,2>(m_Mouse.getPosition().x,m_Mouse.getPosition().y);
}
