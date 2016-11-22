#version 330

in vec2 TexCoord;

uniform vec4 inColor;
uniform sampler2D inTexture;

out vec4 outColor;


void main() {
    outColor = texture(inTexture, TexCoord);

    //if (outColor.x > 0.5)
    //      discard;
}
