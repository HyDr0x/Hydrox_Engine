#ifndef CAMERA_H_
#define CAMERA_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Services/InputManager.h"

class GRAPHICAPI Camera : public Io_service
{
public:
	Camera(InputManager *input, Vec<unsigned int,2> screenSize, float zNear=0.1f, float zFar=10000.0f);
	~Camera();

	void setVelocity(float Kvel, float Mvel);
	void setPosition(Vec<float,3> pos);
	void calculateProjectionMatrix(float fov, float width, float height, float zNear, float zFar);

	Vec<float,3> getPosition();

	Mat<float,4> getViewProjectionMatrix();
	Mat<float,4> getViewMatrix() const;
	Mat<float,4> getProjectionMatrix() const;

	void update(float fps);

private:

	Camera(){}
	Camera(const Camera&){}

	Mat<float,4> m_View, m_Projection;
	Vec<float,3> m_position;
	float m_rotx,m_roty,m_rotz;
	float m_keyboard_velocity,m_mouse_velocity;
	float m_zNear,m_zFar;
	Vec<unsigned int,2> m_oldMousePos;

	Vec<unsigned int,2> m_ScreenMiddle;
	Vec<unsigned int,2> m_ScreenSize;

	InputManager *m_Input;
};

#endif