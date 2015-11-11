#include "stdafx.h"
#include "engine_base.h"


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
    using namespace oglplus;
    // import assets and initialize ext libraries
    this->Initialize();
    gl.Viewport(1280, 720);
    int sceneCount = assetLoader->SceneCount();

    // render loop
    while (!glfwWindowShouldClose(renderWindow.Handler()))
    {
        gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.Clear().ColorBuffer().DepthBuffer();
        // poll window inputs
        glfwPollEvents();
        // draw custom engine ui
        userInterface.NewFrame();
        userInterface.Draw();

        if (execInfo.activeScene >= 0 && execInfo.activeScene < sceneCount)
        {
            Scene &activeScene = assetLoader->GetScene(execInfo.activeScene);
            renderer->Render(activeScene);
        }

        // ui render over scene
        userInterface.Render();
        // finally swap current frame
        glfwSwapBuffers(renderWindow.Handler());
    }

    userInterface.Terminate();
    // release reserved data early (context dependent)
    assetLoader.reset(nullptr);
    renderer.reset(nullptr);
}

void EngineBase::Initialize()
{
    // open window and set rendering context
    renderWindow.Open(); // creates rendering context
    renderWindow.SetAsCurrentContext();
    // initialize context dependant external libs
    initializer.InitializeContextDependant();
    // set interface to current renderwindow
    userInterface.Initialize(renderWindow);
    // load engine demo scene assets
    assetLoader = std::unique_ptr<EngineAssets>(new EngineAssets());
    assetLoader->LoadAssets();
    // initialize deferred shading renderer / manager
    renderer = std::unique_ptr<DeferredRenderer>(new DeferredRenderer(
                   renderWindow));
    renderer->Initialize();
}
