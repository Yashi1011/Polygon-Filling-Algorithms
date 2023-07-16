#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include"clip.h"
#include"polygonfilling.h"

// Array to store all the polygons and clip polygon.
struct Polygon *polygons, clip_polygon;
// File pointer and global varaible to store the number of polygons to be clipped.
FILE *fp;
int no_of_polygons = 0;

// Funtion to read data from file and store in clip_polygon and polygons.
int getData(char *fname){
    printf("%s\n",fname);
    fp = fopen (fname,"r");
    char str[100];
    int vertices, n = 0, i, clip_input = 0;
    if ( fp == NULL ){
        printf( "Could not open file\n" ) ;
        return 0;
    }
    polygons = (struct Polygon *)malloc(sizeof(struct Polygon));
    while(!feof(fp)){
        fgets(str, 100, fp);
        if(str[0] != '#' && str[0] != '\n'){
            if(feof(fp)){
                break;
            }
            // Read the clip polygon only once.
            if(clip_input == 0){
                sscanf(str, "%d", &clip_polygon.no_of_points);
                printf("%d\n", clip_polygon.no_of_points);
                i = 0;
                while(i<clip_polygon.no_of_points){
                    fgets(str, 100, fp);
                    if(str[0] != '#' && str[0] != '\n'){
                        sscanf(str, "%d %d", &clip_polygon.points[i].x, &clip_polygon.points[i].y);
                        printf("%d %d\n", clip_polygon.points[i].x, clip_polygon.points[i].y);
                        i++;
                    }
                    if(feof(fp)){
                        break;
                    }
                }
                clip_input = 1;
            }
            // Read the other polygon details.
            else{
                sscanf( str, "%d\t%f %f %f", &polygons[n].no_of_points, &polygons[n].red, &polygons[n].green, &polygons[n].blue);
                printf("%d %f %f %f\n",polygons[n].no_of_points, polygons[n].red, polygons[n].green, polygons[n].blue);
                i = 0;
                if(polygons[n].no_of_points>maxPoints){
                    printf("POINTS ARE GREATER THAN 100.\n");
                    break;
                }
                while(i < polygons[n].no_of_points){
                    fgets(str, 100, fp);
                    if(str[0] != '#' && str[0] != '\n'){
                        sscanf(str, "%d %d", &polygons[n].points[i].x, &polygons[n].points[i].y);
                        printf("%d %d\n", polygons[n].points[i].x, polygons[n].points[i].y);
                        i++;
                    }
                    if(feof(fp)){
                        break;
                    }
                }
                n++;
                polygons = realloc(polygons, (n+1)*sizeof(struct Polygon));
            }
            if(feof(fp)){
                break;
            }
        }
    }
    no_of_polygons = n;
    return no_of_polygons;
}

// Returns x-value of point of intersectipn of two lines.
int x_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    int num = (x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}
  
