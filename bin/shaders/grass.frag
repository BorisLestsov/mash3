#version 330

in vec2 TexCoord;

uniform vec4 inColor;
uniform sampler2D inTexture;

out vec4 outColor;
flat in int InstanceID;

void main() {
     outColor = texture(inTexture, TexCoord)*(0.9*pow(TexCoord.y, 0.8) + vec4(0.1, 0.1, 0.1, 0))
        + vec4(abs(sin(InstanceID))*0.15, abs(sin(InstanceID))*0.15, 0, 0);
}
