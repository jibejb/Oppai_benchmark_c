#ifndef CLI_MAIN_H_INCLUDED
#define CLI_MAIN_H_INCLUDED

typedef struct {
    double xmin, xmax;
    double ymin, ymax;
} GraphRange;

typedef struct {
    int width, height;
} ScreenSize;

void init_ncurses();
int map_x(double x, GraphRange graph, ScreenSize screen);
int map_y(double y, GraphRange graph, ScreenSize screen);
void draw_function(GraphRange graph, ScreenSize screen, double t);
void zoom(GraphRange *graph, double factor);
void cli_main();

#endif //CLI_MAIN_H_INCLUDED
