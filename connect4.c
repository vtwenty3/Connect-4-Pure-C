#include <stdio.h>
#include <time.h>
#define HEIGHT 6
#define LENGTH 7
#define MAX 43

struct stack
{
    int array[MAX];
    int top;
};

//global variables that need to be accessed from functions and main method
char matrixgrid[HEIGHT][LENGTH];
char token1 = '1';
char token0 = '0';
char moveshistory[43];
FILE *history;
char answer;
int statsfornerds = 0;
int hei,
    len;

//declaration of functions
int matrixupdate(char arr[HEIGHT][LENGTH], int columns, char token);
void win(char arr[HEIGHT][LENGTH], char token);
void whowins();
int input(char token);
void undo(char arr[HEIGHT][LENGTH], int columns, char token);
void writehistory(int input);
void init_stack(struct stack *);
void push(struct stack *, int item);
int *pop(struct stack *);
int savegame();
void matrixinit(int pref);
void printmatrix();
int PressEnterToContinue(); //returns 0 if input is 's'
void tie(char arr[6][7]);
int redoaccess = 0; //used as a bool to prevent accidental redo
char answer = 'n';  //checked at the end of the game
int columnfull = 3;
int main()
{
    //stack is used for keeping track of the current game history, reference 3
    struct stack movehistory;
    init_stack(&movehistory);
    struct stack undohistory;
    init_stack(&undohistory);
    int *i = NULL;
    int *j = NULL;
    int *k = NULL;
    int *l = NULL;
    //each time the player decide to play new game the matrixgrid is populated with stars which replace the last state of the game

    printf("\n    ______      ______                                   ____           ______              ");
    printf("\n  .~      ~.  .~      ~.  |..          | |..          | |             .~      ~. `````|````` ");
    printf("\n |           |          | |  ``..      | |  ``..      | |______      |                |      ");
    printf("\n |           |          | |      ``..  | |      ``..  | |            |                |      ");
    printf("\n  `.______.'  `.______.'  |          ``| |          ``| |___________  `.______.'      |      ");
    printf("\n                                                                                             ");
    printf("\n                ___________    ______                                                        ");
    printf("\n               |             .~      ~.  |         | |`````````,                             ");
    printf("\n               |______      |          | |         | |'''|'''''                              ");
    printf("\n               |            |          | |         | |    `.                                 ");
    printf("\n               |             `.______.'  `._______.' |      `.                               ");
    printf("\n                                                                 40439132, Valentin Kisimov \n");
    printf("\nPress Enter to Enter the Main Menu!");
    //goto flow control, not the best solution, but working and bug free
menu:
    getchar();
    printf("_____________________________________________________________________________\n");
    printf("\n     _______ _______ _____ __   _      _______ _______ __   _ _     _");
    printf("\n     |  |  | |_____|   |   | \\  |      |  |  | |______ | \\  | |     |");
    printf("\n     |  |  | |     | __|__ |  \\_|      |  |  | |______ |  \\_| |_____| \n\n");
    printf("_____________________________________________________________________________\n\n");
    int menuOption;
    while (1)
    {
        clock_t load_game_clock_start; //always declared because of scope
        printf("1. Start a Game\n");
        printf("2. Load a Game\n");
        printf("3. Enable stats for nerds\n");
        printf("9. Exit \n");
        printf("Choose an option:");
        scanf("%d", &menuOption);
        switch (menuOption)
        {
        case 1:
            printf("Game starting...\n\n");
            history = fopen("connect4save.txt", "a");
            fprintf(history, "\n"); //preventing merging the save with the last one
            fclose(history);
            goto start;
        case 2:
            int linesCount = savegame(0);
            int end, loop, linetoread;
            int i = 0;
            char str[120];

            printf("\nLoad a Save Game (1-%d): ", linesCount);

            while (scanf("%d", &linetoread) <= 0 || linesCount < linetoread || !linetoread) //preventing invalid input, reference 1
            {
                printf("Invalid input. Try again!\n");
                printf("\nLoad a Save Game (1-%d): ", linesCount);

                while ((linetoread = getchar()) != '\n' && linetoread != EOF)
                    ; //clear the stdin
            }

            if (statsfornerds == 1)
            {
                load_game_clock_start = clock();
            }
            //Reading a specified line from a txt file, reference 2
            FILE *fd = fopen("connect4save.txt", "r");
            if (fd == NULL)
            {
                printf("\nError! Failed to open the save game file! Terminating the program!\n");
                goto exit;
            }
            //loading the specified line in str, each move 'n' is element in str[n]
            linetoread = linetoread + 1;
            for (end = loop = 0; loop < linetoread; ++loop)
            {
                if (0 == fgets(str, sizeof(str), fd))
                {
                    end = 1;
                    break;
                }
            }
            int moves = strlen(str);
            int remaining = strlen(str);
            remaining = remaining--;
            matrixinit(0);
            int playerswitch = 0;
            int intplayer0 = 3;
            int intplayer1 = 3;
            int plr0 = 2;
            int plr1 = 2;

            for (i = 0; i < moves; i++)
            {

                str[i] = str[i] - '0';

                printf("%d moves remaining.\n", remaining--);

                if (playerswitch == 0)
                {
                    if (statsfornerds == 1)
                    {
                        clock_t load_game_clock_end = clock();
                        double load_game_time = ((double)(load_game_clock_end - load_game_clock_start)) / CLOCKS_PER_SEC;
                        printf("Load time: %f seconds.\n", load_game_time);
                        statsfornerds = 0;
                    }
                    plr0 = PressEnterToContinue(); //retuns 0 if input is 's'
                    if (plr0 == 0)
                    {
                        printmatrix();
                        goto player0;
                    }

                    if (str[i] == 9) // 9 - undo
                    {                //using pre-written stackAPI from lab03 for tracking undo and redo
                        k = pop(&movehistory);
                        push(&undohistory, *k);
                        undo(matrixgrid, *k, token1);
                        playerswitch = 1;
                        continue;
                    }
                    else if (str[i] == 8) // 8 - redo
                    {
                        l = pop(&undohistory);
                        push(&movehistory, *l);
                        matrixupdate(matrixgrid, *l, token0);
                        playerswitch = 1;
                        continue;
                    }
                    else // 1-7
                    {
                        push(&movehistory, str[i]);
                        matrixupdate(matrixgrid, str[i], token0);
                        playerswitch = 1; //using a int for flow control between player0 and player1
                        continue;
                    }
                }
                else if (playerswitch == 1)
                {
                    plr1 = PressEnterToContinue();

                    if (plr1 == 0)
                    {
                        printmatrix();
                        goto player1;
                    }
                    if (str[i] == 9)
                    {
                        k = pop(&movehistory);
                        push(&undohistory, *k);
                        undo(matrixgrid, *k, token0);
                        playerswitch = 0;
                    }
                    else if (str[i] == 8)
                    {
                        l = pop(&undohistory);
                        push(&movehistory, *l);
                        matrixupdate(matrixgrid, *l, token1);
                        playerswitch = 0;
                    }
                    else
                    {
                        push(&movehistory, str[i]);
                        matrixupdate(matrixgrid, str[i], token1);
                        playerswitch = 0;
                    }
                }
            }

            fclose(fd);
            continue;

        case 3:
            printf("\nEnabaling stats for nerds...\n");
            statsfornerds = 1;
            printf("\nStats for nerds: Enabled\n");
            break;
        case 9:
            goto exit;
        default:
            printf("\n*Error! Invalid Menu Option!*\n");
            goto menu;
        }
    }

start:

    clock_t player0_clock_start, player1_clock_start, player0_clock_end, player1_clock_end;

    matrixinit(1);

    //infinite while loop for swithcin between the players, it stops execution if win, tie or exit
    while (1)
    {
        //used as a bool again, have value because of unpredictability of null int
    player0:
        int player0move = input(token0); //input checks if the input is correct and returns a value which corresponds to the column of choice
        if (statsfornerds == 1)
        {
            player0_clock_start = clock();
        }
        if (player0move == 9) //undo
        {
            redoaccess = 1; // only after undo user can redo
            i = pop(&movehistory);
            if (i == NULL)
            {
                goto player0;
            }

            push(&undohistory, *i);
            undo(matrixgrid, *i, token1);
            writehistory(player0move);
        }
        else if (player0move == 8) //redo
        {
            if (redoaccess == 1)
            {
                j = pop(&undohistory);
                if (j == NULL)
                {
                    goto player0;
                }
                push(&movehistory, *j);
                matrixupdate(matrixgrid, *j, token0);
                writehistory(player0move);
            }
            else
            {
                printf("\nAccess denied. You have to undo first.\n");
                goto player0;
            }
        }
        else if (player0move == 123)
        {
            savegame(1);
            goto exit;
        }
        else
        {
            redoaccess = 0;
            columnfull = matrixupdate(matrixgrid, player0move, token0); //returns 0 if not full
            if (columnfull == 0)                                        //if column is not full
            {
                push(&movehistory, player0move); //save to move history stack
                win(matrixgrid, token0);         //check if there is a win
                tie(matrixgrid);                 // check if there is a tie
                writehistory(player0move);       // write the move to a txtfile
                if (statsfornerds == 1)
                {
                    player0_clock_end = clock();
                    double player0_clock_time = (double)(player0_clock_end - player0_clock_start) / CLOCKS_PER_SEC;
                    printf("Compute time: %f seconds\n", player0_clock_time);
                }

                //check if the program should continue or stop, depending on the state of answer value
                if (answer == 'r')
                {
                    answer = 'n';
                    break; //goes to start:
                }
                if (answer == 'n')
                {
                    printf("Last Move Column: %d\n", player0move);
                }
                else
                {
                    goto exit; //skips start, program terminates
                }
            }
            else if (columnfull == 1) // error message is printed before repeating
            {
                goto player0;
            }
        }

    player1:
        if (statsfornerds == 1)
        {
            player1_clock_start = clock();
        }
        //almost identical code for player1, it is not a function because of the difficulty working with stacks in functions
        int player1move = input(token1);
        if (player1move == 9)
        {
            redoaccess = 1;
            i = pop(&movehistory);
            if (i == NULL)
            {
                goto player1;
            }
            push(&undohistory, *i);
            undo(matrixgrid, *i, token0);
            writehistory(player1move);
        }
        else if (player1move == 8)
        {
            if (redoaccess == 1)
            {
                j = pop(&undohistory);
                if (j == NULL)
                {
                    goto player1;
                }
                push(&movehistory, *j);
                matrixupdate(matrixgrid, *j, token1);
                writehistory(player1move);
            }
            else
            {
                printf("\nAccess denied. You have to undo first.\n");
                goto player1;
            }
        }
        else if (player1move == 123)
        {
            savegame(1);
            goto exit;
        }
        else
        {
            redoaccess = 0;
            columnfull = matrixupdate(matrixgrid, player1move, token1);
            if (columnfull == 0)
            {
                push(&movehistory, player1move);

                win(matrixgrid, token1);
                tie(matrixgrid);

                writehistory(player1move);
                if (statsfornerds == 1)
                {
                    player1_clock_end = clock();
                    double player1_clock_time = (double)(player1_clock_end - player1_clock_start);
                    printf("That is: %f CPU ticks.\n", player1_clock_time);
                }
                if (answer == 'r')
                {
                    break; //goes to start:
                }
                if (answer == 'n')
                {
                    printf("Last Move Column: %d\n", player1move);
                }
                else
                {
                    goto exit; //skips start, program terminates
                }
            }
            else if (columnfull == 1)
            {
                goto player1;
            }
        }
    }
    goto menu;
exit:
    fclose(history);
    return 0;
}

