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

#define BUF_SIZE 4096
#define BUF_READ 128
#define SIZE_INCREMENT 100

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

char **words;
char *directory,
     *save_directory,
     *temporary_directory,
     *test,
     *copy_file,
     *copy_dir,
     *name_copy_file;
char *dir_array;
char *in_run,
     *out_run;

struct dirent *entry;

//int type_file(char *directory, char **words, unsigned choice, WINDOW **my_wins);

int main() {
    WINDOW *my_wins[4];
    PANEL  *my_panels[4];
    PANEL  *top;

    create_wins_and_panel(my_wins, my_panels);
    top = my_panels[0];

    directory = malloc_array(directory);
    save_directory = malloc_array(save_directory);
    temporary_directory = malloc_array(temporary_directory);
    test = malloc_array(test);
    copy_file = (char*) malloc(4096);
    copy_dir = (char*) malloc(4096);
    dir_array = (char*) malloc(4096);
    name_copy_file = (char*) malloc(128);
    words = (char**) malloc(SIZE_INCREMENT*sizeof(char*));

    unsigned choice = 0;
    bool exit = false, display_wins_1 = true, update_dir = true, param = true;
    int wordCounter;

    while ( !exit ) {
        wclear(panel_window(top));
        if (update_dir != false) {
            DIR *dir;
            unsigned length, max_length = 0, size = SIZE_INCREMENT;
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
            if (display_wins_1 == true) {  // вывод директории на второй
                mvwprintw(my_wins[1], i + 1, 2, "%s\n", words[i]);
            }
        }
        display_wins_1 = false;
        print_box_and_update(my_wins);
        int ch = getch(), type_f = 1;
        pid_t result;

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
            case '\n':  // обработка нажатия клавиши Enter
                type_f = type_file(directory, words, choice, my_wins);
                if (type_f == 0) {
                    if (strlen(directory) != 1) {
                        strcat(directory, "/");
                        strcat(directory, words[choice]);
                    } else {
                        strcat(directory, words[choice]);
                    }
                    update_dir = true;
                    choice = 0;
                }
                if (type_f == 2) {
                    char *run;
                    char * newprog_args[] = {
                            NULL
                    };
                    run = test_return(run, directory, words, choice);
                    execve (run, newprog_args, NULL);
                    free(run);
                }
                break;
            case 99:  // Нажатие клавиши "C" - копирование файла
                type_f = type_file(directory, words, choice, my_wins);
                if (type_f == 1) {
                    int tfd;
                    ssize_t byt;
                    float count = 0;
                    tfd = open ("type.c", O_RDONLY);
                    while ((byt = read (tfd, buffer, BUF_READ)) > 0)
                        count++;
                    close (tfd);

                    strcpy(copy_dir, directory);
                    copy_file = add_file_or_direct(copy_file, directory, words, choice);
                    strcpy(name_copy_file, words[choice]);

                    pthread_t thread;
                    int result;
                    struct thread_arg targ;
                    targ.my_wins[0] = my_wins[3];
                    targ.count = count;
                    strcpy(targ.cp_file, copy_file);

                    result = pthread_create (&thread, NULL, &percent_func, &targ);
                }
                break;
            case 112: // Нажатие клавиши "P" - вставить файл
                strcpy(dir_array, directory);
                if (strcmp(directory, copy_dir) == 0) {
                    struct thread_arg_paste cp_arg;

                    strcat(name_copy_file, "(copy)");
                    strcat(dir_array, "/");
                    strcat(dir_array, name_copy_file);
                    strcpy(cp_arg.cp_file_p, copy_file);
                    strcpy(cp_arg.paste_direct, dir_array);

                    pthread_t thread1;
                    int result1;
                    result1 = pthread_create(&thread1, NULL, &cp_func, &cp_arg);
                } else {
                    struct thread_arg_paste cp_arg;

                    strcat(dir_array, name_copy_file);
                    strcpy(cp_arg.cp_file_p, copy_file);
                    strcpy(cp_arg.paste_direct, dir_array);

                    pthread_t thread1;
                    int result1;
                    result1 = pthread_create(&thread1, NULL, &cp_func, &cp_arg);
                }
                update_dir = true;
                break;
            case 105: // Нажатие клавиши "I" - выбор исполняемой программы для получения входных данных.
                type_f = type_file(directory, words, choice, my_wins);
                if (type_f == 2)
                    in_run = test_return(in_run, directory, words, choice);
                break;
            case 111: // Нажатие клавиши "O" - выбор исполняемой программы для запуска.
                type_f = type_file(directory, words, choice, my_wins);
                if (type_f == 2)
                    out_run = test_return(out_run, directory, words, choice);
                int pf[2], pid1, pid2;
                char spf [2][32];
                if (pipe (pf) == -1) {
                    fprintf (stderr, "pipe() error\n");
                    return 1;
                }
                sprintf (spf[0], "%d", pf[0]);
                sprintf (spf[1], "%d", pf[1]);

                if ((pid1 = fork ()) == 0) {
                    dup2 (pf[1], 1);
                    close (pf[0]);
                    execlp (in_run, "in", spf[1], NULL);
                    fprintf (stderr, "exec() [1] error\n");
                    return 1;
                }
                if ((pid1 = fork ()) == 0) {
                    dup2 (pf[0], 0);
                    close (pf[1]);
                    execlp (out_run, "out", spf[0], NULL);
                    fprintf (stderr, "exec() [2] error\n");
                    return 1;
                }
                close (pf[1]);
                waitpid (pid1, NULL, 0);
                close (pf[0]);
                waitpid (pid2, NULL, 0);
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
