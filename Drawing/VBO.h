/*
 * VBO.h
 *
 *  Created on: Apr 13, 2015
 *      Author: ericwadkins
 */

#ifndef DRAWING_VBO_H_
#define DRAWING_VBO_H_

#include "../Drawing/GLTriangle.h"
#include "../Drawing/Vertex.h"
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class VBOBase {
public:
	enum Type {STATIC, DYNAMIC};
	virtual ~VBOBase() {
	}
	virtual void updateData() {};
	virtual void create() {};
	virtual void draw() {};
};

template<typename T>
class VBO : public VBOBase {
public:
	VBO(Type type);
	virtual ~VBO() {
	}
	void add(T* d);
	virtual void updateData();
	virtual void create();
	virtual void draw();
private:
	Type _type;
	std::vector<T*> _drawables;
	std::vector<GLfloat> _data;
	GLuint _vao, _vbo;
};

#endif /* DRAWING_VBO_H_ */
