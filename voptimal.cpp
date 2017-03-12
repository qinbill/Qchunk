//
//  voptimal.cpp
//  voptimal
//
//  Created by Yifei Lu on 14/09/11.
//  Copyright 2011 School of Computer Science & Engineering, UNSW. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "voptimal.h"

#define MIN( a, b ) ( ( (a) < (b) ) ? a : b )


static void print_array(char* head, float* arr, int size) {
    if (NULL != head) {
        printf("%s", head);
    }
    if (arr != NULL) {
        for (float* i = arr; i < arr+size; i++)   printf("%.1f ", *i);
        printf("\n");
    }
}

static void print_array(char* head, int* arr, int size) {
    if (NULL != head) {
        printf("%s", head);
    }
    if (arr != NULL) {
        for (int* i = arr; i < arr+size; i++)   printf("%3d ", *i);
        printf("\n");
    }
}

/* output:    a pointor to a voptimal structure if successfull. NULL if failed  */
v_opt_t* build_voptimal(int bucks_num, int data_num, int* len_list, int* freq_list) {
    
//    print_array("len_list[]=", len_list, data_num);                        // FIXME!!!
//    print_array("freq_list[]=", freq_list, data_num);                        // FIXME!!!

    v_opt_t* hist = new v_opt_t;
    hist->bucks_num = bucks_num;
    hist->buck_total_freq = new int[bucks_num+1];
    hist->buck_end = new int[bucks_num+1];
//    hist->buck_count = new int[bucks_num+1];
    
    // initialise the prefix sum arrays
    // the indices are right-shifted 1 to facilitate computation
    // p[i] = freq[0] + ... + freq[i-1]
    // pp[i] = freq[0]^2 + ... + freq[i-1]^2
    int* p = new int[data_num+1];
    int* pp = new int[data_num+1];
    p[0] = pp[0] = 0;
    for (int i = 1; i <= data_num; i++) {
        p[i] = p[i-1] + freq_list[i-1];
        pp[i] = pp[i-1] + freq_list[i-1]*freq_list[i-1];
    }
    
    
    // dynamic programming
    float* sse = new float[bucks_num * (data_num+1)];
    int* trace = new int[bucks_num * (data_num+1)];           // trace the optimal buckets
    memset(sse, 0, sizeof(float)*bucks_num*(data_num+1));
    memset(trace, 0, sizeof(int)*bucks_num*(data_num+1));
    
    // k=1, initialise sse[(k-1)*bucks_num...(k-1)*bucks_num+data_num]
    for (int i = 1; i <= data_num; i++) {
        // sse*(i, 1) = sse([1, i])
        float avg = (float)p[i] / i;
        sse[i] = (pp[i] - i * avg * avg);
    }
    
//    print_array("  sse[]=", sse, data_num + 1);                        // FIXME!!!
//    print_array("trace[]=", trace, data_num + 1);                        // FIXME!!!
    
    for (int k = 2; k <= bucks_num; k++) {
        int cur_row = (k-1)*(data_num+1);
        int pre_row = (k-2)*(data_num+1);
        memset(sse + cur_row, 0, sizeof(float)*k);              // sse[cur_row+0..cur_row+k-1] = 0
        memset(trace + cur_row, 0, sizeof(int)*k);              // trace[cur_row+0..cur_row+k-1] = 0
        
        sse[cur_row + k] = sse[pre_row + k-1];
        trace[cur_row + k] = k-1;
        for (int i = k+1; i <= data_num; i++) {                     // sse[cur_row+i] = min{ sse[pre_row+j] + sse([j+1, i]) }
            // j = i-1;
            sse[cur_row + i] = sse[pre_row + i-1];
            trace[cur_row + i] = i-1;
            
            // j = i-2 to 1
            for (int j = i-2; j >= 1; j--) {
                // calculate last_bucket_sse = sse([j+1, i])
                float avg = (p[i] - p[j]) * 1.0 / (i-j);
                float last_bucket_sse = ((pp[i] - pp[j]) - (i-j) * avg * avg);
                // early stop, due to the monotonicity of sse([j+1, i]) as j decreases
                if (last_bucket_sse >= sse[cur_row + i])    break;
                // newPartition = sse[k-1, j] + last_bucket_sse
                float newPartition = sse[pre_row + j] + last_bucket_sse;
                // take the minimum
                if (newPartition < sse[cur_row + i]) {
                    sse[cur_row + i] = newPartition;
                    trace[cur_row + i] = j;
                }
            }
        }
//        print_array("  sse[]=", sse + cur_row, data_num + 1);                        // FIXME!!!
//        print_array("trace[]=", trace + cur_row, data_num + 1);                        // FIXME!!!
    }

    int cur = data_num;
    int pre = 0;
    for (int k = bucks_num; k > 0; k--) {
        int cur_row = (k-1)*(data_num+1);

        hist->buck_end[k] = len_list[cur-1];

        pre = trace[cur_row + cur];
//        hist->buck_count[k] = cur - pre ;
        hist->buck_total_freq[k] = p[cur];
        
        cur = pre;
    }
    hist->buck_end[0] = len_list[0]-1;
    //
    //      |       bin_i       |        i = 1 .. bucks_num
    //    (end[i-1]            end[i]]
    //      buck_total_freq = f((end[0], end[i]))
    //
    
//    print_array("buck_end[]=", hist->buck_end, bucks_num+1);                        // FIXME!!!
//    print_array("buck_count[]=", hist->buck_count, bucks_num+1);                        // FIXME!!!
//    print_array("buck_total_freq[]=", hist->buck_total_freq, bucks_num+1);                        // FIXME!!!
    
    return hist;
}


