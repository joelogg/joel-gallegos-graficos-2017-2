#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "Angel-yjc.h"

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>
#include <vector>

#include<time.h>

#include "FractalNoise.h"

using namespace std;


GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint colorbuffer;
GLuint programID;

GLfloat fovy = 60.0;
GLfloat aspect;
GLfloat zNear = 0.1, zFar = 100.0;

GLuint  ModelView, Projection;


static const GLfloat g_color_buffer_data[] = {
    1.0f,  1.0f,  1.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  0.0f,  1.0f
};


const int cols=80;
const int rows=80;
const int sizeAux = cols*rows*2*3;//2 puntosos*cada punto tiene xyz
const int size = cols*rows*6*3;//por 3, por que cada vertice tiene xyz

GLfloat g_vertex_buffer_dataAux[sizeAux];
GLfloat g_vertex_buffer_data[size];


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);


FractalNoise *noiseMaker = new FractalNoise();

float terreno[rows][cols];

float flying = 0;
void generarTereno()
{    
    //flying -= 0.0003;
    float yoff = flying;
    for(int y=0; y<rows; y++)
    {
        float xoff = 0;
        for(int x=0; x<cols; x++)
        {
            terreno[y][x] = noiseMaker->noise(xoff, yoff, 0.0);
            xoff += 0.003;
        }   
        yoff += 0.003;
    }
}

void generarMallaAux()
{    
    float xa = 4.0f/cols;//ancho espaciado
    float ya = 4.0f/rows;//alto espaciado
    
    int i=0;
    
    int yIni = rows/2;
    int yFin = -yIni;
    
    int xIni = -cols/2;
    int xFin = -xIni;
    
    for(int y=yIni, yT=0; y>yFin; y--, yT++)
    {
        for(int x=xIni, xT=0; x<xFin; x++, xT++)
        {
            g_vertex_buffer_dataAux[i] = xa*x;
            i++;
            g_vertex_buffer_dataAux[i] = ya*y;
            i++;
            g_vertex_buffer_dataAux[i] = terreno[yT][xT];
            i++;
            
            g_vertex_buffer_dataAux[i] = xa*x;
            i++;
            g_vertex_buffer_dataAux[i] = ya*(y-1);
            i++;
            g_vertex_buffer_dataAux[i] = terreno[yT+1][xT];
            i++;
        }
    }
}

void generarMalla()
{    
    int j=0;
    int aux;
    
    for(int y=0; y< rows; y++)
    {
        for(int x=0; x< cols-1; x++)
        {
            aux = cols*6*y + x*6;
            //1
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+1];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+2];
            j++;  
            //2
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+3];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+4];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+5];
            j++; 
            //3
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+6];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+7];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+8];
            j++; 
            
            //2
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+3];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+4];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+5];
            j++; 
            //3
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+6];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+7];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+8];
            j++;             
            //4
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+9];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+10];
            j++;
            g_vertex_buffer_data[j] = g_vertex_buffer_dataAux[aux+11];
            j++; 
        }
    }
}

void init(void)
{
    noiseMaker->setBaseFrequency(4.0f);
    
    glEnable(GL_DEPTH_TEST);    
    glClearColor(0.0, 0.0, 0.0, 1.0);	// sky blue 
    
    glGenVertexArrays(1, &VertexArrayID );
    glBindVertexArray(VertexArrayID);
    
    
    srand(time(NULL));
    
    
    // Para los vertices
    /*generarTereno();
    generarMallaAux();
    generarMalla();*/
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    //Para los colores de los vertices    
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
    // Crear y compilar el programa GLSL desde los shaders
    programID = LoadShaders( "Shaders/vs.glsl", "Shaders/fs.glsl" );
    //programID = LoadShaders( "Shaders/vs.glsl", "Shaders/fS.glsl" );
    
}

int zoom = 0;
void display(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(programID);
    
    ModelView = glGetUniformLocation( programID, "ModelView" );
    Projection = glGetUniformLocation( programID, "Projection" );
    
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, p);
    
    
    const vec4 eye(0.0, 0.0, 0.5, 1.0);
          vec4 at(0.0, 0.0, 0.0, 1.0);
          vec4 up(0.0, 1.0, 0.0, 0.0);
          mat4 mv = LookAt(eye, at, up); // model-view matrix using Correct LookAt()
    mat4 model_view = mv * Translate(0, -0.4, 0) * Rotate(-60, 1.0, 0.0, 0.0) ; 
    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view );
    
    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
       0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );
    
    // 2do atributo del buffer : colores
    /*glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                                // Atributo. No hay razón especial para el 1, pero debe corresponder al número en el shader.
        3,                                // tamaño
        GL_FLOAT,                         // tipo
        GL_FALSE,                         // normalizado?
        0,                                // corrimiento
        (void*)0                          // corrimiento de buffer
    );*/
    
    generarTereno();
    generarMallaAux();
    generarMalla();
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, size); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    //glDisableVertexAttribArray(1);
    
    glFlush();
    glutPostRedisplay();
}
//---------------------------------
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    aspect = (GLfloat) w / (GLfloat) h;
    glutPostRedisplay();
}
//---------------------------------
void keyboard( unsigned char key, int x, int y )
{
    switch( key ) 
    {
        case 'w': case 'W':
        {
            flying -= 0.003;
            
	    break;
        }
        case 's': case 'S':
        {
            flying += 0.003;
            
	    break;
        }
        case 'e': case 'E':
        {
            zoom++;
            
	    break;
        }
        case 'd': case 'D':
        {
            zoom--;
            
	    break;
        }
	case 033: // Escape Key
	case 'q': case 'Q':
        {
            exit( EXIT_SUCCESS );
	    break;
        }
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    int err;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tarea4");

    // Call glewInit() and error checking 
    err = glewInit();
    if (GLEW_OK != err)
    { 
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
        exit(1);
    }
 
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
        
    init();
   
    glutMainLoop();
    return 0; 
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    string VertexShaderCode;
    ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
    	string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
	VertexShaderStream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
	FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
	std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
	
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
