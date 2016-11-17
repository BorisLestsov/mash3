#version 330

uniform vec4 inColor;
out vec4 outColor;

void main() {
    //outColor = vec4(0, 1, 0, 0);
    outColor = inColor;
}
