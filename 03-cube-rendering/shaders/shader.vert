#version 460 core

in vec4 aPosition;
in vec3 aNormal;

out vec4 oColor;

uniform mat4 uModelViewProjectionMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uLightDirection;
uniform vec4 uColor;

void main() {

    vec3 normal = uNormalMatrix * aNormal;

 	oColor = uColor * 0.3 + uColor * max(dot(normal, uLightDirection), 0.0);
    gl_Position = uModelViewProjectionMatrix * aPosition;
}
