#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_SEQUENCES 100
#define MAX_CHAR_IN_LINE 102
#define HEADER_CHAR '>'
#define NEW_LINE "\n"

/**
 * Gets the path to the input file, and returns the sequences and their names generated from the
 * file. In addition, returns the number of sequences found in the file.
 * @param path The path to the file.
 * @param sequences The array to enter the sequences to.
 * @param names The array to enter the sequences' names to.
 * @return The number of sequences found in the file.
 */
int readFile(char *path, char *sequences[MAX_SEQUENCES], char *names[MAX_SEQUENCES])
{
    FILE *stream = fopen(path, "r");
    if (stream == NULL)
    {
        fprintf(stderr, "Error. Couldn't open input file.\n");
        exit(EXIT_FAILURE);
    }
    int isNewSequence = 0;
    char buffer[MAX_CHAR_IN_LINE + 1];
    int count = 0;

    for (int i = 0; i < MAX_SEQUENCES; i++)
    {
        sequences[i] = malloc(sizeof(char));
        if (sequences[i] == NULL)
        {
            fprintf(stderr, "Error at allocating memory.");
            exit(EXIT_FAILURE);
        }
    }

    while (fgets(buffer, MAX_CHAR_IN_LINE + 1, stream) != NULL && count < MAX_SEQUENCES)
    {
        strtok(buffer, NEW_LINE);
        if (strcmp(buffer, NEW_LINE) == 0)
        {
            continue;
        }
        if (buffer[0] == HEADER_CHAR)
        {
            count++;
            names[count - 1] = (char*) malloc(strlen(buffer)); // we subtract 1 because we
            // remove the header, but then add 1 for '\0'.
            if (names[count - 1] == NULL)
            {
                fprintf(stderr, "Error at allocating memory.");
                exit(EXIT_FAILURE);
            }
            names[count - 1] = strcpy(names[count - 1], buffer + 1);
            isNewSequence = 1;
            continue;
        }
        if (isNewSequence)
        {
            isNewSequence = 0;
        }
        sequences[count - 1] = (char*) realloc(sequences[count - 1], strlen(sequences[count - 1]) +
        strlen(buffer) + 1);
        if (names[count - 1] == NULL)
        {
            fprintf(stderr, "Error at allocating memory.");
            exit(EXIT_FAILURE);
        }
        strcat(sequences[count - 1], buffer);

    }
    fclose(stream);
    return count;
}

/**
 * Gets three number and returns their maximum value.
 * @param a The first number.
 * @param b The second number.
 * @param c The third number.
 * @return The maximum value.
 */
int maxThree(int a, int b, int c)
{
    int result = a;
    result = result > b ? result : b;
    result = result > c ? result : c;
    return result;
}

/**
 * Gets three numbers and returns the absolute value of their maximum.
 * @param a The first number.
 * @param b The second number.
 * @param c The third number
 * @return Their maximum, positive.
 */
int maxAndAbs(int a, int b, int c)
{
    int result = maxThree(a, b, c);
    result = result > 0 ? result : -1 * result;
    return result;
}

/**
 * Calculates a cell in the table. If another cell is needed and it wasn't calculated yet, it
 * calls for itself for that cell.
 * @param table The dynamic table.
 * @param x The first string to compare.
 * @param y The second string to compare.
 * @param match The match value.
 * @param mismatch The mismatch value.
 * @param gap The gap value.
 * @param max An upper bound for the values in the table. Tells if a cell had been calculated or
 * not.
 * @param i The i index of the cell being calculated.
 * @param j The j index of the cell being calculated.
 */
