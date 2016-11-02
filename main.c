#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <dirent.h>
#include <panel.h>

#define SIZE_INCREMENT 10

struct dirent *entry;
int row, col;
char **words;
char *directory,
     *save_directory,
     *temporary_directory;

WINDOW *my_wins[3];
PANEL  *my_panels[3];
PANEL  *top;

void init_wins(WINDOW **wins, int n);
void free_all();
void malloc_all_array();
void realloc_all_array(unsigned length);
bool move_between_directory(unsigned length, unsigned choice);

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (!has_colors()) {
        endwin();
        printf("\nОшибка! Не поддерживаются цвета\n");
        return 1;
    }
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_GREEN, COLOR_RED);

    // инициализируются окна
    getmaxyx(stdscr, row, col);
    if (col % 2 != 0) {
        col -= 1;
    }
    init_wins(my_wins, 2);
    my_wins[2] = newwin(3, col, 0, 0);
    wbkgdset(my_wins[2], COLOR_PAIR(1));
    wclear(my_wins[2]);

    // отрисовка рамки в окнах
    for (int i = 0; i < 3; ++i)
        box(my_wins[i], 0, 0);

    // создание панелей, на основе окон
    for (int i = 0; i < 3; ++i)
        my_panels[i] = new_panel(my_wins[i]);

    // ссылки между панелями
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);

    mvwprintw(my_wins[2], 1, 1,
        "Tab - next panel | F2 - exit | Enter - choise |");

    top = my_panels[0];
    bool exit = false, display_wins_1 = true;
    unsigned length, choice = 0;
    unsigned size = SIZE_INCREMENT;

    malloc_all_array();

    while ( !exit ) {
        DIR *dir;
        unsigned i, wordCounter = 0;
        wclear(panel_window(top));
        words = (char**) malloc(size*sizeof(char*));
        dir = opendir(directory);

        for (i = 0; (entry = readdir(dir)) != NULL; ++i) {
            if ( i == choice ) {
                mvwaddch(panel_window(top), i + 1, 1, '>' | A_STANDOUT);
            } else {
                mvwaddch(panel_window(top), i + 1, 1, ' ');
            }

            length = strlen(entry->d_name);
            if (i >= size) {
                size += SIZE_INCREMENT;
                words = (char**) realloc(words, size*sizeof(char*));
            }

            words[i] = (char*) malloc(length + 1);
            strcpy(words[i], entry->d_name);
            mvwprintw(panel_window(top), i + 1, 2, "%s\n", entry->d_name);

            if (display_wins_1 == true) {  // вывод директории на второй
                mvwprintw(my_wins[1], i + 1, 2, "%s\n", entry->d_name);
            }
            wordCounter++;
        }

        display_wins_1 = false;
        box(my_wins[0], 0, 0);
        box(my_wins[1], 0, 0);
        update_panels();
        doupdate();
        int ch = getch();

        switch ( ch ) {
            case KEY_F(2):
                exit = true;
                for (int i = 0; i < wordCounter; ++i) {
                    free(words[i]);
                }
                free_all();
                break;

            case '\t':  // переход на следующую панель
                top = (PANEL *)panel_userptr(top);
                top_panel(top);
                strcpy(temporary_directory, save_directory);
                strcpy(save_directory, directory);
                strcpy(directory, temporary_directory);
                break;

            case '\n':
                exit = move_between_directory(length, choice);
                choice = 0;
                break;

            case KEY_UP:
                if (choice != 0)
                    choice--;
                break;

            case KEY_DOWN:
                if (choice != i - 1)
                    choice++;
                break;
        }
        closedir(dir);
    }
    return 0;
}

void init_wins(WINDOW **wins, int n) {
    int x = 0, i;
    for (i = 0; i < n; ++i) {
        wins[i] = newwin(row - 3, col / 2, 3, x);
        wbkgdset(wins[i], COLOR_PAIR(1));
        wclear(wins[i]);
        wrefresh(wins[i]);
        x += col / 2;
    }
}

void malloc_all_array() {
    directory = (char*) malloc(SIZE_INCREMENT);
    save_directory = (char*) malloc(SIZE_INCREMENT);
    temporary_directory = (char*) malloc(SIZE_INCREMENT);
    directory = strcpy(directory, "/");
    save_directory = strcpy(save_directory, "/");
    temporary_directory = strcpy(temporary_directory, "/");
}

void realloc_all_array(unsigned length) {
    directory = (char*) realloc(directory, strlen(directory) + length);
    save_directory = (char*) realloc(save_directory, strlen(directory) + length);
    temporary_directory = (char*) realloc(temporary_directory, strlen(directory) + length);
}

bool move_between_directory(unsigned length, unsigned choice) {
    if (strlen(directory) != 2) {
        length = strlen(words[choice]) + 1;
    } else {
        length = strlen(words[choice]);
    }
    realloc_all_array(length);
    if (directory != NULL) {
        strcat(directory, "/");
        strcat(directory, words[choice]);
    } else {
        free(directory);
        return true;
    }
    return false;
}

void free_all() {
    free(words);
    free(directory);
    free(save_directory);
    free(temporary_directory);
    for (int i = 0; i < 3; ++i) {
        del_panel(my_panels[i]);
        delwin(my_wins[i]);
    }
    endwin();
}
