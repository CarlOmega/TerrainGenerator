// Note used Open-tutorials tutorial to get started (common openGL code)
// Include standard headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "common/controls.hpp"

GLfloat* grid;
int sizex;
int sizez;
float minY = 1.0;
float maxY = -1.0;

// void getPreset() {
// 	GLfloat preset[] = {
// 	};
// 	memcpy(grid, preset, sizeof(preset));
// }


void square(int x, int z, int size, float value)
{
    int hs = size / 2;
	GLfloat a, b, c, d;
    a = grid[((x - hs) % (sizex + 1)) * (sizez + 1) + ((z - hs) % (sizez + 1))];
	b = grid[((x + hs) % (sizex + 1)) * (sizez + 1) + ((z - hs) % (sizez + 1))];
	c = grid[((x - hs) % (sizex + 1)) * (sizez + 1) + ((z + hs) % (sizez + 1))];
	d = grid[((x + hs) % (sizex + 1)) * (sizez + 1) + ((z + hs) % (sizez + 1))];

	if (x - hs < 0 && z - hs < 0) {
		a = grid[((x - hs + sizex + 1) % (sizex + 1)) * (sizez + 1) + ((z - hs + sizez + 1) % (sizez + 1))];
	} else if (z - hs < 0) {
		b = grid[((x + hs) % (sizex + 1)) * (sizez + 1) + ((z - hs + sizez + 1) % (sizez + 1))];
	} else if (x - hs < 0) {
		c = grid[((x - hs + sizex + 1) % (sizex + 1)) * (sizez + 1) + ((z + hs) % (sizez + 1))];
	}

	GLfloat avg = ((a + b + c + d) / 4.0) + value; 
	if (avg > maxY)
		maxY = avg;
	if (avg < minY)
		minY = avg;
	
	grid[(x % sizex) * (sizez + 1) + (z % sizez)] = avg;
}
 
void diamond(int x, int z, int size, float value)
{
    int hs = size / 2;
	GLfloat a, b, c, d;
	a = grid[((x - hs) % (sizex + 1)) * (sizez + 1) + (z % (sizez + 1))];
	b = grid[((x + hs) % (sizex + 1)) * (sizez + 1) + (z % (sizez + 1))];
	c = grid[(x % (sizex + 1)) * (sizez + 1) + ((z + hs) % (sizez + 1))];
	d = grid[(x % (sizex + 1)) * (sizez + 1) + ((z - hs) % (sizez + 1))];
	
	if (x - hs < 0) {
		a = grid[((x - hs + sizex + 1) % (sizex + 1)) * (sizez + 1) + (z % (sizez + 1))];
	}
	if (z - hs < 0) {
		d = grid[(x % (sizex + 1)) * (sizez + 1) + ((z - hs + sizez + 1) % (sizez + 1))];
	}

    
	GLfloat avg = ((a + b + c + d) / 4.0) + value; 
	if (avg > maxY)
		maxY = avg;
	if (avg < minY)
		minY = avg;
	grid[(x % (sizex + 1)) * (sizez + 1) + (z % (sizez + 1))] = avg;
}

void diamondSquare(int stepsize, float scale)
{
    int half = stepsize / 2;
 
    for (int z = half; z <= sizez + half; z += stepsize) {
        for (int x = half; x <= sizex + half; x += stepsize) {
            square(x, z, stepsize, ((float)rand() / RAND_MAX) * scale);
        }
    }
 
    for (int z = 0; z <= sizez + half; z += stepsize) {
        for (int x = 0; x <= sizex + half; x += stepsize) {
            diamond(x + half, z, stepsize, ((float)rand() / RAND_MAX) * scale);
			diamond(x, z + half, stepsize, ((float)rand() / RAND_MAX) * scale);
		}
    }
 
}

void getFractal() {
	int feature = 32;
	int samplesize = 32;

	double scale = 1.0;
	for (int z = 0; z <= sizez + feature; z += feature) {
		for (int x = 0; x <= sizex + feature; x += feature) {
			grid[(x % (sizex + 1)) * (sizez + 1) + (z % (sizez + 1))] = ((float)rand() / RAND_MAX) / 2.0;
		}
	}
	
	while (samplesize > 1) {
		diamondSquare(samplesize, scale);
		samplesize /= 2;
		scale /= 2.0;
	}
}

