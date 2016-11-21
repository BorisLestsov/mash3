#version 330

in vec4 point;
out vec2 TexCoord;
uniform mat4 camera;

void main() {
    gl_Position = camera * point;
    TexCoord = vec2(point.x, point.z);
}