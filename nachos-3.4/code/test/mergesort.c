#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

void swap(int *a, int *b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int temp[100];
void mergesort(int a[], int left, int right) {
    int i, j, k, mid;
    float f1, f2;
    if (left < right) {
        mid = (left + right) / 2;
        mergesort(a, left, mid);
        mergesort(a, mid + 1, right);
        for (i = left; i <= right; i++) {
            temp[i] = a[i];
        }
        i = left;
        j = mid + 1;
        k = left;
        while (i <= mid && j <= right) {
            if (CompareFloat(temp[i], temp[j]) < 0) {
                a[k] = temp[i];
                i++;
            }
            else {
                a[k] = temp[j];
                j++;
            }
            k++;
        }
        while (i <= mid) {
            a[k] = temp[i];
            k++;
            i++;
        }
    }
}

int main() {
    int id, n, i, j, count, x, decimalPosition;
    int precision;
    float f;
    int a[100];
    char s[20]; // Changed to character array
    n = ReadInt();
    for (i = 0; i < n; i++) {
        a[i] = ReadFloat();
    }

    PrintString("Read array successfully\n");

    // mergesort without function
    mergesort(a, 0, n - 1);

    // write to mergesort.txt
    Create("mergesort.txt");
    id = Open("mergesort.txt", 0);
    if (id < 0) {
        PrintString("Failed to open file\n");
        return 1;
    }

    precision = 1000;
    for (i = 0; i < n; i++) {
        f = a[i];
        f *= precision;
        x = (int)f;
        count = 0;
        decimalPosition = count; // Store the position of the decimal point

        // Count the number of digits before the decimal point
        while (x != 0) {
            x /= 10;
            count++;
        }

        x = (int)f;
        for (j = count - 1; j >= 0; j--) {
            s[j] = (x % 10) + '0';
            x /= 10;
        }

        // Insert the decimal point at the appropriate position
        if (decimalPosition == 0) {
            s[count++] = '0'; // If there are no digits before the decimal point, insert '0'
        }
        for (j = count; j > decimalPosition; j--) {
            s[j] = s[j - 1];
        }
        s[decimalPosition] = '.';

        s[count + 1] = '\0'; // Ensure the string is null-terminated

        // write to file
        for (j = 0; j <= count; j++) // Modified loop to include the decimal point
        {
            Write(&s[j], 1, id);
        }
    }

    Close(id);
    PrintString("Write mergesort to file mergesort.txt successfully\n");
}