void getHeatMapColor(float value, float *red, float *green, float *blue)
{
	const int NUM_COLORS = 14;
	static float color[NUM_COLORS][3] = {
		{255 / 255.0, 255 / 255.0, 0},
		{255 / 255.0, 255 / 255.0, 0},
		{255 / 255.0, 255 / 255.0, 0},
		{255 / 255.0, 255 / 255.0, 0},
		{255 / 255.0, 255 / 255.0, 0},
		{255 / 255.0, 255 / 255.0, 0},
		{140 / 255.0, 224 / 255.0, 61 / 255.0},
		{140 / 255.0, 224 / 255.0, 61 / 255.0},
		{49 / 255.0, 99 / 255.0, 26 / 255.0},
		{27 / 255.0, 99 / 255.0, 18 / 255.0},
		{13 / 255.0, 33 / 255.0, 11 / 255.0},
		{99 / 255.0, 70 / 255.0, 26 / 255.0},
		{122 / 255.0, 122 / 255.0, 122 / 255.0},
		{203 / 255.0, 214 / 255.0, 211 / 255.0}};

	int idx1;				// |-- Our desired color will be between these two indexes in "color".
	int idx2;				// |
	float fractBetween = 0; // Fraction between "idx1" and "idx2" where our value is.

	if (value <= 0)
	{
		idx1 = idx2 = 0;
	} // accounts for an input <=0
	else if (value >= 1)
	{
		idx1 = idx2 = NUM_COLORS - 1;
	} // accounts for an input >=0
	else
	{
		value = value * (NUM_COLORS - 1);   // Will multiply value by 3.
		idx1 = floor(value);				// Our desired color will be after this index.
		idx2 = idx1 + 1;					// ... and before this index (inclusive).
		fractBetween = value - float(idx1); // Distance between the two indexes (0-1).
	}

	*red = (color[idx2][0] - color[idx1][0]) * fractBetween + color[idx1][0];
	*green = (color[idx2][1] - color[idx1][1]) * fractBetween + color[idx1][1];
	*blue = (color[idx2][2] - color[idx1][2]) * fractBetween + color[idx1][2];
}

