/* compiles with command line  gcc yhan.c -lX11 -lm -L/usr/X11R6/lib */
/* run with command line ./a.out obstacles */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define N 500
#define M 1000
#define INT_MAX	999999

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

GC gc, gc_yellow, gc_red, gc_grey, gc_green, gc_blue;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values, gc_green_values, gc_blue_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

struct Point
{
  int x,y;
};

struct Segment
{
	struct Point p;
	struct Point q;
	int length;
};

static struct Point p_vertical[N];
static struct Point q_vertical[N];
static struct Point p_horizontal[N];
static struct Point q_horizontal[N];
struct Segment sv[M][M];
struct Segment sh[M][M];
struct Point array[N];

static int v_count = 0;
static int h_count = 0;
static int count;
static int press;

int parent[M];
int value[M];
int visited[M];
int Graph_V[M][M];
int Graph_H[M][M];
int G[M][M];
int sum_v,sum_h;
int sum,my_sum;

void InputSegments(char *filename)
{
	FILE *pFile;
    int p1,p2,p3;
	char name;
    int i, j;
	i = 0;
	j = 0;
    pFile = fopen (filename, "r");
    if (pFile == NULL) 
        perror ("Cannot open file");
    else
        while (!feof(pFile)) {
            fscanf(pFile, "%c %d, %d, %d\n", &name, &p1, &p2, &p3);
			if(name == 'v')
			{
				p_vertical[i].x = p1;
				p_vertical[i].y = p2;
				q_vertical[i].x = p1;
				q_vertical[i].y = p3;
				i++;
			}
			else
			{
				p_horizontal[j].x = p2;
				p_horizontal[j].y = p1;
				q_horizontal[j].x = p3;
				q_horizontal[j].y = p1;
				j++;
			}
        }  
    v_count = i;
	h_count = j;
    fclose (pFile);
}

int min(int a, int b)
{
	if(a < b)
		return a;
	else
		return b;
}

int max(int a, int b)
{
	if(a > b)
		return a;
	else 
		return b;
}

int edgeLength(struct Point p, struct Point q)
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

int Orientation (struct Point p, struct Point q, struct Point r) 
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

int onSegment(struct Point p, struct Point q, struct Point r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return 1;
   else
	   return 0;
}

int Intersect(struct Point p1, struct Point q1, struct Point p2, struct Point q2)
{
    int o1 = Orientation(p1, q1, p2);
    int o2 = Orientation(p1, q1, q2);
    int o3 = Orientation(p2, q2, p1);
    int o4 = Orientation(p2, q2, q1);
 
    if (o1 != o2 && o3 != o4) return 1;
	
    if (o1 == 0 && onSegment(p1, p2, q1)) return 1;
    if (o2 == 0 && onSegment(p1, q2, q1)) return 1;
    if (o3 == 0 && onSegment(p2, p1, q2)) return 1;
    if (o4 == 0 && onSegment(p2, q1, q2)) return 1;
	
	return 0;
}

int minValue(int value[N], int visited[N], int n)
{
    int min, min_index;
    int v;
    min = INT_MAX;

    for (v = 0; v < n; v++) 
	{
		if (visited[v] == 0 && value[v] < min)
		{
			min = value[v];
			min_index = v;
		}
    }
	
    return min_index;
}


int Prim(int G[M][M], int n)
{
    int i, j, k, u, v;
    int sum;

    for ( i = 0; i < n; i++)
	{
		value[i] = INT_MAX;
		visited[i] = 0;
		parent[i] = -1;
    }

    value[0] = 0;
    parent[0] = -1;

    for (k = 0; k < n - 1; k++)
	{
		int u = minValue(value, visited, n);
		visited[u] = 1;
		for ( v = 0; v < n; v++)
		{
			if (G[u][v] && visited[v] == 0 && G[u][v] < value[v])
			{
				parent[v] = u;
				value[v] = G[u][v];
			}
		}
    }

    sum = 0; 
    for ( i = 1; i < n; i++)
	{
		sum = sum + G[i][parent[i]]; 
    }

    return sum;
}

