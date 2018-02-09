#version 120
attribute vec3 vPosition;
attribute vec3 vertexColor;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 uColor;

varying vec3 fragmentColor;
void main()
{
  	vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
 	gl_Position = Projection * ModelView * vPosition4;
 	//gl_Position = ModelView * vPosition4;
 	//gl_Position = Projection * vPosition4;

  	//gl_Position =  vPosition4;
  	fragmentColor = vertexColor;
}