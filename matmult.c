#include<stdlib.h>
#include<pthread.h>

#define Item(X, I, J, NCOL)      X[((J) + (I) * (NCOL))]

// This struct will be passed to each thread to be able to execute its function.
// Actually not all these attributes are needed in the struct, another solution is to define some of them as global variables.
typedef struct
{
    int *A; // Input matrix 1
    int *B; // Input matrix 2
    int *C; // Output matrix
    int l; // rows of A
    int m; // rows of B
    int n; // columns of B and C
    int i; // current row
    int j; // current column
} Arguments;

/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult(int* A, int* B, int* C, int l, int m, int n)
{
    for(int i=0; i<l; i++)
        for(int j=0; j<n; j++)
        {
            int sum = 0;
            for(int k=0; k<m; k++)
                sum += Item(A, i, k, m) * Item(B, k, j, n);
            Item(C, i, j, n) = sum;
        }
}


/*
 * matrix multiplication
 * version 1: The computation of each element of the output matrix happens in a thread.
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *    C has l rows and n cols
 */

 // This is the function executed by each thread.
 // Its goal its to compute a single element in the output matrix.
 void *function_V1(void *data)
{
    Arguments *args = (Arguments *)data;
    args = (Arguments *)data; //This struct contains all the arguments needed by the function.
    int sum = 0;
    for(int k = 0; k < args->m; k++)
    {
        sum += Item(args->A, args->i, k, args->m) * Item(args->B, k, args->j, args->n);
    }
    Item(args->C, args->i, args->j, args->n) = sum; // Store the value in the correct position in the output matrix C.
    pthread_exit(NULL); // Terminate the thread.
}

void matmult_v1(int* A, int* B, int* C, int l, int m, int n)
{
    int num_threads = l * n; // We need a thread for each element.
    pthread_t thread_id[num_threads];
    int i, j;
    int t = 0;
    for(int i=0; i<l; i++)
    {
        for(int j=0; j<n; j++)
        {
            Arguments *args = malloc(sizeof(Arguments));
            args->A = A;
            args->B = B;
            args->C = C;
            args->i = i;
            args->j = j;
            args->l = l;
            args->m = m;
            args->n = n;
            pthread_create( &thread_id[t], NULL, function_V1, args );
            t++;
        }
    }
    // Execute all threads concurrently.
    for(j=0; j < num_threads; j++)
    {
        pthread_join( thread_id[j], NULL);
    }

}



/*
 * matrix multiplication
 * version 2: The computation of each row of the output matrix happens in a thread.
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */


// This is the function executed by each thread.
// Its goal its to compute a single row in the output matrix.
void *function_V2(void *data)
{
    Arguments *args = (Arguments *)data;
    for(int j=0; j<args->n; j++)
        {
            int sum = 0;
            for(int k=0; k<args->m; k++)
                sum += Item(args->A, args->i, k, args->m) * Item(args->B, k, j, args->n);
            Item(args->C, args->i, j, args->n) = sum;
        }
    pthread_exit(NULL); // Terminate the thread.
}

void matmult_v2(int* A, int* B, int* C, int l, int m, int n)
{
    int num_threads = l; // A thread for each row.
    pthread_t thread_id[num_threads];
    int i, j;
    int t = 0;
    for(int i=0; i<l; i++)
    {
            Arguments *args = malloc(sizeof(Arguments));
            args->A = A;
            args->B = B;
            args->C = C;
            args->i = i;
            args->j = -1;
            args->l = l;
            args->m = m;
            args->n = n;
            pthread_create( &thread_id[t], NULL, function_V2, args );
            t++;
    }
    for(j=0; j < num_threads; j++)
    {
        pthread_join( thread_id[j], NULL);
    }
}