void mapTerrain(GLfloat* grid, GLfloat* g_vertex_buffer_data, GLfloat* g_color_buffer_data, int sizex, int sizez) {
	float stepx = 5.0 / (float)sizex;
	float stepz = 5.0 / (float)sizex;
	for (int x = 0; x < sizex; x++) {
		for (int z = 0; z < sizez; z++) {
			int t10 = ((x * sizez) + z) * 18;
			int t11 = t10 + 3;
			int t12 = t10 + 6;

			int t20 = t10 + 9;
			int t21 = t10 + 12;
			int t22 = t10 + 15;

			// triangle 1 point 0,1,2 x y z
			g_vertex_buffer_data[t10] = (x * stepx);
			g_vertex_buffer_data[t10 + 1] = grid[x * (sizez + 1) + z];
			g_vertex_buffer_data[t10 + 2] = (z * stepz);
			g_vertex_buffer_data[t11] = (x * stepx + stepx);
			g_vertex_buffer_data[t11 + 1] = grid[(x + 1) * (sizez + 1) + z + 1];
			g_vertex_buffer_data[t11 + 2] = (z * stepz + stepz);
			g_vertex_buffer_data[t12] = (x * stepx + stepx);
			g_vertex_buffer_data[t12 + 1] = grid[(x + 1) * (sizez + 1) + z];
			g_vertex_buffer_data[t12 + 2] = (z * stepz);

			// triangle 2 point 0,1,2 x y z
			g_vertex_buffer_data[t20] = (x * stepx + stepx);
			g_vertex_buffer_data[t20 + 1] = grid[(x + 1) * (sizez + 1) + z + 1];
			g_vertex_buffer_data[t20 + 2] = (z * stepz + stepz);
			g_vertex_buffer_data[t21] = (x * stepx);
			g_vertex_buffer_data[t21 + 1] = grid[x * (sizez + 1) + z];
			g_vertex_buffer_data[t21 + 2] = (z * stepz);
			g_vertex_buffer_data[t22] = (x * stepx);
			g_vertex_buffer_data[t22 + 1] = grid[x * (sizez + 1) + z + 1];
			g_vertex_buffer_data[t22 + 2] = (z * stepz + stepz);

			float r;
			float g;
			float b;

			if (x == 0) {
				// top face
				int side = ((sizex * sizez) + z) * 18;
				g_vertex_buffer_data[side] = 0;
				g_vertex_buffer_data[side + 1] = grid[z];
				g_vertex_buffer_data[side + 2] = (z * stepz);
				g_vertex_buffer_data[side + 3] = 0;
				g_vertex_buffer_data[side + 4] = 0;
				g_vertex_buffer_data[side + 5] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 6] = 0;
				g_vertex_buffer_data[side + 7] = grid[z + 1];
				g_vertex_buffer_data[side + 8] = (z * stepz + stepz);

				g_vertex_buffer_data[side + 9] = 0;
				g_vertex_buffer_data[side + 10] = 0;
				g_vertex_buffer_data[side + 11] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 12] = 0;
				g_vertex_buffer_data[side + 13] = 0;
				g_vertex_buffer_data[side + 14] = (z * stepz);
				g_vertex_buffer_data[side + 15] = 0;
				g_vertex_buffer_data[side + 16] = grid[z];
				g_vertex_buffer_data[side + 17] = (z * stepz);

				for (int i = side; i < side + 18; i += 3) {
					g_color_buffer_data[i] = 191 / 255.0; 
					g_color_buffer_data[i + 1] = 171 / 255.0;
					g_color_buffer_data[i + 2] = 128 / 255.0; 
				}
			}
			if (z == 0) {
				int side = ((sizex * sizez) + sizez + x) * 18;
				g_vertex_buffer_data[side] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 1] = grid[(x + 1) * (sizez + 1)];
				g_vertex_buffer_data[side + 2] = 0;
				g_vertex_buffer_data[side + 3] = (x * stepx);
				g_vertex_buffer_data[side + 4] = 0;
				g_vertex_buffer_data[side + 5] = 0;
				g_vertex_buffer_data[side + 6] = (x * stepx);
				g_vertex_buffer_data[side + 7] = grid[x * (sizez + 1)];
				g_vertex_buffer_data[side + 8] = 0;

				g_vertex_buffer_data[side + 9] = (x * stepx);
				g_vertex_buffer_data[side + 10] = 0;
				g_vertex_buffer_data[side + 11] = 0;
				g_vertex_buffer_data[side + 12] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 13] = grid[(x + 1) * (sizez + 1)];
				g_vertex_buffer_data[side + 14] = 0;
				g_vertex_buffer_data[side + 15] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 16] = 0;
				g_vertex_buffer_data[side + 17] = 0;

				for (int i = side; i < side + 18; i += 3) {
					g_color_buffer_data[i] = 191 / 255.0; 
					g_color_buffer_data[i + 1] = 171 / 255.0;
					g_color_buffer_data[i + 2] = 128 / 255.0; 
				}
			}
			if (x == sizex - 1) {
				//side
				int side = (sizex * sizez + sizex + sizez + z) * 18;
				g_vertex_buffer_data[side] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 1] = grid[(x + 1) * (sizez+1) + z];
				g_vertex_buffer_data[side + 2] = (z * stepz);
				g_vertex_buffer_data[side + 3] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 4] = 0;
				g_vertex_buffer_data[side + 5] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 6] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 7] = grid[(x + 1) * (sizez+1) + z + 1];
				g_vertex_buffer_data[side + 8] = (z * stepz + stepz);

				g_vertex_buffer_data[side + 9] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 10] = 0;
				g_vertex_buffer_data[side + 11] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 12] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 13] = 0;
				g_vertex_buffer_data[side + 14] = (z * stepz);
				g_vertex_buffer_data[side + 15] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 16] = grid[(x + 1) * (sizez+1)+ z];
				g_vertex_buffer_data[side + 17] = (z * stepz);

				for (int i = side; i < side + 18; i += 3) {
					g_color_buffer_data[i] = 191 / 255.0; 
					g_color_buffer_data[i + 1] = 171 / 255.0;
					g_color_buffer_data[i + 2] = 128 / 255.0; 
				}
			}
			if (z == sizez - 1) {
				// bottom face
				int side = (sizex * sizez + 2 * sizez + sizex + x) * 18;
				g_vertex_buffer_data[side] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 1] = grid[(x + 1) * (sizez + 1) + z + 1];
				g_vertex_buffer_data[side + 2] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 3] = (x * stepx);
				g_vertex_buffer_data[side + 4] = 0;
				g_vertex_buffer_data[side + 5] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 6] = (x * stepx);
				g_vertex_buffer_data[side + 7] = grid[x * (sizez + 1) + z + 1];
				g_vertex_buffer_data[side + 8] = (z * stepz + stepz);

				g_vertex_buffer_data[side + 9] = (x * stepx);
				g_vertex_buffer_data[side + 10] = 0;
				g_vertex_buffer_data[side + 11] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 12] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 13] = grid[(x + 1) * (sizez + 1) + z + 1];
				g_vertex_buffer_data[side + 14] = (z * stepz + stepz);
				g_vertex_buffer_data[side + 15] = (x * stepx + stepx);
				g_vertex_buffer_data[side + 16] = 0;
				g_vertex_buffer_data[side + 17] = (z * stepz + stepz);

				for (int i = side; i < side + 18; i += 3) {
					g_color_buffer_data[i] = 191 / 255.0; 
					g_color_buffer_data[i + 1] = 171 / 255.0;
					g_color_buffer_data[i + 2] = 128 / 255.0; 
				}
			}

			// triangle 1 colour 0,1,2 r g b
			getHeatMapColor((grid[x * (sizez + 1) + z] + 1) / 2.0, &r, &g, &b);
			g_color_buffer_data[t10] = r;
			g_color_buffer_data[t10 + 1] = g;
			g_color_buffer_data[t10 + 2] = b;
			g_color_buffer_data[t21] = r;
			g_color_buffer_data[t21 + 1] = g;
			g_color_buffer_data[t21 + 2] = b;

			getHeatMapColor((grid[(x + 1) * (sizez + 1) + z + 1] + 1) / 2.0, &r, &g, &b);
			g_color_buffer_data[t11] = r;
			g_color_buffer_data[t11 + 1] = g;
			g_color_buffer_data[t11 + 2] = b;
			g_color_buffer_data[t20] = r;
			g_color_buffer_data[t20 + 1] = g;
			g_color_buffer_data[t20 + 2] = b;

			getHeatMapColor((grid[(x + 1) * (sizez + 1) + z] + 1) / 2.0, &r, &g, &b);
			g_color_buffer_data[t12] = r;
			g_color_buffer_data[t12 + 1] = g;
			g_color_buffer_data[t12 + 2] = b;

			// triangle 2 colour 0,1,2 r g b
			getHeatMapColor((grid[x * (sizez + 1) + z + 1] + 1) / 2.0, &r, &g, &b);
			g_color_buffer_data[t22] = r;
			g_color_buffer_data[t22 + 1] = g;
			g_color_buffer_data[t22 + 2] = b;
		}
	}
}

