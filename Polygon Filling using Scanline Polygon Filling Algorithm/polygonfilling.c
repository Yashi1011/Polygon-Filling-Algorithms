#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include"polygonfilling.h"

// Defining Edge table and Active Edge table
struct EdgeTableEntry ET[maxScanlines], AET;
// File pointer
FILE *fp;
// Polygons storing array
struct Polygon *polygons;
int yMax = -1, no_of_polygons = 0;

// function to read and store data.
int getData(char *fname){
    printf("%s\n",fname);
    fp = fopen (fname,"r");
    char str[100];
    int vertices, n = 0, i;
    if ( fp == NULL ){
        printf( "Could not open file\n" ) ;
        return 0;
    }
    while(!feof(fp)){
        fgets(str, 100, fp);
        if(str[0] != '#'){
            break;
        }
    }
    polygons = (struct Polygon *)malloc(sizeof(struct Polygon));
    sscanf( str, "%d\t%f %f %f", &polygons[0].no_of_points, &polygons[0].red, &polygons[0].green, &polygons[0].blue);
    
    printf("%d %f %f %f\n",polygons[0].no_of_points, polygons[0].red, polygons[0].green, polygons[0].blue);
    while(!feof(fp)){
        for(i = 0; i<polygons[n].no_of_points; i++){
            fscanf(fp, "%d %d", &polygons[n].points[i].x, &polygons[n].points[i].y);
            printf("%d %d\n", polygons[n].points[i].x, polygons[n].points[i].y);
        }

        fgets(str, 100, fp);
        if(feof(fp)){
            break;
        }
        n++;

        polygons = realloc(polygons, (n+1)*sizeof(struct Polygon));
        fscanf(fp, "%d\t%f %f %f", &polygons[n].no_of_points, &polygons[n].red, &polygons[n].green, &polygons[n].blue);
        printf("%d %f %f %f\n",polygons[n].no_of_points, polygons[n].red, polygons[n].green, polygons[n].blue);
    }
    fclose(fp);
    no_of_polygons = n+1;
    return no_of_polygons;
}

// Initializing Active Edge Tabel and Edge Table.
void initiateEdgeTables(){
    int i = 0;
    for(i = 0; i < maxScanlines; i++){
        ET[i].edges = NULL;
    }
    AET.edges = NULL;
    return;
}

// Funtion to add edge to edge table
struct Edge *addEdgeEntry(struct EdgeTableEntry *e, int ymax, float x_ymin, float slope_inverse){

    struct Edge *b = (struct Edge *)malloc(sizeof(struct Edge));
    
    b->y_max = ymax;
    b->x_ymin = x_ymin;
    b->inverse_slope = slope_inverse;
    
    struct Edge *head = e->edges;
    // If head is null that is no edge is added then it is added at starting.
    // Else the edge is added at head and all the edges ate attached to head.
    if(head == NULL){
        head = b;
        head->next = NULL;
        return head;
    }
    b->next = head;
    head = b;
    return head;
}

// Function to remove edge table
void removeEdgeEntry(struct EdgeTableEntry *e, int y){
    struct Edge *curr = e->edges, *prev=NULL;
    while(curr != NULL){
        //If the current bucket has y_max value same as current y(scanline value) in the algorithm
        //then remove it
        if(curr->y_max == y){
            printf("Removed %d\n", curr->y_max);
            if(prev == NULL){
                //First node (head) is the match
                e->edges = curr->next;
                free(curr);
                curr = e->edges;
                prev = NULL;
                
            }else{
                prev->next = curr->next;
                free(curr);
                curr = prev->next;
            }
        }else{
            prev = curr;
            curr = curr->next;
        }    
    }
}

