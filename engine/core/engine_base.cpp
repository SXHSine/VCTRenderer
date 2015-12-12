#include "engine_base.h"

#include "deferred_renderer.h"
#include "engine_assets.h"
#include "../misc/utils.h"

#include <oglplus/gl.hpp>

// initializes base engine assets and libs
EngineBase::EngineBase()
{
}


EngineBase::~EngineBase()
{
}

std::shared_ptr<EngineBase> &EngineBase::Instance()
{
    static std::shared_ptr<EngineBase> instance = nullptr;

    if (!instance)
    {
        instance.reset(new EngineBase());
    }

    return instance;
}

void EngineBase::MainLoop()
{
    static oglplus::Context gl;
    // import assets and initialize ext libraries
    this->Initialize();
    // set rendering view port
    gl.Viewport(1280, 720);

    // render loop
    while (!renderWindow.ShouldClose())
    {
        gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.Clear().ColorBuffer().DepthBuffer();
        // poll window inputs
        renderWindow.Events();
        // draw custom engine ui
        userInterface.NewFrame();
        userInterface.Draw();
        // render main scene
        renderer->Render();
        // ui render over scene
        userInterface.Render();
        // finally swap current frame
        renderWindow.SwapBuffers();
    }

    userInterface.Terminate();
    // release reserved data early (context dependent)
    assetLoader.reset(nullptr);
    renderer.reset(nullptr);
}

void EngineBase::Initialize()
{
    // open window and set rendering context
    renderWindow.WindowHint(RenderWindow::WindowHints::Resizable, false);
    renderWindow.Open(WindowInfo(1280, 720, 0, 0, "VCTRenderer"), false);
    renderWindow.SetAsCurrentContext();
    // initialize OpenGL API
    oglplus::GLAPIInitializer();
    // set interface to current renderwindow
    userInterface.Initialize(renderWindow);
    // print libs version info
    utils::PrintDependenciesVersions();
    // load engine demo scene assets
    assetLoader = std::make_unique<EngineAssets>();
    assetLoader->LoadScenes();
    // initialize deferred shading renderer / manager
    renderer = std::make_unique<DeferredRenderer>(renderWindow);
    renderer->Initialize();
}