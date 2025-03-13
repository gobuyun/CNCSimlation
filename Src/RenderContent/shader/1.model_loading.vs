#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int intersects;

void main()
{
    TexCoords = aTexCoords;
	vec3 useColor = vec3(0.7, 0.7, 0.75) * (1-intersects);
	vec3 interectsColor = vec3(1.0, 0.0, 0.0) * intersects;
	useColor = useColor + interectsColor;
    vec3 eyeSpaceNormal = normalize((view * vec4(aNormal, 0.0)).xyz);
	vec3 diffuse = useColor * abs(dot(eyeSpaceNormal, vec3(0.0, 0.0, 0.7)));
	vec3 ambient = useColor * 0.4;
    fColor = diffuse + ambient;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
