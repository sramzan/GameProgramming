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

float LEFT   = -4.0;
float RIGHT  = 4.0;
float BOTTOM = -4.0;
float TOP    = 4.0;
float NEAR   = -1.0;
float FAR    = 1.0;

class SimpleAnimation{

public:
    
    SimpleAnimation(Matrix PM, Matrix MM, Matrix VM){
        this->modelMatrix = MM;
        this->projectionMatrix = PM;
        this->viewMatrix = VM;
    }
    
    SDL_Window* displayWindow;
    
    void resetWindowColorTo(float r, float g, float b, float c){
        glClearColor(r,g,b,c);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    void createWindow(){
        SDL_Init(SDL_INIT_VIDEO);
        displayWindow = SDL_CreateWindow("Extremely Intense Outrageous Flying Dragonball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 1500, SDL_WINDOW_OPENGL);
        SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
        SDL_GL_MakeCurrent(displayWindow, context);
        
        glViewport(0, 0, 1200, 1500);
    }
    
    void initializePerspective(){
        projectionMatrix.setOrthoProjection(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
    }
    
    void blendTexture(GLuint* texture) {
        
        // enables 2D texture blending which is what is needed for our scope of learning at the moment
        glEnable(GL_TEXTURE_2D);
        
        // binding "texturE" to texture target (GL_TEXTURE_2D)
        glBindTexture(GL_TEXTURE_2D, *texture);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisable(GL_TEXTURE_2D);
        
    }
    
    GLuint LoadTexture(const char *image_path) {
        SDL_Surface *surface = IMG_Load(image_path);
        GLuint textureID = 0;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, surface->w, surface->h, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE, surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        SDL_FreeSurface(surface);
        return textureID;
    }
    
    void setMatrices(ShaderProgram* program){
        program->setModelMatrix(modelMatrix);
        program->setProjectionMatrix(projectionMatrix);
        program->setViewMatrix(viewMatrix);
    }
    
    void render(GLuint* texture, ShaderProgram* program, float* vertices, float* textureCords){
        blendTexture(texture);
        
        glUseProgram(program->programID);
        setMatrices(program);
        initializePerspective();
        
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
    }
    

    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;

    
};

int main(int argc, char *argv[])
{
    
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    Matrix projectionMatrix, modelMatrix, viewMatrix;
    
    SimpleAnimation newAnimation(projectionMatrix, modelMatrix, viewMatrix);
    
    // Creates basic window for display purposes
    newAnimation.createWindow();
    
    // Setup/general run tasks
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsi");
    SDL_Event event;
    bool done = false;
    
    // Load a new texture
    GLuint dbzTexture = newAnimation.LoadTexture("Dragon-Ball-icon.png");
    
    // Define vertex points for use with internal matrices
    float vertices[]     = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    float textureCords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
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
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        newAnimation.modelMatrix.identity();
        newAnimation.modelMatrix.Translate(xPosition, 0, 0);
        
        newAnimation.resetWindowColorTo(0, .2, .4, 1);
        newAnimation.render(&dbzTexture, &program, vertices, textureCords);
        xPosition += elapsed * 1.1;
        
        
        newAnimation.modelMatrix.Translate(.5 * elapsed, 0, 0);
        
        
        
        //        modelMatrix.Translate(1.5, 1, 1);
        // Happens every frame -- passing matrices to our program
        
        //        glBindTexture(GL_TEXTURE_2D, dbzTexture);
        
        
        cout << "xPos: " << xPosition << endl;
        
        
        if (xPosition >= 1){
            newAnimation.modelMatrix.setPosition(-.5, 0, 0);
            xPosition = -.5;
        } else
            newAnimation.modelMatrix.Translate(xPosition, 0.0, 0.0);
        
        
        
        SDL_GL_SwapWindow(newAnimation.displayWindow);
        
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


