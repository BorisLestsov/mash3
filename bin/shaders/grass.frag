#version 330

in vec2 TexCoord;

uniform vec4 inColor;
uniform sampler2D inTexture;
uniform float GRASS_HEIGHT;

out vec4 outColor;
//varying int instanceID;

void main() {
    outColor = texture(inTexture, TexCoord)*pow(TexCoord.y, 0.5);

    //if (outColor.x > 0.5)
    //      discard;
}
