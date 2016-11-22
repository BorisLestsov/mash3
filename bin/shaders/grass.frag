#version 330

in vec2 TexCoord;

uniform vec4 inColor;
uniform sampler2D inTexture;

out vec4 outColor;
//varying int instanceID;

void main() {
    outColor = texture(inTexture, TexCoord)*(0.9*pow(TexCoord.y, 0.3) + vec4(0.1, 0.1, 0.1, 0));

    //if (outColor.x > 0.5)
    //      discard;
}
