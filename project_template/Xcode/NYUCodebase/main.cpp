#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <math.h>
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

static int WINDOW_HEIGHT = 900;
static int WINDOW_WIDTH  = 1500;

static SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
}

void createWindow(const char* nameOfGame){
    SDL_Init(SDL_INIT_VIDEO); // initializes SDL
    displayWindow = SDL_CreateWindow(nameOfGame, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // will set the rendering area of the window. Tells OpenGL the rendering area, as it does not know how large the window is. 0,0 defines a starting position. You can actually have multiple viewports in one window (i.e. multiple views in an editing software)
}

void setProgramID(ShaderProgram* program){
    glUseProgram(program->programID);
}

void clearScreen(float red, float green, float blue, float alpha){
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

class SimpleAnimation{
public:
    SimpleAnimation(ShaderProgram* prog, GLuint text){
        this->texture = text;
        this->program = prog;
    }
    
    void setPerspective(){
        projectionMatrix.setOrthoProjection(-4, 4, -2.0f, 2.0f, -1.0f, 1.0f); /// will look the same anywhere as the viewport sets the amount of pixels. This sets a projection on the screen for where images can be drawn (check this)
    }
    
    void setMatrices(){
        program->setModelMatrix(modelMatrix);
        program->setProjectionMatrix(projectionMatrix);
        program->setViewMatrix(viewMatrix);
    }
    
    void drawTexture(float* verticesArray, float* texCords){
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesArray);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
    }
    
    void rotate(float rotateAmt){
        modelMatrix.Rotate(rotateAmt);
    }
    
    void moveToTheRight(float xPos){
        modelMatrix.Translate(xPos, 0, 0);
    }
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    GLuint texture;
    ShaderProgram* program;
};

int main(int argc, char *argv[])
{
    
    createWindow("Extremely Intense Outrageous Card Assembly");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    static ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    setProgramID(&program);
    
    SDL_Event event;
    bool done = false;
    float rotateSpeedQ = -7 * (M_1_PI/180);
    float rotateSpeedK = -5 * (M_1_PI/180);
    float rotateSpeedJ = -5 * (M_1_PI/180);
    
    // define var for tracking current fram tick time
    float lastFrameTicks = 0.0;
    float xpos = 0;
    
    // Define vertex points for use with internal matrices
    float queenVertices[] = {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        -0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };
    
    float queenTextCords[] = {
        0.0, 1.0, 1.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 1.0,
        0.0, 0.0, 0.0
    };
    
    float kingVertices[]  = {
        -0.5 - 1, -0.5 - 1, 0.5 - 1,
        -0.5 - 1,  0.5 - 1, 0.5 - 1,
        -0.5 - 1, -0.5 - 1, 0.5 - 1,
         0.5 - 1, -0.5 - 1, 0.5 - 1
    };
    
    float kingTextCords[] = {
        0.0, 1.0, 1.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 1.0,
        0.0, 0.0, 0.0
    };
    
    float jackVertices[]  = {
        -0.5 + 1, -0.5 + 1, 0.5 + 1,
        -0.5 + 1,  0.5 + 1, 0.5 + 1,
        -0.5 + 1, -0.5 + 1, 0.5 + 1,
         0.5 + 1, -0.5 + 1, 0.5 + 1
    };
    
    float jackTextCords[] = {
        0.0, 1.0, 1.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 1.0,
        0.0, 0.0, 0.0};
    
    /*
     - Following code defines the various textures, and performs logic in this prorgam
     - Various textures are created via the SimpleAnimation class
     
     Usage:
     
     SimpleAnimation (ShaderProgram* programPtr, GLuint texture)
            programPtr - can be a ShaderProgram* or the address of the program created
            texture - this should be the already loaded texture id
     
     .setPerspective()
     .setMatrices()
     .setTexture(float[] vertices, float[] textureCords)
     .moveToTheRight
     
     */
    
    SimpleAnimation queenCard = SimpleAnimation(&program, LoadTexture("cardSpadesQ.png"));
    SimpleAnimation kingCard  = SimpleAnimation(&program, LoadTexture("cardSpadesK.png"));
    SimpleAnimation jackCard  = SimpleAnimation(&program, LoadTexture("cardSpadesJ.png"));
    
    queenCard.setPerspective();
    kingCard.setPerspective();
    jackCard.setPerspective();
    
    
    while (!done) {
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        xpos += elapsed * .9;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        clearScreen(.5f, 0.3f, .7f, 0.0f);
        
        queenCard.setMatrices();
        queenCard.drawTexture(queenVertices, queenTextCords);
        
        queenCard.modelMatrix.Rotate(rotateSpeedQ);
        
        
        kingCard.setMatrices();
        kingCard.drawTexture(kingVertices, kingTextCords);
        kingCard.rotate(rotateSpeedK);
        
        jackCard.setMatrices();
        jackCard.drawTexture(jackVertices, jackTextCords);
        jackCard.rotate(rotateSpeedJ);

        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}

