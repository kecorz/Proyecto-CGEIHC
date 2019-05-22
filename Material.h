#pragma once

#include <glew.h>
//Esta clase está pensada para crear materiales asignables a nuestros diferentes objetos y luces
class Material //Asignar el sombreado de phong o de lambert
{
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shine);

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

	~Material();

private: 
	GLfloat specularIntensity;
	GLfloat shininess;
};

