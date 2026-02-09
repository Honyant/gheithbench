#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"

const int array_size = 8;

void merge(int arr[array_size], int temp[array_size], int left_start, int left_end, int right_end) {
    int i_left = left_start;
    int i_right = left_end + 1;
    int i = left_start;

    while (i_left <= left_end && i_right <= right_end) {
        if (arr[i_left] <= arr[i_right]) {
            temp[i++] = arr[i_left++];
        } else {
            temp[i++] = arr[i_right++];
        }
    }

    while (i_left <= left_end) {
        temp[i++] = arr[i_left++];
    }

    while (i_right <= right_end) {
        temp[i++] = arr[i_right++];
    }

    for (int j = left_start; j <= right_end; j++) {
        arr[j] = temp[j];
    }

}

void merge_sort (int arr[array_size], int temp[array_size], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        Barrier* b = new Barrier(3);

        thread([&arr, &temp, left, mid, right, b] {
            merge_sort(arr, temp, left, mid);
            b->sync();
        });

        thread([&arr, &temp, left, mid, right, b] {
            merge_sort(arr, temp, mid + 1, right);
            b->sync();
        });

        b->sync();
        
        merge(arr, temp, left, mid, right);
    }
}

/* Called by one core */
void kernelMain(void) {
    int arr[array_size] = {18, 13, 2, 4, 9, 0, -9, 100};
    int temp[array_size];

    Debug::printf("*** Beginning merge sort...\n");

    merge_sort(arr, temp, 0, array_size - 1);

    Debug::printf("*** Sorted array: ");
    for (int i = 0; i < array_size; i++) {
        Debug::printf("%d ", arr[i]);
    }
    Debug::printf("\n");

    Debug::printf("*** Merge sort completed.\n");
}

