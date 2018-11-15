#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_NUMBER_OF_SEQUENCES 100
#define MAX_NUMBER_OF_CHARS_IN_LINE 102
#define LENGTH_OF_OPENING_CHAR 1
#define NUMBER_OF_ARGS 5

int readFromFile(char *sequences[MAX_NUMBER_OF_SEQUENCES], int i, char **argv,
                 char sequencesNames[MAX_NUMBER_OF_SEQUENCES][MAX_NUMBER_OF_CHARS_IN_LINE]);

void deleteEndOfLineChars(char *line);

int algorithmGradeCalculation(char *firstString, char *secondString, int matchWeight,
                              int mismatchWeight, int gapWeight);

int convertArgumentCharToInt(char *argument);

int returnMaxOfThreeInts(int a, int b, int c);

/**
 * The main function of the program which in the end prints the scores.
 * @param argc number of arguments given by the user.
 * @param argv the vector of arguments given by user.
 * @return 0 if all is well.
 */
int main(int argc, char **argv)
{
    if (argc != NUMBER_OF_ARGS)
    {
        fprintf(stderr, "Error, wrong num of args");
        exit(EXIT_FAILURE);
    }
    char *sequences[MAX_NUMBER_OF_SEQUENCES];
    char sequencesNames[MAX_NUMBER_OF_SEQUENCES][MAX_NUMBER_OF_CHARS_IN_LINE];
    int matchingWeight = convertArgumentCharToInt(argv[2]);
    int mismatchingWeight = convertArgumentCharToInt(argv[3]);
    int gapWeight = convertArgumentCharToInt(argv[4]);
    int numberOfSequences = readFromFile(sequences, 1, argv, sequencesNames);
    if (numberOfSequences == -1)
    {
        fprintf(stderr, "Error, not enough sequences");
    }
    for (int i = 0; i < numberOfSequences; ++i)
    {
        for (int j = i + 1; j < numberOfSequences; ++j) //j=i so we won't calculate two sequences
            // twice.
        {
            int score = algorithmGradeCalculation(sequences[i], sequences[j], matchingWeight,
                                                  mismatchingWeight, gapWeight);
            printf("Score for alignment of %s to %s is %d\n", sequencesNames[i],
                   sequencesNames[j], score); // todo repair
        }
    }
    for (int k = 0; k < numberOfSequences; ++k)
    {
        free(sequences[k]);
    }
    return 0;
}


int convertArgumentCharToInt(char *argument)
/**
 * Converts the weights given by chars to ints.
 * @param argument the weight given by the user.
 * @return the weight as an int.
 */
{
    char *end;
    errno = 0;
    float weight = strtof(argument, &end);
    if (weight == 0 && (errno != 0 || end == argument))
    {
        fprintf(stderr, "Error in weight conversion");
        exit(EXIT_FAILURE);
    }
    int result = (int) weight;
    return result;
}


int returnMaxOfThreeInts(int a, int b, int c)
/**
 * @param a first int.
 * @param b  second int.
 * @param c third int.
 * @return the max.
 */
{
    if ((a > b) && (a > c))
    {
        return a;
    }
    else if (b > c)
    {
        return b;
    }
    else
    {
        return c;
    }
}


int algorithmGradeCalculation(char *firstString, char *secondString, int matchWeight,
                              int mismatchWeight, int gapWeight)
