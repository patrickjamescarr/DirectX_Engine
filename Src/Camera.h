#pragma once
class Camera
{
public:
	Camera();
	~Camera();

	void							Update();
	DirectX::SimpleMath::Matrix		getCameraMatrix();
	DirectX::SimpleMath::Vector3	getPosition() const;
	DirectX::SimpleMath::Vector3	getForward();
	DirectX::SimpleMath::Vector3	getRight();
	DirectX::SimpleMath::Vector3	getRotation();
	float							getMoveSpeed();
	float							getRotationSpeed();

    DirectX::SimpleMath::Vector3    forwardHeading;
    DirectX::SimpleMath::Vector3    rightHeading;
    DirectX::SimpleMath::Vector3    upHeading;

    virtual void    setPosition(DirectX::SimpleMath::Vector3 newPosition);
    virtual void    setRotation(DirectX::SimpleMath::Vector3 newRotation);
protected:
	DirectX::SimpleMath::Matrix		m_cameraMatrix;			//camera matrix to be passed out and used to set camera position and angle for wrestling
	DirectX::SimpleMath::Vector3	m_lookat;
	DirectX::SimpleMath::Vector3	m_position;
	DirectX::SimpleMath::Vector3	m_forward;
	DirectX::SimpleMath::Vector3	m_right;
	DirectX::SimpleMath::Vector3	m_up;
	DirectX::SimpleMath::Vector3	m_orientation;			//vector storing pitch yaw and roll. 

	// convert from degrees to radians
	float rad(float degrees)
	{
		return degrees * 3.1415f / 180.0f;
	}

	float	m_movespeed;	
	float	m_camRotRate;
	bool	m_active;
    float   m_height;
};

