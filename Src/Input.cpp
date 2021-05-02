#include "pch.h"
#include "Input.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// Mouse tutorial globals
namespace
{
	constexpr float ROTATION_GAIN = 0.08f;
	constexpr float MOVEMENT_GAIN = 0.07f;
}

Input::Input() : m_MouseMode(DirectX::Mouse::MODE_ABSOLUTE)
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
    m_mouse->SetMode(m_MouseMode);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;

	m_MouseMovement.pitch	= 0.0f;
	m_MouseMovement.yaw		= 0.0f;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	// toggle active camera
	m_GameInput.toggleCamera = m_KeyboardTracker.pressed.C;

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	UpdateMouseMovement(mouse);

	UpdateKeyboardInput(kb);
}

bool Input::UpdateMouseMode()
{
    auto kb = m_keyboard->GetState();	//updates the basic keyboard state
    m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 

    if (kb.Escape)// check has escape been pressed.  if so, quit out. 
    {
        m_quitApp = true;
        return false;
    }

    if ((m_KeyboardTracker.pressed.F1 && m_MouseMode == Mouse::MODE_RELATIVE) || (m_KeyboardTracker.pressed.Enter && m_MouseMode == Mouse::MODE_ABSOLUTE))
    {
        m_MouseMode = m_MouseMode == Mouse::MODE_RELATIVE ? Mouse::MODE_ABSOLUTE : Mouse::MODE_RELATIVE;
        m_mouse->SetMode(m_MouseMode);
        return true;
    }

    return false;
}

void Input::UpdateKeyboardInput(DirectX::Keyboard::State &kb)
{
	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;

	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;
}

void Input::UpdateMouseMovement(DirectX::Mouse::State &mouse)
{
	Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * ROTATION_GAIN;

	m_MouseMovement.pitch = -delta.y;
	m_MouseMovement.yaw = -delta.x;

	// limit pitch to straight up or straight down
	// with a little fudge-factor to avoid gimbal lock
	float limit = XM_PI / 2.0f - 0.01f;
	m_MouseMovement.pitch = std::max(-limit, m_MouseMovement.pitch);
	m_MouseMovement.pitch = std::min(+limit, m_MouseMovement.pitch);
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}

MouseMovement Input::getMouseMovement()
{
	return m_MouseMovement;
}
