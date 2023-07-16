#define maxPoints 100
#define maxScanlines 700
#define maxWidth 700

struct Edge{
    int y_max;
    float x_ymin, inverse_slope;
    struct Edge* next;
};

struct EdgeTableEntry{
    int size_of_table;
    struct Edge *edges;
};

// function to read and store data.
int getData(char *fname);

// Initializing Active Edge Tabel and Edge Table.
void initiateEdgeTables();

// Function to add Edge entry
struct Edge *addEdgeEntry(struct EdgeTableEntry *e, int ymax, float x_ymin, float slope_inverse);

// Funtion to remove Edge
void removeEdgeEntry(struct EdgeTableEntry *e, int y);

void sortedInsert(struct Edge **sorted, struct Edge *newNode);

// function to sort a singly linked list using insertion sort
void Sort(struct EdgeTableEntry *e);

// Store the edge in edge table
void storeEdgeInTable (int x1, int y1, int x2, int y2);

// Function to print Edge table
void printTable();

void update_x_ymin(struct EdgeTableEntry *e);

// Polygon filling algorithm implementation.
void FillPolygon(int n);

// Draws the Polygon outline and store edges in Edge Table.
// n is the index of the polygon that we are drawing.
void drawPolygon(int n);

// Function that calls drawPolygon() ans FillPolygon() functions foe each polygon.
void drawDiagram(int n);
