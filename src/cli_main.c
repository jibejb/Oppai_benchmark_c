#include "benchmark.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <cli_main.h>

void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
}

int map_x(double x, GraphRange graph, ScreenSize screen) {
    return (int)((x - graph.xmin) / (graph.xmax - graph.xmin) * (screen.width - 1));
}

int map_y(double y, GraphRange graph, ScreenSize screen) {
    return (int)((graph.ymax - y) / (graph.ymax - graph.ymin) * (screen.height - 1));
}

void draw_function(GraphRange graph, ScreenSize screen, double t) {
    for (double x = graph.xmin; x <= graph.xmax; x += (graph.xmax - graph.xmin) / screen.width) {
        double y = Oppai_func(x, t); // 示例函数
        if (y >= graph.ymin && y <= graph.ymax) {
            int screen_x = map_x(x, graph, screen);
            int screen_y = map_y(y, graph, screen);
            mvaddch(screen_y, screen_x, '.'); // 绘制点
        }
    }
}

void zoom(GraphRange *graph, double factor) {
    double x_center = (graph->xmin + graph->xmax) / 2;
    double y_center = (graph->ymin + graph->ymax) / 2;
    double x_range = (graph->xmax - graph->xmin) / 2;
    double y_range = (graph->ymax - graph->ymin) / 2;

    x_range *= factor;
    y_range *= factor;

    graph->xmin = x_center - x_range;
    graph->xmax = x_center + x_range;
    graph->ymin = y_center - y_range;
    graph->ymax = y_center + y_range;
}

void cli_main() {



    Data *sharedData = malloc(sizeof(*sharedData));
    memset(sharedData, 0, sizeof(*sharedData));
    sharedData->t = 0.0;
    sharedData->S = 0.0;
    sharedData->Score = 0.0;
    sharedData->benchmark_running = 1;
    pthread_mutex_init(&sharedData->mutex, NULL);
    pthread_cond_init(&sharedData->cond, NULL);
    pthread_t benchmark_thread;
    if (pthread_create(&benchmark_thread, NULL, benchmark, (void*)sharedData) != 0) {
        perror("Failed to create main thread");
        exit(EXIT_FAILURE);
    }

    // 初始化屏幕
    init_ncurses();

    while (sharedData->benchmark_running) {
        pthread_mutex_lock(&sharedData->mutex);
        pthread_cond_wait(&sharedData->cond, &sharedData->mutex);

        clear();
        double t = sharedData->t;
        ScreenSize screen;
        getmaxyx(stdscr, screen.height, screen.width);

        GraphRange graph = {0, 300, 0, 300};
        move(0,0);
        printw("S: %lf, Score: %lf\n", sharedData->S, sharedData->Score);
//        zoom(&graph, 1.2);
        draw_function(graph, screen, t);
        pthread_mutex_unlock(&sharedData->mutex);
        refresh();

    }
    getch();
    endwin();

    if (pthread_join(benchmark_thread, NULL) != 0) {
        perror("Failed to join main thread");
        exit(EXIT_FAILURE);
    }

}


