/**
 * @file CompareSequences.c
 * @author Johnathan Regev
 * @brief Program to compare sequences. The program gets name of a sequences file in a known
 * format and the weights of match, mismatch and gap. The program compares each pair of sequences
 * in the file using a dynamic programming algorithm, and prints the score and the match for each
 * pair.
 */

// ------------------------------------------- includes -------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// ------------------------------------- constants definition -------------------------------------
#define NUMBER_OF_ARGUMENTS 5
#define FILE_NAME_INDEX 1
#define M_INDEX 2
#define S_INDEX 3
#define G_INDEX 4
#define MINIMAL_NUMBER_OF_SEQUENCES 2
#define DECIMAL_BASE 10
#define MAXIMAL_NUMBER_OF_SEQUENCES 100
#define MAXIMAL_ROW_LENGTH 101

const char HEADER_LINE_FIRST_CHAR = '>';
const char MEMORY_ALLOCATION_FAILED_MESSAGE[] = "Error - memory allocation failed\n";

// ------------------------------------------- functions ------------------------------------------
/**
 * @brief A function that checks valid usage of the program, and reads the program arguments.
 * @param argc The number of program arguments.
 * @param argv The program arguments.
 * @param fileNameAddress A pointer to the sequences file name.
 * @param mAddress A pointer to the weight of a match.
 * @param sAddress A pointer to the weight of a mismatch.
 * @param gAddress A pointer to the weight of a gap.
 * @return 0 if the usage is valid, -1 else.
 */
int checkUsage(int argc, char *argv[], char **fileNameAddress,
               int *mAddress, int *sAddress, int *gAddress);
/**
 * @brief A function that checks valid integer input, and reads it.
 * @param str A string (should represents an integer).
 * @param numberAddress A pointer to a variable represents an integer.
 * @return 0 if the usage is valid, -1 else.
 */
int checkNumber(char *str, int *numberAddress);
/**
 * @brief A function that reads a sequences file.
 * @param fileName A string represents the file name.
 * @param sequencesNames An array of strings, that the function reads the names of the sequences
 * to.
 * @param sequences An array of strings, that the function reads the sequences to.
 * @param numberOfSequencesAddress A pointer to the number of sequences that read.
 */
void readSequencesFile(char *fileName, char *sequencesNames[], char *sequences[],
                       int *numberOfSequencesAddress);
/**
 * @brief A function that deletes the newline or end-of-file characters from the end of a string
 * (if it end with one of them), and returns the string's length.
 * @param row A string.
 * @return The string's length (after the deletion).
 */
int deleteNewline(char *row);
/**
 * @brief A function that gets an array of sequences, compares each pair of sequences in the array
 * using a dynamic programming algorithm, and prints the score and the match for each pair.
 * @param sequencesNames An array of sequences names.
 * @param sequences An array of sequences.
 * @param numberOfSequences The number of sequences in the array.
 * @param m The weight of a match.
 * @param s The weight of a mismatch.
 * @param g The weight of a gap.
 */
void compareSequences(char *sequencesNames[], char *sequences[], int numberOfSequences,
                      int m, int s, int g);
/**
 * @brief A function that compares two sequences using a dynamic programming algorithm, and prints
 * their score and match.
 * @param sequencesNames The sequences names array.
 * @param sequences The sequences array.
 * @param numberOfSequences The number of sequences in the array.
 * @param sequence1Name The first sequence's name.
 * @param sequences2Name The second sequence's name.
 * @param sequence1 The first sequence to compare.
 * @param sequence2 The second sequence to compare.
 * @param m The weight of a match.
 * @param s The weight of a mismatch.
 * @param g The weight of a gap.
 */
void compareTwoSequences(char *sequencesNames[], char *sequences[], int numberOfSequences,
                         char *sequence1Name, char *sequences2Name,
                         char *sequence1, char *sequence2, int m, int s, int g);
/**
 * @brief A function that frees the memory allocated for the sequences array.
 * @param sequences The sequences array.
 * @param numberOfSequences The number of sequences in the array.
 */