void calculateCell(int **table, char* x, char* y, int match, int mismatch, int gap, int max, int
                   i, int j)
{
    if (table[i][j] != max)
    {
        return;
    }
    if (i == 0)
    {
        table[i][j] = j * gap;
        return;
    }
    if (j == 0)
    {
        table[i][j] = i * gap;
        return;
    }
    if (table[i - 1][j] == max)
    {
        calculateCell(table, x, y, match, mismatch, gap, max, i - 1, j);
    }
    if (table[i][j - 1] == max)
    {
        calculateCell(table, x, y, match, mismatch, gap, max, i, j - 1);
    }
    if (table[i - 1][j - 1] == max)
    {
        calculateCell(table, x, y, match, mismatch, gap, max, i - 1, j - 1);
    }
    int option1, option2, option3;
    option1 = x[i - 1] == y[j - 1] ? table[i - 1][j - 1] + match : table[i - 1][j - 1] + mismatch;
    option2 = table[i][j - 1] + gap;
    option3 = table[i - 1][j] + gap;
    table[i][j] = maxThree(option1, option2, option3);
}

/**
 * Gets two strings and returns the optimal comparison value.
 * @param x The first string to compare.
 * @param y The second string to compare.
 * @param match The match value.
 * @param mismatch The mismatch value.
 * @param gap The gap value.
 * @return The comparison value.
 */
int compareStrings(char* x, char* y, int match, int mismatch, int gap)
{
    int **table;
    table = (int**) malloc(sizeof(int*) * (strlen(x) + 1));
    if (table == NULL)
    {
        fprintf(stderr, "Error at allocating memory.");
        exit(EXIT_FAILURE);
    }
    int maxValue = maxAndAbs(match, mismatch, gap) + 1;
    for (int i = 0; i < (int) strlen(x) + 1; i++)
    {
        table[i] = (int *) malloc(sizeof(int) * (strlen(y) + 1));
        if (table[i] == NULL)
        {
            fprintf(stderr, "Error at allocating memory.");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < (int) strlen(y) + 1; j++)
        {
            table[i][j] = maxValue;
        }
    }
    for (int i = 0; i < (int) strlen(x) + 1; i++)
    {
        for (int j = 0; j < (int) strlen(y) + 1; j++)
        {
            calculateCell(table, x, y, match, mismatch, gap, maxValue, i, j);
        }
    }
    int result = table[strlen(x)][strlen(y)];
    for (int i = 0; i < (int) strlen(x) + 1; i++)
    {
        free(table[i]);
    }
    free(table);
    return result;

}

/**
 * Parses the values from the user to C variables.
 * @param argv The string array representing the user input.
 * @param match A pointer to the match value.
 * @param mismatch A pointer to the mismatch value.
 * @param gap A pointer to the gap value.
 */
void getValuesFromUser(char **argv, int *match, int *mismatch, int *gap)
{
    char *end;
    *match = (int) strtol(argv[2], &end, 10);
    if (*match == 0 && (errno != 0 || end == argv[2]))
    {
        fprintf(stderr, "Error. Match value should be integer.\n");
        exit(EXIT_FAILURE);
    }
    *mismatch = (int) strtol(argv[3], &end, 10);
    if (*mismatch == 0 && (errno != 0 || end == argv[3]))
    {
        fprintf(stderr, "Error. Mismatch value should be integer.\n");
        exit(EXIT_FAILURE);
    }
    *gap = (int) strtol(argv[4], &end, 10);
    if (*gap == 0 && (errno != 0 || end == argv[4]))
    {
        fprintf(stderr, "Error. Gap value should be integer.\n");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "Error. Not correct usage. Usage should be: CompareSequences.c "
                        "<input_file> <match_value> <mismatch_value> <gap_value>\n");
        exit(EXIT_FAILURE);
    }
    char *sequences[MAX_SEQUENCES], *names[MAX_SEQUENCES];
    int num = readFile(argv[1], sequences, names);
    if (num == 0 || num == 1)
    {
        fprintf(stderr, "Error. Not enough sequences in the file.\n");
        exit(EXIT_FAILURE);
    }
    int match, mismatch, gap;
    getValuesFromUser(argv, &match, &mismatch, &gap);
    for (int i = 0; i < num - 1; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            int result = compareStrings(sequences[i], sequences[j], match, mismatch, gap);
            printf("Score for alignment of %s to %s is %d\n", names[i], names[j], result);
        }
    }
    return 0;
}