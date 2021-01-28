#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
using namespace std;

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "common/tangentspace.hpp"

#include <iostream>
#include <random>

#include "controls.hpp"
#include "light.h"

std::random_device rd;
std::mt19937 e2(rd());
std::uniform_real_distribution<> dist(0.0f, 1.0f);

GLFWwindow* window;
void computeNormals(GLfloat* vertexarray, vector<glm::vec3>& normals);

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 1024;
	int height = 768;

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Playground", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Set the mouse at the center of the screen
	glfwSetCursorPos(window, width / 2, height / 2);

	//Creating VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create  and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");

	// Get a handle for our "MVP" uniform
		// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");

	// Load texture
	GLuint Texture = loadBMP_custom("uvtemplate.bmp");
	GLuint NormalTexture = loadBMP_custom("normal.bmp");
#pragma region cube

	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	const int nColor = 3 * 3 * 12;

	// One color for each vertex. They were generated randomly.
	static GLfloat g_color_buffer_data[nColor];

	for (int i = 0; i < std::size(g_color_buffer_data); i += 9)
	{
		float randomColorR = dist(e2);
		float randomColorG = dist(e2);
		float randomColorB = dist(e2);

		//std::cout << "Color triangle " << i / 9 << " : " << randomColorR << ", " << randomColorG << ", " << randomColorB << std::endl;

		g_color_buffer_data[i] = randomColorR;
		g_color_buffer_data[i + 1] = randomColorG;
		g_color_buffer_data[i + 2] = randomColorB;

		g_color_buffer_data[i + 3] = randomColorR;
		g_color_buffer_data[i + 4] = randomColorG;
		g_color_buffer_data[i + 5] = randomColorB;

		g_color_buffer_data[i + 6] = randomColorR;
		g_color_buffer_data[i + 7] = randomColorG;
		g_color_buffer_data[i + 8] = randomColorB;
	}

	vector<glm::vec3>normals;

	computeNormals(g_vertex_buffer_data, normals);

	// This will identity our vertex buffer
	GLuint vertexBuffer;
	// Generate 1 buffer, put the resulting identifier in vertexBuffer
	glGenBuffers(1, &vertexBuffer);
	// The following commands will talk about our 'vertexBuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Give our vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	static const GLfloat g_uv_buffer_dataCube[] = {
	0.000059f, 1.0f - 0.000004f,
	0.000103f, 1.0f - 0.336048f,
	0.335973f, 1.0f - 0.335903f,
	1.000023f, 1.0f - 0.000013f,
	0.667979f, 1.0f - 0.335851f,
	0.999958f, 1.0f - 0.336064f,
	0.667979f, 1.0f - 0.335851f,
	0.336024f, 1.0f - 0.671877f,
	0.667969f, 1.0f - 0.671889f,
	1.000023f, 1.0f - 0.000013f,
	0.668104f, 1.0f - 0.000013f,
	0.667979f, 1.0f - 0.335851f,
	0.000059f, 1.0f - 0.000004f,
	0.335973f, 1.0f - 0.335903f,
	0.336098f, 1.0f - 0.000071f,
	0.667979f, 1.0f - 0.335851f,
	0.335973f, 1.0f - 0.335903f,
	0.336024f, 1.0f - 0.671877f,
	1.000004f, 1.0f - 0.671847f,
	0.999958f, 1.0f - 0.336064f,
	0.667979f, 1.0f - 0.335851f,
	0.668104f, 1.0f - 0.000013f,
	0.335973f, 1.0f - 0.335903f,
	0.667979f, 1.0f - 0.335851f,
	0.335973f, 1.0f - 0.335903f,
	0.668104f, 1.0f - 0.000013f,
	0.336098f, 1.0f - 0.000071f,
	0.000103f, 1.0f - 0.336048f,
	0.000004f, 1.0f - 0.671870f,
	0.336024f, 1.0f - 0.671877f,
	0.000103f, 1.0f - 0.336048f,
	0.336024f, 1.0f - 0.671877f,
	0.335973f, 1.0f - 0.335903f,
	0.667969f, 1.0f - 0.671889f,
	1.000004f, 1.0f - 0.671847f,
	0.667979f, 1.0f - 0.335851f
	};

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_dataCube), g_uv_buffer_dataCube, GL_STATIC_DRAW);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

#pragma endregion