/**
 * Calculates the score of two sequences/
 * @param firstString the first string.
 * @param secondString the second string.
 * @param matchWeight the weight of a matching.
 * @param mismatchWeight the weight of a mismatching.
 * @param gapWeight the weight of a gap.
 * @return the score.
 */
{
    int numberOfRows = strlen(firstString) + 1;
    int numberOfColumns = strlen(secondString) + 1;
    int **algorithmTable = (int **) malloc((numberOfRows) * sizeof(int *));
    for (int j = 0; j < numberOfRows; ++j)
    {
        algorithmTable[j] = (int *) malloc((numberOfColumns) * sizeof(int));
    }
    for (int i = 0; i < numberOfRows; ++i)
    {
        algorithmTable[i][0] = gapWeight * i;
    }
    for (int k = 0; k < numberOfColumns; ++k)
    {
        algorithmTable[0][k] = gapWeight * k;
    }
    for (int row = 1; row < numberOfRows; ++row)
    {
        for (int column = 1; column < numberOfColumns; ++column)
        {
            int addMatching;
            if (firstString[row - 1] == secondString[column - 1])
            {
                addMatching = algorithmTable[row - 1][column - 1] + matchWeight;
            }
            else
            {
                addMatching = algorithmTable[row - 1][column - 1] + mismatchWeight;
            }
            int matchGapWithXi = algorithmTable[row - 1][column] + gapWeight;
            int matchGapWithYj = algorithmTable[row][column - 1] + gapWeight;
            algorithmTable[row][column] = returnMaxOfThreeInts(addMatching,
                                                               matchGapWithXi, matchGapWithYj);
        }
    }
    int result = algorithmTable[numberOfRows - 1][numberOfColumns - 1];
    for (int l = 0; l < numberOfRows; ++l)
    {
        free(algorithmTable[l]);
    }
    free(algorithmTable);
    return result;
}


void deleteEndOfLineChars(char *line)
/**
 * Does as in the header.
 * @param line without unwanted chars.
 */
{
    for (size_t i = 0; i < strlen(line); ++i)
    {
        if (line[i] == '\n' || line[i] == '\r')
        {
            line[i] = '\0';
            break;
        }
    }
}


int readFromFile(char *sequences[MAX_NUMBER_OF_SEQUENCES], int i, char **argv,
        char sequencesNames[MAX_NUMBER_OF_SEQUENCES][MAX_NUMBER_OF_CHARS_IN_LINE])
/**
 * Reads the file and writes the sequences in an array.
 * @param atomCoordinates the atoms coordinates array.
 * @param i number of param in argv.
 * @param argv the program arguments.
 * @return the number of atoms in the file (j is the number of valid lines read).
 */
{
    FILE *fp;
    fp = fopen(argv[i], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", argv[i]);
        exit(EXIT_FAILURE);
    }
    char line[MAX_NUMBER_OF_CHARS_IN_LINE];
    int j = -1;
    int temp = 0;
    while (fgetc(fp) != '>')
    {
    }
    fseek(fp, -1, SEEK_CUR); // Take care of the option that the files first rows
    // are empty lines.
    while (fgets(line, MAX_NUMBER_OF_CHARS_IN_LINE, fp) != NULL)
    {
        deleteEndOfLineChars(line);
        char openingChar[LENGTH_OF_OPENING_CHAR];
        memcpy(openingChar, line, LENGTH_OF_OPENING_CHAR);
        if ((strncmp(openingChar, ">", LENGTH_OF_OPENING_CHAR) == 0))
        {
            j += 1;
            char newLine[MAX_NUMBER_OF_CHARS_IN_LINE];
            strncpy(newLine, line + 1, strlen(line) - 1);
            strcpy(sequencesNames[j], newLine);
            temp = 1;
            continue;
        }
        int lineLen = strlen(line);
        if (temp == 1)
        {// make a new sequence and allocate memory needed.
            sequences[j] = (char *) malloc(lineLen * sizeof(char) + 1);
            strcpy(sequences[j], line);
            temp = 0;
        }
        else // cat the new line to the existing sequence.
        {
            sequences[j] = (char *) realloc(sequences[j], strlen(sequences[j]) * sizeof(char) +
                                                          lineLen * sizeof(char) + 1);
            if (sequences[j] == NULL)
            {
                fprintf(stderr, "Error, not enough memory left");
                exit(EXIT_FAILURE);
            }
            strcat(sequences[j], line);
        }
    }
    if (j <= 0) // there are less than two sequences in the file.
    {
        return -1;
    }
    return j + 1;
}

