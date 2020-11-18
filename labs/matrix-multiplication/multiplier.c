#include <stdio.h>
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 2000

int NUM_BUFFERS = 0;
long **buffers;
char* RESULT_MATRIX_FILE;
long * result;
long * matrixA;
long * matrixB;
long **buffers;
pthread_mutex_t *mutexes;
pthread_t threads[NUM_THREADS];

struct dictionary {
    int row;
    int col;
    long* matA;
    long* matB;
};
long * getColumn(int col, long *matrix);
long * readMatrix(char *filename);
long * multiply(long *matA, long *matB);
int releaseLock(int lock);
long multiplyMethod(struct dictionary *data);
long * getRow(int r, long *matrix);
int saveResultMatrix(long *result, char* RESULT_MATRIX_FILE);
long dotProduct(long *vec1, long *vec2);
int getLock();



int main(int argc, char* argv[]){
    
    if(argc!=5){
        errorf("Failed, parameters incomplete ");
        errorf("usage: ➜ ./multiplier -n 10 -out result.txt");
        return 0;
    }
    if( !(strcmp(argv[1], "-n")) && !(strcmp(argv[3],"-out")) ){
        NUM_BUFFERS = atoi(argv[2]);
        RESULT_MATRIX_FILE = argv[4];
    }else if( !(strcmp(argv[1], "-out")) && !(strcmp(argv[3],"-n"))  ){
        RESULT_MATRIX_FILE = argv[2];
        NUM_BUFFERS = atoi(argv[4]);
    }else{
        errorf("Failed to detect all parameters correctly, \n");
        errorf("usage: ➜ ./multiplier -n 10 -out result.txt\n");
        return 0;
    }

    infof("NUM_BUFFERS %d", NUM_BUFFERS);
    infof("RESULT_MATRIX_FILE %s", RESULT_MATRIX_FILE);

    //read the files and sorage into the arrays ->
    matrixA = readMatrix("matA.dat");
    matrixB = readMatrix("matB.dat");
    buffers = malloc(NUM_BUFFERS * sizeof(long *));
    mutexes = malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));

    for(int i = 0; i < NUM_BUFFERS; i++){
        pthread_mutex_init(&mutexes[i], NULL);
    }
    long *result = multiply(matrixA, matrixB);
    saveResultMatrix(result, RESULT_MATRIX_FILE);
    infof("%s saved succesfully", RESULT_MATRIX_FILE);
    free(buffers);
    free(mutexes);
    free(matrixA);
    free(matrixB);
    free(result);
    return 0;
}

long * readMatrix(char *filename) {
    long lines = 0;
    long *data;
    FILE *fileObj = fopen(filename, "r");
    if(fileObj == NULL){
        return NULL;
    }
    // counting number of lines therefore number of data
    char c;
    while((c = fgetc(fileObj)) != EOF){
        if(c=='\n')
            lines++;
    }

    rewind(fileObj);
    data = malloc(lines* sizeof(long));
    if(data == NULL){
        return NULL;
    }
    int i = 0;
    while(fscanf(fileObj, "%ld", &data[i]) != EOF){
        i++;
    }
    fclose(fileObj);
    return data;
}

long * multiply(long *matrixA, long *matrixB) {
    long *data = malloc(4000000 * sizeof(long));
    for(int i = 0; i < NUM_THREADS; i++){
        for(int j = 0; j < NUM_THREADS; j++)
        {
            struct dictionary * curr_args = malloc(sizeof(struct dictionary));
            curr_args->matA = matrixA;
            curr_args->matB = matrixB;
            curr_args->row = i;
            curr_args->col = j;
            
            pthread_create(&threads[j], NULL, (void * (*)(void *))multiplyMethod, (void *)curr_args);
        }

        for(int j = 0; j < NUM_THREADS; j++) {
            void *status;
            pthread_join(threads[j], &status);
            data[2000*j+i] = ((long*)status);
        }
    }
    return data;
}

long multiplyMethod(struct dictionary *data){
    int buffer1 = -1, buffer2 = -1;
    while(buffer1 == -1 || buffer2 == -1){
        if(buffer1 == -1){
            buffer1 = getLock();
        }
        if(buffer2 == -1){
            buffer2 = getLock();
        }
    }
    buffers[buffer1] = getRow((int)(data->row), (long*)(data->matA));
    buffers[buffer2] = getColumn(data->col, data->matB);
    long res = dotProduct(buffers[buffer1], buffers[buffer2]);
    free(buffers[buffer1]);
    free(buffers[buffer2]);
    free(data);
    releaseLock(buffer1);
    releaseLock(buffer2);
    return res;
}

long * getColumn(int col, long *matrix) {
    long *column = malloc(2000 * sizeof(long));
    for(int i = 0; i < 2000; i++){
        column[i] = matrix[i*2000 + col];
    }
    return column;
}

long * getRow(int r, long *matrix) {
    long *row = malloc(2000 * sizeof(long));
    for(int i = 0; i < 2000; i++){
        row[i] = matrix[r*2000 + i];
    }
    return row;
}

int getLock() {
    for(int i = 0; i < NUM_BUFFERS; i++){
        if(pthread_mutex_trylock(&mutexes[i])==0){
            return i;
        }   
    }
    return -1;
}

int releaseLock(int lock) {
    return pthread_mutex_unlock(&mutexes[lock]);
}

long dotProduct(long *vec1, long *vec2) {
    long sum = 0;
    for(int i = 0; i < 2000; i ++) {
        sum += (vec1[i] * vec2[i]);
    }
    return sum;
}

int saveResultMatrix(long *result, char* RESULT_MATRIX_FILE) {    
    FILE *resultFile;
    resultFile = fopen(RESULT_MATRIX_FILE, "w+");
    for(int i = 0; i < 4000000; i++) {
        fprintf(resultFile, "%ld\n", result[i]);
    }
    fclose(resultFile);
    return -1;
}