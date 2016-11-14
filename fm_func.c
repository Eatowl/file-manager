#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include "fm_func.h"

#define SIZE_INCREMENT 50

struct dirent *entry;

char *malloc_array(char *array) {
    array = (char*) malloc(2);
    array = strcpy(array, "/");
    return array;
}
 
void realloc_all_array(unsigned length, char *directory, char *save_directory, char *temporary_directory, char *test) {
    directory = (char*) realloc(directory, strlen(directory) + length + 1);
    save_directory = (char*) realloc(save_directory, strlen(directory) + length + 1);
    temporary_directory = (char*) realloc(temporary_directory, strlen(directory) + length + 1);
    test = (char*) realloc(test, strlen(test) + length + 1);
}

bool move_between_directory(unsigned choice, char *directory, char *save_directory, char *temporary_directory, char **words, char *test) {
    //unsigned length;
    //if (strlen(directory) != 1) {
    //    length = strlen(words[choice]) + 1;
    //} else {
    //    length = strlen(words[choice]);
    //}
    //realloc_all_array(length, directory, save_directory, temporary_directory, test);
    if (strlen(directory) != 1) {
        strcat(directory, "/");
        strcat(directory, words[choice]);
    } else {
        strcat(directory, words[choice]);
        //return true;
    }
    return false;
}

void print_box_and_update(WINDOW **wins) {
    for (int i = 0; i < 3; ++i)
        box(wins[i], 0, 0);
    update_panels();
    doupdate();
}

void init_wins(WINDOW **wins, int n, int row, int col) {
    int x = 0;
    for (int i = 0; i < n; ++i) {
        wins[i] = newwin(row - 3, col / 2, 3, x);
        wbkgdset(wins[i], COLOR_PAIR(1));
        wclear(wins[i]);
        wrefresh(wins[i]);
        x += col / 2;
    }
}

void create_wins_and_panel(WINDOW **my_wins, PANEL **my_panels) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (!has_colors()) {
        endwin();
        printf("\nОшибка! Не поддерживаются цвета\n");
    }
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_GREEN, COLOR_RED);
    // инициализируются окна
    int row, col;
    getmaxyx(stdscr, row, col);
    if (col % 2 != 0) {
        col -= 1;
    }
    init_wins(my_wins, 2, row, col);
    my_wins[2] = newwin(3, col, 0, 0);
    wbkgdset(my_wins[2], COLOR_PAIR(1));
    wclear(my_wins[2]);

    print_box_and_update(my_wins);
    // создание панелей, на основе окон
    for (int i = 0; i < 3; ++i)
        my_panels[i] = new_panel(my_wins[i]);
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);

    mvwprintw(my_wins[2], 1, 1,
        "Tab - next panel | F2 - exit | Enter - choise |");
}

char **sort_array(char **words, unsigned counter_sort) {
    unsigned size_sort = SIZE_INCREMENT;
    char *str;
    str = (char*) malloc(SIZE_INCREMENT);
    for (int i = 1; i < counter_sort; ++i) {
        for (int j = 0; j < counter_sort - i; ++j) {
            if (strlen(words[j]) > size_sort
                || strlen(words[j + 1]) > size_sort) {
                size_sort = strlen(words[j]) >= strlen(words[j + 1]) ?
                                size_sort + strlen(words[j]) : size_sort + strlen(words[j + 1]);
                str = (char*) realloc(str, size_sort);
            }
            if (strcmp(words[j], words[j+1]) > 0) {
                strcpy(str, words[j]);
                strcpy(words[j], words[j+1]);
                strcpy(words[j+1], str);
            }
        }
    }
    free(str);
    return words;
}

void free_all(char **words, char *directory, char *save_directory, char *temporary_directory, int wordCounter) {
    for (int i = 0; i < wordCounter; ++i) {
        free(words[i]);
    }
    free(words);
    free(directory);
    free(save_directory);
    free(temporary_directory);
    endwin();
}
