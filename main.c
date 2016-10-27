#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>

#define NLINES 37
#define NCOLS 67

struct dirent *entry;

char test_array[2555][25555]; // позор №1
char directory[25555] = "/";  // позор №2

void init_wins(WINDOW **wins, int n);
void init_wins_table(WINDOW **wins, int n);

int main()
{	
    WINDOW *my_wins[5];
    PANEL  *my_panels[5];
    PANEL  *top;
    int ch, i;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

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
    init_wins_table(my_wins, 4);

    my_wins[4] = newwin(3, 138, 0, 2);
    wbkgdset(my_wins[4], COLOR_PAIR(1));
    wclear(my_wins[4]);


    for(i = 2; i < 5; ++i)

		box(my_wins[i], 0, 0);

    for (i = 0; i < 5; ++i) 

        my_panels[i] = new_panel(my_wins[i]);


    // устанавливаем указатели на следующее окно
    // для перехода при нажатии Tab на следующее окно
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);

    mvwprintw(my_wins[4], 1, 60, "Tab - next panel F2 - exit");

    update_panels();
    doupdate();

    top = my_panels[1];

    keypad(my_wins[0], TRUE);

    unsigned choice = 0; //Выбор пользователя
    bool exit = false;

    while ( !exit )
    { 

        wclear(my_wins[0]);
        wclear(my_wins[1]);

        DIR *dir;
        int count = 0;
        dir = opendir(directory);

        for (unsigned i = 0; (entry = readdir(dir)) != NULL; ++i)
        {
            if ( i == choice ) { 

                waddch(my_wins[0],'>'); 

            } else {         

                waddch(my_wins[0],' ');

            }

            strcpy(test_array[count], entry->d_name);
            wprintw(my_wins[0], "%s\n", entry->d_name);
            wprintw(my_wins[1], "%s\n", entry->d_name);
            count++;
        }

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
    for(i = 0; i < 5; ++i)
    {
		del_panel(my_panels[i]);
		delwin(my_wins[i]);
    }

    endwin();

    return 0;
}

void init_wins(WINDOW **wins, int n)
{
    int x, y, i;

    y = 4;
    x = 3;

    for(i = 0; i < n; ++i)
    {
		wins[i] = newwin(NLINES, NCOLS, y, x);
		wbkgdset(wins[i], COLOR_PAIR(1));
		wclear(wins[i]);
		wrefresh(wins[i]);
			
		y += 0;
		x += 69;
    }
}

void init_wins_table(WINDOW **wins, int n)  // да это очередной стыд и позор, но его скоро не будет
{
    int x, i;

    x = 2;

    for(i = 2; i < n; ++i)
    {
        wins[i] = newwin(39, 69, 3, x);
        wbkgdset(wins[i], COLOR_PAIR(1));
        wclear(wins[i]);
        wrefresh(wins[i]);

        x += 69;
    }
}