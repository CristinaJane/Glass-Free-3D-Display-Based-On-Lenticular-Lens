//
//  main.cpp
//  Swapping3d
//
//  Created by Zhang Enjie(54157069) on 15/10/21.
//  Copyright (c) 2015年 Znj. All rights reserved.
//


/*
 Requirements:
 OpenGL and GLUT Library are required
 stdio.h and stdlib.h are required
 vector.h is required
 iostream is required
 */

//#include <windows.h>      //if running on windows os
#include <iostream>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#define head_Length 54           //BMP file has 54bytes head
#define widthLocation 0x0012     //BMP width address
#define heightLocation 0x0016    //BMP height address

using namespace std;

//-----------------------------------------------Definition-----------------------

int num;                      //the number of files
int widthOfBMP;               //the width of BMP
int heightOfBMP;              //the height of BMP
int imageDirection=1;           //positive of negative

vector<char*> filenamevessel; //the array of filename
vector<FILE*> file;           //the array of file pointer
vector<GLubyte*> Buffers;     //the array of Buffers
vector<char*> head;           //array of headInfo of BMP file
vector<int*> width;           //the array of width of BMP
vector<int*> height;          //the array of height of BMP
vector<GLuint*> length;       //the array of pixelLength
GLubyte* pixelBuffer;         //the BMP pixels
GLuint pixelLength;           //the pixelLength
GLubyte* MixedBuffer;         //the Mixed Image Buffer
GLubyte* MaskRenderedBuffer;  //the Buffer rendered by the Mask

int mask[9][9]={              //Mask array given in the PPT
    {0,1,2,6,7,8,3,4,5},
    {8,0,1,5,6,7,2,3,4},
    {7,8,0,4,5,6,1,2,3},
    {6,7,8,3,4,5,0,1,2},
    {5,6,7,2,3,4,8,0,1},
    {4,5,6,1,2,3,7,8,0},
    {3,4,5,0,1,2,6,7,8},
    {2,3,4,8,0,1,5,6,7},
    {1,2,3,7,8,0,4,5,6}
};

//these definitions are not essential in this MiniProject, just for testing of the effect of images on OpenGL
int interval=200;
int addition=10;
int SwapNumber=0;
GLfloat Angle=0.0f;
GLuint texture[5];
int mouseX=-1,mouseY=-1;
float c=M_PI/180.0f;
float r=1.5f,h=0.0f;
int V=0.1f;

//-------------------------------------------Functions------------------------


//-------------------------------------------Processing-----------------------


void load_BMP(){        //load pixelBuffers of BMP
    
    //load files
    
    //you have to add the names of files into the 1.txt file, and separate each of them by a space ' '.
    
    filenamevessel.clear();
    file.clear();
    cout<<"enter the number of files in txt: ";
    cin>>num;
    cout<<endl;
    file.push_back(fopen("/test/1.txt", "rb")); //modify the first parameter to your own path
    
    for(int i=0;i<num;i++){
        char *filename=new char[50];            //filename
        fscanf(file[0],"%s",filename);       //read filename in the txt
        //cout<<filename<<" ";
        filenamevessel.push_back(filename);          //put filename in the vessel[]
        cout<<"filename: "<<filenamevessel[i]<<" ";
        file.push_back(fopen(filename, "rb"));     //put file pointer in the file[]
        cout<<"fileAddress: "<<file[i+1]<<endl;
    }
    
    //read head and pixels
    
    for(int j=1;j<file.size();j++){
        char *headInfo=new char[54];            //store the headInfo of BMP file
        fseek(file[j], 0, SEEK_SET);
        fread(headInfo, sizeof(char), head_Length, file[j]);      //read head of BMP
        cout<<"the internal Pointer: "<<ftell(file[j])<<endl;
        head.push_back(headInfo);             //head[j-1]------>file[j], cuz file[0] is 1.txt
        
        fseek(file[j], widthLocation, SEEK_SET);                 //read width and height of BMP
        fread(&widthOfBMP, sizeof(int), 1, file[j]);
        fseek(file[j], heightLocation, SEEK_SET);
        fread(&heightOfBMP, sizeof(int), 1, file[j]);
        if(heightOfBMP<=0){heightOfBMP=-heightOfBMP;imageDirection=-1;}
        cout<<filenamevessel[j-1]<<": "<<heightOfBMP<<"*"<<widthOfBMP<<" pixels"<<endl;
        
        pixelLength=widthOfBMP*3;                      //in case of the size of BMP is not the multiple of 4
        while(pixelLength%4!=0){pixelLength++;}
        pixelLength*=heightOfBMP;
       
        pixelBuffer=(GLubyte*)malloc(pixelLength);
        fseek(file[j], head_Length, SEEK_SET);
        fread(pixelBuffer, sizeof(GLubyte),pixelLength, file[j]);   //read pixels
        Buffers.push_back(pixelBuffer);           //Buffers[j-1]----->file[j]
        
    }
}

//------------------------------------Wobbling_3d functions-------------------------------


void Wobbling_3d(){                        //draw the wobbling Buffer on the screen
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    if(SwapNumber==num){SwapNumber=0;}  //num=the number of images
    
    glDrawPixels(widthOfBMP, heightOfBMP, GL_BGR_EXT, GL_UNSIGNED_BYTE, Buffers[SwapNumber++]);
    
    
    
    glutSwapBuffers();
    
}


//--------------------------------------Parallax_3d--------------------------------------


