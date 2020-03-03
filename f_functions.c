#include "matrix.h"
static omp_lock_t lock;
extern int Num_thread;

Clist f_ScalarMuti(Clist M, float f)
{
    OLink p;
    omp_set_num_threads(Num_thread);
#pragma omp parallel for private(p)
    for (size_t i = 0; i < M->m; i++)
    {
        p = M->row_head[i];
        while (p != NULL)
        {
//#pragma omp task firstprivate(p) //doable,but low performance
            {
                p->f_value*=f;
            }
            p = p->right;
        }
    }
    //print_crosslist(M);
    return(M);
}
float f_Trace(Clist M)
{
    OLink p;
    float trace = 0;
    omp_set_num_threads(Num_thread);
#pragma omp parallel
{
#pragma omp for private(p) reduction(+: trace)

    for (size_t i = 0; i < M->m; i++)
    {
        p = M->row_head[i];
        while (p != NULL)
        {
            //#pragma omp task firstprivate(p) untied //doable,but low performance
            {
                if (p->col == p->row)
                {
                    trace += p->f_value;
                }
            }
            p = p->right;
        }
    }
}
    //print_crosslist(M);
    return trace;
}

Clist f_Add(Clist M1,Clist M2)
{
    OLink p,q;
    int flg=20;
    if(M1->m!=M2->m||M2->n!=M1->n)
    {
        printf("error, different shape of matrixs");
        return(NULL);
    }
    omp_set_num_threads(Num_thread);
#pragma omp parallel
{
#pragma omp for private(p, q,flg)
    for (size_t i = 0; i < M1->m; i++)
    {
        // printf("%d\n",omp_get_thread_num());
        p=M1->row_head[i];
        q=M2->row_head[i];
        while (q)// let M1=M1+M2
        {
            if (p==NULL || p->col>q->col) flg=1;
            else if (p->col<q->col) flg=-1;
            else if (p->col==q->col) flg=0;

            switch (flg) 
            {
            case -1://Element (i,col) in M2 is 0, but in M1 it isn't,so just keep it. 
                p=p->right;
                break;
            case 0: //Element (i,col) is existed in M1 and M2, add them, and put the result to M1.
            {
                p->f_value+=q->f_value;
                if (!p->f_value)// If addition=0, remove the node
                {
                    M1->len--;
                    OLink s;
                    if (!M1->row_head[i]->f_value)//when pointer is row_head, connecting next node directly 
                    {
                        M1->row_head[i]=p->right;
                    }
                    else
                    {
                        for (s = M1->row_head[i]; (s->right) && (s->right->col < p->col); s = s->right)
                            continue; // get the node before the pointer
                        s->right = p->right;
                    } 
                    s=p;
                    p=p->right;
                    q=q->right;
                    free(s);   
                }
                else 
                {
                    p = p->right;
                    q = q->right;
                }
                break;
            }
            case 1: //Element (i,col) in M1 is 0, while in M2 it isn't.Therefore created a node, and connected it to M1.
            {
                OLink r; 
                r=(OLink)malloc(sizeof(OLNode));
                r->row=q->row;
                r->col=q->col;
                r->f_value=q->f_value;
                if((!M1->row_head[i])||M1->row_head[i]->col>r->col)
                {
                    r->right=M1->row_head[i];
                    M1->row_head[i]=r;
                }   
                else
                {
                    OLink s;
                    for (s = M1->row_head[i]; (s->right) && (s->right->col < r->col); s = s->right)
                    continue;
                    r->right = s->right;
                    s->right = r;
                }
                // connect the row
#pragma omp critical // following block is dependent on other threads(connecting the col)
{
                if ((!M1->col_head[r->col])|| M1->col_head[r->col]->row > r->col)
                {
                    r->down = M1->col_head[r->col];
                    M1->col_head[r->col] = r;
                }

                else
                {
                    OLink s;
                    for (s = M1->col_head[r->col]; (s->down) && (s->down->row < r->row); s = s->down)
                        continue;
                    r->down = s->down;
                    s->down = r;
                }
}
                // connect the col
                q=q->right;
                M1->len++;
                break;
            }
            default:
                printf("flg error %d",flg);
                //return(NULL);
                break;
            }
        }
        
    }
}
    //print_crosslist(M1);
    return(M1);
    
}

