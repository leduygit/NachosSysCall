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

int temp[100];
void mergesort(int a[], int left, int right)
{
    int i, j, k, mid;
    // int temp[100];
    if (left < right)
    {
        mid = (left + right) / 2;
        mergesort(a, left, mid);
        mergesort(a, mid + 1, right);
        for (i = left; i <= right; i++)
        {
            temp[i] = a[i];
        }
        i = left;
        j = mid + 1;
        k = left;
        while (i <= mid && j <= right)
        {
            if (temp[i] < temp[j])
            {
                a[k] = temp[i];
                i++;
            }
            else
            {
                a[k] = temp[j];
                j++;
            }
            k++;
        }
        while (i <= mid)
        {
            a[k] = temp[i];
            k++;
            i++;
        }
    }
}

int main()
{
    int id, n, x, i, j, count, st;
    int a[100];
    char *s[20];
    n = ReadInt();
    // a = (int *)malloc(n * sizeof(int));

    for (i = 0; i < n; i++)
    {
        a[i] = ReadInt();
    }

    PrintString("Read array successfully\n");
    // mergesort without function
    mergesort(a, 0, n - 1);

    // write to mergesort.txt
    Create("mergesort.txt");
    id = Open("mergesort.txt", 0);
    if (id < 0)
    {
        PrintString("Failed to open file\n");
        return 1;
    }

    for (i = 0; i < n; i++)
    {
        // convert a[i] to char*
        int x = a[i] < 0 ? -a[i] : a[i];
        int count = 0;
        int st = 0;

        // Count the number of digits
        while (x != 0)
        {
            x /= 10;
            count++;
        }

        // Reset x to a[i]
        x = a[i];
        // handle negative number
        if (a[i] < 0)
        {
            s[0] = '-';
            st = 1;
        }

        // Convert each digit to characters and store them in the s array
        for (j = count - 1 + st; j >= st; j--)
        {
            s[j] = (x % 10) + '0';
            x /= 10;
        }

        // Null-terminate the string
        s[count + st] = '\0';

        // write to file
        Write(s, count + st, id);
        Write(" ", 1, id);
    }

    Close(id);
    PrintString("Write mergesort to file mergesort.txt successfully\n");
    // free(a);
}