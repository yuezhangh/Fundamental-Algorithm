/* compiles with command line  gcc yhan.c -lX11 -lm -L/usr/X11R6/lib */
/* run with command line ./a.out obstacles */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#define maxnum 3000  //Less than 1000 obstacles

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Example Window";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey,gc_white;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

struct Point
{
  int x,y;
};

static int num_vertices = 0;//count vertices
static int total_num_vertices = 0;
static int graph[maxnum][maxnum]; //Distance between vertices
static struct Point vertices[maxnum];

int distance[maxnum];
int parent[maxnum];
int visited[maxnum];
struct Point test; //For judging if mouseclick in the triangles

int npoints;
int sign=0; //For judging start or target

void InputObstacles(char *filename) //Get coordinate of vertices
{
	FILE *pFile;
    int p_x,p_y,q_x,q_y,r_x,r_y; 
    int i = 0;    
    pFile = fopen (filename, "r");
    if (pFile == NULL) 
        perror ("Cannot open file");
    else
        while (!feof(pFile)) {
            fscanf (pFile, "T (%d, %d) (%d, %d) (%d, %d)\n", &p_x, &p_y, &q_x, &q_y,&r_x,&r_y);
            assert(p_x >= 0); 
			assert(p_y >= 0); 
			assert(q_x >= 0); 
			assert(q_y >= 0);
			assert(r_x >= 0);
			assert(r_y >= 0);
            vertices[i].x = p_x; vertices[i].y = p_y;
            vertices[i+1].x = q_x; vertices[i+1].y = q_y;
            vertices[i+2].x = r_x; vertices[i+2].y = r_y;
            i = i + 3;
            num_vertices = num_vertices + 3;
        }  
    total_num_vertices = num_vertices + 2;
    fclose (pFile);
}

int EdgeLength(struct Point p, struct Point q) //Calculate euclidean distance between p and q
{
    int m;
	m = p.x - q.x;
    if (m < 0)     
        m = m * -1;
    int n; 
	n = p.y - q.y;
    if (n < 0)
        n = n * -1;
    return (int) sqrt(m * m + n * n);
}

int Orientation (struct Point p, struct Point q, struct Point r) //Orientation of vectors
{
    double m;
	m = q.x * r.y + r.x * p.y + p.x * q.y - q.x * p.y - p.x * r.y - r.x * q.y;
    if(m > 0)
		return 1;
	else if (m < 0)
		return -1;
	else 
		return 0;
}

int SegmentIntersect(struct Point p, struct Point q, struct Point r, struct Point s) //Judge if egdes are intersect
{
	double d1,d2,d3,d4;
	d1 = Orientation(r, s, p);
	d2 = Orientation(r, s, q);
	d3 = Orientation(p, q, r);
	d4 = Orientation(p, q, s);
	if(d1 * d2 < 0 && d3 * d4 < 0)
		return 1;
	else
		return 0;
}

int InsideTriangle(struct Point p, struct Point q, struct Point r, struct Point s) //Judge if mouseclick is in the triangle
{
	double m,n,l;
	l = Orientation(p, q, r) * Orientation(s, q, r);
	m = Orientation(p, r, s) * Orientation(q, r, s);
	n = Orientation(p, q, s) * Orientation(r, q, s);
	if(m > 0 && n > 0 && l > 0)
		return 1;
	else
		return 0;
}

int Inside(int x, int y)
{
	test.x = x;
	test.y = y;
	int inside=0;
	int j;
	for(j = 0; j < num_vertices; j = j + 3)
	{
		inside = InsideTriangle(test, vertices[j], vertices[j+1], vertices[j+2]);
		if(inside == 1)
			break;
	}
	if(inside == 1)
		return 1;
	else
		return 0;
}

