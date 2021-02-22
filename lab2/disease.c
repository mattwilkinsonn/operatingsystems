#include <stdio.h>

double findCases(double *casesPerDay, int days)
{
    return *casesPerDay * days;
}

int main()
{
    int cases, days;

    printf("How many cases? \n");
    scanf("%d", &cases);

    printf("How many days for cases to double? \n");
    scanf("%d", &days);

    double casesPerDay = (cases / days);

    printf("Cases in one month: %f \n", findCases(&casesPerDay, 30));

    printf("Cases in two months: %f \n", findCases(&casesPerDay, 60));

    printf("Cases in two months: %f \n", findCases(&casesPerDay, 90));
}