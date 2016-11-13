#include <sys/types.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include "fm_func.h"

char **words;
char *directory,
     *save_directory,
     *temporary_directory,
     *test;

bool type_file(char *directory, char **words, unsigned choice, WINDOW **my_wins, char *test);

int main() {
    WINDOW *my_wins[3];
    PANEL  *my_panels[3];
    PANEL  *top;

    create_wins_and_panel(my_wins, my_panels);
    top = my_panels[0];

    directory = malloc_array(directory);
    save_directory = malloc_array(save_directory);
    temporary_directory = malloc_array(temporary_directory);
    test = malloc_array(test);

    unsigned choice = 0;
    bool exit = false, display_wins_1 = true, update_dir = true, param = true;
    int *ptr, wordCounter;
    ptr = &wordCounter;

    while ( !exit ) {
        wclear(panel_window(top));
        if (update_dir != false) {
            *ptr = 0;
            words = update_directory(words, directory, ptr, my_wins);
            update_dir = false;
        }
        for (int i = 0; i < wordCounter; ++i) {
            if ( i == choice ) {
                mvwaddch(panel_window(top), i + 1, 1, '>' | A_STANDOUT);
            } else {
                mvwaddch(panel_window(top), i + 1, 1, ' ');
            }
            mvwprintw(panel_window(top), i + 1, 2, "%s\n", words[i]);
            if (display_wins_1 == true) {  // вывод директории на второй
                mvwprintw(my_wins[1], i + 1, 2, "%s\n", words[i]);
            }
        }

        display_wins_1 = false;
        print_box_and_update(my_wins);
        int ch = getch();

        switch ( ch ) {
            case KEY_F(2):
                exit = true;
                wclear(panel_window(top));
                free_all(words, directory, save_directory, temporary_directory, wordCounter);
                for (int i = 0; i < 3; ++i) {
                    del_panel(my_panels[i]);
                    delwin(my_wins[i]);
                }
                endwin();
                break;
            case '\t':  // переход на следующую панель
                top = (PANEL *)panel_userptr(top);
                top_panel(top);
                strcpy(temporary_directory, save_directory);
                strcpy(save_directory, directory);
                strcpy(directory, temporary_directory);
                update_dir = true;
                break;
            case '\n':
                //exit = move_between_directory(choice, directory, save_directory, temporary_directory, words);
                exit = type_file(directory, words, choice, my_wins, test);
                //wprintw(my_wins[2], "%d\n", param);
                update_dir = true;
                choice = 0;
                break;
            case KEY_UP:
                if ( choice != 0 )
                    choice--;
                break;
            case KEY_DOWN:
                if ( choice != wordCounter - 1 )
                    choice++;
                break;
        }
    }
    return 0;
}

struct stat buf;

bool type_file(char *directory, char **words, unsigned choice, WINDOW **my_wins, char *test) {
    int st;
    bool ret = true;
    unsigned length;
    length = strlen(words[choice]) + 1;
    test = (char*) realloc(test, strlen(test) + length);
    strcat(test, directory);
    //strcat(test, "/");
    strcat(test, words[choice]);
    lstat(test, &buf);
    //wprintw(my_wins[2], "====%d\n", ret);
    if (S_ISREG(buf.st_mode)) st = 1;
    else if (S_ISDIR(buf.st_mode)) st = 0;
    //wprintw(my_wins[2], "=%s\n", test);

    switch( st ) {
        case 0:
            ret = move_between_directory(choice, directory, save_directory, temporary_directory, words);
            //wprintw(my_wins[2], "%d\n", ret);
            wprintw(my_wins[2], "=%s\n", test);
            return ret;
            break;
        case 1:
            if (buf.st_mode & S_IXUSR == 64) {
                wprintw(my_wins[2], "++++");
                return true;
            } else {
                return false;
            }
            break;
    }
    return false;
}