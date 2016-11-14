#include <sys/types.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include "fm_func.h"

//char words[500][1024];
char **words;
char *directory,
     *save_directory,
     *temporary_directory,
     *test;

struct dirent *entry;

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
    words = (char**) malloc(100*sizeof(char*));

    unsigned choice = 0;
    bool exit = false, display_wins_1 = true, update_dir = true, param = true;
    int wordCounter;

    while ( !exit ) {
        wclear(panel_window(top));
        if (update_dir != false) {
            DIR *dir;
            unsigned length, max_length = 0;
            dir = opendir(directory);
            wordCounter = 0;
            for (unsigned i = 0; (entry = readdir(dir)) != NULL; ++i) {
                length = strlen(entry->d_name);
                if (length > max_length) {
                    max_length = length;
                }
                words[i] = (char*) malloc(max_length + 1);
                strcpy(words[i], entry->d_name);
                wordCounter++;
            }
            words = sort_array(words, wordCounter);
            closedir(dir);
            update_dir = false;
        }
        for (int i = 0; i < wordCounter; ++i) {
            if ( i == choice ) {
                mvwaddch(panel_window(top), i + 1, 1, '>' | A_STANDOUT);
            } else {
                mvwaddch(panel_window(top), i + 1, 1, ' ');
            }
            mvwprintw(panel_window(top), i + 1, 2, "%s\n", words[i]);
            //mvwprintw(panel_window(top), i + 1, 2, "%s %s %d\n", words[i], test, wordCounter);
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
                //free_all(words, directory, save_directory, temporary_directory, wordCounter);
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
                //exit = move_between_directory(choice, directory, save_directory, temporary_directory, words, test);
                //exit = type_file(directory, words, choice, my_wins, test);
                //wprintw(my_wins[2], "%d\n", param);
                if (strlen(directory) != 1) {
                    strcat(directory, "/");
                    strcat(directory, words[choice]);
                } else {
                    strcat(directory, words[choice]);
                    //return true;
                }
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
    int st = 0;
    bool ret = true;
    unsigned length;
    //wprintw(my_wins[2], "=%d\n", strlen(directory));
    if (strlen(directory) != 1) {
        length = strlen(words[choice]) + 1;
    } else {
        length = strlen(words[choice]);
    }
    realloc_all_array(length, directory, save_directory, temporary_directory, test);
    //wprintw(my_wins[2], "=%d\n", strlen(directory));
    if (strlen(directory) != 1) {
        strcat(test, "/");
        strcat(test, words[choice]);
    } else {
        strcat(test, words[choice]);
        //return true;
    }
    //wprintw(my_wins[2], "=%s\n", test);
    
    //if (words[choice] == ".." || words[choice] == ".") {
    //    st = 0;
    //} else {
    //    lstat(test, &buf);
        //test = (char*) realloc(test, strlen(test) + length);
    //}
    //lstat(test, &buf);
    //wprintw(my_wins[2], "====%d\n", ret);
    //if (S_ISREG(buf.st_mode)) st = 1;
    //else if (S_ISDIR(buf.st_mode)) st = 0;
    //wprintw(my_wins[2], "=%s\n", st);

    switch( st ) {
        case 0:
            ret = move_between_directory(choice, directory, save_directory, temporary_directory, words, test);
            //wprintw(my_wins[2], "=%s\n", test);
            //wprintw(my_wins[2], "=%s\n", directory);
            //wprintw(my_wins[2], "%d\n", ret);
            //wprintw(my_wins[2], "=%s\n", test);
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