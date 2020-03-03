#include "matrix.h"
int Num_thread=1;

int main(int argc, char *argv[])
{
    char *filename1;
    char *filename2;
    int flg_SM = 0,n_scalar; 
    float f_scalar;
    int flg_TR = 0,flg_AD=0,flg_TS=0,flg_MM=0,flg_L=0;
    Clist M1,M2,M_new;
    char operate[3];

    double delta1,delta2;
    struct timeval start1, end1,start2,end2;

    static struct option long_options[] = {
        {"sm", 1, NULL, 'c'},
        {"tr", no_argument, NULL, 'r'},
        {"ad", no_argument, NULL, 'a'},
        {"ts", no_argument, NULL, 's'},
        {"mm", no_argument, NULL, 'm'},
        {0, 0, 0, 0}};
    static const char *short_options = "lf:t:";
    int opt;

    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'c':
            flg_SM=1;
            sprintf(operate,"%s","sm");
            f_scalar=atof(optarg);
            n_scalar=atoi(optarg);
            printf("scalar: %f\n", f_scalar);
            printf("Scalar multiplication\n");
            break;
        case 'r':
            flg_TR=1;
            sprintf(operate,"%s","tr");
            printf("Trace\n");
            break;
        case 'a':
            flg_AD=1;
            sprintf(operate,"%s","ad");
            printf("Addition\n");
            break;
        case 's':
            flg_TS=1;
            sprintf(operate,"%s","ts");
            printf("Transpose\n");
            break;
        case 'm':
            flg_MM=1;
            sprintf(operate,"%s","mm");
            printf("Matrix multiplication\n");
            break;
        case 'l':
            flg_L=1;
            break;
        case 't':
            Num_thread=atoi(optarg);
            printf("thread %d\n",Num_thread);
            break;
        case 'f':
            gettimeofday(&start1, NULL);
            filename1 = optarg;
            if(filename1) M1=readfile(filename1);
            filename2 = argv[optind];
            if(!filename2) filename2=NULL;
            else if(!strcmp(&filename2[(strlen(filename2)-3)],".in")) M2=readfile(filename2);
            else filename2=NULL;
            //printf("getfile %s %s\n", filename1, filename2);
            gettimeofday(&end1, NULL);
            delta1 = ((end1.tv_sec - start1.tv_sec) * 1000000u + end1.tv_usec - start1.tv_usec) / 1.e6;
            printf("reading files time=%12.10f\n", delta1);
            break;
        }
    }
    if ((flg_AD||flg_MM)&&!filename2) {printf("need one more matrix"); exit(-1);}
    if (flg_MM && M1->n!=M2->m){printf("matrixs shape are not match"); exit(-1);}
    if (flg_AD && (M1->n!=M2->n || M1->m!=M2->m)){printf("matrixs shape are not match"); exit(-1);}
    
    if (!strncmp(M1->data_type, "float", 5))
    {
        gettimeofday(&start2, NULL);
        if (flg_SM) M_new=f_ScalarMuti(M1,f_scalar);
        if (flg_AD) M_new=f_Add(M1,M2);
        if (flg_MM) M_new=f_MatrixMuti(M1, M2);
        if (flg_TS) M_new=f_Transpose(M1);
        if (flg_TR) 
        {
            // float a = f_Trace(M1);
            // printf("trace=%f\n", a);
            if(M1->m==M1->n) M_new=M1;
            else {printf("matrix shape is not square");exit(-1);}
        }
        gettimeofday(&end2, NULL);
        delta2 = ((end2.tv_sec - start2.tv_sec) * 1000000u + end2.tv_usec - start2.tv_usec) / 1.e6;
        printf("operation time=%12.10f\n", delta2);
    }
    else
    {
        gettimeofday(&start2, NULL);
        if (flg_SM) M_new=ScalarMuti(M1,n_scalar);
        if (flg_AD) M_new=Add(M1,M2);
        if (flg_MM) M_new=MatrixMuti(M1, M2);
        if (flg_TS) M_new=Transpose(M1);
        if (flg_TR) 
        {
            // int a = Trace(M1);
            // printf("trace=%d\n", a);
            if (M1->m == M1->n) M_new = M1;
            else {printf("matrix shape is not square"); exit(-1);};
        }
        gettimeofday(&end2, NULL);
        delta2 = ((end2.tv_sec - start2.tv_sec) * 1000000u + end2.tv_usec - start2.tv_usec) / 1.e6;
        printf("operation time=%12.10f\n", delta2);
    }
    if(flg_L)
    {
        printf("Logging file...\n");
        outputfile(operate, filename1, filename2, M_new, delta1, delta2);
    }
        
    return 0;
}
