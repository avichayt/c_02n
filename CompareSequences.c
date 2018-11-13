#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_AMOUNT_OF_SEQUENCES 100
#define MAX_LINE_LENGTH 100
#define AMOUNT_OF_ARGC 5

#define FILE_NAME_ARGUMENT 1
#define MATCH_WEIGHT_ARGUMENT 2
#define MISMATCH_WEIGHT_ARGUMENT 3
#define GAP_WEIGHT_ARGUMENT 4

//todo exact errors
#define FILE_OPEN_ERROR "Error in opening file: %s"

char **getSequencesFromFile(char *fileName, int *amountOfSequences);

bool isHeaderLine(char *line);

void calculateAlignmentScore(char *seq1, char *seq2, int m, int s, int g);

int max(int n1, int n2);

int main(int argc, char **argv)
{
    if (argc < AMOUNT_OF_ARGC)
    {
        // todo
        exit(EXIT_FAILURE);
    }

    // todo strtol instead of atoi
    int m = atoi(argv[MATCH_WEIGHT_ARGUMENT]);
    int s = atoi(argv[MISMATCH_WEIGHT_ARGUMENT]);
    int g = atoi(argv[GAP_WEIGHT_ARGUMENT]);

    int amountOfSeqs;
    char **sequences = getSequencesFromFile(argv[FILE_NAME_ARGUMENT], &amountOfSeqs);

    for (int i = 0; i < amountOfSeqs; ++i)
    {
        for (int j = i + 1; j < amountOfSeqs; ++j)
        {
            printf("i:%d, j:%d\n", i, j);
            calculateAlignmentScore(sequences[i], sequences[j], m, s, g);
        }
    }

    // todo free malloc
    // todo comments
    return 0;
}

void calculateAlignmentScore(char *seq1, char *seq2, int m, int s, int g)
{
    int len1 = (int) strlen(seq1) + 1, len2 = (int) strlen(seq2) + 1;
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


            *(dataValues + (i) * len2 + (j)) = max(matchXMinusYMinus, max(matchXYMinus, matchXMinusY));
            int a = 0;
        }
    }
    printf("%d\n", *(dataValues + (len1) * (len2) - 1));
}

int max(int n1, int n2)
{
    if (n1 > n2)
    {
        return n1;
    }
    return n2;
}

char **getSequencesFromFile(char *fileName, int *amountOfSequences)
{
    //todo make function shorter
    FILE *fileHandle;
    char line[MAX_LINE_LENGTH];

    // try open file
    fileHandle = fopen(fileName, "r");
    if (fileHandle == NULL)
    {
        printf(FILE_OPEN_ERROR, fileName);
        exit(EXIT_FAILURE);
    }

    char **sequences = (char **) malloc(MAX_AMOUNT_OF_SEQUENCES * sizeof(char *));
    int seqNumber = -1;
    char *currentSeq = NULL;
    size_t currentSeqLength = 0;
    size_t currentSeqMemory = 0;
    size_t currentLineLength;
    // read file line by line into 'line' array
    while (fgets(line, MAX_LINE_LENGTH, fileHandle) != NULL)
    {
        if (isHeaderLine(line))
        {
            if (seqNumber >= 0)
            {
                sequences[seqNumber] = currentSeq;
            }
            seqNumber++;

            // todo check allocation
            // todo free memory
            currentSeq = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
            currentSeqLength = 0;
            currentSeqMemory = MAX_LINE_LENGTH;
        } else
        {
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
            strcat(currentSeq, line);
            currentSeqLength += currentLineLength;
        }
    }
    sequences[seqNumber] = currentSeq;
    fclose(fileHandle);
    *amountOfSequences = seqNumber + 1;
    return sequences;
}

bool isHeaderLine(char *line)
{
    // todo magic number
    return *line == '>';
}

