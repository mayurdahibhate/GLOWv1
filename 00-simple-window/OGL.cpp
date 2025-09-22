#include <GLOW/window.hpp>

int main(int argc, char **argv) {

	GLOWwindow* window = glowCreateWindow("Hello window!", 600, 400);

	while (!(window->glowWindowShouldClose())) {

    }

	window->glowDestroyWindow();

	return 0;
}
