/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 150

in  vec4 color; 
out vec4 sfColor;

//uniform vec3 objectColor;

void main() 
{ 
    sfColor = color;
} 

