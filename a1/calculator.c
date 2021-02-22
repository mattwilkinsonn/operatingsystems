#include <stdio.h>

int add()
{
}

int main(int argc, char **argv)
{
    char input;

    int array[100];
    int i = 0;
    size_t bufsize = 50;
    size_t characters;
    char *buffer;

    buffer = (char *)malloc(bufsize * sizeof(char));

    printf("Calculator>");
    characters = getline(&buffer, &bufsize, stdin);
    // scanf("%c", &input);
    // while (i < 100 && scanf(" %d", &array[i]) == 1)
    // {
    //     i++;
    // }

    char operation = buffer[0];

    sscanf(buffer, "%s %d %d %d", );

        printf("%s", buffer);

    printf("%c \n", operation);
}