void freeSequencesMemory(char *sequences[], int numberOfSequences);
/**
 * @brief A function that allocates memory for the table used in the dynamic algorithm to compare
 * two sequences (if the allocation failed, the function frees the memory aready allocated by the
 * program).
 * @param sequencesNames The sequences names array.
 * @param sequences The sequences array.
 * @param numberOfSequences The number of sequences in the array.
 * @param tableAddress A pointer to the table.
 * @param tableRows The number of rows in the table.
 * @param tableColumns The number of columns in the table.
 */
void allocateTable(char *sequencesNames[], char *sequences[], int numberOfSequences,
                   int ***tableAddress, int tableRows, int tableColumns);
/**
 * @brief A function that fills the table used in the dynamic algorithm to compare two sequences.
 * @param sequence1 The first sequence compared.
 * @param sequence2 The second sequence compared.
 * @param table The (empty) table.
 * @param tableRows The number of rows in the table.
 * @param tableColumns The number of columns in the table.
 * @param m The weight of a match.
 * @param s The weight of a mismatch.
 * @param g The weight of a gap.
 */
void fillTable(char *sequence1, char *sequence2, int **table,
               int tableRows, int tableColumns, int m, int s, int g);
/**
 * brief A function that initializes the table used in the dynamic algorithm to compare two
 * sequences.
 * @param table The (empty) talbe.
 * @param tableRows The number of rows in the table.
 * @param tableColumns The number of columns in the table.
 * @param g The weight of a gap.
 */
void initializeTable(int **table, int tableRows, int tableColumns, int g);
/**
 * @brief A function that fills a cell in the table used in the dynamic algorithm to compare two
 * sequences.
 * @param sequence1 The first sequence compared.
 * @param sequence2 The second sequence compared.
 * @param table The table.
 * @param row The number of row of the cell.
 * @param column The number of column of the cell.
 * @param m The weight of a match.
 * @param s The weight of a mismatch.
 * @param g The weight of a gap.
 */
void fillTableCell(char *sequence1, char *sequence2, int **table,
                   int row, int column, int m, int s, int g);
/**
 * @brief A function that computes the first opportunity for a cell in the table used in the
 * dynamic algorithm to compare two sequences (according to the exercise PDF).
 * @param sequence1 The first sequence compared.
 * @param sequence2 The second sequence compared.
 * @param table The table.
 * @param row The number of row of the cell.
 * @param column The number of column of the cell.
 * @param m The weight of a match.
 * @param s The weight of a mismatch.
 * @return  The first opportunity for a cell in the table used in the dynamic algorithm to compare
 * two sequences (according to the exercise PDF).
 */
int computeFirstMatchScore(char *sequence1, char *sequence2, int **table,
                           int row, int column, int m, int s);
/**
 * @brief A function that computes the second opportunity for a cell in the table used in the
 * dynamic algorithm to compare two sequences (according to the exercise PDF).
 * @param table The table.
 * @param row The number of row of the cell.
 * @param column The number of column of the cell.
 * @param g The weight of a gap.
 * @return The second opportunity for a cell in the table used in the dynamic algorithm to compare
 * two sequences (according to the exercise PDF).
 */
int computeSecondMatchScore(int **table, int row, int column, int g);
/**
 * @brief A function that computes the third opportunity for a cell in the table used in the
 * dynamic algorithm to compare two sequences (according to the exercise PDF).
 * @param table The table.
 * @param row The number of row of the cell.
 * @param column The number of column of the cell.
 * @param g The weight of a gap.
 * @return The third opportunity for a cell in the table used in the dynamic algorithm to compare
 * two sequences (according to the exercise PDF).
 */
int computeThirdMatchScore(int **table, int row, int column, int g);
/**
 * @brief A function that computes the maximum of three integers.
 * @param n1 The first integer.
 * @param n2 The second integer.
 * @param n3 The third integer.
 * @return The maximum of the three integers.
 */
