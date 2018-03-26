#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sorting.h"





// This tester generate a cost array and calculate
// the dpselect and output it.
int main()
{
  int list[] = {
    12, 23, 18, 10, 11, 
    2,  3,  1,  19, 11,
    0,  90, 15, 17, 10,
    5,  3,  80, 19, 99
  };

  for (int i = 0; i < 20; i ++){
    quickSelectKOneKey(list, 0, 19, i);
    printf("%d\n", list[i]);
  }

//   quickSelectKOneKey(list, 0, 20, 8);
//   printf("%d\n", list[8]);
  
//   quickSelectKOneKey(list, 0, 20, 11);
//   printf("%d\n", list[11]);
  
  return 0;
}

