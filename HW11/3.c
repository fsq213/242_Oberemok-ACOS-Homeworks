#include <stdio.h>
#include <stdlib.h>

int main() {
    int N, M;
    
    printf("Enter number of rows N and columns M: ");
    scanf("%d %d", &N, &M);
    
    int **matrix = (int**)malloc(N * sizeof(int*));
    if (matrix == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    for (int i = 0; i < N; i++) {
        matrix[i] = (int*)malloc(M * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }
    }
    
    printf("Enter %d integers for the matrix:\n", N * M);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            scanf("%d", &matrix[i][j]);
        }
    }
    
    int **transpose = (int**)malloc(M * sizeof(int*));
    if (transpose == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    for (int i = 0; i < M; i++) {
        transpose[i] = (int*)malloc(N * sizeof(int));
        if (transpose[i] == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            transpose[j][i] = matrix[i][j];
        }
    }
    
    printf("Transposed matrix (%d x %d):\n", M, N);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", transpose[i][j]);
        }
        printf("\n");
    }
    
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);
    for (int i = 0; i < M; i++) {
        free(transpose[i]);
    }
    free(transpose);
    
    return 0;
}