int max3(int n1, int n2, int n3);
/**
 * @brief A function that computes the maximum of two numbers.
 * @param n1 The first number.
 * @param n2 The second number.
 * @return The maximum of the two numbers.
 */
int max(int n1, int n2);
/**
 * @brief A function that prints the score of the comparison of two sequences.
 * @param table The table used in the dynamic algorithm to compare two sequences.
 * @param tableRows The number of rows in the table.
 * @param tableColumns The number of columns in the table.
 * @param sequence1Name The name of the first sequence in the sequences array.
 * @param sequence2Name The name of the second sequence in the sequences array.
 */
void printScore(int **table, int tableRows, int tableColumns,
                char *sequence1Name, char *sequence2Name);
/**
 * @brief A function that frees the memory allocated for the table used in the dynamic algorithm to
 * compare two sequences.
 * @param table The table.
 * @param tableRows The number of rows in the table.
 */
void freeTableMemory(int **table, int tableRows);

/**
 * @brief The main function of the program. The function checks the validity of the usage of the
 * program, reads the sequences file, compares each pair of sequences in the file (using a dynamic
 * programming algorithm), and prints the score and the match for each pair.
 * @param argc The number of program arguments.
 * @param argv The program arguments.
 * @return 0 if the function succeed, -1 else.
 */
int main(int argc, char *argv[])
{
    char *fileName = NULL;
    int m, s, g;
    int usage = checkUsage(argc, argv, &fileName, &m, &s, &g);
    if (usage) // if the usage is wrong
    {
        fprintf(stdout, "Usage: CompareSequences <path_to_sequences_file> <m> <s> <g>\n");
        return -1;
    }
    char *sequencesNames[MAXIMAL_NUMBER_OF_SEQUENCES];
    char *sequences[MAXIMAL_NUMBER_OF_SEQUENCES];
    int numberOfSequences = 0;
    readSequencesFile(fileName, sequencesNames, sequences, &numberOfSequences);
    if (numberOfSequences < MINIMAL_NUMBER_OF_SEQUENCES)
    {
        fprintf(stderr, "Error - the sequences file contains less than 2 sequences\n");
        freeSequencesMemory(sequencesNames, numberOfSequences);
        freeSequencesMemory(sequences, numberOfSequences);
    }
    compareSequences(sequencesNames, sequences, numberOfSequences, m, s, g);
    freeSequencesMemory(sequencesNames, numberOfSequences);
    freeSequencesMemory(sequences, numberOfSequences);
    return 0;
}

int checkUsage(int argc, char *argv[], char **fileNameAddress,
               int *mAddress, int *sAddress, int *gAddress)
{
    if (argc != NUMBER_OF_ARGUMENTS)
    {
        return -1;
    }
    *fileNameAddress = argv[FILE_NAME_INDEX];
    if (checkNumber(argv[M_INDEX], mAddress) ||
        checkNumber(argv[S_INDEX], sAddress) ||
        checkNumber(argv[G_INDEX], gAddress))
    {
        return -1;
    }
    return 0;

}

int checkNumber(char *str, int *numberAddress)
{
    char *end = NULL;
    *numberAddress = (int)strtol(str, &end, DECIMAL_BASE);
    if ((*numberAddress == 0 && (errno != 0 || end == str)) || *end != '\0')
    {
        return -1;
    }
    return 0;
}

