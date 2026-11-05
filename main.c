/*
    NAME : AHUSAN FASIQ R
    ROLL NO : 6001B_015
*/


#include "header.h"
char prompt[500];
char input_string[500];
 
int main()
{
    // clearing the screen
    system("clear");
 
    strcpy(prompt,"minishell$: ");
 
    //minishell
    scan_input(prompt,input_string);

    return 0;
}