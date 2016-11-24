#version 330

in vec2 TexCoord;

uniform vec4 inColor;
uniform sampler2D inTexture;

out vec4 outColor;

void main() {
    vec4 col = texture(inTexture, TexCoord + vec2(0, 2));
    if (col.x < 0.07){
        discard;
    } else {
        outColor = col*(0.9*pow(TexCoord.y, 0.4) + vec4(0.1, 0.1, 0.1, 0));
    }
}
