/*
 * OpenGLApplication.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: ericwadkins
 */

#define GLEW_STATIC

#include "OpenGLApplication.h"
#include "Main.h"

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
		int mods) {

	// Get the OpenGLApplication context running in the window
	OpenGLApplication* application = nullptr;
	for (int i = 0; i < Main::applications.size(); i++)
		if (Main::applications[i]->_window == window)
			application = Main::applications[i];

	// Handle key events
	if (action == GLFW_PRESS)
		application->_logger->log("Key pressed ").log(char(key)).endLine();
	if (action == GLFW_RELEASE)
		application->_logger->log("Key released ").log(char(key)).endLine();
	if (action == GLFW_REPEAT)
		application->_logger->log("Key repeated ").log(char(key)).endLine();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Handle window resizes
	glViewport(0, 0, width, height);

	// Get the OpenGLApplication context running in the window
	OpenGLApplication* application = nullptr;
	for (int i = 0; i < Main::applications.size(); i++)
		if (Main::applications[i]->_window == window)
			application = Main::applications[i];

	application->_windowSizeX = width;
	application->_windowSizeY = height;
	application->_camera->updateProjectionMatrices();
}

OpenGLApplication::OpenGLApplication(int screenSizeX, int screenSizeY,
		bool fullScreen) {
	_application = this;
	_window = nullptr;
	_logger = new Logger();
	_windowSizeX = screenSizeX;
	_windowSizeY = screenSizeY;
	_fullScreen = fullScreen;
	_renderer = nullptr;
	_camera = nullptr;
	_averageFPS = 0;
}

void OpenGLApplication::setupWindow() {
	// Basic window setup
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION_MINOR);

	// Make the window non-resizable
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create the window
	if (!_fullScreen)
		_window = glfwCreateWindow(_windowSizeX, _windowSizeY,
				"OpenGL Application", NULL, NULL);
	else
		_window = glfwCreateWindow(_windowSizeX, _windowSizeY,
				"OpenGL Application", glfwGetPrimaryMonitor(), NULL);

	// If window creation fails, then exit
	if (!_window) {
		std::string s(
				"Window creation failed. Can your hardware handle OpenGL version "
						+ std::to_string(VERSION_MAJOR) + "."
						+ std::to_string(VERSION_MINOR) + "?");
		stop(s.c_str());
	}

	// Creates an OpenGL context in the window
	glfwMakeContextCurrent(_window);

}

void OpenGLApplication::setupDisplay() {
	// Depth buffer setup
	glClearDepth(1.0f);

	// Enables depth testing
	glEnable(GL_DEPTH_TEST);

	// Type of depth testing
	glDepthFunc(GL_LEQUAL);

	// Sets background color
	glClearColor(0, 0, 0, 1);
}

void OpenGLApplication::initialize() {

	clock_t start, finish;

	_logger->log("Testing matrices...").endLine().increaseIndent();
	start = clock();

	/*
	GLMatrix<float> m = GLMatrix<float>(4, 4);
	float arr[] = { 1, 3, 2, 1, 0, 1, 4, -4, 2, 5, -2, 9, 3, 7, 0, 1 };
	std::vector<float> values(arr, arr + sizeof(arr) / sizeof(arr[0]));
	m.setValues(values);
	*/

	GLMatrix<float> m1 = GLMatrix<float>(4, 4);
	float arr[] = { 5, 3, 2, 1, 0, 1, 4, -4, 2, 5, -2, 9, 3, 7, 0, 1 };
	std::vector<float> values(arr, arr + sizeof(arr) / sizeof(arr[0]));
	m1.setVector(values);

	m1.print();
	m1.determinant().print();
	std::cout << m1.determinant().value(0) << std::endl;

	float num[] = { 5, 1, 2 };
	std::vector<float> numVector(num, num + sizeof(num) / sizeof(num[0]));
	float den[] = { 1, 0, 0, 0 };
	std::vector<float> denVector(den, den + sizeof(den) / sizeof(den[0]));
	m1.set(0, 0, Polynomial<float>(numVector, denVector));

	m1.print();
	m1.determinant().print();
	std::cout << m1.determinant().value(0) << std::endl;

	/*
	(GLMatrix<float>::rotationMatrixZ(30) << GLMatrix<float>::scalarMatrix(4.0/3, 4, 1)).print();
	m1 = m1 << GLMatrix<float>::rotationMatrixZ(30) << GLMatrix<float>::scalarMatrix(4.0/3, 4, 1);

	m1.print();
	*/


	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	stop();

	_logger->log("Initializing GLFW..").endLine().increaseIndent();
	start = clock();
	{
		// Initialize GLFW, if it fails, then exit
		if (!glfwInit()) {
			stop("GLFW failed to initialize");
		}

		// Creates error listener
		glfwSetErrorCallback(error_callback);
	}
	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	_logger->log("Setting up window..").endLine().increaseIndent();
	start = clock();
	{
		// Creates and setups the window
		setupWindow();

		// Creates key callback
		glfwSetKeyCallback(_window, key_callback);

		// Creates framebuffer callback
		glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
	}
	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	_logger->log("Initializing GLEW..").endLine().increaseIndent();
	start = clock();
	{
		// Initialize GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			stop("GLEW failed to initialize");
		}
	}
	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	_logger->log("Setting up display..").endLine().increaseIndent();
	start = clock();
	{
		// Setup the display
		setupDisplay();
	}
	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	_logger->log("Setting up renderer..").endLine().increaseIndent();
	start = clock();
	{
		// Create and initialize the renderer
		_renderer = new Renderer(_application);
		_renderer->initialize();
	}
	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	_logger->log("Setting up camera..").endLine().increaseIndent();
	start = clock();
	{
		// Create and initialize the renderer
		_camera = new Camera(this, 0, 0, 0, 0, 0, 0);
		_camera->initialize();
	}
	finish = clock();
	_logger->log("(Took ").log(
			(double(finish) - double(start)) / CLOCKS_PER_SEC * 1000).log(
			" ms)").endLine().decreaseIndent();

	// Get info of GPU and supported OpenGL version
	_logger->endLine().log("###").endLine();
	_logger->log("OpenGL version: ").log(glGetString(GL_VERSION)).endLine();
	_logger->log("GLSL version: ").log(glGetString(GL_SHADING_LANGUAGE_VERSION)).endLine();
	_logger->log("Vendor: ").log(glGetString(GL_VENDOR)).endLine();
	_logger->log("Renderer: ").log(glGetString(GL_RENDERER)).endLine();
	_logger->log("###").endLine().endLine();
}

