/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 120

varying  vec4 color; 


//uniform vec3 objectColor;

void main() 
{ 
    gl_FragColor = color;
} 

