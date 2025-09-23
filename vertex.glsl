#version 330 core

void main()
{
	const vec4 vertex[4] =	vec4[4] (vec4(-0.25, -0.25, 0.5, 1.0),
							vec4(0.25, -0.25, 0.5, 1.0),
							vec4(0.0, 0.25, 0.5, 1.0),
							vec4(0.0, -0.5, 0.5, 1.0));

	gl_Position = vertex [gl_VertexID];
} 