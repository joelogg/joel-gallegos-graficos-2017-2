/*********************************************************************
 * Handout: checker-new.cpp (modified by Yi-Jen Chiang) 
 *  
 * This program texture maps a checkerboard image onto two squares. 
 *********************************************************************/

#include "Angel-yjc.h"
#include <stdio.h>

using namespace std;

typedef Angel::vec3 point3;
typedef Angel::vec4 color4;

/*  Create checkerboard texture */
#define ImageWidth  96
#define ImageHeight 80
//#define ImageWidth  64
//#define ImageHeight 64
static  GLubyte Image[ImageHeight][ImageWidth][4];
#define stripeImageWidth 32
GLubyte stripeImage[4*stripeImageWidth];

static GLuint texName;

/*--- Quad arrays: 6 vertices of 2 triangles, for the quad (a b c d). 
      Triangles are abc, cda. --*/ 
point3 quad_vert[6] = {
    point3(-1.0, -1.0, 0.0),  // a
    point3(-1.0, 1.0, 0.0),   // b
    point3(1.0, 1.0, 0.0),    // c

    point3(1.0, 1.0, 0.0),    // c
    point3(1.0, -1.0, 0.0),   // d
    point3(-1.0, -1.0, 0.0),  // a
};
/*vec2 quad_texCoord[6] = {
    vec2(0.0, 0.0),  // for a
    vec2(0.0, 1.25),  // for b
    vec2(1.5, 1.25),  // for c
    vec2(1.5, 1.25),  // for c
    vec2(1.5, 0.0),  // for d
    vec2(0.0, 0.0),  // for a 
};*/
vec2 quad_texCoord[6] = {
    vec2(0.0, 0.0),  // for a
    vec2(0.0, 1.0),  // for b
    vec2(1.0, 1.0),  // for c

    vec2(1.0, 1.0),  // for c
    vec2(1.0, 0.0),  // for d
    vec2(0.0, 0.0),  // for a 
};

GLuint program;
GLuint quad_buffer;

/*--- Parameters for Perspective() function ---*/
GLfloat fovy = 60.0;
GLfloat aspect;
GLfloat zNear = 1.0, zFar = 30.0;

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

vec4 quad_color(0.8, 0.8, 0.0, 1.0); // original quad color: yellowish

bool   gp;                      // G Pressed? ( New )
GLuint filter;                      // Which Filter To Use
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter= 0;                    // Which Fog To Use
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};      // Fog Color


int texture_app_flag = 1;  // 0: no texture application: obj color
                           // 1: texutre color
                           // 2: (obj color) * (texture color)
//---------------------------
void image_set_up(void)
{
    int i, j, c; 
 
    /* --- Generate checkerboard image to the image array ---*/
    for (i = 0; i < ImageHeight; i++)
    {
        for (j = 0; j < ImageWidth; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

            if (c == 1) /* white */
            {
                  
                Image[i][j][0] = (GLubyte) 0;
                Image[i][j][1] = (GLubyte) 255;
                Image[i][j][2] = (GLubyte) 0;
            }
            else  /* green */
            {
                Image[i][j][0] = (GLubyte) 0;
                Image[i][j][1] = (GLubyte) 100;
                Image[i][j][2] = (GLubyte) 0;
            }

            Image[i][j][3] = (GLubyte) 255;
        }        
    }

} /* end function */

