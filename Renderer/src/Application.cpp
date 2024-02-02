#include "Application.h"
#include "Core/GL.h"
#include "Core/Scene.h"
#include "Core/AssetManager.h"
#include "Renderer/Renderer.h"
#include "Editor/Editor.h"

void Application::Run()
{
	Init();


	double lastTime = glfwGetTime();

	while (!GL::WindowShouldClose())
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		GL::ProcessWindowInput();

		Scene::Update((float)deltaTime);
		Renderer::RenderFrame();
		Editor::RenderEditor();

		GL::PollEventsSwapBuffers();

		std::cout << deltaTime << "\n";
		lastTime = currentTime;
	}
}

void Application::Init()
{
	GL::Init(1280, 720, "Renderer");
	Editor::Init(GL::GetWindowPtr());
	AssetManager::LoadAssets();
	Scene::Init();

	Renderer::Init();
}