void GenerateGraph() //Generate edges of graph
{
    int i, j, k;
    int blocked;
    for (i = 0; i < total_num_vertices - 1; i++) {
        for (j = i + 1; j < total_num_vertices; j++ ) 
		{
			if(i / 3 == j / 3 && i != num_vertices);
			else
			{
				blocked = 0; 
				for (k = 0; k < total_num_vertices - 2; k = k + 3) 
				{
					if (SegmentIntersect(vertices[i], vertices[j], vertices[k], vertices[k+1]) 
						|| SegmentIntersect(vertices[i], vertices[j], vertices[k+1], vertices[k+2]) 
					|| SegmentIntersect(vertices[i], vertices[j], vertices[k+2], vertices[k])) 
					{
						blocked = 1;
						break;
					}
				}	
            if (blocked == 0)
                graph[i][j] = EdgeLength (vertices[i], vertices[j]); 
                graph[j][i] = graph[i][j];
			}
        }
    }
}

void Dijkstra (int source) //Dijkstra algorithm
{
    int i,j,k; 
    for (i = 0; i < total_num_vertices; i++) {
        distance[i] = INT_MAX;  
        visited[i] = 0;  
        parent[i] = -1; 
    }
    distance[source] = 0;   
    for (j = 0; j < total_num_vertices - 1; j++) {
        int min = LastVertex(distance, visited);
        visited[min] = 1;
        for (k = 0; k < total_num_vertices; k++) {
            if (!visited[k] && graph[min][k] 
                    && distance[min] != INT_MAX 
                    && distance[min] + graph[min][k] < distance[k]) {
                distance[k] = distance[min] + graph[min][k];
                parent[k] = min;
            }
        }
        if (min == total_num_vertices - 1)
            break;
    }
}

int LastVertex(int dist[], int sptSet[]) //Get the last visited vertex
{
    int min = INT_MAX;
    int l;
    int v;
    for (v = 0; v < total_num_vertices; v++)
    if (sptSet[v] == 0 && dist[v] <= min) {
        min = dist[v];
        l = v;
     }
   return l;
}

void ShortestPath () //Draw the shortest path   
{    
    int i,next,target;
	target = total_num_vertices - 1;                            
    while (next != -1) 
	{ 
        next = parent[target];
        if (next != -1)
            XDrawLine(display_ptr, win, gc, 
                    vertices[next].x, vertices[next].y,
                    vertices[target].x, vertices[target].y);
        target = next;
    }
}

//The following three functions are all for repeating shortest path process
void ResetGraph() 
{
    int i,j;
    for(i = 0; i < total_num_vertices; i++)
        for(j = 0; j < total_num_vertices; j++)
            graph[i][j] = 0;
}

void Draw()
{
	GenerateGraph();

	Dijkstra(total_num_vertices-2);
    
    ShortestPath(); 
    
    ResetGraph();     
}

void ReDraw () 
{
    XFillRectangle(display_ptr, win, gc_white, 0, 0, display_width, display_height);
    int i;
    for (i = 0; i <= num_vertices; i =  i + 3)
	{
		XPoint points[] = {{vertices[i].x,vertices[i].y},{vertices[i+1].x,vertices[i+1].y},
		{vertices[i+2].x,vertices[i+2].y},{vertices[i].x,vertices[i].y}};
		npoints = sizeof(points)/sizeof(XPoint);
        XFillPolygon(display_ptr,win,gc_grey,points,npoints,Convex,CoordModeOrigin);
    }

}