int main(void)
{
	srand(time(NULL));
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Terrain", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	// glEnable(GL_CULL_FACE);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	sizex = 128;
	sizez = 96;
	float stepx = 5.0 / (float)sizex;
	float stepz = 5.0 / (float)sizex;
	
	int s = (sizex * sizez + 2 * (sizex + sizez)) * 18 + 36;
	GLfloat *g_vertex_buffer_data = new GLfloat[s];
	GLfloat *g_color_buffer_data = new GLfloat[s];
	grid = new GLfloat[(sizex + 1) * (sizez + 1)]();
	getFractal();

	for (int i = 0; i < (sizex + 1)*(sizez + 1); i++) {
		grid[i] = ((grid[i] -  minY) / (maxY - minY));
	}
	mapTerrain(grid, g_vertex_buffer_data, g_color_buffer_data, sizex, sizez);
	

	// water
	float waterheight = 0.2;
	int water = s - 18;
	g_vertex_buffer_data[water + 0] = 0;
	g_vertex_buffer_data[water + 2] = 0;

	g_vertex_buffer_data[water + 3] = stepx * sizex;
	g_vertex_buffer_data[water + 5] = stepz * sizez;

	g_vertex_buffer_data[water + 6] = stepx * sizex;
	g_vertex_buffer_data[water + 8] = 0;

	g_vertex_buffer_data[water + 9] = stepx * sizex;
	g_vertex_buffer_data[water + 11] = stepz * sizez;

	g_vertex_buffer_data[water + 12] = 0;
	g_vertex_buffer_data[water + 14] = stepz * sizez;

	g_vertex_buffer_data[water + 15] = 0;
	g_vertex_buffer_data[water + 17] = 0;

	for (int i = water; i < s; i += 3) {
		g_color_buffer_data[i + 0] = 0;
		g_color_buffer_data[i + 1] = 0;
		g_color_buffer_data[i + 2] = 1;
	}
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data) * s, g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data) * s, g_color_buffer_data, GL_STATIC_DRAW);

	do
	{
		g_vertex_buffer_data[water + 1] = waterheight;
		g_vertex_buffer_data[water + 4] = waterheight;
		g_vertex_buffer_data[water + 7] = waterheight;
		g_vertex_buffer_data[water + 10] = waterheight;
		g_vertex_buffer_data[water + 13] = waterheight;
		g_vertex_buffer_data[water + 16] = waterheight;

		
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			waterheight = waterheight + 0.01 > 0.7 ? waterheight : waterheight + 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			waterheight = waterheight - 0.01 < 0.0 ? 0.0 : waterheight - 0.01;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data) * s, g_vertex_buffer_data, GL_STATIC_DRAW);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,		  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void *)0 // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,		  // attribute. No particular reason for 1, but must match the layout in the shader.
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void *)0 // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, s / 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		usleep(16667);

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	delete[] g_vertex_buffer_data;
	delete[] g_color_buffer_data;

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
