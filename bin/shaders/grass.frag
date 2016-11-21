#version 330

uniform vec4 inColor;
out vec4 outColor;

in vec2 TexCoord;
uniform sampler2D inTexture;

void main() {
    outColor = texture(inTexture, TexCoord);

    //if (outColor.x > 0.5)
    //      discard;
}
