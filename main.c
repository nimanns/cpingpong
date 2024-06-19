#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 30
#define PADDLE_HEIGHT 3

typedef struct {
    int x, y;
} Ball;

typedef struct {
    int y;
} Paddle;

void clear_screen() {
    printf("\033[H\033[J");
}

void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void draw_border() {
    for (int i = 0; i <= WIDTH; i++) {
        move_cursor(i, 0);
        printf("🧱");
        move_cursor(i, HEIGHT);
        printf("🧱");
    }
    for (int i = 0; i <= HEIGHT; i++) {
        move_cursor(0, i);
        printf("🧱");
        move_cursor(WIDTH, i);
        printf("🧱");
    }
}

void draw_paddle(Paddle paddle, int x) {
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        move_cursor(x, paddle.y + i);
        printf("");
    }
}

void draw_ball(Ball ball) {
    move_cursor(ball.x, ball.y);
    printf("⚽");
}

void setup_terminal() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void reset_terminal() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int main() {
    Ball ball = {WIDTH / 2, HEIGHT / 2};
    Paddle paddle1 = {HEIGHT / 2 - PADDLE_HEIGHT / 2};
    Paddle paddle2 = {HEIGHT / 2 - PADDLE_HEIGHT / 2};
    int ball_dir_x = 1, ball_dir_y = 1;
    int score1 = 0, score2 = 0;

    setup_terminal();
    srand(time(NULL));

    while (1) {
        clear_screen();
        draw_border();
        draw_paddle(paddle1, 1);
        draw_paddle(paddle2, WIDTH - 2);
        draw_ball(ball);

        move_cursor(WIDTH / 2 - 4, HEIGHT + 2);
        printf("Score: %d - %d", score1, score2);

        if (kbhit()) {
            char c = getchar();
            if (c == 'w' && paddle1.y > 1) paddle1.y--;
            if (c == 's' && paddle1.y < HEIGHT - PADDLE_HEIGHT - 1) paddle1.y++;
            if (c == 'o' && paddle2.y > 1) paddle2.y--;
            if (c == 'l' && paddle2.y < HEIGHT - PADDLE_HEIGHT - 1) paddle2.y++;
            if (c == 'q') break;  // Quit the game
        }

        ball.x += ball_dir_x;
        ball.y += ball_dir_y;

        if (ball.y == 1 || ball.y == HEIGHT - 1) ball_dir_y = -ball_dir_y;
        if (ball.x == 2 && ball.y >= paddle1.y && ball.y <= paddle1.y + PADDLE_HEIGHT - 1) ball_dir_x = -ball_dir_x;
        if (ball.x == WIDTH - 3 && ball.y >= paddle2.y && ball.y <= paddle2.y + PADDLE_HEIGHT - 1) ball_dir_x = -ball_dir_x;

        if (ball.x == 1) {
            score2++;
            ball.x = WIDTH / 2;
            ball.y = HEIGHT / 2;
            ball_dir_x = 1;
        }
        if (ball.x == WIDTH - 1) {
            score1++;
            ball.x = WIDTH / 2;
            ball.y = HEIGHT / 2;
            ball_dir_x = -1;
        }

        usleep(50000); 
    }

    reset_terminal();
    return 0;
}

