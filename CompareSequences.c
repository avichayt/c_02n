#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>

#define MAX_AMOUNT_OF_SEQUENCES 100
#define MAX_LINE_LENGTH 100
#define AMOUNT_OF_ARGC 5

#define FILE_NAME_ARGUMENT 1
#define MATCH_WEIGHT_ARGUMENT 2
#define MISMATCH_WEIGHT_ARGUMENT 3
#define GAP_WEIGHT_ARGUMENT 4
#define SEQ_OPENER '>'

//todo exact errors
#define FILE_OPEN_ERROR "Error in opening file: %s"
#define INVALID_NUMBER_ERROR "Error: cannot convert %s to number."
#define TOO_FEW_SEQUENCES "Error: File contains less than two sequences."

char **
getSequencesFromFile(char *fileName, int *amountOfSequences, char names[MAX_AMOUNT_OF_SEQUENCES][MAX_LINE_LENGTH]);

bool isHeaderLine(const char *line);

void calculateAlignmentScore(char *seq1, char *seq2, int m, int s, int g);

int max(int n1, int n2);

int getIntFromString(char *string);

bool isWhiteSpace(const char *string);

/**
 * @brief the main function of the program
 * @param argc args counter
 * @param argv args vector
 * @return status
 */
int main(int argc, char **argv)
{
    if (argc < AMOUNT_OF_ARGC)
    {
        // todo
        exit(EXIT_FAILURE);
    }

    int m = getIntFromString(argv[MATCH_WEIGHT_ARGUMENT]);
    int s = getIntFromString(argv[MISMATCH_WEIGHT_ARGUMENT]);
    int g = getIntFromString(argv[GAP_WEIGHT_ARGUMENT]);


    int amountOfSeqs = 0;
    char names[MAX_AMOUNT_OF_SEQUENCES][MAX_LINE_LENGTH];
    char **sequences = getSequencesFromFile(argv[FILE_NAME_ARGUMENT], &amountOfSeqs, names);

    if (amountOfSeqs < 2)
    {
        fprintf(stderr, TOO_FEW_SEQUENCES);
    }

    for (int i = 0; i < amountOfSeqs; ++i)
    {
        for (int j = i + 1; j < amountOfSeqs; ++j)
        {
            printf("Score for alignment of sequence %s to sequence %s is ", names[i], names[j]);
            calculateAlignmentScore(sequences[i], sequences[j], m, s, g);
        }
    }

    // todo comments
    // todo errors
    // todo in1.txt 1 -1 -1
    // todo run tests
    // stderr
    // todo seq names
    // todo compile without errors

    // free all allocations
    for (int k = 0; k < amountOfSeqs; ++k)
    {
        free(sequences[k]);
    }
    free(sequences);
    return 0;
}

/**
 * @brief calculates the alignment score of sequences
 * @param seq1 first one
 * @param seq2 second seq
 * @param m match weight
 * @param s mismatch
 * @param g gap
 */
void calculateAlignmentScore(char *seq1, char *seq2, int m, int s, int g)
{
    int len1 = (int) strlen(seq1) + 1, len2 = (int) strlen(seq2) + 1;

    // build alignment matrix
    int *dataValues = (int *) malloc((len1) * (len2) * sizeof(int));
    for (int i = 0; i < len1; ++i)
    {
        *(dataValues + (i) * len2 + (0)) = g * i;
    }
    for (int i = 0; i < len2; ++i)
    {
        *(dataValues + (0) * len2 + (i)) = g * i;
    }

    int matchXMinusYMinus, matchXYMinus, matchXMinusY;
    for (int i = 1; i < len1; ++i)
    {
        for (int j = 1; j < len2; ++j)
        {
            if (seq1[i - 1] == seq2[j - 1])
            {
                matchXMinusYMinus = (int) (*(dataValues + (i - 1) * len2 + (j - 1)) + m);//dataValues[i - 1][j - 1] + m;
            } else
            {
                matchXMinusYMinus = (int) (*(dataValues + (i - 1) * len2 + (j - 1)) + s);
            }

            matchXMinusY = (int) (*(dataValues + (i - 1) * len2 + (j)) + g); //dataValues[i - 1][j] + g;
            matchXYMinus = (int) (*(dataValues + (i) * len2 + (j - 1)) + g);//dataValues[i][i - 1] + g;

            // get the maximum of the three
            *(dataValues + (i) * len2 + (j)) = max(matchXMinusYMinus, max(matchXYMinus, matchXMinusY));
        }
    }
    printf("%d\n", *(dataValues + (len1) * (len2) - 1));
    free(dataValues);
}