void readSequencesFile(char *fileName, char *sequencesNames[], char *sequences[],
                       int *numberOfSequencesAddress)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }
    char row[MAXIMAL_ROW_LENGTH];
    int startSequence = 0;
    int rowLength = 0;
    int sequenceLength = 0;
    char *temp = NULL;
    while(fgets(row, MAXIMAL_ROW_LENGTH, file) != NULL)
    {
        //when malloc add 1 to numberOfSequences
        if (row[0] == HEADER_LINE_FIRST_CHAR)
        {
            rowLength = deleteNewline(row);
            sequencesNames[*numberOfSequencesAddress] = (char *)malloc(rowLength * sizeof(char));
            if (sequencesNames[*numberOfSequencesAddress] == NULL)
            {
                fprintf(stderr, MEMORY_ALLOCATION_FAILED_MESSAGE);
                freeSequencesMemory(sequencesNames, *numberOfSequencesAddress);
                freeSequencesMemory(sequences, *numberOfSequencesAddress);
                exit(EXIT_FAILURE);
            }
            else
            {
                strncpy(sequencesNames[*numberOfSequencesAddress], row + 1, (size_t)rowLength - 1);
                sequencesNames[*numberOfSequencesAddress][rowLength - 1] = '\0';
            }
            startSequence = 1;
        }
        else
        {
            rowLength = deleteNewline(row);
            if (startSequence)
            {
                sequences[*numberOfSequencesAddress] = (char *)malloc(
                                                       (rowLength + 1) * sizeof(char));
                if (sequences[*numberOfSequencesAddress] == NULL)
                {
                    fprintf(stderr, MEMORY_ALLOCATION_FAILED_MESSAGE);
                    freeSequencesMemory(sequencesNames, *numberOfSequencesAddress);
                    freeSequencesMemory(sequences, *numberOfSequencesAddress);
                    exit(EXIT_FAILURE);
                }
                strncpy(sequences[*numberOfSequencesAddress], row, rowLength * sizeof(char));
                sequences[*numberOfSequencesAddress][rowLength] = '\0';
                (*numberOfSequencesAddress)++;
                startSequence = 0;
            }
            else
            {
                sequenceLength = (int)strlen(sequences[*numberOfSequencesAddress - 1]);
                temp = (char *)realloc(sequences[*numberOfSequencesAddress - 1],
                        (sequenceLength + rowLength + 1) * sizeof(char));
                if (temp == NULL)
                {
                    fprintf(stderr, MEMORY_ALLOCATION_FAILED_MESSAGE);
                    freeSequencesMemory(sequencesNames, *numberOfSequencesAddress);
                    freeSequencesMemory(sequences, *numberOfSequencesAddress);
                    exit(EXIT_FAILURE);
                }
                sequences[*numberOfSequencesAddress - 1] = temp;
                strncpy(sequences[*numberOfSequencesAddress - 1] + sequenceLength, row,
                        rowLength * sizeof(char));
                sequences[*numberOfSequencesAddress - 1][sequenceLength + rowLength] = '\0';
            }
        }
    }
    if (!feof(file))
    {
        fprintf(stderr, "Error reading file\n");
        freeSequencesMemory(sequences, *numberOfSequencesAddress);
        exit(EXIT_FAILURE);
    }
    if (fclose(file))
    {
        fprintf(stderr, "Error closing file\n");
        freeSequencesMemory(sequences, *numberOfSequencesAddress);
        exit(EXIT_FAILURE);
    }
}

int deleteNewline(char *row)
{
    int rowLength = 0;
    for (int i = 0; i < MAXIMAL_ROW_LENGTH - 1; i++)
    {
        if (row[i] == '\n' || row[i] == '\r' || row[i] == EOF)
        {
            row[i] = '\0';
            break;
        }
        rowLength++;
    }
    return rowLength;
}

void compareSequences(char *sequencesNames[], char *sequences[], int numberOfSequences,
                      int m, int s, int g)
{
    for (int i = 0; i < numberOfSequences - 1; i++)
    {
        for (int j = i + 1; j < numberOfSequences; j++)
        {
            compareTwoSequences(sequencesNames, sequences, numberOfSequences,
                                sequencesNames[i], sequencesNames[j],
                                sequences[i], sequences[j], m, s, g);
        }
    }
}

void compareTwoSequences(char *sequencesNames[], char *sequences[], int numberOfSequences,
                         char *sequence1Name, char *sequences2Name,
                         char *sequence1, char *sequence2, int m, int s, int g)
{
    int tableRows = (int)strlen(sequence1) + 1, tableColumns = (int)strlen(sequence2) + 1;
    int **table = NULL;
    allocateTable(sequencesNames, sequences, numberOfSequences, &table, tableRows, tableColumns);
    fillTable(sequence1, sequence2, table, tableRows, tableColumns, m, s, g);
    printScore(table, tableRows, tableColumns, sequence1Name, sequences2Name);
    freeTableMemory(table, tableRows);
}

