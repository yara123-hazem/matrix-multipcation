#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

struct element {
    int row;
    int col;
    int (*matrix1)[];
    int (*matrix2)[];
    int (*output)[];
    int col1;
    int col2;
    int row1;
};

void *multiplyElement(void *arg) {
    struct element *params = (struct element *)arg;
    int row = params->row;
    int col = params->col;
    int col1 = params->col1;
    int col2 = params->col2;
    int (*matrix1)[col1] = params->matrix1;
    int (*matrix2)[col2] = params->matrix2;
    int (*output)[col2] = params->output;

    int sum = 0;
    for (int k = 0; k < col1; k++) {
        sum += matrix1[row][k] * matrix2[k][col];
    }
    output[row][col] = sum;
    free(params);
    pthread_exit(NULL);
}

void multiplyMatricesElement(int row1, int col1, int row2, int col2, int (*matrix1)[col1], int (*matrix2)[col2], int (*output)[col2]) {
    pthread_t *tid = malloc(row1 * col2 * sizeof(pthread_t));

    int Count = 0;
    for (int i = 0; i < row1; ++i) {
        for (int j = 0; j < col2; ++j) {
            struct element *data = malloc(sizeof(struct element));
            data->row = i;
            data->col = j;
            data->matrix1 = matrix1;
            data->matrix2 = matrix2;
            data->output = output;
            data->col1 = col1;
            data->col2 = col2;
            pthread_create(&tid[Count], NULL, multiplyElement, (void *)data);
            Count++;
            
        }
       
    }
    for (int i = 0; i < Count; ++i) {
        pthread_join(tid[i], NULL);
    }

    free(tid);
}

void *multiplyRow(void *arg) {
struct element *params = (struct element *)arg;
    int row = params->row;
    int col1 = params->col1;
    int col2 = params->col2;
    int (*matrix1)[col1] = params->matrix1;
    int (*matrix2)[col2] = params->matrix2;
    int (*output)[col2] = params->output;
  for (int col = 0; col < col2; ++col)
 {
    int sum = 0;
    for (int k = 0; k < col1; k++) {
        sum += matrix1[row][k] * matrix2[k][col];
    }
    output[row][col] = sum;
    }
    free(params);
    pthread_exit(NULL);
}

void multiplyMatricesRow(int **matrix1, int **matrix2, int **result, int row1, int rows2, int col1, int col2) 
  {  pthread_t threads[row1];

    int Count = 0;
    for (int i = 0; i < row1; ++i) {
        struct element* data = malloc(sizeof(struct element));
       data->row = i;
       data->col1 = col1;
       data->col2 = col2;
       data->matrix1 = matrix1;
       data->matrix2 = matrix2;
       data->output = result;
       pthread_create(&threads[Count], NULL, multiplyRow, (void*)data);
       Count++;
    }

    for (int i = 0; i < Count; ++i) {
        pthread_join(threads[i], NULL);
    }
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "file not found\n");
        return -1;
    }

    int row1, col1, row2, col2;
    fscanf(f, "%d %d", &row1, &col1);

    int (*matrix1)[col1] = malloc(row1 * sizeof(int[col1]));
    for (int i = 0; i < row1; ++i) {
        for (int j = 0; j < col1; ++j) {
            fscanf(f, "%d", &matrix1[i][j]);
        }
    }

    fscanf(f, "%d %d", &row2, &col2);

    int (*matrix2)[col2] = malloc(row2 * sizeof(int[col2]));
    for (int i = 0; i < row2; ++i) {
        for (int j = 0; j < col2; ++j) {
            fscanf(f, "%d", &matrix2[i][j]);
        }
    }

    fclose(f);

    if (col1 != row2) {
        printf("Multiplication of matrices is not possible!!\n");
        return -1;
    }

    int (*result)[col2] = malloc(row1 * sizeof(int[col2]));
    int (*output)[col2] = malloc(row1 * sizeof(int[col2]));
     
     clock_t start1 = clock();
    multiplyMatricesRow(matrix1,matrix2,output,row1,row2,col1,col2);
    clock_t end1 = clock();
    printf("by Row\n");
    for (int i = 0; i < row1; ++i) {
        for (int j = 0; j < col2; ++j) {
            printf("%d ", output[i][j]);
        }
        printf("\n");
        }
    double total_elapsed_time1 = ((double)(end1- start1)) / CLOCKS_PER_SEC;
    printf("Time: %f s\n", total_elapsed_time1);
    printf("\n");
    
    
    clock_t start = clock();
    multiplyMatricesElement(row1, col1, row2, col2, matrix1, matrix2, result);
    clock_t end = clock();
    printf("by Element\n");
    for (int i = 0; i < row1; ++i) {
        for (int j = 0; j < col2; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    double total_elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time: %f s\n", total_elapsed_time);
  
  // so each row of output is a thread is better then each element of output is a thread because it is faster so when number of thread is smaller yhe algorithm is better    

    
    free(matrix1);
    free(matrix2);
    free(result);

    return 0;
}
