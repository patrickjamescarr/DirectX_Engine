//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "PlayerCamera.h"


//toreorganise
#include <fstream>
#include <comdef.h>

// Mouse tutorial globals
namespace
{
    const XMVECTORF32 START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
    const XMVECTORF32 ROOM_BOUNDS = { 8.f, 6.f, 12.f, 0.f };
}

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ImGui;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);

    m_playerCamera = std::make_shared<PlayerCamera>();
    m_activeCamera = std::make_shared<Camera>();
    m_renderToTextureCamera = std::make_shared<Camera>();
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

    m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    //setup imgui.  its up here cos we need the window handle too
    //pulled from imgui directx11 example
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window);		//tie to our window
    ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());	//tie to directx

    m_fullscreenRect.left = 0;
    m_fullscreenRect.top = 0;
    m_fullscreenRect.right = 800;
    m_fullscreenRect.bottom = 600;

    m_CameraViewRect.left = 500;
    m_CameraViewRect.top = 0;
    m_CameraViewRect.right = 800;
    m_CameraViewRect.bottom = 240;

    //setup camera

    m_titlePosition = Vector3(23.9194317, 0.648975432, 13.3919287);
    m_titleRotation = Vector3(-83.244370, -48.061272, 180.000000);

    m_currentPosition = Vector3(5.304, 2.8422, 23.4729);
    m_currentRotation = Vector3(-91.12, -79.95, 180.000000);

    m_playerCamera->initPosition(m_titlePosition);
    m_playerCamera->setHeight(0.648975432);
    m_playerCamera->setRotation(m_titleRotation);	//orientation is -90 becuase zero will be looking up at the sky straight up. 

    m_renderToTextureCamera->setPosition(Vector3(9.f, 1.0f, 6.f));
    m_renderToTextureCamera->setRotation(Vector3(-85.6783371, 55.7404251, 180.000000));//orientation is -90 becuase zero will be looking up at the sky straight up. 

    // Set main camera as the active camera
    m_activeCamera = m_playerCamera;

    // Initialise viewing frustum
    m_frustum->ConstructFrustum(m_screeDepth, m_deviceResources->GetProjection(), m_activeCamera->getCameraMatrix());

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    //m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"Sound\\underwater-loop-amb.wav");
    m_effect1 = m_soundEffect->CreateInstance();

    //m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    //m_effect2->Play();
