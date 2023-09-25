#include <stdio.h>
#include <SDL2/SDL.h>
#include <time.h>

#define WIN_WIDTH 1280
#define WIN_HEIGHT 960

struct Snake{
    SDL_Rect current;
    SDL_Rect prev;
};

enum Direction{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

Uint32 move(Uint32 interval, void *p){
    enum Direction direction = (enum Direction)p;
    SDL_Event sdlevent = {};
    switch( direction ){
        case UP:
            sdlevent.type = SDL_KEYDOWN;
            sdlevent.key.keysym.sym = SDLK_UP;
            SDL_PushEvent(&sdlevent);
            break;

        case DOWN:
            sdlevent.type = SDL_KEYDOWN;
            sdlevent.key.keysym.sym = SDLK_DOWN;
            SDL_PushEvent(&sdlevent);
            break;

        case LEFT:
            sdlevent.type = SDL_KEYDOWN;
            sdlevent.key.keysym.sym = SDLK_LEFT;
            SDL_PushEvent(&sdlevent);
            break;

        case RIGHT:
            sdlevent.type = SDL_KEYDOWN;
            sdlevent.key.keysym.sym = SDLK_RIGHT;
            SDL_PushEvent(&sdlevent);
            break;

        default:
            break;
    }
                        
    return interval;
}

int main(int argc, char *argv[]){

    // initialize random time
    time_t t;
    srand((unsigned) time(&t));

    int quit = 0;
    int xcenter = WIN_WIDTH / 2;
    int ycenter = WIN_HEIGHT / 2;
    
    int grid_cell_size = 36;
    int grid_width = 29;
    int grid_height = 23;
    int window_width = (grid_width * grid_cell_size) + 1;
    int window_height = (grid_height * grid_cell_size) + 1;

    int snakeSize = 0;

    enum Direction direction = UP;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("couldn't initialize SDL");
        return -1;
    }

    // create game window
    SDL_Window* window = SDL_CreateWindow("snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
    if(!(window)){
        printf("window creation error: %s", SDL_GetError());
        return -1;
    }

    // initialize renderer to draw objects
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!(renderer)){
        printf("renderer error: %s", SDL_GetError());
        return -1;
    }

    // initialize movable main Head to center of window
    SDL_Rect snakeHead = {
        .w = grid_cell_size,
        .h = grid_cell_size,
        .x = (grid_width - 1) / 2 * grid_cell_size,
        .y = (grid_height - 1) / 2 * grid_cell_size,
    };

    struct Snake snake[100];
    snake[0].current = snakeHead;


    // initialize PowerUp to increase snake size
    SDL_Rect PowerUp = {
        .w = grid_cell_size,
        .h = grid_cell_size,
        .x = rand() % grid_width * grid_cell_size,
        .y = rand() % grid_height * grid_cell_size,
    };

    // Dark theme.  
    SDL_Color grid_background = {22, 22, 22, 255}; // Barely Black
    SDL_Color grid_line_color = {44, 44, 44, 255}; // Dark grey

    SDL_TimerID timerID;

    timerID = SDL_AddTimer(1000 / 5, &move, (enum Direction*)direction);
    // main game loop
    while(!(quit)){
        // handle window Events
        SDL_Event windowEvent;
        while( SDL_PollEvent(&windowEvent) ){
            
            switch( windowEvent.type ){
                // if close button pressed close window
                case SDL_QUIT:
                    quit = 1;
                    break;
                
                // check key pressed to move player
                case SDL_KEYDOWN:
                    SDL_RemoveTimer(timerID);
                    switch( windowEvent.key.keysym.sym){
                        case SDLK_UP:
                        case SDLK_w:
                            snake[0].prev = snake[0].current;
                            snake[0].current.y -= grid_cell_size;
                            for(int i = 1; i <= snakeSize; i++){
                                snake[i].prev = snake[i].current;
                                snake[i].current = snake[i-1].prev;
                            }
                            direction = UP;
                            break;

                        case SDLK_DOWN:
                        case SDLK_s:
                            snake[0].prev = snake[0].current;
                            snake[0].current.y += grid_cell_size;
                            for(int i = 1; i <= snakeSize; i++){
                                snake[i].prev = snake[i].current;
                                snake[i].current = snake[i-1].prev;
                            }
                            direction = DOWN;
                            break;

                        case SDLK_LEFT:
                        case SDLK_a:
                            snake[0].prev = snake[0].current;
                            snake[0].current.x -= grid_cell_size;
                            for(int i = 1; i <= snakeSize; i++){
                                snake[i].prev = snake[i].current;
                                snake[i].current = snake[i-1].prev;
                            }
                            direction = LEFT;
                            break;

                        case SDLK_RIGHT:
                        case SDLK_d:
                            snake[0].prev = snake[0].current;
                            snake[0].current.x += grid_cell_size;
                            for(int i = 1; i <= snakeSize; i++){
                                snake[i].prev = snake[i].current;
                                snake[i].current = snake[i-1].prev;
                            }
                            direction = RIGHT;
                            break;

                        default:
                            break;
                    }
                    timerID = SDL_AddTimer(1000 / 5, &move, (enum Direction*)direction);
                    break;

                default:
                    break;
            }
        }

        // handle PowerUp
        if( snake[0].current.x == PowerUp.x && snake[0].current.y == PowerUp.y ){
            // respawn PowerUp to random position
            PowerUp.x = rand() % grid_width * grid_cell_size;
            PowerUp.y = rand() % grid_height * grid_cell_size;

            // increase snake size
            snakeSize += 1;
            snake[snakeSize].current.w = grid_cell_size;
            snake[snakeSize].current.h = grid_cell_size;
            snake[snakeSize].current.x = (snake[snakeSize - 1].current.x);
            snake[snakeSize].current.y = (snake[snakeSize - 1].current.y);
            snake[snakeSize].prev.x = 0;
            snake[snakeSize].prev.y = 0;
            
        }  
        else{
            for(int i = 1; i <= snakeSize; i++){
                if((snake[0].current.x == snake[i].current.x && snake[0].current.y == snake[i].current.y) && snake[i].prev.x != 0){
                    quit = 1;
                }
            }
        }
        // draw window background colour
        SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g,
                               grid_background.b, grid_background.a);
        SDL_RenderClear(renderer);    

        // draw grid lines.
        SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                               grid_line_color.b, grid_line_color.a);

        for (int x = 0; x < 1 + grid_width * grid_cell_size;
             x += grid_cell_size) {
            SDL_RenderDrawLine(renderer, x, 0, x, window_height);
        }

        for (int y = 0; y < 1 + grid_height * grid_cell_size;
             y += grid_cell_size) {
            SDL_RenderDrawLine(renderer, 0, y, window_width, y);
        }

        //SDL_RenderFillRects(renderer, snake->current, snakeSize + 1);
        for(int j = 0; j <= snakeSize; j++){
            // set player rectangle colour and draw it
            SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
            SDL_RenderFillRect(renderer, &snake[j].current);
            // draw outline
            SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                               grid_line_color.b, grid_line_color.a);
            SDL_RenderDrawRect(renderer, &snake[j].current);
        }

        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
        SDL_RenderFillRect(renderer, &PowerUp);
        SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                               grid_line_color.b, grid_line_color.a);
        SDL_RenderDrawRect(renderer, &PowerUp);

        // send draw request
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);
    }

    // clean up
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

