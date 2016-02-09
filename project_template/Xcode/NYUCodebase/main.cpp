#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

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

void setTextures(ShaderProgram program, GLuint texture, float* vertices, float* texCords){
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

class SimpleAnimation{
public:
    void createWindow(){
        SDL_Init(SDL_INIT_VIDEO);
        displayWindow = SDL_CreateWindow("Extremely Intense Outrageous Flying Dragonball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 720, SDL_WINDOW_OPENGL);
        SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
        SDL_GL_MakeCurrent(displayWindow, context);
    }
};

int main(int argc, char *argv[])
{
    
//    SimpleAnimation newAnimation;
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Extremely Intense Outrageous Flying Dragonball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    // Creates basic window for display purposes
//    newAnimation.createWindow();
#ifdef _WINDOWS
    glewInit();
#endif
    
//    Matrix projectionMatrix, modelMatrix, viewMatrix;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    
    // Setup/general run tasks
    glViewport(0, 0, 1440, 720);
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    bool done = false;
    
    // Load a new texture
    GLuint dbzTexture = LoadTexture("cardSpadesQ.png");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    // Define vertex points for use with internal matrices
    float vertices[]     = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    float textureCords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    SDL_Event event;
    
    // define xposition for dbz texture
    float xPosition = 0;
    
    // define var for tracking current fram tick time
    float lastFrameTicks = 0.0;
    
    while (!done) {
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        modelMatrix.Rotate(.1 * (3.14159/180));
        
//        float ticks = (float)SDL_GetTicks()/1000.0f;
//        float elapsed = ticks - lastFrameTicks;
//        lastFrameTicks = ticks;
//        glBindTexture(GL_TEXTURE_2D, dbzTexture);
        setTextures(program, dbzTexture, vertices, textureCords);
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    
    
    SDL_Quit();
    return 0;
}


//glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
//glEnableVertexAttribArray(program.positionAttribute);
//glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
//glEnableVertexAttribArray(program.texCoordAttribute);
//glDrawArrays(GL_TRIANGLES, 0, 6);
//glDisableVertexAttribArray(program.positionAttribute);
//glDisableVertexAttribArray(program.texCoordAttribute);


