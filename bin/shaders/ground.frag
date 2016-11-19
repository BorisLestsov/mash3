#version 330

out vec4 outColor;

void main() {
    outColor = vec4(0.5, 0.5, 0, 0);
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