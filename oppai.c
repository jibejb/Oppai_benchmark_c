#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include <stdlib.h>

#define NPOINTS 10000
#define PI acos(-1)


struct Frame
{
    SDL_Renderer* renderer;
    SDL_Point* points;
    size_t points_len;
    int windowWidth;
    int windowHeight;
    char * txt;
    TTF_Font * font;
    SDL_Event event;
    int state;
};


double Oppai_func(double y, double t) {
	y = 0.02 * (y - 100);

	double a1 = (1.5 * exp((0.12*sin(t)-0.5)*pow((y+0.16*sin(t)), 2))) / (1 + exp(-20*(5*y+sin(t))));
	double a2 = ((1.5 + 0.8*pow((y+0.2*sin(t)), 3)) * pow(1+exp(20*(5*y+sin(t))), -1)) / (1 + exp(-(100*(y+1) + 16*sin(t))));
	double a3 = (0.2 * (exp(-pow(y+1, 2)) + 1)) / (1 + exp(100*(y+1)+16*sin(t)));
	double a4 = 0.1 / exp(2*pow((10*y+1.2*(2+sin(t))*sin(t)), 4));

	return 65 * (a1 + a2 + a3 + a4);

}

void draw_aixes(SDL_Renderer* renderer, int windowWidth, int windowHeight)
{


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // 绘制X轴
    SDL_RenderDrawLine(renderer, 0, 300, windowWidth, 300);

    // 绘制Y轴
    SDL_RenderDrawLine(renderer, 10, 0, 10, windowHeight);
}


void draw_txt(SDL_Renderer* renderer, char* txt, TTF_Font* font, int windowWidth, int windowHeight){

    SDL_Color sc = {255, 255, 255, 255};
    SDL_Surface *sur = TTF_RenderUTF8_Blended_Wrapped(font, txt, sc, 350);
    SDL_Rect rect = { 0, 0, sur->w, sur->h };
    rect.x = (windowWidth - rect.w) * 3 / 4;
    rect.y = (windowHeight - rect.h) / 4;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_FreeSurface(sur);
    SDL_RenderPresent(renderer);

    SDL_RenderCopy(renderer, tex, NULL, &rect);
	SDL_RenderPresent(renderer);
    SDL_DestroyTexture(tex);

}


int update_screen(void* data) {
    struct Frame * frame = (struct Frame*) data;
    double dx = 10.0*PI / 64.0;
    for (int t = 0; t < 64; ++t)
    {

        SDL_PollEvent(&(frame->event)); 
        if (frame->event.type == SDL_QUIT) {
            frame->state = 1;
            return 0;
        }
            
        for (int i =0 ; i < NPOINTS; ++i)
        {
           ((struct SDL_Point*)(frame->points))[i].y = 480-(i);
           ((struct SDL_Point*)(frame->points))[i].x = 10.0 + Oppai_func(i-180, (double)t * dx);
        }


        SDL_SetRenderDrawColor(frame->renderer, 0, 0, 0, 0); // 设置背景颜色为黑色
        SDL_RenderClear(frame->renderer);
        draw_aixes(frame->renderer, frame->windowWidth, frame->windowHeight);
        char *txt = malloc(1000*(sizeof(char)));
        sprintf(txt, "%s: %d scores\n", frame->txt, t);

        draw_txt(frame->renderer, txt, frame->font, frame->windowWidth, frame->windowHeight);

	    SDL_SetRenderDrawColor(frame->renderer, 0, 255, 0, 255);
	    SDL_RenderDrawLines(frame->renderer, frame->points, NPOINTS);
        SDL_RenderPresent(frame->renderer);

        SDL_Delay(25);
        free(txt);
    }
    return 0;
}

int main(void)
{
    SDL_Point points[NPOINTS];


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", SDL_GetError());
        return 1;
    }  

    SDL_Window* window = SDL_CreateWindow("你好 Oppai", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }
        
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    char * txt = "申し訳ございませんがたくさんあります。\n忘れている世界によって忘れている世界。\n汚れのない心の永遠の日差し！\nそれぞれの祈りが受け入れられ、それぞれが辞任を望む\n上面的日文到底什么意思？我不知道\n不知道这个字体能不能显示中文\n挺好奇的。";
    TTF_Font *kosugi = TTF_OpenFont("../fonts/KosugiMaru-Regular.ttf", 16);




    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    struct Frame frame;

    SDL_Event event;

    frame.points_len = NPOINTS;
    frame.renderer = renderer;
    frame.points = points;
    frame.windowWidth = windowWidth;
    frame.windowHeight = windowHeight;
    frame.txt = txt;
    frame.font = kosugi;
    frame.event = event;
    frame.state = 0;

    update_screen(&frame);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(kosugi);
    SDL_Quit();

    return 0;

}