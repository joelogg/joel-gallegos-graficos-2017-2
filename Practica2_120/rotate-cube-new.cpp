/************************************************************
 * Handout: rotate-cube-new.cpp (A Sample Code for Shader-Based OpenGL ---
                                 for OpenGL version 3.1 and later)
 * Originally from Ed Angel's textbook "Interactive Computer Graphics" 6th Ed
              sample code "example3.cpp" of Chapter 4.
 * Moodified by Yi-Jen Chiang to include the use of a general rotation function
   Rotate(angle, x, y, z), where the vector (x, y, z) can have length != 1.0,
   and also to include the use of the function NormalMatrix(mv) to return the
   normal matrix (mat3) of a given model-view matrix mv (mat4).

   (The functions Rotate() and NormalMatrix() are added to the file "mat-yjc-new.h"
   by Yi-Jen Chiang, where a new and correct transpose function "transpose1()" and
   other related functions such as inverse(m) for the inverse of 3x3 matrix m are
   also added; see the file "mat-yjc-new.h".)

 * Extensively modified by Yi-Jen Chiang for the program structure and user
   interactions. See the function keyboard() for the keyboard actions.
   Also extensively re-structured by Yi-Jen Chiang to create and use the new
   function drawObj() so that it is easier to draw multiple objects. Now a floor
   and a rotating cube are drawn.

** Perspective view of a color cube using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
   those colors across the triangles.
**************************************************************/
#include "Angel-yjc.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint sphere_buffer;   /* vertex buffer object id for cube */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint x_buffer;
GLuint y_buffer;
GLuint z_buffer;

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
//GLfloat  zNear = 0.5, zFar = 3.0;
GLfloat  zNear = 0.5, zFar = 100.0;

