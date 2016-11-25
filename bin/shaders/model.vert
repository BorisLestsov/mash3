#version 330

layout (location = 0) in vec4 point;
layout (location = 1) in vec2 TexCoord_in;

out vec2 TexCoord;
uniform mat4 camera;
uniform vec4 pos;

void main() {
    mat4 rotMatrix = mat4(0.0);
    rotMatrix[0][0] = cos(0.5);
    rotMatrix[1][1] = 1.0;
    rotMatrix[0][2] = sin(0.5);
    rotMatrix[2][0] = -sin(0.5);
    rotMatrix[2][2] = cos(0.5);
    rotMatrix[3][3] = 1.0;

	gl_Position = camera * (rotMatrix * point + pos);

	TexCoord = TexCoord_in;
}