Clist f_Transpose(Clist M)
{
    OLink p,q,temp;
    int n_temp;
    OLink * p_temp;
    omp_set_num_threads(Num_thread);
#pragma omp parallel
{
#pragma omp for private(p,q,temp,n_temp)
    for (size_t i = 0; i < M->m; i++)
    {
        p=M->row_head[i];
        while (p)
        {
            q = p->right; 
            // The following code will change the structure of list disorderly
            // So storing the p->right firstly
#pragma omp task firstprivate(p) //doable,but low performance
{
                temp = p->down;
                p->down = p->right;
                p->right = temp;
                n_temp = p->col;
                p->col = p->row;
                p->row = n_temp;
                // exchanging the row and the col
}
            p = q;              
        }
        
    }
}
    p_temp = M->col_head;
    M->col_head = M->row_head;
    M->row_head=p_temp;
    //print_crosslist(M);
    return(M);
}




Clist f_MatrixMuti(Clist M1, Clist M2)
{
    omp_init_lock(&lock);
    OLink p=NULL,q=NULL,q1=NULL,q2=NULL;
    float f_value;
    if(M1->n!=M2->m)
    {
        printf("Type matrix error!");
        return(NULL);
    }
    Clist M3 = (Clist)malloc(sizeof(CrossList));
    sprintf(M3->data_type,"%s","float");
    M3->m=M1->m;
    M3->n=M2->n;
    M3->row_head = (OLink *)calloc((M3->m + 1) , sizeof(OLink));
    M3->col_head = (OLink *)calloc((M3->n + 1) , sizeof(OLink));
    // initialize the crosslist
    omp_set_num_threads(Num_thread);
#pragma omp parallel 
{
#pragma omp for collapse(2) private(q, p, q1, q2, f_value)
    for (size_t i = 0; i < M3->m; i++)
    {
        for (size_t j = 0; j < M3->n; j++)
        {
            f_value=0;
            p=M1->row_head[i];
            q=M2->col_head[j];
            while (p&&q)
            {
                if (p->col>q->row) q=q->down;
                else if(q->row>p->col) p=p->right;
                else
                {
                    f_value+= q->f_value*p->f_value;
                    q=q->down;
                    p=p->right;
                }
                
            }// get the value of every elements
            if (!f_value) continue;
            M3->len++;
            q=(OLink)malloc(sizeof(OLNode));
            q->row=i;
            q->col=j;
            q->f_value=f_value;
            q->down=NULL;
            q->right=NULL;
            // create the new node

#pragma omp critical
{           //Because following block is to connected the crossing list which is not independent for other threads
            if (!M3->row_head[i]) M3->row_head[i]=q;//if NULL then add the node directly
            else
            {
                //when orderly execute,codes should be:
                // q1=M3->row_head[i];
                // while (q1->right)
                // q1=q1->right;
                // q1->right=q;
                for (q1 = M3->row_head[i]; (q1->right) && (q1->right->col < q->col); q1 = q1->right)
                    continue;
                // finding the last node,and add the node to its right
                q->right = q1->right;
                q1->right = q;
            }
            //insert to row
            if (!M3->col_head[q->col])
                M3->col_head[q->col] = q;
            else
            {
                for (q2 = M3->col_head[q->col]; (q2->down) && (q2->down->row < q->row); q2 = q2->down)
                    continue;
                q->down = q2->down;
                q2->down = q;
            }
            //insert to col
}
        }
        
    }
}
    omp_destroy_lock(&lock);
    //print_crosslist(M3);
    return (M3);
    
}