#pragma region triangle
	//TRIANGLE
	static GLfloat g_vertex_buffer_dataTriangle[] = {
		-2.0f,-2.0f,0.0f, // triangle 1 : begin
		-2.0f,-2.0f, 2.0f,
		-3.0f, 2.0f, 2.0f, // triangle 1 : end
	};
	const int nColorTriangle = std::size(g_vertex_buffer_dataTriangle);
	// One color for each vertex. They were generated randomly.
	static GLfloat g_color_buffer_dataTriangle[nColorTriangle];


	for (int i = 0; i < std::size(g_color_buffer_dataTriangle); i += 3)
	{
		float randomColorR = dist(e2);
		float randomColorG = dist(e2);
		float randomColorB = dist(e2);

		g_color_buffer_dataTriangle[i] = randomColorR;
		g_color_buffer_dataTriangle[i + 1] = randomColorR;
		g_color_buffer_dataTriangle[i + 2] = randomColorR;
	}


	std::vector<glm::vec3> normalsTriangle;
	computeNormals(g_vertex_buffer_dataTriangle, normalsTriangle);


	GLuint vertexBufferTriangle;
	glGenBuffers(1, &vertexBufferTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_dataTriangle), g_vertex_buffer_dataTriangle, GL_STATIC_DRAW);

	GLuint colorBufferTriangle;
	glGenBuffers(1, &colorBufferTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_dataTriangle), g_color_buffer_dataTriangle, GL_STATIC_DRAW);

	GLuint normalBufferTriangle;
	glGenBuffers(1, &normalBufferTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferTriangle);
	glBufferData(GL_ARRAY_BUFFER, normalsTriangle.size() * sizeof(glm::vec3), &normalsTriangle[0], GL_STATIC_DRAW);

#pragma endregion

#pragma region monkey
	
	// Read our .obj file
	std::vector< glm::vec3 > inVerticesMonkey;
	std::vector< glm::vec2 > inUvsMonkey;
	std::vector< glm::vec3 > inNormalsMonkey; 
	bool res = loadOBJ("monkey.obj", inVerticesMonkey, inUvsMonkey, inNormalsMonkey);

	vector< glm::vec3 > inTangentsMonkey;
	vector< glm::vec3 > inBitangentsMonkey;
	computeTangentBasis(inVerticesMonkey, inUvsMonkey, inNormalsMonkey,
		inTangentsMonkey, inBitangentsMonkey);

	// Create VBO
	vector<unsigned short> indicesMonkey;
	vector< glm::vec3 > verticesMonkey;
	vector< glm::vec2 > uvsMonkey;
	vector< glm::vec3 > normalsMonkey;
	vector< glm::vec3 > tangentsMonkey;
	vector< glm::vec3 > bitangentsMonkey;
	indexVBO_TBN(inVerticesMonkey, inUvsMonkey, inNormalsMonkey, inTangentsMonkey, inBitangentsMonkey,
		indicesMonkey, verticesMonkey, uvsMonkey, normalsMonkey, tangentsMonkey, bitangentsMonkey);



	std::cout << "Nombre de inVerticesMonkey : " << size(inVerticesMonkey)<<std::endl;
	std::cout << "Nombre de verticesMonkey : " << size(verticesMonkey) << std::endl;

	float randomColorR = dist(e2);
	float randomColorG = dist(e2);
	float randomColorB = dist(e2);
	vector<glm::vec3> colorsMonkey;
	for (int i = 0; i < size(verticesMonkey); i++) {
		colorsMonkey.push_back(glm::vec3(randomColorR, randomColorG, randomColorB));
	}


	GLuint elementbufferMonkey;
	glGenBuffers(1, &elementbufferMonkey);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferMonkey);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesMonkey.size() * sizeof(unsigned short), &indicesMonkey[0], GL_STATIC_DRAW);

	GLuint vertexBufferMonkey;
	glGenBuffers(1, &vertexBufferMonkey);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferMonkey);
	glBufferData(GL_ARRAY_BUFFER, verticesMonkey.size() * sizeof(glm::vec3), &verticesMonkey[0], GL_STATIC_DRAW);

	GLuint uvBufferMonkey;
	glGenBuffers(1, &uvBufferMonkey);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferMonkey);
	glBufferData(GL_ARRAY_BUFFER, uvsMonkey.size() * sizeof(glm::vec2), &uvsMonkey[0], GL_STATIC_DRAW);

	GLuint normalBufferMonkey;
	glGenBuffers(1, &normalBufferMonkey);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferMonkey);
	glBufferData(GL_ARRAY_BUFFER, normalsMonkey.size() * sizeof(glm::vec3), &normalsMonkey[0], GL_STATIC_DRAW);

	GLuint colorBufferMonkey;
	glGenBuffers(1, &colorBufferMonkey);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferMonkey);
	glBufferData(GL_ARRAY_BUFFER, colorsMonkey.size() * sizeof(glm::vec3), &colorsMonkey[0], GL_STATIC_DRAW);

	GLuint tangentbufferMonkey;
	glGenBuffers(1, &tangentbufferMonkey);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbufferMonkey);
	glBufferData(GL_ARRAY_BUFFER, tangentsMonkey.size() * sizeof(glm::vec3), &tangentsMonkey[0], GL_STATIC_DRAW);

	GLuint bitangentbufferMonkey;
	glGenBuffers(1, &bitangentbufferMonkey);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbufferMonkey);
	glBufferData(GL_ARRAY_BUFFER, bitangentsMonkey.size() * sizeof(glm::vec3), &bitangentsMonkey[0], GL_STATIC_DRAW);



