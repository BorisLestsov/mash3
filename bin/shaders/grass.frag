#version 330

uniform vec4 inColor;
out vec4 outColor;

in vec2 TexCoord;
uniform sampler2D inTexture;

void main() {
    //outColor = vec4(0, 1, 0, 0);
    //outColor = inColor;
    outColor = texture(inTexture, TexCoord);
}