/**
 * @brief get maximum of two numberes
 * @param n1 first number
 * @param n2 second number
 * @return maximum
 */
int max(int n1, int n2)
{
    if (n1 > n2)
    {
        return n1;
    }
    return n2;
}

/**
 * @brief get sequences from data file
 * @param fileName  the file name
 * @param amountOfSequences  the amount of sequneces read (out parmater)
 * @return sequences array
 */
char **
getSequencesFromFile(char *fileName, int *amountOfSequences, char names[MAX_AMOUNT_OF_SEQUENCES][MAX_LINE_LENGTH])
{

    FILE *fileHandle;
    char line[MAX_LINE_LENGTH];

    // try open file
    fileHandle = fopen(fileName, "r");
    if (fileHandle == NULL)
    {
        fprintf(stderr, FILE_OPEN_ERROR, fileName);
        exit(EXIT_FAILURE);
    }

    char **sequences = (char **) malloc(MAX_AMOUNT_OF_SEQUENCES * sizeof(char *));
    int seqNumber = -1;
    char *currentSeq = NULL;
    size_t currentSeqLength = 0;
    size_t currentSeqMemory = 0;
    size_t currentLineLength;

    //char names[100][100];


    // read file line by line into 'line' array
    while (fgets(line, MAX_LINE_LENGTH, fileHandle) != NULL)
    {
        if (isHeaderLine(line))
        {
            // only if not the first one
            if (seqNumber >= 0)
            {
                sequences[seqNumber] = currentSeq;
            }
            seqNumber++;

            strcpy(names[seqNumber], line + 1);
            names[seqNumber][strlen(names[seqNumber]) - 1] = 0;

            // todo check allocation
            // todo free memory
            currentSeq = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
            currentSeq[0] = '\0';
            currentSeqLength = 0;
            currentSeqMemory = MAX_LINE_LENGTH;
        } else
        {
            if (isWhiteSpace(line))
            {
                continue;
            }

            //todo check whitespace line
            currentLineLength = strlen(line);

            // remove new line char from line
            if (line[currentLineLength - 1] == '\n')
            {
                line[currentLineLength - 1] = '\0';
                currentLineLength--;
            }
            if (line[currentLineLength - 1] == '\r')
            {
                line[currentLineLength - 1] = '\0';
                currentLineLength--;
            }

            if (currentSeqLength + currentLineLength < currentSeqMemory)
            {
                if (currentSeqLength == 0)
                {
                    currentSeqLength++; // for null terminator
                }
            } else
            {
                // todo check NULL
                currentSeqMemory = currentSeqMemory + MAX_LINE_LENGTH * sizeof(char);
                currentSeq = (char *) realloc(currentSeq, currentSeqMemory);
            }

            // todo not initialized error
            currentSeq[currentSeqLength] = '\0';
            strcat(currentSeq, line);
            currentSeqLength += currentLineLength;
        }
    }
    sequences[seqNumber] = currentSeq;
    fclose(fileHandle);
    *amountOfSequences = seqNumber + 1;
    return sequences;
}

/**
 * @brief check if line is header line
 * @param line line to check
 * @return bool of line is header
 */
bool isHeaderLine(const char *line)
{
    // todo magic number
    return *line == SEQ_OPENER;
}


/**
 * @brief get int value from string
 * @param string string to get number from
 * @return the number
 */
int getIntFromString(char *string)
{
    errno = 0;
    char *endOfNumber;
    int result = 0;


    result = (int) strtol(string, &endOfNumber, 10);
    if (result == 0 && (errno != 0 || endOfNumber == string))
    {
        fprintf(stderr, INVALID_NUMBER_ERROR, string);
        exit(EXIT_FAILURE);
    }
    return result;
}

/**
 * @brief check if line is empty
 * @param string the line
 * @return is empty
 */
bool isWhiteSpace(const char *string)
{
    while (*string)
    {
        if (!isspace(*string))
            return false;
        string++;
    }
    return true;
}

