#ifndef FM_FUNC_H
#define FM_FUNC_H

char *add_file_or_direct(char *new_way, char *directory, char **words, unsigned choice);
char *test_return(char *input_direct, char *directory, char **words, unsigned choice);
int type_file(char *directory, char **words, unsigned choice, WINDOW **my_wins);
void * percent_func (void * arg);
void * cp_func (void * arg);
char *malloc_array(char *array);
void realloc_all_array(unsigned length, char *directory, char *save_directory, char *temporary_directory);
bool move_between_directory(unsigned choice, char *directory, char *save_directory, char *temporary_directory, char **words, char *test);
void print_box_and_update(WINDOW **wins);
void init_wins(WINDOW **wins, int n, int row, int col);
void create_wins_and_panel(WINDOW **my_wins, PANEL **my_panels);
char **update_directory(char **words, char *directory, int *ptr, WINDOW **my_wins);
char **sort_array(char **words, unsigned counter_sort);
void free_all(char **words, char *directory, char *save_directory, char *temporary_directory, int wordCounter);

#endif
