#version 460 core

in vec4 aPosition;
in vec4 aColor;

out vec4 oColor;

uniform mat4 uMVPmatrix;

void main() {
    gl_Position = uMVPmatrix * aPosition;
    oColor = aColor;
}
