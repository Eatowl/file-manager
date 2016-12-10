#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ncurses.h>
#include <dirent.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include "fm_func.h"
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#define SIZE_INCREMENT 50
#define BUF_SIZE 4096
#define BUF_READ 128

char buffer[BUF_SIZE];

struct thread_arg {
    WINDOW *my_wins[1];
    float count;
    char cp_file[BUF_SIZE];
};

struct thread_arg_paste {
    char cp_file_p[BUF_SIZE];
    char paste_direct[BUF_SIZE];
};

struct dirent *entry;
struct stat buf;

int type_file(char *directory, char **words, unsigned choice, WINDOW **my_wins) {
    int st = 0;
    bool ret = true;
    char *test;
    test = test_return(test, directory, words, choice);
    if (words[choice] == ".." || words[choice] == ".") {
        st = 3;
    } else {
        lstat(test, &buf);
    }
    if (S_ISREG(buf.st_mode)) st = 1;
    else if (S_ISDIR(buf.st_mode)) st = 0;
    switch ( st ) {
        case 0:
            return 0;
            break;
        case 1:
            if (buf.st_mode & S_IXUSR == 64) {
                return 2;
                break;
            } else {
                return 1;
                break;
            }
            return 1;
            break;
    }
    free(test);
}

char *test_return(char *input_direct, char *directory, char **words, unsigned choice) {
    unsigned length;
    input_direct = malloc_array(input_direct);
    if (strlen(directory) != 1) {
        length = strlen(words[choice]) + 1;
    } else {
        length = strlen(words[choice]);
    }
    input_direct = (char*) realloc(input_direct, strlen(input_direct) + length + 1);
    if (strlen(directory) != 1) {
        input_direct = add_file_or_direct(input_direct, directory, words, choice);
    } else {
        strcat(input_direct, words[choice]);
    }
    return input_direct;
}

char *add_file_or_direct(char *new_way, char *directory, char **words, unsigned choice) {
    strcpy(new_way, directory);
    strcat(new_way, "/");
    strcat(new_way, words[choice]);
    return new_way;
}

void * percent_func (void * arg) {
    struct thread_arg targ = *(struct thread_arg *) arg;
    float countOut = 0;
    ssize_t bytes;
    int rez = 0, yfd;

    yfd = open (targ.cp_file, O_RDONLY);
    while ((bytes = read (yfd, buffer, BUF_READ)) > 0) {
        wclear(targ.my_wins[0]);
        rez += BUF_READ;
        countOut = rez / (targ.count * BUF_READ / 100);
        mvwprintw(targ.my_wins[0], 0, 1,
            "percent - %f\n", countOut);
        update_panels();
        doupdate();
        sleep(1);
    }
    close(yfd);
    return NULL;
}

void * cp_func (void * arg) {
    struct thread_arg_paste cp_arg = *(struct thread_arg_paste *) arg;
    int ifd, ofd, tfd;
    float count = 0, countOut = 0;
    ssize_t bytes;
    ssize_t byt;

    ifd = open (cp_arg.cp_file_p, O_RDONLY);
    ofd = open (cp_arg.paste_direct, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    while ((bytes = read (ifd, buffer, BUF_READ)) > 0) {
        write (ofd, buffer, bytes);
    }
    close (ifd);
    close (ofd);
    return NULL;
}

char *malloc_array(char *array) {
    array = (char*) malloc(2);
    array = strcpy(array, "/");
    return array;
}
 
void realloc_all_array(unsigned length, char *directory, char *save_directory, char *temporary_directory) {
    directory = (char*) realloc(directory, strlen(directory) + length + 1);
    save_directory = (char*) realloc(save_directory, strlen(directory) + length + 1);
    temporary_directory = (char*) realloc(temporary_directory, strlen(directory) + length + 1);
}

bool move_between_directory(unsigned choice, char *directory, char *save_directory, char *temporary_directory, char **words, char *test) {
    unsigned length;
    if (strlen(directory) != 1) {
        length = strlen(words[choice]) + 1;
    } else {
        length = strlen(words[choice]);
    }
    realloc_all_array(length, directory, save_directory, temporary_directory);
    if (strlen(directory) != 1) {
        strcat(directory, "/");
        strcat(directory, words[choice]);
    } else {
        strcat(directory, words[choice]);
        return true;
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

    my_wins[3] = newwin(1, col, 1, col - (col - 110));
    wbkgdset(my_wins[3], COLOR_PAIR(1));
    wclear(my_wins[3]);

    print_box_and_update(my_wins);
    // создание панелей, на основе окон
    for (int i = 0; i < 4; ++i)
        my_panels[i] = new_panel(my_wins[i]);
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);

    mvwprintw(my_wins[2], 1, 1,
        "Tab - next panel | F2 - exit | Enter - choise | C - Copy | P - Paste");
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
