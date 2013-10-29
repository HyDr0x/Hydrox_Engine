#include "Hydrox/Services/Camera.h"


Camera::Camera(InputManager *input, Vec<unsigned int,2> screenSize, float zNear, float zFar) : m_zNear(zNear), m_zFar(zFar)
{
	this->m_Input = input;

	m_keyboard_velocity = m_mouse_velocity = 1.0f;
	m_rotx = m_roty = m_rotz = 0;
	m_position = Vec<float,3>(0, 0, 0);

	m_ScreenSize = screenSize;
	screenSize /= 2;
	m_ScreenMiddle = screenSize;
	
	m_oldMousePos = m_Input->getMousePos();

  calculateProjectionMatrix(math::PI_QUARTER, static_cast<float>(m_ScreenSize[0]), static_cast<float>(m_ScreenSize[1]), m_zNear, m_zFar);
}

Camera::~Camera()
{
}

void Camera::setPosition(Vec<float,3> pos)
{
	m_position = pos;
}

void Camera::setVelocity(float Kvel, float Mvel)
{
	m_keyboard_velocity = Kvel;
	m_mouse_velocity = Mvel;
}

void Camera::calculateProjectionMatrix(float fov, float width, float height, float zNear, float zFar)
{//math::createOrthographic(-10,10,-10,10,-10,10);//math::createPerspective(-10,10,-10,10,zNear,zFar);//math::createPerspective(fov, static_cast<float>(width)/static_cast<float>(height), zNear, zFar);
  m_Projection = math::createPerspective(fov, static_cast<float>(width) / static_cast<float>(height), zNear, zFar);
}

Vec<float,3> Camera::getPosition()
{
	return m_position;
}

Mat<float,4> Camera::getViewProjectionMatrix()
{
  return m_Projection * m_View;
}

Mat<float,4> Camera::getViewMatrix() const
{
	return m_View;
}

Mat<float,4> Camera::getProjectionMatrix() const
{
	return m_Projection;
}

void Camera::update(float fps)
{
	Vec<int,2> mouseDiff = Vec<int,2>(0, 0);

	if(m_Input->mouseDown(sf::Mouse::Left))
  {
		mouseDiff = math::vector_cast<int>(m_Input->getMousePos() - m_oldMousePos);
  }
	else
  {
		m_oldMousePos = m_Input->getMousePos();
  }

	if(abs(m_roty+mouseDiff[1] * m_mouse_velocity) <= 89.f)
  {
		m_roty += mouseDiff[1] * m_mouse_velocity;
  }

	m_rotx += mouseDiff[0] * m_mouse_velocity;

  Mat<float,4> rotYMat = math::rotAxis(Mat<float,4>::identity(), m_rotx,Vec<float,3>(0, 1, 0));
  Vec<float,4> tmpVec = rotYMat * Vec<float,4>(1, 0, 0, 0);
	Vec<float,3> sideVec(tmpVec[0], tmpVec[1], tmpVec[2]);
	Mat<float,4> rotXMat = math::rotAxis(rotYMat, m_roty, sideVec);
  tmpVec = rotXMat * Vec<float,4>(0, 0, -1, 0);
	Vec<float,3> forwardVec(tmpVec[0], tmpVec[1], tmpVec[2]);
  sideVec.normalize();
  forwardVec.normalize();

	if(m_Input->keyDown(sf::Keyboard::Up))
  {
		m_position += (forwardVec * m_keyboard_velocity * fps);
  }

	if(m_Input->keyDown(sf::Keyboard::Down))
  {
		m_position -= (forwardVec * m_keyboard_velocity * fps);
  }

	if(m_Input->keyDown(sf::Keyboard::Left))
  {
		m_position -= (sideVec * m_keyboard_velocity * fps);
  }

	if(m_Input->keyDown(sf::Keyboard::Right))
  {
		m_position += (sideVec * m_keyboard_velocity * fps);
  }

  m_View = math::createLookAt(m_position, m_position + forwardVec, Vec<float,3>(0, 1, 0));

	if(m_Input->mouseDown(sf::Mouse::Left))
  {
		m_Input->setMousePos(m_oldMousePos);
  }
}
