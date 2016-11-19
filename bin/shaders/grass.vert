#version 330

in vec4 point;
in vec2 position;
in vec4 variance;

uniform mat4 camera;

void main() {
    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = 0.01;
    scaleMatrix[1][1] = 0.1;
    mat4 positionMatrix = mat4(1.0);
    positionMatrix[3][0] = position.x;
    positionMatrix[3][2] = position.y;

    // random rotation matrix
    mat4 rotMatrix = mat4(0.0);
    rotMatrix[0][0] = sin(gl_InstanceID);
    rotMatrix[1][1] = 1.0;
    rotMatrix[0][2] = cos(gl_InstanceID);
    rotMatrix[2][0] = -sin(gl_InstanceID);
    rotMatrix[2][2] = cos(gl_InstanceID);
    rotMatrix[3][3] = 1.0;

	gl_Position = camera * (positionMatrix * rotMatrix * scaleMatrix * point + variance * (pow(point.y, 1.4)));
}
