#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include"polygonfilling.h"

void myInit(){

    glClearColor(0.0,0.0,0.0,0.0);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(0, 700, 0, 700);
    glClear(GL_COLOR_BUFFER_BIT);

    return;
}

int main(int argc, char *argv[]){

    int n = getData(argv[1]);
    printf("no of polygons = %d\n", n);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    glutInitWindowSize(maxWidth, maxScanlines);
    glutInitWindowPosition(100, 150);
    glutCreateWindow("Polygon Scanline Filling");
    myInit(); 
    glutDisplayFunc(drawDiagram);     
    glutMainLoop();
    
    return 0;
}