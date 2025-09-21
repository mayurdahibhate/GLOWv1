#version 460 core

in vec4 aPosition;
in vec2 aTexCoord;

out vec2 oTexCoord;

uniform mat4 uMVPmatrix;

void main() {
    oTexCoord = aTexCoord;
    gl_Position = uMVPmatrix * aPosition;
}
