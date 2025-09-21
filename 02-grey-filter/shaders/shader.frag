#version 460 core

in vec2 oTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture; 

void main() {
	vec4 color = texture(uTexture, oTexCoord);

	if (oTexCoord.s >= 0.5) {
		float grey = color.r*0.299 + color.g*0.587 + color.b*0.114;
		FragColor = vec4(grey, grey, grey, 1.0f);
	}
	else {
		FragColor = color;
	}
}