void allocateTable(char *sequencesNames[], char *sequences[], int numberOfSequences,
                   int ***tableAddress, int tableRows, int tableColumns)
{
    *tableAddress = (int **)malloc(tableRows * sizeof(int *));
    if (*tableAddress == NULL)
    {
        fprintf(stderr, MEMORY_ALLOCATION_FAILED_MESSAGE);
        freeSequencesMemory(sequencesNames, numberOfSequences);
        freeSequencesMemory(sequences, numberOfSequences);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < tableRows; i++)
    {
        (*tableAddress)[i] = (int *)malloc(tableColumns * sizeof(int));
        if ((*tableAddress)[i] == NULL)
        {
            fprintf(stderr, MEMORY_ALLOCATION_FAILED_MESSAGE);
            freeTableMemory(*tableAddress, i - 1);
            freeSequencesMemory(sequencesNames, numberOfSequences);
            freeSequencesMemory(sequences, numberOfSequences);
            exit(EXIT_FAILURE);
        }
    }
}

void fillTable(char *sequence1, char *sequence2, int **table, int tableRows,
               int tableColumns, int m, int s, int g)
{
    initializeTable(table, tableRows, tableColumns, g);
    for (int i = 1; i < tableRows; i++)
    {
        for (int j = 1; j < tableColumns; j++)
        {
            fillTableCell(sequence1, sequence2, table, i, j, m, s, g);
        }
    }
}

void initializeTable(int **table, int tableRows, int tableColumns, int g)
{
    for (int i = 0; i < tableRows; i++)
    {
        table[i][0] = i * g;
    }
    for (int j = 1; j < tableColumns; j++)
    {
        table[0][j] = j * g;
    }
}

void fillTableCell(char *sequence1, char *sequence2, int **table,
                   int row, int column, int m, int s, int g)
{
    int firstMatchScore = computeFirstMatchScore(sequence1, sequence2, table, row, column, m, s);
    int secondMatchScore = computeSecondMatchScore(table, row, column, g);
    int thirdMatchScore = computeThirdMatchScore(table, row, column, g);
    int score = max3(firstMatchScore, secondMatchScore, thirdMatchScore);
    table[row][column] = score;
}

int computeFirstMatchScore(char *sequence1, char *sequence2, int **table,
                           int row, int column, int m, int s)
{
    if (sequence1[row - 1] == sequence2[column - 1])
    {
        return table[row - 1][column - 1] + m;
    }
    else
    {
        return table[row - 1][column - 1] + s;
    }
}

int computeSecondMatchScore(int **table, int row, int column, int g)
{
    return table[row][column - 1] + g;
}

int computeThirdMatchScore(int **table, int row, int column, int g)
{
    return table[row - 1][column] + g;
}

int max3(int n1, int n2, int n3)
{
    return max(max(n1, n2), n3);
}

int max(int n1, int n2)
{
    if (n1 >= n2)
    {
        return n1;
    }
    return n2;
}

void printScore(int **table, int tableRows, int tableColumns,
                char *sequence1Name, char *sequence2Name)
{
    int score = table[tableRows - 1][tableColumns - 1];
    printf("Score for alignment of %s to %s is %d\n",
           sequence1Name, sequence2Name, score);
}

void freeTableMemory(int **table, int tableRows)
{
    for (int i = 0; i < tableRows; i++)
    {
        free(table[i]);
        table[i] = NULL;
    }
    free(table);
    table = NULL;
}

void freeSequencesMemory(char *sequences[], int numberOfSequences)
{
    for (int i = 0; i < numberOfSequences; i++)
    {
        free(sequences[i]);
        sequences[i] = NULL;
    }
}