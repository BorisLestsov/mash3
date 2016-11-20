#version 330

in vec4 point;
out vec2 TexCoord;
uniform mat4 camera;

void main() {
    gl_Position = camera * point;
    TexCoord = vec2(point.x, point.z);
}



//
//#version 330 core
//layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color;
//layout (location = 2) in vec2 texCoord;
//out vec3 ourColor;
//out vec2 TexCoord;
//
//void main()
//{
//    //gl_Position = vec4(position, 1.0f);
//    gl_Position = camera * point;
//    ourColor = color;
//    TexCoord = texCoord;
//}