//---------------------------
void init(void)
{
    glEnable(GL_DEPTH_TEST);    
    glClearColor(0.529, 0.807, 0.92, 1.0);  /* sky blue */
    
 
    glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
    glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
    glFogf(GL_FOG_DENSITY, 0.35f);              // How Dense Will The Fog Be
    glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
    glFogf(GL_FOG_START, 1.0f);             // Fog Start Depth
    glFogf(GL_FOG_END, 5.0f);               // Fog End Depth
    glEnable(GL_FOG);                   // Enables GL_FOG



    image_set_up();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*--- Create and Initialize a texture object ---*/
    glGenTextures(1, &texName);      // Generate texture obj name(s)

    glActiveTexture( GL_TEXTURE0 );  // Set the active texture unit to be 0 
    glBindTexture(GL_TEXTURE_2D, texName); // Bind the texture to this texture unit

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

    /** Note: If using multiple textures, repeat the above process starting from
              glActiveTexture(), but each time use a *different texture unit*,
              so that each texture is bound to a *different texture unit*.    **/

    /*--- Create and initialize vertex buffer object for quad ---*/
     glGenBuffers(1, &quad_buffer);
     glBindBuffer(GL_ARRAY_BUFFER, quad_buffer);
     glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vert)+sizeof(quad_texCoord),
                  NULL, GL_STATIC_DRAW);
     glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_vert), quad_vert);
     glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad_vert), 
                     sizeof(quad_texCoord), quad_texCoord);

     // Load shaders and create a shader program (to be used in display())
     program = InitShader( "Shaders/vTexture.glsl", "Shaders/fTexture.glsl" );
}
//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(0) );

    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" ); 
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(quad_vert)) ); 
    // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vTexCoord);
}
//---------------------------------
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram( program );

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    /*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    // Set the value of the fragment shader texture sampler variable
    //   ("texture_2D") to the appropriate texture unit. In this case,
    //   0, for GL_TEXTURE0 which was previously set in init() by calling
    //   glActiveTexture( GL_TEXTURE0 ).
    glUniform1i( glGetUniformLocation(program, "texture_2D"), 0 );

    /** Note: If using multiple textures, each texture must be bound to a
              *different texture unit* (as commented in the "Note" in init()),
              and here each sampler variable must be set to the *corresponding
              texture unit*.                                                  **/

    const vec4 eye(0.0, 0.0, 3.6, 1.0);
          vec4 at(0.0, 0.0, 0.0, 1.0);
          vec4 up(0.0, 1.0, 0.0, 0.0);
          mat4 mv = LookAt(eye, at, up); // model-view matrix using Correct LookAt()

    // Pass on the quad_color to the uniform var "uColor" in vertex shader
    glUniform4fv( glGetUniformLocation(program, "uColor"), 1, quad_color);

    // Pass on the value of texture_app_flag to the fragment shader
    glUniform1i( glGetUniformLocation(program, "Texture_app_flag"), 
                 texture_app_flag);

    // Draw the first quad with translation only
    mat4 model_view = mv * Translate(0.0, 0.5, 0.0)  * Translate(0.8, 0.0, 0.0);
    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view );
    drawObj(quad_buffer, 6);

    // Draw the 2nd quad with both rotation & translation
    model_view = mv * Translate(-1.4, 0.5, -0.6) * Rotate(-35, 0.0, 1.0, 0.0); 
    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view );
    drawObj(quad_buffer, 6);
    
    model_view = mv * Translate(0.0, -1.4, -0.5)  * Rotate(-70, 1.0, 0.0, 0.0); 
    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view );
    drawObj(quad_buffer, 6);

    glFlush();
}
//---------------------------------
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    aspect = (GLfloat) w/(GLfloat) h;
    glutPostRedisplay();
}
//---------------------------------
void keyboard( unsigned char key, int x, int y )
{
    switch( key ) 
    {
    case 033: // Escape Key
    case 'q': case 'Q':
        exit( EXIT_SUCCESS );
        break;
   
        case ' ':  // Toggle among No Texture (obj color), Texture Only, 
               //        and Modulate the two.
        texture_app_flag++;
            if (texture_app_flag > 2)
            texture_app_flag = 0;
            glutPostRedisplay();
            break;
    }
}


void menu(int id) 
{  
    switch(id)      
    {         
        //Fog
        case 1: 
            cout<<"Fog"<<endl;  
        break;
        //Texture Mapped Ground
        case 2: 
            cout<<"Texture Mapped Ground"<<endl;  
        break;        
        //Quit
        case 6: exit( EXIT_SUCCESS );  
        break;
    }
    glutPostRedisplay();
}
void fogOptionsMenu(int id) 
{  
    switch(id)      
    {         
        //no fog
        //disable the fog effect
        case 1: 
            cout<<"no fog"<<endl;   
        break;  
        //linear
        //enable the fog effect, using the linear fog equation, 
        //with the fog starting(0.0) and ending(18.0)
        case 2: 
            cout<<"linear"<<endl;  
        break;
        //exponential
        //enable the fog effect, using the exponential fog equation, 
        //with the fog density value 0.09
        case 3: 
            cout<<"exponential"<<endl;  
        break;
        //exponential square
        //enable the fog effect, using the exponential square fog equation, 
        //with the fog density value 0.09
        case 4: 
            cout<<"exponential square"<<endl;  
        break;
    }
    glutPostRedisplay();
}
void textureMappedGroundMenu(int id) 
{  
    switch(id)      
    {         
        //no efecto textura
        case 1: 
            texture_app_flag = 0;
        break;  
        //si efecto de mapeo
        case 2: 
            texture_app_flag = 1;
        break;
    }
    glutPostRedisplay();
}

void contenidoMenur()
{
    int subM_fogOptions = glutCreateMenu(fogOptionsMenu);
    glutAddMenuEntry("no fog",1);  
    glutAddMenuEntry("linear",2); 
    glutAddMenuEntry("exponential",3); 
    glutAddMenuEntry("exponential square",4); 
    
    int subM_textureMappedGround = glutCreateMenu(textureMappedGroundMenu);
    glutAddMenuEntry("No",1);  
    glutAddMenuEntry("Si",2);  
    
    glutCreateMenu(menu);  
    glutAddSubMenu("Fog Options",subM_fogOptions);  
    glutAddSubMenu("Texture Mapped Ground",subM_textureMappedGround);  
    glutAddMenuEntry("Quit",6); 
    glutAttachMenu(GLUT_LEFT_BUTTON); 
}
//---------------------------------
int main(int argc, char** argv)
{
    int err;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Checkerboard");

    /* Call glewInit() and error checking */
    err = glewInit();
    if (GLEW_OK != err)
    { 
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
        exit(1);
    }
 
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
        
    contenidoMenur();
    init();
   
    glutMainLoop();
    return 0; 
}