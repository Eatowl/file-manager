#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include "fm_func.h"

char **words;
char *directory,
     *save_directory,
     *temporary_directory;

int main() {   
    WINDOW *my_wins[3];
    PANEL  *my_panels[3];
    PANEL  *top;

    create_wins_and_panel(my_wins, my_panels);
    top = my_panels[0];

    directory = malloc_array(directory);
    save_directory = malloc_array(save_directory);
    temporary_directory = malloc_array(temporary_directory);

    unsigned choice = 0;
    bool exit = false, display_wins_1 = true, update_dir = true;
    int *ptr, wordCounter;
    ptr = &wordCounter;

    while ( !exit ) {
        wclear(panel_window(top));
        if (update_dir != false) {
            *ptr = 0;
            words = update_directory(words, directory, ptr);
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
                break;
            case '\t': // переход на следующую панель
                top = (PANEL *)panel_userptr(top);
                top_panel(top);
                strcpy(temporary_directory, save_directory);
                strcpy(save_directory, directory);
                strcpy(directory, temporary_directory);
                update_dir = true;
                break;
            case '\n':
                exit = move_between_directory(choice, directory, save_directory, temporary_directory, words);
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
    // уничтожение созданных панелей и окон
    for(int i = 0; i < 3; ++i) {
        del_panel(my_panels[i]);
        delwin(my_wins[i]);
    }
    endwin();
    return 0;
}
