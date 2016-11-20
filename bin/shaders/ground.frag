#version 330

out vec4 outColor;
in vec2 TexCoord;

uniform sampler2D inTexture;

void main() {
    outColor = texture(inTexture, TexCoord);
}

//#version 330 core
//
//in vec2 TexCoords;
//out vec4 color;
//
//uniform sampler2D gr_text;
//
//void main() {
//    color = texture(gr_text, TexCoords);
//}