// Returns y-value of point of intersectipn of two lines.
int y_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    int num = (x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// Funtion to draw the polygon of index n.
// void drawPolygon(int n){
//     int i=0;
//     glColor3f(polygons[n].red, polygons[n].green, polygons[n].blue);
//     // glBegin(GL_POLYGON);
//     glBegin(GL_LINE_LOOP);
//     for(i=0; i < polygons[n].no_of_points; i++){
//         glVertex2i(polygons[n].points[i].x, polygons[n].points[i].y);
//     }
//     glEnd();
//     glFlush();
// }

// Function to draw the clipping polygon.
void drawClipPolygon(){    
    int i=0;
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for(i=0; i < clip_polygon.no_of_points; i++){
        // int f = i, l = (i+1)%polygons[n].no_of_points;
        glVertex2i(clip_polygon.points[i].x, clip_polygon.points[i].y);
        // glVertex2i(polygons[n].points[l].x, polygons[n].points[l].y);
    }
    glEnd();
    glFlush();
}

// Funtion that clips the polygon n
// This works for all the non-convex clipping polygons.
void clipPolygon(int n){
    for(int j=0; j < clip_polygon.no_of_points; j++){
        printf("SIDE %d:\n", j+1);
        // j and secondpt are indices of two points of clipping polygon.
        int secondpt = (j+1)%clip_polygon.no_of_points;
        int x1,x2,y1,y2;
        // p1 and p2 information.
        x1 = clip_polygon.points[j].x;
        y1 = clip_polygon.points[j].y;
        x2 = clip_polygon.points[secondpt].x;
        y2 = clip_polygon.points[secondpt].y;

        // Store the new points in array of points.
        struct Point newPoints[maxPoints];
        int new_n = 0;

        for (int i = 0; i < polygons[n].no_of_points; i++){
            // i and k form a line in polygon
            int k = (i+1) % polygons[n].no_of_points;
            int fx = polygons[n].points[i].x, fy = polygons[n].points[i].y;
            int lx = polygons[n].points[k].x, ly = polygons[n].points[k].y;
      
            // Calculating position of first point w.r.t. clipper line
            int fpoint_pos = (x2-x1) * (fy-y1) - (y2-y1) * (fx-x1);
      
            // Calculating position of second point w.r.t. clipper line
            int lpoint_pos = (x2-x1) * (ly-y1) - (y2-y1) * (lx-x1);
            printf("first point position: %d\nlast point position: %d\n", fpoint_pos, lpoint_pos);
            // Case 1 : When both points are inside
            if (fpoint_pos < 0  && lpoint_pos < 0){
                //Only second point is added
                newPoints[new_n].x = lx;
                newPoints[new_n].y = ly;
                new_n++;
            }
      
            // Case 2: When only first point is outside
            else if (fpoint_pos >= 0  && lpoint_pos < 0){
                // Point of intersection with edge and the second point is added
                if(lpoint_pos!=0){
                    newPoints[new_n].x = x_intersect(x1,
                                      y1, x2, y2, fx, fy, lx, ly);
                    newPoints[new_n].y = y_intersect(x1,
                                      y1, x2, y2, fx, fy, lx, ly);
                    new_n++;
                }
                newPoints[new_n].x = lx;
                newPoints[new_n].y = ly;
                new_n++;
            }
      
            // Case 3: When only second point is outside
            else if (fpoint_pos < 0  && lpoint_pos >= 0){
                // Only point of intersection with edge is added
                newPoints[new_n].x = x_intersect(x1,
                                  y1, x2, y2, fx, fy, lx, ly);
                newPoints[new_n].y = y_intersect(x1,
                                  y1, x2, y2, fx, fy, lx, ly);
                new_n++;
            }
      
            // Case 4: When both points are outside
            // therefore nothing is added.

        }
      
        // Copying new points into original array and changing the no. of vertices
        polygons[n].no_of_points = new_n;
        printf("new size = %d\n", new_n);
        for (int i = 0; i < new_n; i++){
            polygons[n].points[i].x = newPoints[i].x;
            polygons[n].points[i].y = newPoints[i].y;
            printf("%d %d\n", newPoints[i].x, newPoints[i].y);
        }
        printf("\n");
    }
    if(polygons[n].no_of_points>3){
        // drawPolygon(n);
        // FillPolygon(n);
        drawDiagram(n);
    }
}

// Funtion to check if the points of clipping polygon are in clockwise or anti-clockwise direction.
int isClockWise(){
    int i, sum = 0;
    for(i=0; i < clip_polygon.no_of_points; i++){
        int l = (i+1)%clip_polygon.no_of_points;
        sum += (clip_polygon.points[l].x - clip_polygon.points[i].x)*(clip_polygon.points[l].y + clip_polygon.points[i].y);
    }
    printf("sum = %d\n", sum);
    if(sum>0){
        return 1;
    }else{
        return 0;
    }
}

//  Funtion to change the points of clip polygon to clockwise if they aren't.
void clipPolygonCheck(){
    if(isClockWise()==0){
        int start = 0, end = clip_polygon.no_of_points-1;
        struct Point temp;
        while (start < end){
            temp = clip_polygon.points[start];
            // temp = arr[start];  
            clip_polygon.points[start] = clip_polygon.points[end];
            clip_polygon.points[end] = temp;
            start++;
            end--;
        }
    }
}

// Funtion to clip all polygons
void clipAllPolygons(){
    int i;
    clipPolygonCheck();
    drawClipPolygon();
    for(i=0; i<no_of_polygons; i++){
        printf("CALLING CLIP POLYGON FOR POLYGON %d\n", i+1);
        // drawPolygon(i);
        clipPolygon(i);
        printf("\n");
    }
}
