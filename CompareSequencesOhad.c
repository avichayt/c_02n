#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MIN_NUM_OF_INPUTS 5
#define MAX_AMOUNT_OF_SEQUENCES 100
#define MAX_LENGTH_OF_LINE 100

/**
 *
 * @param sequence1
 * @param sequence2
 * @param m
 * @param s
 * @param g
 * @return
 */
int createOptimalMatch(char* sequence1, char* sequence2, int m, int s, int g)
{
    long len1 = strlen(sequence1);
    long len2 = strlen(sequence2);
    int *matches2 = (int*) malloc((len1 + 1) * (len2 + 1) * sizeof(int));
    //int matches2[len1*len2] = {0};

    for(int i = 0; i <= len1; i++)
    {
        for (int j = 0; j <= len2; j++)
        {
            int maxMatch = 0;
            if(i > 0 && j > 0)
            {
                int iMinus1JMinus12 = matches2[(i - 1 + (len1 + 1) * (j - 1))];
                if (sequence1[i-1] == sequence2[j - 1])
                {
                    maxMatch = iMinus1JMinus12 + m;
                }
                else
                {
                    maxMatch = iMinus1JMinus12 + s;
                }
            }
            if(i > 0)
            {
                int iMinus12 = matches2[(i - 1 + (len1 + 1) * j)] + g;
                if(iMinus12 > maxMatch)
                {
                    maxMatch = iMinus12;
                }
            }
            else
            {
                maxMatch = -j;
            }
            if(j > 0)
            {
                int jMinus12 = matches2[(i + (len1 + 1) * (j - 1))] + g;

                if (jMinus12 > maxMatch)
                {
                    maxMatch = jMinus12;
                }
            }
            else
            {
                maxMatch = -i;
            }
            matches2[(i + (len1 + 1) * j)] = maxMatch;
        }
    }
    int match = matches2[(len1 + (len1 + 1)*len2)];
    free(matches2);
    return match;
}

int isValidInput(int argc, char *argv[])
{
    if(argc < MIN_NUM_OF_INPUTS)
    {
        return 0;
    }
    int valid = 1;
    for (int i = 2; i < 5; ++i) {
        for (int j = 0; j < strlen(argv[i]); ++j) {
            if(!isdigit(argv[i][j]))
            {
                if(j == 0 && argv[i][j] == '-')
                {
                    continue;
                }
                valid = 0;
                break;
            }
        }
        if(valid == 0)
        {
            break;
        }
    }
    return valid;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    if(!isValidInput(argc, argv))
    {
        fprintf(stdout, "Usage: CompareSequences <path_to_sequences_file> <m> <s> <g>");
        return 1;
    }

    int m, s, g;
    m = atoi(argv[2]);
    s = atoi(argv[3]);
    g = atoi(argv[4]);
    FILE *fp;
    fp = fopen(argv[1], "r");
    char *sequences[MAX_AMOUNT_OF_SEQUENCES];
    char *sequencesNames[MAX_AMOUNT_OF_SEQUENCES];
    for(int i = 0; i < MAX_AMOUNT_OF_SEQUENCES; i++)
    {
        sequences[i] = (char*)malloc(1);
        sequences[i][0] = '\0';
        sequencesNames[i] = (char*)malloc(1);
        sequencesNames[i][0] = '\0';
    }
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int num_of_sequences = 0;
    char *sequence = (char*)malloc(sizeof(char));
    unsigned long sequence_len = 0;
    while(num_of_sequences < MAX_AMOUNT_OF_SEQUENCES)
    {
        char line[MAX_LENGTH_OF_LINE] = {'\0'};
        if (fgets(line, MAX_LENGTH_OF_LINE, fp) == NULL)
        {
            sequences[num_of_sequences - 1] = (char*)realloc(sequence, sequence_len);
            break;
        }
        if(strncmp(">", line, 1) == 0)
        {
            // sequence lable
            sequencesNames[num_of_sequences] = (char*)realloc(sequencesNames[num_of_sequences], strlen(line));
            for(int i = 1; i < strlen(line); i++)
            {
                if(line[i] == '\n')
                {
                    break;
                }
                sequencesNames[num_of_sequences][i-1] = line[i];
            }
            if(num_of_sequences> 0)
            {
                sequences[num_of_sequences - 1] = (char*)malloc(sequence_len);
                strcpy(sequences[num_of_sequences - 1], sequence);
                free(sequence);
                sequence = (char *) malloc(sizeof(char));
                sequence_len = 0;
            }
            num_of_sequences++;
        }
        else
        {
            int seq_tmp_len = 0;
            for (int i = 0; i < MAX_LENGTH_OF_LINE; i++)
            {
                if(line[i] == '\n')
                {
                    break;
                }
                seq_tmp_len++;
            }
            sequence = (char*)realloc(sequence, sequence_len + 1 + seq_tmp_len);
            for (int i = 0; i < seq_tmp_len; i++)
            {
                sequence[sequence_len + i] = line[i];
            }
            sequence_len += seq_tmp_len;
            sequence[sequence_len] = '\0';
        }
    }
    if(num_of_sequences == 0)
    {
        fprintf(stderr, "Error: empty file");
    }

    for(int i = 0; i < num_of_sequences; i++)
    {
        for(int j = i + 1; j < num_of_sequences; j++)
        {
            int match = createOptimalMatch(sequences[i], sequences[j], m, s, g);
            printf("Score for alignment of %s to %s is %d\n", sequencesNames[i], sequencesNames[j], match);
        }
    }


    free(sequence);
    for(int i = 0; i < MAX_AMOUNT_OF_SEQUENCES; i++)
    {
        free(sequences[i]);
    }
    return 0;
}
