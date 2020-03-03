#ifndef _MATRIX_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
typedef struct OLNode
{
    int row, col;
    float f_value; //for float
    int n_value; //for int
    struct OLNode *right, *down;
} OLNode, *OLink; // the point of OLNode

typedef struct
{
    OLink *row_head, *col_head; //a point of points of structs
    char data_type[5];
    int m, n, len; //lenth of rows, lenth of cols, lenth of non-empty elements
    float trace; //trace
} CrossList, *Clist;

Clist readfile(char *);
Clist ScalarMuti(Clist,int);
Clist Add(Clist,Clist);
Clist Transpose(Clist);
Clist MatrixMuti(Clist, Clist);
int Trace(Clist);
Clist f_ScalarMuti(Clist,float);
Clist f_Add(Clist,Clist);
Clist f_Transpose(Clist);
Clist f_MatrixMuti(Clist, Clist);
float f_Trace(Clist);
char * fprintM(Clist);
void print_crosslist(Clist);
void outputfile(char* operate,char* filename1,char* filename2,Clist,double delta1,double delta2);
#endif