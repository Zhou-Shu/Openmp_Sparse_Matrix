#include "matrix.h"
extern int Num_thread;

Clist readfile(char *filename)
{
    FILE *fp;
    int flagc = 1, flagdata = 1, flagr = 1, len = 0;
    float trace = 0;
    Clist Cmatrix = (Clist)malloc(sizeof(CrossList));
    if ((fp = fopen(filename, "r")) == NULL) //open file
    {
        printf("cannot open file\n");
        exit(-1);
    }
    while (!feof(fp)) // get data type, row ,col of matrix
    {
        if (flagdata)
        {
            fscanf(fp, "%s", Cmatrix->data_type);
            flagdata--;
        }
        else if (flagr)
        {
            int *p = &Cmatrix->m;
            fscanf(fp, "%d", p);
            flagr--;
        }
        else if (flagc)
        {
            int *p = &Cmatrix->n;
            fscanf(fp, "%d", p);
            flagc--;
        }
        if (!(flagdata + flagr + flagc))
            break;
    }
    //printf("%d,%d,%s\n", Cmatrix->m, Cmatrix->n, Cmatrix->data_type);
    if (!(Cmatrix->row_head = (OLink *)calloc((Cmatrix->m + 1), sizeof(OLink))))
        exit(-1);
    if (!(Cmatrix->col_head = (OLink *)calloc((Cmatrix->m + 1), sizeof(OLink))))
        exit(-1); // initialize the Cross list of matrix
    if (!strncmp(Cmatrix->data_type, "int", 3))
    {
        // get meanful values of matrix
        int n_va;
        int *p = &n_va;
        for (size_t i = 0; i < Cmatrix->m; i++)
        {
            for (size_t j = 0; j < Cmatrix->n; j++)
            {
                if (fscanf(fp, "%d", p) == EOF)
                {
                    printf("row and col not match!");
                    break;
                }
                //printf("%d", n_va);
                if (n_va == 0)
                    continue;
                //Coordinate Format (COO)
                len++;
                OLink temp;
                if (!(temp = (OLNode *)calloc(1, sizeof(OLNode))))
                    exit(-1); //creat node
                temp->row = i;
                temp->col = j;
                temp->n_value = n_va;
                if (i == j)
                    (trace += n_va); // get trace
                if (Cmatrix->row_head[i] == NULL)
                    Cmatrix->row_head[i] = temp; //insert to row list
                else
                {
                    OLink q = Cmatrix->row_head[i];
                    while ((q->right != NULL) && (q->right->col < j))
                        q = q->right;
                    temp->right = q->right;
                    q->right = temp;
                }
                if (Cmatrix->col_head[j] == NULL)
                    Cmatrix->col_head[j] = temp; //insert to col list
                else
                {
                    OLink q = Cmatrix->col_head[j];
                    while ((q->down != NULL) && (q->down->row < i))
                        q = q->down;
                    temp->down = q->down;
                    q->down = temp;
                }
            }
            //printf("\n");
        }
        fclose(fp);
        Cmatrix->len = len;
        Cmatrix->trace = trace;
    }

    //same as above
    else if (!strncmp(Cmatrix->data_type, "float", 5))
    {
        float f_va;
        float *p = &f_va;
        for (size_t i = 0; i < Cmatrix->m; i++)
        {
            for (size_t j = 0; j < Cmatrix->n; j++)
            {
                if (fscanf(fp, "%f", p) == EOF)
                {
                    printf("row and col not match!");
                    break;
                }
                //printf("%f", f_va);
                if (f_va != 0)
                    len++;
                else
                    continue;
                //Coordinate Format (COO)
                OLink temp;
                if (!(temp = (OLNode *)calloc(1, sizeof(OLNode))))
                    exit(-1); //creat node
                temp->row = i;
                temp->col = j;
                temp->f_value = f_va;
                if (i == j)
                    (trace += f_va);
                if (Cmatrix->row_head[i] == NULL)
                    Cmatrix->row_head[i] = temp; //insert to row list
                else
                {
                    OLink q = Cmatrix->row_head[i];
                    while ((q->right != NULL) && (q->right->col < j))
                        q = q->right;
                    temp->right = q->right;
                    q->right = temp;
                }
                if (Cmatrix->col_head[j] == NULL)
                    Cmatrix->col_head[j] = temp; //insert to col list
                else
                {
                    OLink q = Cmatrix->col_head[j];
                    while ((q->down != NULL) && (q->down->row < i))
                        q = q->down;
                    temp->down = q->down;
                    q->down = temp;
                }
            }
            // printf("\n");
        }
        fclose(fp);
        Cmatrix->len = len;
        Cmatrix->trace = trace;
    }
    //print_crosslist(Cmatrix);
    return (Cmatrix);
}