int matrixupdate(char arr[HEIGHT][LENGTH], int columns, char token)
{
    int row, column;
    int i;

    if (arr[0][columns - 1] != '*')
    {
        printf("\n*Error! Column: %d is full!*\n", columns);
        return 1;
    }

    //main algorithm for updating the matrix;
    for (row = 5; row >= 0; row--)
    {
        if (arr[row][columns - 1] == '*') //if * is present at the position
        {
            arr[row][columns - 1] = token; //update it with token (0 or 1)
            row = -1;
        }
        else
            continue;
    }
    //algorithm for priting spaces between columns
    for (row = 0; row < 6; row++)
    {
        for (column = 0; column < 7; column++)
        {
            printf("%c   ", arr[row][column]);
        }
        printf("\n\n");
    }
    return 0;
}

void win(char arr[HEIGHT][LENGTH], char token)
{

    int row, column;
    for (row = 5; row >= 0; row--) //horizontal check
    {
        //      printf("first loop");
        for (column = 0; column < 4; column++)
        {
            //      printf("first if");
            if (arr[row][column] == token)
            {
                //       printf("Second if");
                if ((((arr[row][column] == arr[row][column + 1]) && arr[row][column] == arr[row][column + 2]) && arr[row][column] == arr[row][column + 3]))
                {
                    whowins(token);
                }
            }
        }
    }

    for (row = 5; row >= 3; row--)
    {
        for (column = 0; column < 7; column++) //vertical check
        {
            if (arr[row][column] == token)
            {
                if ((((arr[row][column] == arr[row - 1][column]) && arr[row][column] == arr[row - 2][column]) && arr[row][column] == arr[row - 3][column]))
                {
                    whowins(token);
                }
            }
        }
    }

    for (row = 5; row >= 3; row--) //diagonal check
    {

        for (column = 0; column < 4; column++)
        {
            if (arr[row][column] == token)
            {
                if ((((arr[row][column] == arr[row - 1][column + 1]) && arr[row][column] == arr[row - 2][column + 2]) && arr[row][column] == arr[row - 3][column + 3]))
                {
                    whowins(token);
                }
            }
        }
    }

    for (row = 5; row >= 3; row--) //other diagonal check
    {
        for (column = 3; column < 7; column++)
        {
            if (arr[row][column] == token)
            {
                if ((((arr[row][column] == arr[row - 1][column - 1]) && arr[row][column] == arr[row - 2][column - 2]) && arr[row][column] == arr[row - 3][column - 3]))
                {
                    whowins(token);
                }
            }
        }
    }
}

