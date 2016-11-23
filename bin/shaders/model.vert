#version 330

in vec4 point;
in vec2 TexCoord_in;
out vec2 TexCoord;
uniform mat4 camera;

void main() {
    mat4 rotMatrix = mat4(0.0);
    rotMatrix[0][0] = cos(0.5);
    rotMatrix[1][1] = 1.0;
    rotMatrix[0][2] = sin(0.5);
    rotMatrix[2][0] = -sin(0.5);
    rotMatrix[2][2] = cos(0.5);
    rotMatrix[3][3] = 1.0;



	gl_Position = camera * (rotMatrix * point + vec4(1.5, 0, 1.5, 0.3));

	TexCoord = TexCoord_in;
}