#pragma endregion

	//Create Light
	Light firstLight(glm::vec3(-10, 0, 0), glm::vec3(0.8, 0.2, 0.1), 1000.0f);

	//Get handle for Lights uniforms
	GLuint LPosID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint LColorID = glGetUniformLocation(programID, "uLightColor");
	GLuint LPowerID = glGetUniformLocation(programID, "uLightPower");

	//Uniform Opacity
	GLuint OpacityID = glGetUniformLocation(programID, "opacity");

	// Get a handle for our Texture uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLuint NormalTextureID = glGetUniformLocation(programID, "NormalTextureSampler");

	GLuint ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");
	
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Removal of back sides 
	glDisable(GL_CULL_FACE);

	// Enable transparencies
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float deltaTime = 0.0f;
	do {
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);


		//Send ligth properties
		glUniform3f(LPosID, firstLight.GetPosition().x, firstLight.GetPosition().y, firstLight.GetPosition().z);
		glUniform3f(LColorID, firstLight.GetColor().x, firstLight.GetColor().y, firstLight.GetColor().z);
		glUniform1f(LPowerID, firstLight.GetPower());

		//Send opacity value
		float opacity = 1.f;
		glUniform1f(OpacityID, opacity);

		//Compute the MVP matrix from keyboard inputs
		deltaTime += computeMatricesFromInputs(window);

		// Projection matrix
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		// Camera matrix
		glm::mat4 ViewMatrix = getViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f,-2.0f,0.0f));
		glm::mat4 ModelViewMatrix = ModelMatrix * ViewMatrix;
		// Take the upper-left part of ModelViewMatrix
		glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around

			// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// Bind our normal texture in Texture Unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, NormalTexture);
		// Set our "Normal    TextureSampler" sampler to user Texture Unit 0
		glUniform1i(NormalTextureID, 1);

#pragma region DrawCube
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : texture
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3th attribute buffer : normals
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(
			3,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		//Draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
#pragma endregion
		//Send Monkey opacity value
		opacity = 1.f;
		glUniform1f(OpacityID, opacity);
#pragma region Monkey MVP
		//Compute the MVP matrix from keyboard inputs
		computeMatricesFromInputs(window);
		// Projection matrix
		ProjectionMatrix = getProjectionMatrix();
		// Camera matrix
		ViewMatrix = getViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::rotate(ModelMatrix, float(cos(deltaTime)), glm::vec3(0.0,1,0.0));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5));
		ModelViewMatrix = ModelMatrix * ViewMatrix;

		// Take the upper-left part of ModelViewMatrix
		ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
		// Our ModelViewProjection : multiplication of our 3 matrices
		mvp = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
#pragma endregion 

#pragma region DrawMonkey
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferMonkey);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : uv
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferMonkey);
		glVertexAttribPointer(
			2,                                // attribute.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferMonkey);
		glVertexAttribPointer(
			3,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 4th attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferMonkey);
		glVertexAttribPointer(
			1,                                // attribute.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 5th attribute buffer : tangents
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, tangentbufferMonkey);
		glVertexAttribPointer(
			4,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 6th attribute buffer : bitangents
		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, bitangentbufferMonkey);
		glVertexAttribPointer(
			5,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferMonkey);

		//Draw the triangles
		glDrawElements(
			GL_TRIANGLES,         // mode
			indicesMonkey.size(), // count
			GL_UNSIGNED_SHORT,    // type
			(void*)0              // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
#pragma endregion

		//Send Triangle opacity value
		opacity = .3ef;
		glUniform1f(OpacityID, opacity);
#pragma region Triangle MVP
		//Compute the MVP matrix from keyboard inputs
		computeMatricesFromInputs(window);
		// Projection matrix
		ProjectionMatrix = getProjectionMatrix();
		// Camera matrix
		ViewMatrix = getViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f,0.0f, cos(deltaTime)));
		ModelViewMatrix = ModelMatrix * ViewMatrix;

		// Take the upper-left part of ModelViewMatrix
		ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
		// Our ModelViewProjection : multiplication of our 3 matrices
		mvp = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
#pragma endregion 
#pragma region DrawTriangle
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferTriangle);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferTriangle);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferTriangle);
		glVertexAttribPointer(
			3,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		//Draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(3);
#pragma endregion

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}




void computeNormals(GLfloat * vertexarray, vector<glm::vec3> &normals)
{
	for (int i = 0; i < sizeof(vertexarray) ; i += 3)
	{
		glm::vec3 normal(0.0);
		glm::vec3 currentVertex(vertexarray[i],
			vertexarray[i + 1],
			vertexarray[i + 2]);
		int nextI = (i + 3) % 9;
		glm::vec3 nextVertex(vertexarray[nextI],
			vertexarray[nextI + 1],
			vertexarray[nextI + 2]);
		normal.x += (currentVertex.y - nextVertex.y) * (currentVertex.z + nextVertex.z);
		normal.y += (currentVertex.z - nextVertex.z) * (currentVertex.x + nextVertex.x);
		normal.z += (currentVertex.x - nextVertex.x) * (currentVertex.y + nextVertex.y);
		normals.push_back(normal);
	}
}