void print_crosslist(Clist Cmatrix)
{
    OLink p;
    int pre_col;
    float zero=0;
    for (size_t i = 0; i < Cmatrix->m; i++)
    {
        pre_col = -1;
        p = Cmatrix->row_head[i];
        while (p != NULL)
        {
            if (!strncmp(Cmatrix->data_type, "float", 5))
            {
                for (int j = pre_col; j + 1 < p->col; j++)
                    printf("%f ", zero);
                printf("%f ", p->f_value);
            }
            else
            {
                for (int j = pre_col; j + 1 < p->col; j++)
                    printf("0 ");
                printf("%d ", p->n_value);
            }
            //printf(" (%d,%d) ",p->row,p->col);
            pre_col = p->col;
            p = p->right;
        }
        if (!strncmp(Cmatrix->data_type, "float", 5))
        {
            for (int j = pre_col; j < (Cmatrix->n - 1); j++)
                printf("%f ", zero);
        }
        else
        {
            for (int j = pre_col; j < (Cmatrix->n - 1); j++)
                printf("0 ");
        }

        printf("\n");
    }
}

void outputfile(char *operate, char *filename1, char *filename2, Clist M, double delta1, double delta2)
{

    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);
    char output_file_name[50];
    sprintf(output_file_name, "22552162_%d%02d%02d_%02d%02d_%s.out", tm_now->tm_mday,
            tm_now->tm_mon + 1, tm_now->tm_year + 1900, tm_now->tm_hour, tm_now->tm_min, operate);
    //get time and get the file's name

    FILE *f;
    f = fopen(output_file_name, "w");
    fputs(operate, f);
    if (filename2)
    {
        fputc('\n', f);
        fputs(filename1, f);
        fputc('\n', f);
        fputs(filename2, f);
    }
    else
    {
        fputc('\n', f);
        fputs(filename1, f);
    }
    fprintf(f, "\n%d", Num_thread);
    if (strcmp(operate, "tr"))
    {
        fprintf(f, "\n%s\n%d\n%d\n", M->data_type, M->m, M->n);
        OLink p;
        int pre_col;
        for (size_t i = 0; i < M->m; i++)
        {
            pre_col = -1;
            p = M->row_head[i];
            while (p != NULL)
            {
                if (!strncmp(M->data_type, "float", 5))
                {
                    for (int j = pre_col; j + 1 < p->col; j++)
                        fputs("0.000000 ",f);
                    fprintf(f, "%f ", p->f_value);
                }
                else
                {
                    for (int j = pre_col; j + 1 < p->col; j++)
                        fputs("0 ",f);
                    fprintf(f, "%d ", p->n_value);
                }
                pre_col = p->col;
                p = p->right;
            }
            if (!strncmp(M->data_type, "float", 5))
            {
                for (int j = pre_col; j < (M->n - 1); j++)
                    fputs("0.000000 ",f);
            }
            else
            {
                for (int j = pre_col; j < (M->n - 1); j++)
                    fputs("0 ",f);
            }
        }
    }
    else
    {
        if (!strncmp(M->data_type, "float", 5))
        {
            fprintf(f, "\n%f", M->trace);
        }
        else
        {
            fprintf(f, "\n%1.0f", M->trace);
        }
    }
    fprintf(f, "\n%f\n%f", delta1, delta2);
    fclose(f);
    printf("file generated\n");
}