void GenerateGraph()
{
	int i,j,k;
	for (i = 0; i < M; i++ )
	{
        for (j = 0; j < M; j++)
		{
			Graph_V[i][j] = Graph_H[i][j] = INT_MAX;
			sv[i][j].p.x = 0;
			sv[i][j].p.y = 0;
			sv[i][j].q.x = 0;
			sv[i][j].q.y = 0; 
			sv[i][j].length = 0;

			sh[i][j].p.x = 0;
			sh[i][j].p.y = 0;
			sh[i][j].q.x = 0;
			sh[i][j].q.y = 0; 
			sh[i][j].length = 0;
		}
    }
	
	for (i = 0; i < v_count; i++)
	{
		for (j = 0; j < v_count; j++)
		{
			for ( k = 0; k < h_count; k++)
			{
				if (i == j)
				{
					Graph_V[i][j] = 0;
					break;
				}
				else
				{
					if (Intersect(p_vertical[i], q_vertical[i], p_horizontal[k], q_horizontal[k]))
					{
						if (Intersect(p_vertical[j], q_vertical[j], p_horizontal[k], q_horizontal[k]))
						{
							Graph_V[i][j] = abs(q_vertical[i].x - q_vertical[j].x);
							sv[i][j].p.x = p_vertical[i].x;
							sv[i][j].p.y = p_horizontal[k].y;
							sv[i][j].q.x = q_vertical[j].x;
							sv[i][j].q.y = p_horizontal[k].y; 
							sv[i][j].length = Graph_V[i][j];
						}
					}
				}
			}
		}
    }


    for (i = 0; i < h_count; i++)
	{
		for (j = 0; j < h_count; j++)
		{
			for (k = 0; k < v_count; k++)
			{
				if (i == j)
				{
					Graph_H[i][j] = 0;
					break;
				}
				else
				{
					if (Intersect(p_horizontal[i], q_horizontal[i], p_vertical[k], q_vertical[k]))
					{
						if (Intersect(p_horizontal[j], q_horizontal[j], p_vertical[k], q_vertical[k]))
						{
							Graph_H[i][j] = abs(q_horizontal[i].y - q_horizontal[j].y); 			
							sh[i][j].p.x = p_vertical[k].x;
							sh[i][j].p.y = p_horizontal[i].y;
							sh[i][j].q.x = p_vertical[k].x;
							sh[i][j].q.y = q_horizontal[j].y; 
							sh[i][j].length = Graph_H[i][j];
						}
					}
				}
			}
		}
    }
	
	count = 0;
	for (i = 0; i < v_count; i++)
	{
		for (j = 0; j < h_count; j++)
		{
			if (Intersect(p_vertical[i], q_vertical[i], p_horizontal[j], q_horizontal[j]))
			{
				array[count].x = p_vertical[i].x;
				array[count].y = p_horizontal[j].y;    
				count++;
			}
		}
	}
	
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < M; j++)
		{
			G[i][j] = INT_MAX;
		}
	}
	
	for (i = 0; i < count; i++)
	{
		for (j = 0; j < count; j++)
		{
			if (i == j)
			{
				G[i][j] = 0;
			}
			if (i != j)
			{
				if (array[i].x != array[j].x && array[i].y != array[j].y)
				{
					G[i][j] = INT_MAX;
				}
				else
				{
					if (array[i].x == array[j].x)
					{
						for (k = 0; k < v_count; k++)
						{
							if (array[i].x == p_vertical[k].x && min(array[i].y, array[j].y) >= min(p_vertical[k].y, q_vertical[k].y) 
								&& max(array[i].y, array[j].y) <= max(p_vertical[k].y, q_vertical[k].y))
							{
								G[i][j] = abs(array[i].y - array[j].y);	
							}
						}
					}
					else if (array[i].y == array[j].y)
					{
						for (k = 0; k < h_count; k++)
						{
							if (array[i].y == p_horizontal[k].y && min(array[i].x, array[j].x) >= min(p_horizontal[k].x, q_horizontal[k].x) 
								&& max(array[i].x, array[j].x) <= max(p_horizontal[k].x, q_horizontal[k].x))
							{
								G[i][j] = abs(array[i].x - array[j].x);
							}
						}
					}
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	if(argc < 2)
		printf("Please input a filename \n");
	else
	{
		InputSegments(argv[1]);
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
		XSetLineAttributes( display_ptr, gc, 2, LineSolid, CapRound, JoinRound);
	
		gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
		XSetLineAttributes(display_ptr, gc_yellow, 2, LineSolid, CapRound, JoinRound);
		if(XAllocNamedColor(display_ptr, color_map, "yellow", &tmp_color1, &tmp_color2 ) == 0)
		{
			printf("failed to get color yellow\n"); 
			exit(-1); 
		} 
		else
			XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
		

		gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
		XSetLineAttributes(display_ptr, gc_red, 2, LineSolid, CapRound, JoinRound); 
		if( XAllocNamedColor(display_ptr, color_map, "red", &tmp_color1, &tmp_color2) == 0 )
		{
			printf("failed to get color red\n"); 
			exit(-1);
		} 
		else
			XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
		

		gc_grey = XCreateGC(display_ptr, win, valuemask, &gc_grey_values);
		if( XAllocNamedColor(display_ptr, color_map, "light grey",&tmp_color1, &tmp_color2) == 0)
		{
			printf("failed to get color grey\n"); 
			exit(-1);
		} 
		else
			XSetForeground(display_ptr, gc_grey, tmp_color1.pixel);
		

		gc_green = XCreateGC(display_ptr, win, valuemask, &gc_green_values);
		XSetLineAttributes(display_ptr, gc_green, 5, LineSolid, CapRound, JoinRound); 
		if( XAllocNamedColor(display_ptr, color_map, "green", &tmp_color1, &tmp_color2) == 0)
		{
			printf("failed to get color green\n"); 
			exit(-1);
		}
		else
			XSetForeground(display_ptr, gc_green, tmp_color1.pixel);
		

		gc_blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
		XSetLineAttributes(display_ptr, gc_blue, 5, LineSolid, CapRound, JoinRound); 
		if( XAllocNamedColor(display_ptr, color_map, "blue", &tmp_color1, &tmp_color2) == 0)
		{
			printf("failed to get color blue\n"); 
			exit(-1);
		} 
		else
			XSetForeground(display_ptr, gc_blue, tmp_color1.pixel);

		
		int i,j;
		while(1)
		{
			XNextEvent( display_ptr, &report );
			switch( report.type )
			{
				case Expose:
				GenerateGraph();
				
				for(i = 0; i < v_count; i++)
				{
					XDrawLine(display_ptr, win, gc, p_vertical[i].x, p_vertical[i].y, q_vertical[i].x, q_vertical[i].y);
				}
				
				for(i = 0; i < h_count; i++)
				{
					XDrawLine(display_ptr, win, gc, p_horizontal[i].x, p_horizontal[i].y, q_horizontal[i].x, q_horizontal[i].y);
				}
				
				if (v_count != 0)
				{
					sum_v = Prim(Graph_V, v_count);
					i = 1;
					while(1)
					{
						XDrawLine(display_ptr, win, gc_blue, sv[i][parent[i]].p.x, sv[i][parent[i]].p.y, sv[i][parent[i]].q.x, sv[i][parent[i]].q.y);
						XFlush(display_ptr);
						i++;
						if (i == v_count) break;
					}
				}
				
				if (h_count != 0)
				{
					sum_h = Prim(Graph_H, h_count);
					i = 1;
					while(1)
					{
						XDrawLine(display_ptr, win, gc_green, sh[i][parent[i]].p.x, sh[i][parent[i]].p.y, sh[i][parent[i]].q.x, sh[i][parent[i]].q.y);
						XFlush(display_ptr);
						i++;
						if (i == h_count) break;
					}
				}
				sum = sum_v + sum_h;
				
				my_sum = Prim(G, count);
				i = 1;
				while(1)
				{
					XDrawLine(display_ptr, win, gc_red, array[i].x, array[i].y, array[parent[i]].x, array[parent[i]].y);
					XFlush(display_ptr);
					i++;
					if (i == count) break;
				}
				
				printf("Lower bound: %d\n", sum);
				printf("The length of my solution: %d\n", my_sum);
				printf("ratio: %f\n", (float)my_sum/sum);
				
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
					if (report.xbutton.button == Button1)
						XFillArc(display_ptr, win, gc_red, x - win_height/80, y - win_height/80, 
						win_height / 80, win_height / 80, 0, 360*64);
					else
						XFillArc(display_ptr, win, gc_yellow, x - win_height / 70, y - win_height / 70, 
						win_height / 40, win_height / 40, 0, 360*64);
				}
				break;
				default:
				break;
			}

		}
	exit(0);
	}
}
