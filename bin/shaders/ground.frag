#version 330

out vec4 outColor;
in vec2 TexCoord;

uniform sampler2D inTexture;

void main() {
    outColor = texture(inTexture, TexCoord);
}