GLfloat angleX = 0.0; // rotation angle in degrees
GLfloat angleY = 0.0;
GLfloat angleZ = 0.0;
GLfloat angle = 0.0;
vec4 init_eye(7.0, 3.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position

int animationFlag = 1; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'

int cubeFlag = 1;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'
int rolling = 0;
int keyBPress = 0;

const int cantVertices = 1024;
const int sphere_NumVertices = cantVertices*3; //(6 faces)*(2 triangles/face)*(3 vertices/triangle)
const int cantVerticesEje = 2;
const int eje_NumVertices = cantVerticesEje*3;

point3 sphere_points[sphere_NumVertices]; 
color3 sphere_colors[sphere_NumVertices];


const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

point3* leerSphere(string nombre);

string name = "Spheres/sphere."+to_string(cantVertices);
point3 *vertices = leerSphere(name);
// Vertices of a unit cube centered at origin, sides aligned with axes
point3 vertices_piso[6] = {
    point3( 5, 0.0,  8),
    point3( -5, 0.0, -4),
    point3( -5, 0.0,  8),
    point3( 5, 0.0,  8),
    point3( 5, 0.0, -4),
    point3( -5, 0.0, -4)       
};

point3 vertices_eje[6] = {
    point3( 0, 0.0, 2),
    point3( 5, 0.0, 2),
    point3( 0, 0.0, 2),
    point3( 0, 5.0, 2),      
    point3( 0, 0.0, 2),
    point3( 0, 0.0, 9)       
};
point3 x_points[cantVerticesEje]; // positions for all vertices
color3 x_colors[cantVerticesEje]; // colors for all vertices
point3 y_points[cantVerticesEje]; // positions for all vertices
color3 y_colors[cantVerticesEje]; // colors for all vertices
point3 z_points[cantVerticesEje]; // positions for all vertices
color3 z_colors[cantVerticesEje]; // colors for all vertices
// RGBA colors
color3 vertex_colors[5] = {
    color3( 1.0, 0.84, 0.0), //color sphere
    color3( 0.0, 1.0, 0.0), //color plano
    color3( 1.0, 0.0, 0.0), //color eje x
    color3( 1.0, 0.0, 1.0), //color eje y
    color3( 0.0, 0.0, 1.0)  //color eje z
};



point3* leerSphere(string nombre)
{
    point3* vertices2 = new point3[8];
    ifstream ficheroEntrada;
    string frase, aux;
    int n;
    GLfloat x, y, z;
    int tamV;
    int pos;
    

    ficheroEntrada.open (nombre);

    if (ficheroEntrada.is_open()) 
    {
        getline (ficheroEntrada,frase);
        n = stoi(frase);
        point3* vertices = new point3[n*3];
        for(int i=0; i<n; i++)
        {
            getline (ficheroEntrada,frase);
            tamV = stoi(frase);            
            for(int j=0; j<tamV; j++)
            {
                getline (ficheroEntrada,frase);
                pos = frase.find(" ");
                aux = frase.substr (0, pos);
                x = stof(frase);
                
                frase = frase.substr (pos+1);
                
                pos = frase.find(" ");
                aux = frase.substr (0, pos);
                y = stof(frase);
                
                frase = frase.substr (pos+1);
                z = stof(frase);
                                
                vertices[i*tamV+j] = point3( x, y, z);
            }
            
            
        }
        ficheroEntrada.close();
        return vertices;
    }
    else
        cout<<"Error lectura shpere"<<endl;
    
    return vertices2;
}
//----------------------------------------------------------------------------
int Index = 0; // YJC: This must be a global variable since quad() is called
               //      multiple times and Index should then go up to 36 for
               //      the 36 vertices and colors

// quad(): generate two triangles for each face and assign colors to the vertices
void triengulo( int a, int b, int c )
{
    sphere_colors[Index] = vertex_colors[0]; sphere_points[Index] = vertices[a]; Index++;
    sphere_colors[Index] = vertex_colors[0]; sphere_points[Index] = vertices[b]; Index++;
    sphere_colors[Index] = vertex_colors[0]; sphere_points[Index] = vertices[c]; Index++;
}
//----------------------------------------------------------------------------
// generate 12 triangles: 36 vertices and 36 colors
void colorSphere()
{
    for(int i=0; i<cantVertices; i++)
    {
        triengulo( i*3+0, i*3+1, i*3+2 );
    }
    

}
//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors
void floor()
{    
    floor_colors[0] = vertex_colors[1]; floor_points[0] = vertices_piso[0];
    floor_colors[1] = vertex_colors[1]; floor_points[1] = vertices_piso[1];
    floor_colors[2] = vertex_colors[1]; floor_points[2] = vertices_piso[2];

    floor_colors[3] = vertex_colors[1]; floor_points[3] = vertices_piso[3];
    floor_colors[4] = vertex_colors[1]; floor_points[4] = vertices_piso[4];
    floor_colors[5] = vertex_colors[1]; floor_points[5] = vertices_piso[5];
}

void eje_x()
{    
    x_colors[0] = vertex_colors[2]; x_points[0] = vertices_eje[0];
    x_colors[1] = vertex_colors[2]; x_points[1] = vertices_eje[1];
}
void eje_y()
{    
    y_colors[0] = vertex_colors[3]; y_points[0] = vertices_eje[2];
    y_colors[1] = vertex_colors[3]; y_points[1] = vertices_eje[3];
}
void eje_z()
{    
    z_colors[0] = vertex_colors[4]; z_points[0] = vertices_eje[4];
    z_colors[1] = vertex_colors[4]; z_points[1] = vertices_eje[5];
}
//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
    colorSphere();
 // Create and initialize a vertex buffer object for cube, to be used in display()
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(point3)*sphere_NumVertices + sizeof(color3)*sphere_NumVertices,
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices, 
                    sizeof(color3) * sphere_NumVertices,
                    sphere_colors);

    
    floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);
    
    eje_x();
    glGenBuffers(1, &x_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, x_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(x_points) + sizeof(x_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(x_points), x_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(x_points), sizeof(x_colors),
                    x_colors);
    
    eje_y();
    glGenBuffers(1, &y_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, y_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(y_points) + sizeof(y_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(y_points), y_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(y_points), sizeof(y_colors), y_colors);
    
    eje_z();
    glGenBuffers(1, &z_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, z_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(z_points) + sizeof(z_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(z_points), z_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(z_points), sizeof(z_colors),
                    z_colors);

 // Load shaders and create a shader program (to be used in display())
    program = InitShader("Shaders/vshader42.glsl", "Shaders/fshader42.glsl");
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    glLineWidth(2.0);
}
//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices, bool linea)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(point3) * num_vertices) ); 
      // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    if(linea)
        glDrawArrays(GL_LINE_STRIP, 0, num_vertices);
    else
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
}


point3 A = point3( 3, 1.0, 5);
point3 B = point3( -1, 1.0, -4);
point3 C = point3( 3.5, 1.0, -2.5);
float x = A.x, z=A.z;
bool toA = false;
bool toB = true;
bool toC = false;

void display( void )
{
    GLuint  model_view;  // model-view matrix uniform shader variable location
    GLuint  projection;  // projection matrix uniform shader variable location

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(0.529, 0.807, 0.92, 0.0);

    glUseProgram(program); // Use the shader program

    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );    
    

//---  Set up and pass on Projection matrix to the shader ---
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

//---  Set up and pass on Model-View matrix to the shader ---
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
    vec4    at(-7.0, -3.0, 10.0, 0.0);
    vec4    up(0.0, 1.0, 0.0, 0.0);
    mat4  mv = LookAt(eye, at, up);
    
    

//----- Set Up the Model-View matrix for the cube -----

    if(rolling)
        {if(toB)
        {        
            z= ( (B.z-A.z)/(B.x-A.x) )*(x-A.x)+A.z;
            point3 Res = point3( x, 1.0, z);
            mv = mv * Translate(Res) * Scale (1, 1, 1) 
                *RotateX(-angleX)*RotateY(-angleY)*RotateY(angleZ);
            angleX += 1.0;
            angleY += 0.2;
            x = x - 0.015;
            if(x<B.x)
            {
                toB = false;
                toC = true;
                //cout<<"ToC"<<endl;
                x = B.x;
            }        
        }
        else if(toC)
        {     
            z= ( (C.z-B.z)/(C.x-B.x) )*(x-B.x)+B.z;
            point3 Res = point3( x, 1.0, z);
            mv = mv * Translate(Res) * Scale (1, 1, 1) 
                *RotateX(-angleX)*RotateY(-angleY)*RotateY(-angleZ);
            angleZ += 1.0;

            x = x + 0.03;
            if(x>=C.x)
            {
                toC = false;
                toA = true;
                //cout<<"ToA"<<endl;
                x = C.x;

            }
        }
        else if(toA)
        {      
            z= ( (A.z-C.z)/(A.x-C.x) )*(x-C.x)+C.z;
            point3 Res = point3( x, 1.0, z);
            mv = mv * Translate(Res) * Scale (1, 1, 1) 
                *RotateX(-angleX)*RotateY(-angleY)*RotateY(-angleZ);
            angleX -= 1.0;
            x = x - 0.002; 
            if(x<A.x)
            {
                toA = false;
                toB = true;
                //cout<<"ToB"<<endl;
                x = A.x;
                angleX += 1.0;
            }
        }
    }
    else
    {
        point3 Res = point3( x, 1.0, z);
        //mv = mv * Translate(Res) * Scale (1, 1, 1)*RotateY(angle);
        mv = mv * Translate(Res) * Scale (1, 1, 1);
    }
    
    
    //point3 Res = point3( x, 1.0, z);
  // The set-up below gives a new scene (scene 2), using Correct LookAt().
    /*mv = mv * Translate(Res) * Scale (1, 1, 1) 
            *RotateX(angle);*/
            //  * Rotate(-angle, 1.0, 0.0, 0.0);
	    //* RotateY(angle);

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (cubeFlag == 1) // Filled cube
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe cube
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(sphere_buffer, sphere_NumVertices, false);  // draw the cube

//----- Set up the Mode-View matrix for the floor -----
 // The set-up below gives a new scene (scene 2), using Correct LookAt() function
    mv = LookAt(eye, at, up) ;//* Translate(0.3, 0.0, 0.0) * Scale (1.6, 1.5, 3.3);
    
    
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(floor_buffer, floor_NumVertices, false);  // draw the floor
    drawObj(x_buffer, cantVerticesEje, true); 
    drawObj(y_buffer, cantVerticesEje, true); 
    drawObj(z_buffer, cantVerticesEje, true); 

    glutSwapBuffers();
}
//---------------------------------------------------------------------------
void idle (void)
{
    //angle += 0.02;
    angle += 1.0;    //YJC: change this value to adjust the cube rotation speed.
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;

        case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

        case 'a': case 'A': // Toggle between animation and non-animation
	    animationFlag = 1 -  animationFlag;
            if (animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
            break;
	   
        case 'c': case 'C': // Toggle between filled and wireframe cube
	    cubeFlag = 1 -  cubeFlag;   
            break;

        case 'f': case 'F': // Toggle between filled and wireframe floor
	    floorFlag = 1 -  floorFlag; 
            break;
        
        case 'b': case 'B': // Toggle between filled and wireframe floor
	    keyBPress = 1;
            rolling = 1 -  rolling; 
            break;

	case ' ':  // reset to initial viewer/eye position
	    eye = init_eye;
	    break;
    }
    glutPostRedisplay();
}
void myMouseFunc(int button, int state, int x, int y)
{
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
    {
        if(keyBPress)
        {
            rolling = 1 -  rolling; 
        }
    }
}