void whowins(char token) //called only from win, when win = true
{
    if (token == '1')
    {
        printf("\n ***Player 1 WINS!***\n");
    }
    if (token == '0')
    {
        printf("\n  ***Player 0 WINS!***\n");
    }
    printf("\nPress any key for exit\nor 'r' for restart: ");
    scanf(" %c", &answer);
}

//function write history is used to record each move to a connect4save.txt located in main directory
void writehistory(int input)
{
    history = fopen("connect4save.txt", "a");
    fprintf(history, "%d", input);
    fclose(history);
}

void undo(char arr[HEIGHT][LENGTH], int columns, char token) //reverse function of matrixupdate
{
    char star = '*';
    int row, column;
    for (row = 0; row <= 6; row++)
    {
        if (arr[row][columns - 1] == token)
        {
            arr[row][columns - 1] = star;
            row = -1;
            break;
        }
        else
            continue;
    }

    for (row = 0; row < 6; row++)
    {
        for (column = 0; column < 7; column++)
        {
            printf("%c   ", arr[row][column]);
        }
        printf("\n\n");
    }
}

void init_stack(struct stack *s)
{
    s->top = -1;
}

void push(struct stack *s, int item)
{
    if (s->top == MAX - 1)
    {
        printf(" Stack is full . Couldn 't push '%d' onto stack \n", item);
        return;
    }
    s->top++;
    s->array[s->top] = item;
}