void OpenGLApplication::gameLoop() {

	clock_t lastTime;

	// Game loop, while window close is not requested
	while (!glfwWindowShouldClose(_window)) {

		long currentTime = clock();
		long delta = currentTime - lastTime;
		float fps = CLOCKS_PER_SEC / ((float) delta);
		updateAverageFPS(fps);
		_logger->log("FPS: ").log((int)_averageFPS).endLine();
		lastTime = clock();

		static int count = 0;
		count++;

		//_camera->rotateX(2);
		_camera->rotateY(2);
		//_camera->rotateZ(2);
		_camera->useView();

		_renderer->renderTriangle();
		_renderer->display();

		/*
		 //Old drawing
		 float ratio;
		 int width, height;

		 glfwGetFramebufferSize(_window, &width, &height);
		 ratio = width / (float) height;

		 glClear(GL_COLOR_BUFFER_BIT);

		 glMatrixMode(GL_PROJECTION);
		 glLoadIdentity();
		 glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		 glMatrixMode(GL_MODELVIEW);

		 glLoadIdentity();

		 //// Render

		 //Triangle test

		 //FIXME Not drawing the triangle

		 // An array of 3 vectors which represents 3 vertices
		 static const GLfloat vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f,
		 0.0f, 0.0f, 1.0f, 0.0f, };

		 // This will identify our vertex buffer
		 GLuint vbo;

		 // Generate 1 buffer, put the resulting identifier in vbo
		 glGenBuffers(1, &vbo);

		 // The following commands will talk about our 'vbo' buffer
		 glBindBuffer(GL_ARRAY_BUFFER, vbo);

		 // Give our vertices to OpenGL.
		 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		 GL_STATIC_DRAW);

		 // 1rst attribute buffer : vertices
		 glEnableVertexAttribArray(0);
		 glBindBuffer(GL_ARRAY_BUFFER, vbo);
		 glVertexAttribPointer(0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
		 3,                  // size
		 GL_FLOAT,           // type
		 GL_FALSE,           // normalized?
		 0,                  // stride
		 (void*) 0            // array buffer offset
		 );

		 // Draw the triangle !
		 glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

		 glDisableVertexAttribArray(0);

		 ////

		 //Swap buffers
		 glfwSwapBuffers(_window);

		 //Processes the events that have been received, then returns
		 glfwPollEvents();
		 //Puts the thread to sleep until another event is received, used when no need to update continuously
		 //glfwWaitEvents();
		 */

	}
}

int OpenGLApplication::start() {

	int result = -1;
	try {
		try {
			try {
				// Initialization
				_logger->log("*** Starting OpenGLApplication ***").endLine().endLine();
				initialize();

				// Start the game loop
				_logger->log("** Starting game loop **").endLine().endLine();

				gameLoop();

				// Upon exiting the game loop, stop the application
				stop();

			} catch (const char* str) {
				// Fatal error purposely thrown from within the application
				_logger->endLine().setIndent(0).log(
						"*** Stopping OpenGLApplication (").log(str).log(
						") ***").endLine();
			}
		} catch (int e) {
			// Error thrown to signify the end of the application
			if (e == 0) {
				_logger->endLine().setIndent(0).log(
						"*** Stopping OpenGLApplication ***").endLine();
				result = 0;
			}
			else
				throw;
		}
	} catch (...) {
		// Fatal error caused the application to crash
		_logger->endLine().setIndent(0).log(
				"*** Stopping OpenGLApplication (Unknown reason) ***").endLine();
		throw;
	}

	glfwTerminate();

	return result;

}

void OpenGLApplication::warn(const char* warning) {
	// Logs a warning message
	_logger->endLine().log("* WARNING: ").log(warning).log(" *").endLine();
}

void OpenGLApplication::stop(const char* reason) {
	throw reason;
}

void OpenGLApplication::stop() {
	throw 0;
}

void OpenGLApplication::updateAverageFPS(float fps) {

	static long count = 0;
	int minData = std::max(1, std::min(10, (int) fps));
	if (fpsList.size() < minData)
		fpsList.push_back(fps);
	else{
		count++;
		fpsList.erase(fpsList.begin());
		fpsList.push_back(fps);

		double average = 0;
		for (int i = 0; i < fpsList.size(); i++)
			average += fpsList[i];
		average /= fpsList.size();
		_averageFPS = average;
	}
	if (count == 0)
		_averageFPS = fps;
}
