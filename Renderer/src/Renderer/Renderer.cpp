#include "Renderer.h"
#include "../Common.h"
#include "../Core/GL.h"
#include "../Core/Scene.h"
#include "../Core/AssetManager.h"
#include "Shader.h"
#include "Skybox.h"
#include "GBuffer.h"

struct Shaders
{
	Shader color;
	Shader lighting;
	Shader skybox;
	Shader screen;
	Shader geometry;
	Shader depth;
} shaders;

GBuffer gbuffer;
Skybox sky;

void DrawScene(Shader& shader);
void GeometryPass();
void RenderFullscreenQuad();

void Renderer::Init()
{
	shaders.color.Load("res/shaders/color.vert", "res/shaders/color.frag");
	shaders.lighting.Load("res/shaders/lighting.vert", "res/shaders/lighting.frag");
	shaders.skybox.Load("res/shaders/skybox.vert", "res/shaders/skybox.frag");
	shaders.screen.Load("res/shaders/screen.vert", "res/shaders/screen.frag");
	shaders.geometry.Load("res/shaders/geometry.vert", "res/shaders/geometry.frag");
	shaders.depth.Load("res/shaders/depth.vert", "res/shaders/depth.frag");

	gbuffer.Init(GL::GetWindowWidth(), GL::GetWindowHeight());

	std::vector<std::string> skyboxTextureFilepaths
	{
		"res/textures/space/right.png",
		"res/textures/space/left.png",
		"res/textures/space/top.png",
		"res/textures/space/bottom.png",
		"res/textures/space/front.png",
		"res/textures/space/back.png"
	};

	sky.Load(skyboxTextureFilepaths);
}

void Renderer::RenderFrame()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GeometryPass();

	shaders.screen.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexture);
	glDisable(GL_DEPTH_TEST);
	RenderFullscreenQuad();
}

void DrawScene(Shader& shader)
{
	//Skybox - Make its own pass?
	glDepthMask(GL_FALSE);
	shaders.skybox.Bind();
	glm::mat4 modifiedViewMatrix = glm::mat4(glm::mat3(Scene::camera.GetView()));
	shaders.skybox.SetMat4("projection", Scene::camera.GetProjection());
	shaders.skybox.SetMat4("view", modifiedViewMatrix);
	sky.Draw();
	shaders.skybox.Bind();
	glDepthMask(GL_TRUE);

	//Render GameObjects
	shader.Bind();
	for (GameObject& gameObject : Scene::gameObjects)
	{
		if (!gameObject.active) { continue; }
		shader.SetMat4("model", gameObject.transform.GetModelMatrix());
		gameObject.material.color.Bind(0);
		gameObject.material.normal.Bind(1);
		gameObject.material.rma.Bind(2);
		gameObject.model->Draw();
	}
}

void GeometryPass()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	gbuffer.Bind();
	uint32_t attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(sizeof(attachments) / sizeof(uint32_t), attachments);

	shaders.geometry.Bind();
	shaders.geometry.SetMat4("projection", Scene::camera.GetProjection());
	shaders.geometry.SetMat4("view", Scene::camera.GetView());
	DrawScene(shaders.geometry);
	gbuffer.Unbind();
}

void RenderFullscreenQuad()
{
	static uint32_t vao = 0;
	if (vao == 0)
	{
		float quadVertices[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f
		};
		uint32_t vbo;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}