// Funtion to insert the sorted edge.
void sortedInsert(struct Edge **sorted, struct Edge *newNode){
    // Special case for the head end
    if ((*sorted) == NULL || (*sorted)->x_ymin >= newNode->x_ymin){
        newNode->next = *sorted;
        *sorted = newNode;
    }
    else{
        struct Edge *current = *sorted;
        // Locate the node before the point of insertion
        while (current->next != NULL && current->next->x_ymin < newNode->x_ymin){
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// function to sort a singly linked list using insertion sort
void Sort(struct EdgeTableEntry *e){
    struct Edge *sorted = NULL;
    struct Edge *current = e->edges;
    while (current != NULL){
        struct Edge  *next = current->next;
        sortedInsert(&sorted, current);
        current = next;
    }
    e->edges = sorted;
}

// Function to store edge in edge table
void storeEdgeInTable (int x1, int y1, int x2, int y2){
    float slope, slope_inverse, x_ymin;
    int y_max, scanLine;
    //If (x2==x1) slope will be infinite so inverse will be 0;
    if (x2==x1){
        slope_inverse = 0;
    }
    else{
        // horizontal lines are not stored in edge table
        if (y2==y1)
            return;
        slope_inverse = ((float)(x2-x1))/((float)(y2-y1));
        printf("\nSlope string for %d %d & %d %d: %f\n", x1, y1, x2, y2, slope_inverse);
    }
    //The scanline would be the MINIMUM value of y
    if (y1>y2){
        scanLine = y2;
        y_max = y1;
        x_ymin = x2;
    }
    else{
        scanLine = y1;
        y_max = y2;
        x_ymin = x1;
    }
    // the assignment part is done..now storage..
    ET[scanLine].edges = addEdgeEntry(&ET[scanLine], y_max, x_ymin, slope_inverse);
    printf("Successfully stored in ET\n");
}

// Funtion to print edge table
void printTable(){
    int i =0;
    for(i=0; i<maxScanlines;i++){
        if(ET[i].edges){
            // printEdgeEntry(&ET[i]);
            struct Edge *temp = ET[i].edges;
            while(temp != NULL){
                printf("| %d | %f | %f |->",temp->y_max, temp->x_ymin, temp->inverse_slope);
                temp=temp->next;
            }
            printf("\n\n");
        }   
    }
}

// Funtion to update x_ymin
void update_x_ymin(struct EdgeTableEntry *e){
    struct Edge *temp = e->edges;
    while(temp != NULL){
        temp->x_ymin += temp->inverse_slope;
        temp = temp->next;
    }
}

void FillPolygon(int n){
    /* Follow the following rules:
    1. Horizontal edges: Do not include in edge table
    2. Horizontal edges: Drawn either on the bottom or on the top.
    3. Vertices: If local max or min, then count twice, else count once.
    4. Either vertices at local minima or at local maxima are drawn.
    */

    int i, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;

    // we will start from scanline 0;
    // Repeat until last scanline:
    for (i=0; i<yMax; i++){ 
        // AET those edges whose ymin = y (entering edges)
        struct Edge *temp = ET[i].edges;
        while(temp != NULL)        {
            AET.edges = addEdgeEntry(&AET,temp->y_max, temp->x_ymin, temp->inverse_slope);
            temp = temp->next;
        }
        // which y=ymax (not involved in next scan line)
        removeEdgeEntry(&AET, i);
        Sort(&AET);

        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        struct Edge *aet_temp = AET.edges;
        while (aet_temp != NULL){
            if (coordCount%2==0){
                x1 = (int)(aet_temp->x_ymin);
                ymax1 = aet_temp->y_max;
                if (x1==x2){
                    /* three cases can arrive-
                        1. lines are towards top of the intersection
                        2. lines are towards bottom
                        3. one line is towards top and other is towards bottom
                    */
                    if (!(((x1==ymax1) && (x2!=ymax2))||((x1!=ymax1) && (x2==ymax2)))){
                        coordCount++;
                    }
                }

                else{
                    coordCount++;
                }
            }
            else{
                x2 = (int)aet_temp->x_ymin;
                ymax2 = aet_temp->y_max;
                FillFlag = 0;

                // checking for intersection...
                if (x1==x2){
                    if (!(((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2)))){
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else{
                        coordCount++;
                        FillFlag = 1;
                }

                if(FillFlag==1){
                    //drawing actual lines...
                    glColor3f(polygons[n].red, polygons[n].green, polygons[n].blue);
                    glBegin(GL_LINES);
                    glVertex2i(x1,i);
                    glVertex2i(x2,i);
                    glEnd();
                    glFlush();
                }

            }
            aet_temp = aet_temp->next;
        }
        update_x_ymin(&AET);
    }
    printf("\nScanline filling completed\n");
}

// Draws the Polygon outline and store edges in Edge Table.
// n is the index of the polygon that we are drawing.
void drawPolygon(int n){

    int x1, y1, x2, y2, i;
    int first_x=0, first_y = 0;

    GLint no_of_vertices = polygons[n].no_of_points;

    glBegin(GL_LINE_LOOP);

    glColor3f(polygons[n].red, polygons[n].green, polygons[n].blue);
    // 1st vertex
    x1 = polygons[n].points[0].x;
    y1 = polygons[n].points[0].y;
    if(i == 0){
        first_x = x1;
        first_y = y1;
    }
    if(yMax < y1){
        yMax = y1;
    }
    glVertex2i(x1, y1);

    for(int i = 1; i < no_of_vertices; i++){

        x2 = polygons[n].points[i].x;
        y2 = polygons[n].points[i].y;
        if(yMax < y2){
            yMax = y2;
        }
        glVertex2i(x2, y2);
        // printf("SET: i is %d and (%d, %d, %d, %d)\n", i, x1, y1, x2, y2);
        storeEdgeInTable(x1, y1, x2, y2);
        if(i==(no_of_vertices-1)){
            // printf("LAST SET: i is %d and (%d, %d, %d, %d)\n", i, first_x, first_y,  x2, y2);
            storeEdgeInTable(first_x, first_y, x2, y2);
        }
        x1 = x2;
        y1 = y2;
    }   
    glEnd();
    glFlush();

}

// Function that calls drawPolygon() ans FillPolygon() functions foe each polygon.
void drawDiagram(void){
    for(int i=0;i<no_of_polygons;i++){
        initiateEdgeTables();
        drawPolygon(i);
        printf("\nTable\n");
        printTable();
        printf("YMAX is %d\n", yMax);
        FillPolygon(i); // Actual calling of scanline filling.
    }
}
