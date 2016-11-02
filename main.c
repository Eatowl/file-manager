#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>

#define SIZE_INCREMENT 10 // stop

struct dirent *entry;

int row, col;
//char test_array[2555][25555];
char save_directory[25555] = "/",
     temporary_directory[25555] = "/";

void init_wins(WINDOW **wins, int n);

int main() {
    WINDOW *my_wins[3];
    PANEL  *my_panels[3];
    PANEL  *top;

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

    unsigned choice = 0;  // Выбор пользователя
    bool exit = false, display_wins_1 = true;
    char test[100];
    int size1;
    //Массив указателей на слова
    char **words;
    char *directory;
    //Длина введённого слова
    unsigned length;
    //Размер массива слов. Для уменьшения издержек на выделение памяти 
    //каждый раз будем увеличивать массив слов не на одно значение, а на
    //SIZE_INCREMENT слов
    unsigned size = SIZE_INCREMENT;

    unsigned length1 = 0;

    directory = (char*) malloc(size);
    directory = strcpy(directory, "/");

    while ( !exit ) {
        wclear(panel_window(top));
        //Счётчик слов
        unsigned wordCounter = 0;

        words = (char**) malloc(size*sizeof(char*));

        DIR *dir;
        unsigned i;
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
            mvwprintw(panel_window(top), i + 1, 2, "%s %d %d\n", entry->d_name, length);

            if (display_wins_1 == true) {  // вывод директории на второй
                mvwprintw(my_wins[1], i + 1, 2, "%s\n", entry->d_name);
            }

            wordCounter++;
        }

        display_wins_1 = false;
        box(my_wins[0], 0, 0);
        box(my_wins[1], 0, 0);
        //wprintw(my_wins[2], "%s %d-%d\n", directory, strlen(directory), strlen(director));

        update_panels();
        doupdate();

        int ch = getch();

        switch ( ch ) {
            case KEY_F(2):
                exit = true;
                for (int ix = 0; ix < wordCounter; ++ix) {
                    free(words[ix]);
                }
                free(words);
                free(directory);
                break;

            case '\t':  // переход на следующую панель
                top = (PANEL *)panel_userptr(top);
                top_panel(top);
                strcpy(temporary_directory, save_directory);
                strcpy(save_directory, directory);
                strcpy(directory, temporary_directory);
                break;

            case '\n':
                length = strlen(words[choice]);
                //wprintw(my_wins[2], "%s %d-%d\n", pdirector, strlen(pdirector), strlen(pdirector)+length+1);
                directory = (char*) realloc(directory, strlen(directory) + length + 1);
                if(directory != NULL) {
                    strcat(directory, "/");
                    strcat(directory, words[choice]);
                } else {
                    free(directory);
                    exit = true;
                    break;
                }
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

    // уничтожение созданных панелей и окон
    for (int i = 0; i < 3; ++i) {
        del_panel(my_panels[i]);
        delwin(my_wins[i]);
    }

    endwin();
    return 0;
}

void init_wins(WINDOW **wins, int n) {
    int x, i;
    x = 0;

    for (i = 0; i < n; ++i) {
        wins[i] = newwin(row - 3, col / 2, 3, x);
        wbkgdset(wins[i], COLOR_PAIR(1));
        wclear(wins[i]);
        wrefresh(wins[i]);

        x += col / 2;
    }
}
