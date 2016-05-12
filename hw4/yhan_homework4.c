/* compiles with command line  gcc yhan.c -lX11 -lm -L/usr/X11R6/lib */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h>  
#define MAX_N 20 
#define INT_MAX	500

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

struct path  
{  
int current;  
int cost;  
long set;  
struct path *lastNode;  
struct path *next;  
}; 

static struct Point vertices[500];
struct path *D[MAX_N];

static int num_vertices = 0;
static int graph[INT_MAX][INT_MAX];
static int g_group[MAX_N][MAX_N];
static int draw[21];
static int draw_group[MAX_N][21];
static int connection[INT_MAX][INT_MAX];
static int q = 0;
static int totalcost = 0;

void InputVertices(char *filename)
{
	FILE *pFile;
    int p1,p2;
    int i;
	i = 0;
    pFile = fopen (filename, "r");
    if (pFile == NULL) 
        perror ("Cannot open file");
    else
        while (!feof(pFile)) {
            fscanf(pFile, "V (%d, %d)\n", &p1, &p2);
			if(p1 >= 0 && p1 <= 500 && p2 >= 0 && p2 <= 500)
			{
				vertices[i].x = p1;
				vertices[i].y = p2;
				i++;
				num_vertices++;
			}
        }
    fclose (pFile);
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

void GenerateGraph()
{
	int i, j;
	for(i = 0; i < num_vertices; i++)
	{
		for(j = 0; j < num_vertices; j++)
		{
			if(i == j)
			{
				graph[i][j] = 0;
			}
			else
			{
				graph[i][j] = edgeLength(vertices[i], vertices[j]);
				graph[j][i] = edgeLength(vertices[i], vertices[j]);
			}
		}
	}
}

void Sort()
{
	struct Point s;
	int i,j;
	for(i = 0; i < num_vertices; i++)
		for(j = i + 1; j < num_vertices; j++)
		{
			if(vertices[i].x > vertices[j].x)
				{
						s.x = vertices[i].x;
	s.y = vertices[i].y;
	vertices[i].x = vertices[j].x;
	vertices[i].y = vertices[j].y;
	vertices[j].x = s.x;
	vertices[j].y = s.y;
				}
		}
	
}

void Group()
{
	int i,j;
	if(num_vertices <= 20)
	{
		GenerateGraph();
		for(i = 0; i < num_vertices; i++)
			for(j = 0; j < num_vertices; j++)
			{
				g_group[i][j] = graph[i][j];
			}
	}
	else
	{
		Sort();
		GenerateGraph();
	}
}

int mypow(int x, int y)  
{
	return (int)pow((double)x,(double)y);  
}   

int inSet(int i, int set)  
{
	if((mypow(2,i-1)&set)>0)  
		return 1;
	else  
		return 0;  
}  
  
int insertSet(int i,int set)  
{
	if(inSet(i,set)==0)  
		return set|mypow(2,i-1);
	return set;  
}  
 
int deleteSet(int i, int set)  
{
	if(inSet(i,set))
	{
		set = ~(mypow(2,i-1))&set;
	}
	return set;  
}  
  
void print_path(struct path *p)  
{
	int sign;
	q++;
	if(p==NULL)
	{
		sign = 1;
		draw[q] = 1;
		printf("1");
		return;
	}
	print_path(p->lastNode);
	q--;
	draw[q] = p->current + 1;
	printf("->%d",p->current + 1);
}

 int tsp(int n, int (*a)[MAX_N])  
{
	int i,j;
	struct path *temp,*p;
	struct path **address;  
	long set;  
	int cost;  
	int mincost;  
	struct path *lastNode;
	q=0;  
	D[1] = NULL;    
	for(i=1;i<n;i++)
	{
		temp = malloc(sizeof(struct path));  
		temp->current = i;  
		temp->cost = a[0][i];  
		temp->lastNode = NULL;  
		temp->next = D[1];  
		temp->set = mypow(2,i-1);  
		D[1] = temp;
	}
	for(j=2;j<n;j++)
	{
		D[j] = NULL;
		for(i=1;i<n;i++)  
		{
			address = malloc(sizeof(struct path *)*mypow(2,n-1));  
			bzero(address,sizeof(&address));  
			p = D[j-1];  
			while(p!=NULL)
			{
				if(inSet(i,p->set)==0)  
				{
					set = insertSet(i,p->set);
					if(address[set]!=NULL&&address[set]->current==i)
					{
						cost = p->cost + a[p->current][i];  
						if(cost< address[set]->cost)  
						{
							address[set]->cost = cost;
							address[set]->lastNode = p;  
						}  
					}
					else  
					{
						temp = malloc(sizeof(struct path));  
						temp->current = i;  
						temp->cost = p->cost + a[p->current][i];  
						temp->set = set;  
						temp->lastNode = p;  
						temp->next = D[j];  
						D[j] = temp;  
						address[set]=temp;  
					}  
				}  
				p = p->next;  
			}  
			free(address);  
		}  
	}    
	mincost = -1;  
	p = D[n-1];  
	while(p)
	{
		if(mincost==-1 || mincost > p->cost + a[p->current][0])  
		{
			mincost = p->cost + a[p->current][0];  
			lastNode = p;  
		}  
		p = p->next;  
	}
	printf("mincost=%d\n",mincost);
	print_path(lastNode);
	draw[0] = 1;
	printf("->1\n");
	return mincost;  
}

void connectgroup()
{
	int i,j,k,m,n;
	int min;
	int source,target;
	m = num_vertices / 20;
	n = num_vertices % 20;
	for(i=0;i<num_vertices;i++)
		for(j=0;j<num_vertices;j++)
			connection[i][j] = 0;
	for(i = 0; i < m-1; i++)
	{
		min =9999;
		for(j = 0; j < 20; j++)
		{
			for(k = 0;k < 20; k++)
			{
				if(graph[i*20+j][(i+1)*20+k] < min)
				{
					min = graph[i*20+j][(i+1)*20+k];
					source = i*20+j;
					target = (i+1)*20+k;
				}
			}
		}
		printf("mincost between group%d and group%d = %d\n",i+1,i+2,min);
		totalcost = totalcost + min;
		connection[source][target] = 1;
	}
	min = 9999;
	for(j = 0;j<20;j++)
	{
		for(k=0;k<n;k++)
		{
			if(graph[(m-1)*20+j][m*20+k] < min)
			{
				min = graph[(m-1)*20+j][m*20+k];
				source = (m-1)*20+j;
				target = m*20+k;
			}
		}
	}
	printf("mincost between group%d and group%d = %d\n",m,m+1,min);
	totalcost = totalcost + min;
	connection[source][target] = 1;
}

int main(int argc, char **argv)
{
	if(argc < 2)
		printf("Please input a filename \n");
	else
	{
		InputVertices(argv[1]);
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
		XSetLineAttributes(display_ptr, gc_green, 2, LineSolid, CapRound, JoinRound); 
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
		
		int i,j,k,m,n;
		Group();
		if(num_vertices <= 20)
		{
			totalcost = tsp(num_vertices,g_group);
		}
		else
				{
					m = num_vertices / 20;
					n = num_vertices % 20;
					for(i = 0; i < m; i++)
					{
						for(j = 0; j < 20; j++)
						{
							draw[j] = 0;
						}
						for(j = 0; j < 20; j++)
						{
							for(k = 0; k < 20; k++)
							{
								g_group[j][k] = graph[i*20+j][i*20+k];
							}
						}
						totalcost = totalcost + tsp(20,g_group);
						for(j = 0; j < 21; j++)
						{
							draw_group[i][j] = draw[j];
						}
					}
					if(n != 0)
					{
						for(j = 0; j < 20; j++)
						{
							draw[j] = 0;
						}
							
						for(j = 0; j < 20; j++)
						{
							for(k = 0; k < 20; k++)
							{
								g_group[j][k] = 0;
							}
						}
						
						for(j = 0; j < n; j++)
						{
							for(k = 0; k < n;k++)
							{
								g_group[j][k] = graph[m*20+j][m*20+k];
							}
						}
						totalcost = totalcost + tsp(n,g_group);
						for(j = 0; j < 7; j++)
						{
							draw_group[m][j] = draw[j];
						}
					}
					connectgroup();
				}
		printf("totalcost=%d\n",totalcost);
		while(1)
		{
			XNextEvent( display_ptr, &report );
			switch( report.type )
			{
				case Expose:
				for(i = 0; i < num_vertices; i++)
				{
					
					XFillArc(display_ptr, win, gc, vertices[i].x - 3, vertices[i].y - 3, 6, 6, 0, 360*64);
				}
				
				if(num_vertices <= 20)
				{
					for(i = 0; i < num_vertices; i++)
					{
						XDrawLine(display_ptr, win, gc_red, vertices[draw[i]-1].x, vertices[draw[i]-1].y, vertices[draw[i+1]-1].x, vertices[draw[i+1]-1].y);
					}
				}				
				else
				{
					m = num_vertices / 20;
					n = num_vertices % 20;
					for(i = 0; i < m; i++)
					{
						for(j = 0; j < 20; j++)
						{
							XDrawLine(display_ptr, win, gc_red, vertices[i*20+draw_group[i][j]-1].x, vertices[i*20+draw_group[i][j]-1].y, vertices[i*20+draw_group[i][j+1]-1].x, vertices[i*20+draw_group[i][j+1]-1].y);
						}
					}
					if(n != 0)
					{
						for(j = 0; j < n; j++)
						{
							XDrawLine(display_ptr, win, gc_red, vertices[m*20+draw_group[m][j]-1].x, vertices[m*20+draw_group[m][j]-1].y, vertices[m*20+draw_group[m][j+1]-1].x, vertices[m*20+draw_group[m][j+1]-1].y);
						}
					}
					for(i = 0; i < num_vertices; i++)
					{
						for(j = 0; j < num_vertices; j++)
						{
							if(connection[i][j] == 1)
							{
								XDrawLine(display_ptr, win, gc_green, vertices[i].x, vertices[i].y, vertices[j].x, vertices[j].y);
							}
						}
					}
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