void menu(int id) 
{  
    keyBPress = 1;
    rolling = 1;
    switch(id)      
    {         
        //Default View Point
        case 1: 
            eye = init_eye;  
        break;    
        //Lighting
        case 4: 
            cout<<"hola4"<<endl;  
        break;
        
        //Quit
        case 6: exit( EXIT_SUCCESS );  
        break;
    }
    glutPostRedisplay();
}
void lightingMenu(int id) 
{  
    keyBPress = 1;
    rolling = 1;
    switch(id)      
    {         
        //No (como hasta ahora)
        case 1: 
            cout<<"hola21"<<endl;   
        break;  
        //Si
        case 2: 
            cout<<"hola22"<<endl;  
        break;
    }
}
void wireFrameMenu(int id) 
{  
    keyBPress = 1;
    rolling = 1;
    switch(id)      
    {         
        //Solido
        case 1: 
            cubeFlag = 1;  
        break;  
        //Solo Lineas
        case 2: 
            cubeFlag = 0;    
        break;
    }
}
void shadingMenu(int id) 
{  
    keyBPress = 1;
    rolling = 1;
    switch(id)      
    {         
        //Flat Shading
        case 1: 
            cout<<"hola31"<<endl;   
        break;  
        //Smooth Shading
        case 2: 
            cout<<"hola32"<<endl;  
        break;
    }
}
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{ 
    int err;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    // glutInitContextVersion(3, 2);
    // glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Sphere");

    /* Call glewInit() and error checking */
    err = glewInit();
    if (GLEW_OK != err)
    { 
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
        exit(1);
    }
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(myMouseFunc);
    
    int subM_Lighting = glutCreateMenu(lightingMenu);
    glutAddMenuEntry("No",1);  
    glutAddMenuEntry("Yes",2); 
    int subM_wireFrame = glutCreateMenu(wireFrameMenu);
    glutAddMenuEntry("No",1);  
    glutAddMenuEntry("Yes",2); 
    int subM_shading = glutCreateMenu(shadingMenu);
    glutAddMenuEntry("Flat Shading",1);  
    glutAddMenuEntry("Smooth Shading",2);
    
    glutCreateMenu(menu);  
    glutAddMenuEntry("Default View Point",1);  
    glutAddSubMenu("Enable Lighting",subM_Lighting);  
    glutAddSubMenu("Shading",subM_shading);  
    glutAddMenuEntry("Lighting",4); 
    glutAddSubMenu("Wire Frame",subM_wireFrame);  
    glutAddMenuEntry("Quit",6); 
    glutAttachMenu(GLUT_LEFT_BUTTON); 

    init();
    glutMainLoop();
    return 0;
}
