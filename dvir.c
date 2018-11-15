#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//DEF
#define NUM_OF_ARGS 5
#define MAX_LINE_LENGTH 100
#define MAX_NUM_SEQ 100
#define DEFAULT_BASE 10
//ENDEF


char* removeEndOfLine(char* line);
int linesToArray(FILE* readFile, char** sequences);
void buildTable(char* seq1, char* seq2, int match, int mismatch, int gap,
                int numOfSeq1, int numOfSeq2);
void bestMatches(char** sequences, int size, int match, int mismatch, int gap);
/**
 * The start of the program.
 * @param argc The number of arguments given in the call to the program.
 * @param argv The arguments that were given.
 * @return 0 if succeeded, 1 otherwise.
 */
int main(int argc, char** argv)
{
    char **sequences;
    char *filename;
    FILE *readFile;
    int j = 0, numOfSeq = 0;
    int match = 0, mismatch = 0, gap = 0;
    if (argc < NUM_OF_ARGS)
    {
        fprintf(stderr, "Usage: CompareSequences <path_to_file> <m> <s> <g>\n");
        return 1;
    }
    sequences = (char **) malloc(MAX_NUM_SEQ * sizeof(char *));
    for (j = 0; j < MAX_NUM_SEQ; j++)//maybe make a function
    {
        sequences[j] = NULL;
    }
    filename = argv[1];
    match = strtol(argv[2], NULL, DEFAULT_BASE);
    mismatch = strtol(argv[3], NULL, DEFAULT_BASE);
    gap = strtol(argv[4], NULL, DEFAULT_BASE);
    readFile = fopen(filename, "r");
    if (readFile == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 1;
    }
    numOfSeq = linesToArray(readFile, sequences);
    if (numOfSeq == 0)
    {
        fprintf(stderr, "No sequences were found,"
                        " please enter a valid file.\n");
        return 1;
    }
    bestMatches(sequences, numOfSeq, match, mismatch, gap);
    for (j = 0; j < numOfSeq; j++)
    {
        free(sequences[j]);
    }
    free(sequences);
    fclose(readFile);
    return 0;
}
/**
 * Removes the end of line char from the string..
 *
 * @param line The string to remove from the end of line char.
 * @return A new string containing the string without the end of line char.
 */
char* removeEndOfLine(char* line)
{
    //because we need space for \0 we will malloc with the same size.
    char* newLine = (char*)malloc(strlen(line) * sizeof(char));
    strncpy(newLine, line, strlen(line) - 1);
    newLine[strlen(line) - 1] = '\0';
    return newLine;
}
/**
 * Reads the files and add the lines to the array of sequences.
 * @param readFile The file to read from.
 * @param sequences An array for containing the sequences.
 * @return The number of sequences.
 */
int linesToArray(FILE* readFile, char** sequences)
{
    char *line, *tempLine;
    int i = -1, curSize = 0, len =  0;
    line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    while (fgets(line, MAX_LINE_LENGTH, readFile) != NULL && i < MAX_NUM_SEQ)
    {
        if (strncmp(">", line, strlen(">")) == 0)//This is a header
        {
            i++;
            curSize = 0;
            continue;
        }
        else if (line[0] != '\n')//means it's not an empty line
        {
            tempLine = removeEndOfLine(line);
            len = strlen(tempLine);
            curSize += len;
            if (curSize == len)//means it was zero before.
            {
                sequences[i] = (char*)malloc((curSize + 1) * sizeof(char));
                strncpy(sequences[i], tempLine, (size_t)curSize);
            }
            else
            {
                sequences[i] = (char*)realloc(sequences[i],
                                              (curSize + 1) * sizeof(char));
                strncat(sequences[i], tempLine, len);
            }
            free(tempLine);
        }
    }
    free(line);
    return i + 1;
}
/**
 * Prints the best matches for each sequence.
 * @param sequences The sequences to match to each other.
 * @param size The real number of sequences in the array.
 * @param match The match weight.
 * @param mismatch The mismatch weight.
 * @param gap The gap weight.
 */
void bestMatches(char** sequences, int size, int match, int mismatch, int gap)
{
    //counters
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = i + 1; j < size; j++)
        {
            buildTable(sequences[i], sequences[j], match, mismatch, gap, i, j);
        }
    }
}
/**
 * Builds the comparing table.
 * @param seq1 The first sequence of the compare.
 * @param seq2 The second sequence of the compare.
 * @param match The match weight.
 * @param mismatch The mismatch weight.
 * @param gap The gap weight.
 * @param numofSeq1 The index of the first sequence.
 * @param numOfSeq2 The index of the second sequence.
 */
void buildTable(char* seq1, char* seq2, int match, int mismatch, int gap,
                int numofSeq1, int numOfSeq2)
{
    int k = 0, l = 0;
    int m = strlen(seq1) + 1;
    int n = strlen(seq2) + 1;
    int max = 0, temp = 0;
    int** table = (int**)malloc(m * sizeof(int*));
    for (k = 0; k < m; k++)
    {
        table[k] = (int*)malloc(n * sizeof(int));
    }
    for (k = 0; k < m; k++)//initialize first row
    {
        table[k][0] = k * gap;
    }
    for (l = 0; l < n; l++)//initialize first column
    {
        table[0][l] = l * gap;
    }
    for (k = 1; k < m; k++)//run on the first sequence
    {
        for (l = 1; l < n; l++)//run on the second sequence
        {
            //option 1
            if (seq1[k - 1] == seq2[l - 1])
            {
                max = table[k - 1][l - 1] + match;
            }
            else
            {
                max = table[k - 1][l - 1] + mismatch;
            }
            //end of option 1
            //option 2
            temp = table[k][l - 1] + gap;
            max = temp > max ? temp : max;
            //end of option 2
            //option 3
            temp = table[k - 1][l] + gap;
            max = temp > max ? temp : max;
            //end of option 3
            table[k][l] = max;
        }
    }
    printf("Score for alignment of seq%d to seq%d "
           "is %d\n", (numofSeq1 + 1), (numOfSeq2 + 1), table[m - 1][n - 1]);
    for (k = 0; k < m; k++)
    {
        free(table[k]);
    }
    free(table);
}