void interleave_Parallax(){
    
    
    MixedBuffer=(GLubyte*)malloc(pixelLength);
    
    for(int i=0;i<(pixelLength/3);i++){
        if(i%2==0){
            *(MixedBuffer+i*3)=*(Buffers[0]+i*3);
            *(MixedBuffer+i*3+1)=*(Buffers[0]+i*3+1);
            *(MixedBuffer+i*3+2)=*(Buffers[0]+i*3+2);
        }
        else{
            *(MixedBuffer+i*3)=*(Buffers[1]+i*3);
            *(MixedBuffer+i*3+1)=*(Buffers[1]+i*3+1);
            *(MixedBuffer+i*3+2)=*(Buffers[1]+i*3+2);
        }
    }
}

void writeBuffer(){                                   //write the pixels to BMP file
    FILE* write;
    write=fopen("/test/interleaved_3d", "wb");    //modify the first parameter to your own path
    fseek(write, 0, SEEK_SET);
    fwrite(head[0], sizeof(char), 54, write);
    fseek(write, 54, SEEK_SET);
    fwrite(MixedBuffer,sizeof(GLubyte),pixelLength,write);
    fclose(write);
    cout<<"writeBuffer successfully! "<<endl;
}

//--------------------------------Multiview Autostereoscopic functions---------------------

void getMask(){   //display the mask array on the console window
    
    cout<<"the mask array is: "<<endl;
    
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            cout<<mask[i][j]<<" ";
        }
        cout<<endl;
    }
}

void maskRender(){    //integrate the 9 images into 1 image
    
    MaskRenderedBuffer=(GLubyte*)malloc(pixelLength);
    int m,n,maskValue;
    for(int i=0;i<heightOfBMP;i++){
        m=i%9;
        for(int j=0;j<widthOfBMP*3;j++){
            n=j%9;
            maskValue=mask[8-m][n];    //why [8-m]: because the rows of pixels in BMP file are inverted
            *(MaskRenderedBuffer+i*widthOfBMP*3+j)=*(Buffers[maskValue]+i*widthOfBMP*3+j);
        }
    }
}

void writeMaskRenderedBuffer(){                                   //write the pixels to BMP file
    FILE* write;
    write=fopen("/test/saxfinal.bmp", "wb");     //modify the first parameter to your own path
    fseek(write, 0, SEEK_SET);
    fwrite(head[8], sizeof(char), 54, write);
    fseek(write, 54, SEEK_SET);
    fwrite(MaskRenderedBuffer,sizeof(GLubyte),pixelLength,write);
    fclose(write);
    cout<<"writeBuffer successfully! "<<endl;
}



//-----------------------not essential, draw the pixels on the window

void drawMixedBuffer(){
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawPixels(widthOfBMP, heightOfBMP, GL_BGR_EXT, GL_UNSIGNED_BYTE,MixedBuffer);
    
    glutSwapBuffers();
}

void drawMaskRenderedBuffer(){
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawPixels(widthOfBMP, heightOfBMP, GL_BGR_EXT, GL_UNSIGNED_BYTE,MaskRenderedBuffer);
    
    glutSwapBuffers();
}


void textureMapping(){                              //textureMapping
    
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(SwapNumber==2){SwapNumber=0;}
    glTexImage2D(GL_TEXTURE_2D, 0, 4, widthOfBMP,heightOfBMP, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Buffers[SwapNumber++]);
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT|GL_BGR_EXT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.5f);
    //glRotatef(Angle, 0.0f, 1.0f,0.0f);
    //glRotatef(Angle, 1.0f, 0.0f, 0.0f);
    //glRotatef(Angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -0.5f);
    glColor3f(1.0, 1.0f, 1.0f);
    glEnable(GL_SMOOTH);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); //glColor3f(1.0f, 0.0f,0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); //glColor3f(0.0f, 1.0f,0.0f);
    glVertex3f( 0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); //glColor3f(1.0f, 0.0f,1.0f);
    glVertex3f( 0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); //glColor3f(0.5f, 0.5f,0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glPopMatrix();
    glEnd();
    glutSwapBuffers();
}

void Rotate(){                //not essential     Rotation,   case 180: similar to Wobbling
    
    Angle+=0.1f;
    glutPostRedisplay();
}

void Timer(int){                //not essential
    
    glutPostRedisplay();
    //addition+=0;
    //interval+=addition;
    glutTimerFunc(interval,Timer,1);
}

void  mouseControl(int button,int state,int x,int y){   //not essential
    
    
    switch(state)
    {
        case GLUT_DOWN:
            Angle+=1.0f;
            glutPostRedisplay();
            break;
    }
    
}



//-----------------------------------------------Main function-----------------------------

int main(int argc, char * argv[]) {
    
    //load,render and write
    
    getMask();
    
    load_BMP();
    
    maskRender();
    writeMaskRenderedBuffer();
    free(pixelBuffer);          //free the space of pixelbuffer
    free(MaskRenderedBuffer);     //free the space of maskrenderedbuffer
    
    
    /*load_BMP();
    interleave_Parallax();
    writeBuffer();
    *///parallax_3d
    
    //initialization of GLUT Window
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GL_BGR_EXT|GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(widthOfBMP, heightOfBMP);
    glutCreateWindow("EE6618_MiniProject1");
    //glutDisplayFunc(drawMaskRenderedBuffer);

    
    //----------------------------------------GLUT display
    

    //glutDisplayFunc(Wobbling_3d);
    //glutTimerFunc(100, Timer, 1);
    //Wobbling_3d
    
    
    glutDisplayFunc(textureMapping);
    glutTimerFunc(100, Timer, 1);
    ///draw on the window and Rotate
 
    //-------------------------------------------GLUT loop
    
    glutMainLoop();
    
    return 0;
    
}

