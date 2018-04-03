/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 120

varying  vec4 color;

void main() 
{ 
    gl_FragColor = color;
} 

