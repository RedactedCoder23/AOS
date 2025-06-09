#include "ui_graph.h"
#include "branch.h"
#include <ncurses.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int ui_graph_run(void) {
    bm_init();
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    Branch branches[MAX_BRANCHES];
    int count = bm_list(branches);
    int selected = 0;
    char status[80] = "";

    int running = 1;
    while (running) {
        clear();
        int center_x = COLS / 2;
        int center_y = LINES / 2;
        int radius = center_x < center_y ? center_x : center_y;
        radius -= 4;
        if (radius <= 1) {
            mvprintw(center_y, center_x-10, "Terminal too small");
            refresh();
            getch();
            break;
        }
        for (int i = 0; i < count; i++) {
            double angle = 2 * M_PI * i / (count ? count : 1);
            int x = center_x + (int)(cos(angle) * radius);
            int y = center_y + (int)(sin(angle) * radius);
            if (i == selected) {
                attron(A_REVERSE | A_BOLD);
                mvprintw(y, x, "[%s]", branches[i].name);
                attroff(A_REVERSE | A_BOLD);
            } else {
                mvprintw(y, x, "[%s]", branches[i].name);
            }
            // Draw edges
            for (int j = 0; j < branches[i].conn_count; j++) {
                int to_id = branches[i].connections[j];
                if (to_id <= i || to_id >= count) continue;
                double angle2 = 2 * M_PI * to_id / (count ? count : 1);
                int x2 = center_x + (int)(cos(angle2) * radius);
                int y2 = center_y + (int)(sin(angle2) * radius);
                int dx = (x2 - x);
                int dy = (y2 - y);
                int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
                for (int s = 0; s < steps; s++) {
                    int px = x + dx * s / steps;
                    int py = y + dy * s / steps;
                    mvaddch(py, px, '.');
                }
            }
        }
        mvprintw(LINES-2, 0, "[Arrows/hjkl]: move  [n]: new  [c]: connect  [Enter]: switch  [q]: quit");
        mvprintw(LINES-1, 0, "%s", status);
        refresh();
        
        int ch = getch();
        switch (ch) {
            case KEY_LEFT: case 'h':
                if (selected > 0) selected--; break;
            case KEY_RIGHT: case 'l':
                if (selected < count-1) selected++; break;
            case KEY_UP: case 'k':
                if (selected > 0) selected--; break;
            case KEY_DOWN: case 'j':
                if (selected < count-1) selected++; break;
            case '\n':
                if (bm_switch(branches[selected].id) == BM_SUCCESS)
                    snprintf(status, sizeof(status), "Switched to %s", branches[selected].name);
                else
                    snprintf(status, sizeof(status), "Invalid branch");
                break;
            case 'n': {
                echo();
                mvprintw(LINES-2, 0, "Name: ");
                char name[32];
                getnstr(name, sizeof(name)-1);
                noecho();
                int r = bm_create(name);
                if (r >= 0) {
                    count = bm_list(branches);
                    selected = r;
                    snprintf(status, sizeof(status), "Created %s", name);
                } else if (r == BM_ERR_DUPLICATE) {
                    snprintf(status, sizeof(status), "Duplicate name");
                } else {
                    snprintf(status, sizeof(status), "Create failed");
                }
                break;
            }
            case 'c': {
                echo();
                mvprintw(LINES-3, 0, "From ID: ");
                char buf[8];
                getnstr(buf, sizeof(buf)-1);
                int from = atoi(buf);
                mvprintw(LINES-2, 0, "To ID:   ");
                getnstr(buf, sizeof(buf)-1);
                int to = atoi(buf);
                noecho();
                int r = bm_connect(from, to);
                if (r == BM_SUCCESS)
                    snprintf(status, sizeof(status), "Connected %d-%d", from, to);
                else
                    snprintf(status, sizeof(status), "Connect failed");
                break;
            }
            case 'q':
                running = 0; break;
        }
    }

    endwin();
    return 0;
}