int *pop(struct stack *s)
{
    int *data;
    if (s->top == -1)
    {
        printf("\n Error! Stack empty! \n");
        return NULL;
    }
    data = &s->array[s->top];
    s->top--;
    return data;
}

int savegame(int pref)
{
    FILE *fp = fopen("connect4save.txt", "r");
    char ch;
    int lineCounter = 0;
    if (pref == 1)
    {
        printf("\nWe are in save game!\n");
        history = fopen("connect4save.txt", "a");
        fprintf(history, "\n");
        fclose(history);
    }
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            lineCounter++;
        }
    }
    if (pref == 1)
    {
        printf("\n***Game:%d Saved Successful!***\n", lineCounter);
        return 0;
    }
    else
    {
        printf("\nYou have: %d Saved Games!\n", lineCounter);
        return lineCounter;
    }
}

void matrixinit(int pref)
{
    //stars loop
    for (hei = 0; hei < 6; hei++)
    {
        for (len = 0; len < 7; len++)
        {
            matrixgrid[hei][len] = '*';
        }
    }
    if (pref == 1)
    {
        printmatrix();
    }
}

void printmatrix()
{
    for (hei = 0; hei < 6; hei++)
    {
        for (len = 0; len < 7; len++)
        {
            printf("%c   ", matrixgrid[hei][len]);
        }
        printf("\n\n");
    }
}

int input(char token)
{
    int column;

    printf("_________________________\n1   2   3   4   5   6   7\n\n         Player %c\n   undo - 9 | redo - 8\nInput column number(1-7): ", token);
    while (scanf("%d", &column) == 0 || column < 0 || column > LENGTH || !column) //if scanf failed to scan an integer
    {
        if (column == 9)
        {
            return column;
        }
        else if (column == 8)
        {
            return column;
        }
        else if (column == 23)
        {
            return column;
        }
        printf("Invalid input. Try again!\n");
        printf("_________________________\n1   2   3   4   5   6   7\n\n         Player %c\n   undo - 9 | redo - 8\nInput column number(1-7): ", token);

        while ((column = getchar()) != '\n' && column != EOF)
            ; //clear the stdin
    }
    return column;
}
int PressEnterToContinue()
{
    printf("Press enter for next step, press 's' for start from here:\n");
    while (1)
    {
        char c = getchar();
        if (c == 's')
        {
            return 0;
            break;
        }
        if (c == '\n' || c == EOF)
        {
            return 1;
            break;
        }
    }
}

void tie(char arr[6][7]) //if all columns are full tie = true
{
    int i;
    int counter = 0;
    for (i = 0; i < 7; i++)
    {
        if (arr[0][i] != '*')
        {
            counter++;
        }
        if (counter == 7)
        {
            printf("\nGame Over. TIE!!!\n");
            printf("\nPress any key for exit\nor 'r' for restart: ");
            scanf(" %c", &answer);
        }
    }
}