#endif

    m_mainScene.Initialise(*m_deviceResources, m_playerCamera.get(), m_activeCamera.get(), m_frustum.get(), m_audEngine.get());

    UpdateCameras();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    if (m_input.UpdateMouseMode())
    {
        if (m_input.AllowGameInput())
        {
            m_showInfo = false;
            m_playerCamera->initPosition(m_currentPosition);
            m_playerCamera->setHeight(m_currentPosition.y);
            m_playerCamera->setRotation(m_currentRotation);
            UpdateCameraView();
        }
        else
        {
            m_showInfo = true;

            m_currentPosition = m_playerCamera->getPosition();
            m_currentRotation = m_playerCamera->getRotation();

            m_playerCamera->initPosition(m_titlePosition);
            m_playerCamera->setHeight(m_titlePosition.y);
            m_playerCamera->setRotation(m_titleRotation);
            UpdateCameraView();
        }
    }

    if (m_input.AllowGameInput())
    {
        m_input.Update();								//update the hardware
        m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
        m_mouseMovement = m_input.getMouseMovement();
    }

    //Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    //Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    /*create our UI*/
    SetupGUI();

    if (m_input.AllowGameInput())
    {
        UpdateCameras();
    }

    //m_terrain.Update(m_deviceResources->GetD3DDevice());

    m_view = m_activeCamera->getCameraMatrix();
    m_deviceResources->SetView(m_view);
    m_world = Matrix::Identity;

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetDeltaTime();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

    Vector3 rotation = m_activeCamera->getRotation();

    if (ImGui::Begin("Performance"))
    {
        ImGui::Text("FPS: %d", m_timer.GetFramesPerSecond());
        //ImGui::Text("rotation: %f, %f, %f", rotation.x, rotation.y, rotation.z);
    }

    ImGui::End();

    if (m_input.Quit())
    {
        ExitGame();

        //m_deviceResources->OutputDebugInfo();
    }
}
void Game::UpdateCameras()
{
    //// Toggle camera
    //if (m_gameInputCommands.toggleCamera)
    //{
    //    m_activeCamera = m_activeCamera == m_playerCamera ? m_renderToTextureCamera : m_playerCamera;
    //}

    // Rotation
    Vector3 rotation = m_activeCamera->getRotation();
    rotation.y += m_mouseMovement.yaw * m_activeCamera->getRotationSpeed() * m_timer.GetDeltaTime();
    rotation.x += m_mouseMovement.pitch *  m_activeCamera->getRotationSpeed() * m_timer.GetDeltaTime() * 3.0f;
    m_activeCamera->setRotation(rotation);

    // Position
    Vector3 position = m_activeCamera->getPosition(); //get the position
    if (m_gameInputCommands.left)
    {
        position -= (m_activeCamera->getRight() * m_activeCamera->getMoveSpeed() * m_timer.GetDeltaTime()); // strafe left
    }
    if (m_gameInputCommands.right)
    {
        position += (m_activeCamera->getRight() * m_activeCamera->getMoveSpeed() * m_timer.GetDeltaTime()); // strafe right
    }
    if (m_gameInputCommands.forward)
    {
        position += (m_activeCamera->getForward() * m_activeCamera->getMoveSpeed() * m_timer.GetDeltaTime()); // move forward 
    }
    if (m_gameInputCommands.back)
    {
        position -= (m_activeCamera->getForward() * m_activeCamera->getMoveSpeed() * m_timer.GetDeltaTime()); // move backwards
    }
    m_activeCamera->setPosition(position);



    UpdateCameraView();
}
void Game::UpdateCameraView()
{
    m_activeCamera->Update();	//camera update.
    m_renderToTextureCamera->Update();

    // update view frustum
    m_frustum->ConstructFrustum(m_screeDepth, m_deviceResources->GetProjection(), m_activeCamera->getCameraMatrix());
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();



    //Set Rendering states. 
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());
    //	context->RSSetState(m_states->Wireframe());

    m_mainScene.Draw(*m_deviceResources, m_timer);

    //render our GUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Draw Text to the screen
    m_sprites->Begin();
    if (m_showInfo)
    {
        m_titleFont->DrawString(m_sprites.get(), L"Sunken Treasure", XMFLOAT2(50, 50), Colors::Gold);
        m_subTitleFont->DrawString(m_sprites.get(), L"Press Enter to start", XMFLOAT2(50, 500), Colors::Gold);
    }
    else 
    {
        std::string chestsFound = "Treasure chests found: " + std::to_string(m_playerCamera->treasureChestsFound()) + " / 23";
        m_hudFont->DrawString(m_sprites.get(), chestsFound.c_str(), XMFLOAT2(10, 10), Colors::Gold);
    }

    m_sprites->End();

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    XMVECTORF32 darkBlue = { { { 0.0f, 0.412f, 0.58f, 1.000000000f } } };

    context->ClearRenderTargetView(renderTarget, darkBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1600;
    height = 1200;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_titleFont = std::make_unique<SpriteFont>(device, L"treasureMapDeadhand_150.spritefont");
    m_subTitleFont = std::make_unique<SpriteFont>(device, L"treasureMapDeadhand_72.spritefont");
    m_hudFont = std::make_unique<SpriteFont>(device, L"treasureMapDeadhand.spritefont");
    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_frustum = std::make_unique<ViewingFrustum>();

    
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        m_screeDepth
    );

    m_deviceResources->SetProjection(m_projection);
}

void Game::SetupGUI()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_titleFont.reset();
    m_subTitleFont.reset();
    m_hudFont.reset();
    m_batch.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