/* Free the allocated memory of v_optimal */
int destroy_voptimal(v_opt_t *v_opt_p) {
    delete [] (v_opt_p->buck_total_freq);
    delete [] (v_opt_p->buck_end);
//    delete [] (v_opt_p->buck_count);
    delete (v_opt_p);
    return 0;
}


/* Estimate a frequence between length min and max. (Include the length of min and max) */
/* Return the sum. */
int search_range_voptimal(v_opt_t *v_opt_p, int min, int max) {
    int* end = v_opt_p->buck_end;
    int* freq = v_opt_p->buck_total_freq;
    int b = v_opt_p->bucks_num;
    
    int min_index = -1;                     // -1 means not found
    int max_index = -1;
    
    /* trim off the query range that beyond the observed range */
    // assert(min <= max)
    if (max <= end[0] || end[b] < min) {
        return 0;
    }
    if (min <= end[0]) {
        min = end[0];
        min_index = 0;
    }
    if (max > end[b]) {
        max = end[b];   
        max_index = b;
    }

    /*  binary search min starting from buck_end[1]. galloping search may be used instead of binary search */
    int l = 1; int r = b;                   // assert(b >= 1)
    int m = l;
    while (l <= r && min_index == -1) {
        m = (l+r) / 2;
        if (end[m] < min) {
            l = m+1;
        } else if (end[m] > min) {
            r = m-1;
        } else {    // list[m] == value
            min_index = m;
        }
    }
    if (min_index == -1) {
        min_index = (end[m] < min) ?
                        m+1 :   // list[l] == list[r] < value
                        m;      // value < list[l] == list[r]
    }
    
    /*  binary search max starting from buck_end[min_index]. galloping search may be used instead of binary search */
    l = min_index; r = v_opt_p->bucks_num;
    m = l;
    while (l <= r && max_index == -1) {
        m = (l+r) / 2;
        if (end[m] < max) {
            l = m+1;
        } else if (end[m] > max) {
            r = m-1;
        } else {
            max_index = m;
        }
    }
//    printf("max_index=%d, l=%d, r=%d\n", max_index, l, r);                        // FIXME!!!
    if (max_index == -1) {
        max_index = (end[m] < max) ? 
                        m+1 : 
                        m;        
    }
    
//    printf("min_index=%d\n", min_index);                        // FIXME!!!
//    printf("max_index=%d\n", max_index);                        // FIXME!!!
    
    /* count the frequency */
    /*
        end[0]    min       end[1]          end[2]      max
        ____|______v_________|________________|__________v_____|________________|_____
                          min_index                         max_index
     
            |       left part| full bucket    |right part      |                |
     */    
    float f = 0;
    
    /* full buckets */
    f += freq[max_index] - freq[min_index];
//    printf("%.2f\n", f);                        // FIXME!!!
    
    /* left partial */
    f += (min_index > 0) ? 
    (float)(end[min_index] - min + 1) / (end[min_index] - end[min_index-1]) * (freq[min_index] - freq[min_index-1]) :
    0;
//    printf("%.2f\n", f);                        // FIXME!!!

    /* right partial */
    // assert(max_index > 0)
    f -= (float)(end[max_index] - max) / (end[max_index] - end[max_index-1]) * (freq[max_index] - freq[max_index-1]);
//    printf("%.2f\n", f);                        // FIXME!!!
        
    printf("%d -- %d = %d\n", min, max, (int)f);                        // FIXME!!!
    return (int)f;
}