int main(int argc, char **argv)
{
	if(argc < 2)
		printf("Please input a filename \n");
	else
	{
		InputObstacles(argv[1]);
		display_ptr = XOpenDisplay(display_name);
		if( (display_ptr = XOpenDisplay(display_name)) == NULL )
		{
			printf("Could not open display. \n"); 
			exit(-1);
		}
		printf("Connected to X server  %s\n", XDisplayName(display_name) );
		screen_num = DefaultScreen( display_ptr );
		screen_ptr = DefaultScreenOfDisplay( display_ptr );
		color_map  = XDefaultColormap( display_ptr, screen_num );
		display_width  = DisplayWidth( display_ptr, screen_num );
		display_height = DisplayHeight( display_ptr, screen_num );

		printf("Width %d, Height %d, Screen Number %d\n", display_width, display_height, screen_num);

		//10% added space
		border_width = 10;
		win_x = display_width * 0.1; 
		win_y = display_height * 0.1;
		win_width = display_width * 0.8;
		win_height = display_height * 0.8; 
		win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num),win_x, win_y, 
		win_width, win_height, border_width,
		BlackPixel(display_ptr, screen_num),WhitePixel(display_ptr, screen_num) );
	
		size_hints = XAllocSizeHints();
		wm_hints = XAllocWMHints();
		class_hints = XAllocClassHint();
		if( size_hints == NULL || wm_hints == NULL || class_hints == NULL )
		{
			printf("Error allocating memory for hints. \n"); 
			exit(-1);
		}

		size_hints -> flags = PPosition | PSize | PMinSize  ;
		size_hints -> min_width = 60;
		size_hints -> min_height = 60;

		XStringListToTextProperty( &win_name_string,1,&win_name);
		XStringListToTextProperty( &icon_name_string,1,&icon_name);
	
		wm_hints -> flags = StateHint | InputHint ;
		wm_hints -> initial_state = NormalState;
		wm_hints -> input = False;

		class_hints -> res_name = "x_use_example";
		class_hints -> res_class = "examples";

		XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
		size_hints, wm_hints, class_hints );

		XSelectInput( display_ptr, win, ExposureMask | StructureNotifyMask | ButtonPressMask );
	
		XMapWindow( display_ptr, win );

		XFlush(display_ptr);

		gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
		XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
		XSetLineAttributes( display_ptr, gc, 4, LineSolid, CapRound, JoinRound);
	
		gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
		XSetLineAttributes(display_ptr, gc_yellow, 6, LineSolid,CapRound, JoinRound);
		if( XAllocNamedColor( display_ptr, color_map, "yellow", &tmp_color1, &tmp_color2 ) == 0 )
		{
			printf("failed to get color yellow\n"); 
			exit(-1);
		} 
		else
			XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );
	
		gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
		XSetLineAttributes( display_ptr, gc_red, 6, LineSolid, CapRound, JoinRound);
		if( XAllocNamedColor( display_ptr, color_map, "red", &tmp_color1, &tmp_color2 ) == 0 )
		{
			printf("failed to get color red\n"); 
			exit(-1);
		} 
		else
			XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
	
		gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
		if( XAllocNamedColor( display_ptr, color_map, "light grey", &tmp_color1, &tmp_color2 ) == 0 )
		{
			printf("failed to get color grey\n"); 
			exit(-1);
		} 
		else
			XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );
		gc_white = XCreateGC(display_ptr, win, 0, 0);
		XSetForeground(display_ptr, gc_white, WhitePixel( display_ptr, screen_num ));
		
		int i,j;
		while(1)
		{
			XNextEvent( display_ptr, &report );
			switch( report.type )
			{
				case Expose:
				for (i = 0; i <= num_vertices; i =  i + 3)
				{
					XPoint points[] = {{vertices[i].x,vertices[i].y},{vertices[i+1].x,vertices[i+1].y},
					{vertices[i+2].x,vertices[i+2].y},{vertices[i].x,vertices[i].y}};
					npoints = sizeof(points)/sizeof(XPoint);
					XFillPolygon(display_ptr,win,gc_grey,points,npoints,Convex,CoordModeOrigin);
				}
				break;
				case ConfigureNotify:			 
				win_width = report.xconfigure.width;
				win_height = report.xconfigure.height;
				break;
				case ButtonPress:
				{
					int x,y;
					x = report.xbutton.x;
					y = report.xbutton.y;
					if(Inside(x,y) == 0)
					{
						if (report.xbutton.button == Button1 )
							if(sign == 0)
							{
								ReDraw();
								XFillArc( display_ptr, win, gc_red, x - 3, y - 3, 
								6, 6, 0, 360*64);
								vertices[num_vertices].x = x;
								vertices[num_vertices].y = y;
								sign++;
							}
							else
								{
									XFillArc( display_ptr, win, gc_yellow, x - 3, y - 3,
									6, 6, 0, 360*64);
									vertices[num_vertices+1].x = x;
									vertices[num_vertices+1].y = y;
									sign--;
									Draw();
									vertices[num_vertices].x = 0;
									vertices[num_vertices].y = 0;
									vertices[num_vertices+1].x = 0;
									vertices[num_vertices+1].y = 0;
								}
						else
							exit(0);
					}
					else 
					{
						printf("You can not click in the triangle \n");
						ReDraw();
					}
				}
				break;
				default:
				break;
			}

		}
	exit(0);
	}
}
