#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../Common.h"

namespace Editor
{
	void Init(GLFWwindow* window);
	void RenderEditor();
	void RenderEditorBegin();
	void RenderEditorEnd();
}