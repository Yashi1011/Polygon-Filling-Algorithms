#define maxPoints 100
#define maxScanlines 700
#define maxWidth 700

struct Point{
    int x;
    int y;
};

struct Polygon{
    float red;
    float green;
    float blue;
    int no_of_points;
    struct Point points[maxPoints];
};

int getData(char *fname);

// Returns x-value of point of intersectipn of two lines.
int x_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
  
// Returns y-value of point of intersectipn of two lines.
int y_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

// void drawPolygon(int n);

void drawClipPolygon();

void clipPolygon(int n);

int isClockWise();

void clipPolygonCheck();

void clipAllPolygons();
