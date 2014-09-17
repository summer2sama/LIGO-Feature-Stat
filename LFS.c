#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

struct svm_model* model;
struct svm_problem prob;
struct svm_node *x_space;

static char *line = NULL;
static int max_line_len;

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}
static char* readline(FILE *input)
{
	int len;
	
	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void read_problem(const char *filename)
{
	int elements, max_index, inst_max_index, i, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;

	max_line_len = 1024;
	line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
	{
		char *p = strtok(line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			++elements;
		}
		++elements;
		++prob.l;
	}
	rewind(fp);

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct svm_node *,prob.l);
	x_space = Malloc(struct svm_node,elements);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		readline(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;
		x_space[j++].index = -1;
	}
/*
	if(param.gamma == 0 && max_index > 0)
		param.gamma = 1.0/max_index;

	if(param.kernel_type == PRECOMPUTED)
		for(i=0;i<prob.l;i++)
		{
			if (prob.x[i][0].index != 0)
			{
				fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
				exit(1);
			}
			if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
			{
				fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
				exit(1);
			}
		}
*/
	fclose(fp);
}

double Dis_2(const svm_node *x, const svm_node *y)
{
			double sum = 0;
			while(x->index != -1 && y->index !=-1)
			{
				if(x->index == y->index)
				{
					double d = x->value - y->value;
					sum += d*d;
					++x;
					++y;
				}
				else
				{
					if(x->index > y->index)
					{	
						sum += y->value * y->value;
						++y;
					}
					else
					{
						sum += x->value * x->value;
						++x;
					}
				}
			}

			while(x->index != -1)
			{
				sum += x->value * x->value;
				++x;
			}

			while(y->index != -1)
			{ 
			        sum += y->value * y->value;
				++y;
			}
			
			return sum;
}
void Dis_1(const svm_node *x, const svm_node *y, double * dis_1){
			
			while(x->index != -1 && y->index !=-1)
			{
				if(x->index == y->index)
				{
					dis_1[x->index - 1] = fabs(x->value - y->value);
                                        ++x;
					++y;
				}
				else
				{
					if(x->index > y->index)
					{	
						dis_1[y->index - 1] = fabs(y->value);
						++y;
					}
					else
					{
						dis_1[x->index - 1] += fabs(x->value);
						++x;
					}
				}
			}

			while(x->index != -1)
			{
				dis_1[x->index - 1] = fabs(x->value);
				++x;
			}

			while(y->index != -1)
			{ 
			        dis_1[y->index - 1] = fabs(y->value);
				++y;
			}
			
}

int dis_min(double *dis,int num){
double dis_tmp = dis[0];
int idx_tmp = 0;
for(int i = 0; i < num; ++i){
    if(dis[i] < dis_tmp){
        dis_tmp = dis[i];
        idx_tmp = i;
    }
}
return idx_tmp;
}

int dis_max(double *dis,int num){
double dis_tmp = 0;
int idx_tmp = 0;
for(int i = 0; i < num; ++i){
    if(dis[i] > dis_tmp){
        dis_tmp = dis[i];
        idx_tmp = i;
    }
}
return idx_tmp;
}

void swap(int *a, int *b)
{
  int tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

int quicksort(int *arr, int *arr_idx, int left, int right)
{
  int i = left;
  int j = right;
  int key = arr[left];
  int key_idx = arr_idx[left];
  if (i >= j)
    return 0;
  while (i != j) {
  //accomplish one quicksort
    while (arr[j] > key) {
      j--;
    }
    swap(&arr[j], &key);
    swap(&arr_idx[j], &key_idx);
    while (arr[i] < key) {
      i++;
    }
    swap(&arr[i], &key);
    swap(&arr_idx[i], &key_idx);
  }
  quicksort(arr, arr_idx, left, i - 1); //accomplish the left remaining quicksort
  quicksort(arr, arr_idx, j + 1, right); //accomplish the right remaining quicksort
  return 1;
}


int main(int argc, char **argv)
{
model = svm_load_model(argv[1]);
read_problem(argv[2]);
prob.l = 5000;
double decval = 0;
double * decision_value = new double[model->l];
int * predict_label = new int[model->l];
int * boundary_state = new int[model->l];
memset(boundary_state, 0, sizeof(*boundary_state) * (model->l));
int num_bsv = 0;
printf("Get the Boundary Vectors\n");
for(int i = 0; i < model->l; ++i)
{
predict_label[i] = svm_predict_values(model, model->SV[i], &decval);
decision_value[i] = decval;
if(fabs(decval) < 0.2){
    boundary_state[i] = 1;
    ++ num_bsv;
}
printf("%d:%f:%d\n",i,decval,boundary_state[i]);
}
printf("num_bsv:%d\n",num_bsv);

int *index_bsv = new int[num_bsv];
memset(index_bsv, 0, sizeof(*index_bsv) * (num_bsv));
int index_bsv_count = 0;
for(int i = 0; i < model->l; ++i){
    if(boundary_state[i] == 1){
        index_bsv[index_bsv_count] = i;
        index_bsv_count ++;
    }
}

double *dis_2 = new double[num_bsv];
memset(dis_2, 0, sizeof(*dis_2) * (num_bsv));
int num_feature = 1250;
double *dis_1 = new double[num_feature];
memset(dis_1,0,sizeof(*dis_1) * (num_feature));
int *weight_feature = new int[num_feature];
memset(weight_feature,0,sizeof(*weight_feature) * (num_feature));
double threshold_dis_2 = 0;
int idx_feature_max = 0;
for(int i = 0; i < prob.l; ++i){
    //if(boundary_state[i] != 1){
        for(int j = 0; j < num_bsv; ++j){
            dis_2[j] = Dis_2(prob.x[i], model->SV[index_bsv[j]]);
        }
        int idx_bsv_min = dis_min(dis_2,num_bsv);
        printf("%d: idx_bsv_min:%d dis_min:%f \n",i,idx_bsv_min,dis_2[idx_bsv_min]);
        if(dis_2[idx_bsv_min] > threshold_dis_2){
            memset(dis_1,0,sizeof(*dis_1) * (num_feature));
            Dis_1(prob.x[i], model->SV[index_bsv[idx_bsv_min]],dis_1);
            idx_feature_max = dis_max(dis_1,num_feature);
            weight_feature[idx_feature_max] ++;
            printf("%d: idx_feature_max:%d dis_max:%f \n",i,idx_feature_max,dis_1[idx_feature_max]);
        }
       
    //}
}//Big For:prob

int *idx_feature = new int[num_feature];
for(int i = 0; i < num_feature; ++i)
    idx_feature[i] = i + 1;

for(int i = 0; i < num_feature; ++i)
    printf("%d : %d\n",idx_feature[i], weight_feature[i]);
quicksort(weight_feature,idx_feature,0,num_feature - 1);
printf("Rank the Features:\n");
for(int i = num_feature - 1; i >= 0; --i)
    printf("%d : %d\n",idx_feature[i], weight_feature[i]);
}//main
