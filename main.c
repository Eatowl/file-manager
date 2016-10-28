#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>

struct dirent *entry;

int row, col;
char test_array[2555][25555];
char directory[25555] = "/", 
     save_directory[25555] = "/", 
     temporary_directory[25555] = "/";  

void init_wins(WINDOW **wins, int n);

int main()
{   
    WINDOW *my_wins[3];
    PANEL  *my_panels[3];
    PANEL  *top;
    int ch, i;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, row, col);

    if (col % 2 != 0)

        col -= 1;

    curs_set(0);

    if (!has_colors())
    {
        endwin();
        printf("\nОшибка! Не поддерживаются цвета\n");
        return 1;
    }

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_GREEN, COLOR_RED);

    // инициализируются окна
    init_wins(my_wins, 2);

    my_wins[2] = newwin(3, col, 0, 0);
    wbkgdset(my_wins[2], COLOR_PAIR(1));
    wclear(my_wins[2]);


    for(i = 0; i < 3; ++i)

        box(my_wins[i], 0, 0);

    for (i = 0; i < 3; ++i) 

        my_panels[i] = new_panel(my_wins[i]);

    // устанавливаем указатели на следующее окно
    // для перехода при нажатии Tab на следующее окно
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);

    mvwprintw(my_wins[2], 1, 1, "Tab - next panel | F2 - exit | Enter - choise |");

    update_panels();
    doupdate();

    top = my_panels[0];

    keypad(my_wins[0], TRUE);

    unsigned choice = 0; //Выбор пользователя
    bool exit = false, display_wins_1 = true;

    while ( !exit )
    { 

        wclear(panel_window(top));

        DIR *dir;
        int count = 0;
        dir = opendir(directory);

        for (unsigned i = 0; (entry = readdir(dir)) != NULL; ++i)
        {

            if ( i == choice ) { 

                mvwaddch(panel_window(top), i + 1, 1, '>' | A_STANDOUT); 

            } else {         

                mvwaddch(panel_window(top), i + 1, 1, ' ');

            }

            strcpy(test_array[count], entry->d_name);
            mvwprintw(panel_window(top), i + 1, 2, "%s\n", entry->d_name);

            if (display_wins_1 == true) { // вывод директории на второй панели

                mvwprintw(my_wins[1], i + 1, 2, "%s\n", entry->d_name);

            }

            count++;
        }

        display_wins_1 = false;
        box(my_wins[0], 0, 0);
        box(my_wins[1], 0, 0);

        update_panels();
        doupdate();

        int ch = getch();

        switch ( ch )
        {
            case KEY_F(2): 

                exit = true;
                break;

            case '\t': // переход на следующую панель

                top = (PANEL *)panel_userptr(top);
                top_panel(top);
                strcpy(temporary_directory, save_directory);
                strcpy(save_directory, directory);
                strcpy(directory, temporary_directory);
                break;

            case '\n': 

                strcat(directory, "/");
                strcat(directory, test_array[choice]);
                choice = 0;
                break;

            case KEY_UP:

                if ( choice != 0 ) 
                    choice--; 
                break;

            case KEY_DOWN:

                if ( choice != count - 1 ) 
                    choice++;
                break;
        }

        closedir(dir);

    }

    // уничтожение созданных панелей и окон
    for(i = 0; i < 3; ++i)
    {
        del_panel(my_panels[i]);
        delwin(my_wins[i]);
    }

    endwin();

    return 0;
}

void init_wins(WINDOW **wins, int n)
{
    int x, i;

    x = 0;

    for(i = 0; i < n; ++i)
    {

        wins[i] = newwin(row - 3, col / 2, 3, x);
        wbkgdset(wins[i], COLOR_PAIR(1));
        wclear(wins[i]);
        wrefresh(wins[i]);

        x += col / 2;
    }
}