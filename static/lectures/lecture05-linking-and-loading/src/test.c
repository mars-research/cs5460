// Type your code here, or load an example.
#include <stdio.h>

//static
int add(int a, int b) {
    printf("Numbers are added together\n");
    return a + b; 
}

int main () {
    int a, b; 
    a = 3; 
    b = 4; 

    int ret = add (a, b);

    printf("Result:%d\n", ret);
    return 0;
}
