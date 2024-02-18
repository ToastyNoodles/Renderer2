#version 420 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 fWorldPos;
out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fTangent;
out vec3 fBitangent;

void main()
{
	mat4 normalMatrix = transpose(inverse(model));

	fWorldPos = model * vec4(aPosition, 1.0);
	fNormal = normalize((normalMatrix * vec4(aNormal, 0)).xyz);
	fTexCoord = aTexCoord;
	fTangent = (model * vec4(aTangent, 0.0)).xyz;
	fBitangent = normalize(cross(fNormal, fTangent));

	gl_Position = projection * view * fWorldPos;
}