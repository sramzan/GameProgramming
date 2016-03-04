////
////  Pong.cpp
////  
////
////  Created by Sean Ramzan on 2/14/16.
////
////
//
//#ifdef _WINDOWS
//#include <GL/glew.h>
//#endif
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_image.h>
//#include <cmath>
//
//#include "Matrix.h"
//#include "ShaderProgram.h"
//
//#include "../programGenerator.cpp"
//#include "windowGenerator.cpp"
//#include "../screenObject.cpp"
//#include "../global-variables.cpp"
//
//using namespace std;
//
//#ifdef _WINDOWS
//#define RESOURCE_FOLDER ""
//#else
//#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
//#endif
//
//
//float xDir = -3.0;
//float yDir = 3.0;
//float moveIncrement = 0.5;
//
//float r = .4f;
//float g = .3f;
//float b = .5f;
//float a = 1.0f;
//
//float ballSpeed = 0.0;
//
//bool blockAVictory = false;
//bool blockBVictory = false;
//
//bool startup = true;
//
//SDL_Window* displayWindow;
// blockA.setBoundaries(2.0, -2.0, -14.0, -13.0);
//float vertices_blockA[] =
//{
//    -14.0,  2.0,
//    -14.0, -2.0,
//    -13.0,  2.0,
//    
//    -14.0, -2.0,
//    -13.0, -2.0,
//    -13.0,  2.0
//};
//
//float vertices_blockB[] =
//{
//    13.0, 2.0,
//    13.0, -2.0,
//    14.0, 2.0,
//    
//    13.0, -2.0,
//    14.0, -2.0,
//    14.0, 2.0
//};
//
//float vertices_ball[] =
//{
//    -0.5,   0.5,
//    -0.5,  -0.5,
//     0.5,   0.5,
//
//    -0.5,  -0.5,
//     0.5,  -0.5,
//     0.5,   0.5,
//    
//};
//
//void moveBall(ScreenObject* ball){
//    
//
//}
//
//
//void checkInput(ScreenObject* ball, ScreenObject* blockA, ScreenObject* blockB){
//    const Uint8 *keys = SDL_GetKeyboardState(NULL);
//    
//    if(keys[SDL_SCANCODE_UP] && ((blockB->getTopPos() + moveIncrement) <= blockB->getTopBoundary()))
//        blockB->moveUp(moveIncrement);
//    
//    if(keys[SDL_SCANCODE_DOWN] && ((blockB->getBottomPos() - moveIncrement) >= blockB->getBottomBoundary()))
//        blockB->moveDown(-moveIncrement);
//        
//    if(keys[SDL_SCANCODE_A] && ((blockA->getTopPos() + moveIncrement) <= blockA->getTopBoundary()))
//        blockA->moveUp(moveIncrement);
//    
//    if(keys[SDL_SCANCODE_Z] && ((blockA->getBottomPos() - moveIncrement) >= blockA->getBottomBoundary()))
//        blockA->moveDown(-moveIncrement);
//    
//}
//
//void checkForCollisions(ScreenObject* ball, ScreenObject* blockA, ScreenObject* blockB){
//    float ballLeftPos = ball->getLeftPos();
//    
//    
//    //Check if the ball has gone off screen (only other option after the other cases above have faile)
//    if ( (ball->getLeftPos() <= ball->getLeftBoundary()) ||
//        
//        ( ball->getLeftPos() < blockA->getRightPos()  &&
//          ball->getBottomPos() > blockA->getTopPos()  ) ||
//        
//        (
//          ball->getLeftPos() < blockA->getRightPos() &&
//          ball->getTopPos() < blockA->getBottomPos() )
//        ){
//            blockBVictory = true;
//            return;
//        }
//    
//    if ( (ball->getRightPos() >= ball->getRightBoundary()) ||
//        
//        ( (ball->getRightPos() > blockB->getLeftPos()) &&
//         ball->getBottomPos() > blockB->getTopPos()  ) ||
//        
//        (
//         ball->getRightPos() > blockB->getLeftPos() &&
//         ball->getTopPos() < blockB->getBottomPos() )
//        ){
//        blockAVictory = true;
//        return;
//    }
//    
//    //Check if ball hit top part of screen
//    if (ball->getTopPos() >= ball->getTopBoundary())
//        yDir *= -1;
//    
//    //Check if ball hit bottom part of screen
//    if (ball->getBottomPos() <= ball->getBottomBoundary())
//        yDir *= -1;
//    
//    //Check if ball hit left paddle
//    if ( (ballLeftPos <= blockA->getRightPos()) && (ball->getBottomPos() <= blockA->getTopPos()) && (ball->getTopPos() >= blockA->getBottomPos()) )
//        xDir *= -1.0;
//    
//    //Check is ball hit right paddle
//    if ( (ball->getRightPos() >= blockB->getLeftPos()) && (ball->getBottomPos() <= blockB->getTopPos()) && (ball->getTopPos() >= blockB->getBottomPos()) )
//        xDir *= -1.0;
//}
//
////
////void clearScreen(Program* program){
////    program->clearScreen(r,g,b,a);
////}
//
//
//void setProgramID(ShaderProgram* program){
//    glUseProgram(program->programID);
//}
//
//void setup(const char* windowTitle){
////    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
//    
//    static ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
//    setProgramID(&program);
//    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, windowTitle);
//    ScreenObject ball(&program);
//    
//}
//
//
//
//void runPong(){
////    setup("Pong");
//    
//    
//
//}
//
//void pong()
//{
//    /*
//     
//     Usage:
//     
//        - A,Z & UP/DOWN Arrow Keys are used more movement of the left and right paddles (respectively)
//     
//        - The screen will change color to indicate the winner of the previous vollery
//            - Dark Blue means the left side won the previous volley
//            - Light Gray means the right side won the previous volley
//     
//        - "ballSpeed" can be varied to change the "level" of the game (aka, manages how fast the ball with move to inc/dec the challenge
//     
//     */
//    
//    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "Pong");
//    
//#ifdef _WINDOWS
//    glewInit();
//#endif
//    
//    Program program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
////    setProgramID(&program);
//    ScreenObject ball(program.getShaderProgram());
//    ScreenObject blockA(program.getShaderProgram());
//    ScreenObject blockB(program.getShaderProgram());
//    
//    ball.setProjection();
//    ball.setBoundaries(0.5, -0.5, -0.5, 0.5);
//    
//    blockA.setProjection();
//    blockA.setBoundaries(2.0, -2.0, -14.0, -13.0);
//    
//    blockB.setProjection();
//    blockB.setBoundaries(2.0, -2.0, 13.0, 14.0);
//    
//    ball.setMatrices();
//    blockA.setMatrices();
//    blockB.setMatrices();
//    
//    float lastFrameTicks = 0.0f;
//    
//    //    ball.setVertices(vertices_ball);
//    
//    SDL_Event event;
//    bool done = false;
//    
//    while (!done) {
//        
//        float ticks    = (float)SDL_GetTicks()/1000.0f;
//        float elapsed  = ticks - lastFrameTicks;
//        lastFrameTicks = ticks;
//        
//        while (SDL_PollEvent(&event)) {
//            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
//                done = true;
//            }
//        }
//        
//        program.clearScreen(r,g,b,a);
//        
////        xPos += elapsed * 1.1;
////        yPos += elapsed * 1.1;
//        
//        blockA.setMatrices();
//        blockA.drawPolygon(vertices_blockA);
////        blockB.resetPosition();
////        blockA.resetPosition();
//        
//        blockB.setMatrices();
//        blockB.drawPolygon(vertices_blockB);
//        
//        ball.setMatrices();
//        ball.drawPolygon(vertices_ball);
//        
////        moveBall(&ball);
//        checkInput(&ball, &blockA, &blockB);
//        checkForCollisions(&ball, &blockA, &blockB);
//        
//        if ( (blockAVictory == true) || (blockBVictory == true)){
//            if (blockAVictory){
//                r = 0.1; g = 0.1; b = 0.3; a = 1;
//            }
//            else{
//                r = 0.7; g = 0.7; b = 0.7; a = 1;
//            }
//            ball.resetPosition();
//            blockA.resetPosition();
//            blockB.resetPosition();
//            
//            ball.setBoundaries(0.5, -0.5, -0.5, 0.5);
//            blockA.setBoundaries(2.0, -2.0, -14.0, -13.0);
//            blockB.setBoundaries(2.0, -2.0, 13.0, 14.0);
//            elapsed = 0.0f;
//            
//            blockAVictory = false;
//            blockBVictory = false;
//            
//        }
//        
//        
//        ball.move(elapsed * xDir * ballSpeed, (elapsed * yDir)/2, 0.0);
//        
//        SDL_GL_SwapWindow(window.getDispWindow());
//        
//        
//        if (startup){
//            ballSpeed = 10.5;
//        }
//        startup = false;
//        
//    }
//    
//    SDL_Quit();
//    
//}