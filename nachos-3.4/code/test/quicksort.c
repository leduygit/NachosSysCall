#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255


void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void quicksort(int a[], int left, int right)
{
    int i, j, pivot;
    i = left, j = right;
    pivot = a[(left + right) / 2];

    while (i <= j)
    {
        while (a[i] < pivot)
            i++;
        while (a[j] > pivot)
            j--;
        if (i <= j)
        {
            swap(&a[i], &a[j]);
            i++;
            j--;
        }
    }

    if (left < j)
        quicksort(a, left, j);
    if (i < right)
        quicksort(a, i, right);
}

int main()
{
    int id, n, x, i, j, count;
    int a[100];
    char* s[20];
    n = ReadInt();
    //a = (int *)malloc(n * sizeof(int));
    
    for (i = 0; i < n; i++)
    {
        a[i] = ReadInt();
    }

    PrintString("Read array successfully\n");
    // quicksort without function
    quicksort(a, 0, n - 1);
    
    // write to quicksort.txt
    Create("quicksort.txt");
    id = Open("quicksort.txt", 0);
    if (id < 0)
    {
        PrintString("Failed to open file\n");
        return 1;
    }

    for (i = 0; i < n; i++)
    {
        PrintIntToFile(a[i], id);
        Write("\n", 1, id);
    }


    Close(id);
    PrintString("Write quicksort to file quicksort.txt successfully\n");
    //free(a);
}