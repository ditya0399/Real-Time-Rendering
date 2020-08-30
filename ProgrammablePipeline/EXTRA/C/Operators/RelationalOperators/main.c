#include<stdio.h>


void main()
{
    //variables 
    int a;
    int b;
    int result;

    //code
    printf("Enter one integer:");
    scanf("%d",&a);

    printf("\n\n");
    printf("Enter Another Integer: ");
    scanf("%d",&b);

    printf("\n\n");
    printf("If Answer is 0,It is FALSE.\n");
    printf("If Answer is 1,It is TRUE.\n");

    result = (a < b);
    printf("(a < b) A = %d  and B = %d  and result is %d\n",a,b,result);
    




}