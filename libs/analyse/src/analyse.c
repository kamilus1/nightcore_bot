#include "analyse.h"
#include <string.h>


#define DEFAULT_BPM -1.0

static void merge(float array[], int left, int mid, int right);

static void merge_sort(float array[], int left, int right);



static float calculate_medium(float array[], int num){
    float medium = 0.0;
    for(int i = 0; i<num;i++)
    {
        medium += array[i];
    }
    return medium;
}

static float calculate_max(float array[], int num)
{
    float min_candidate;
    if(num <= 0)
    {
        return -1;
    }
    min_candidate = array[0];
    for(int i = 1; i < num;i++)
    {
        if(array[i] < min_candidate)
        {
            min_candidate = array[i];
        }
    }
    return min_candidate;
}

static float calculate_min(float array[], int num)
{
    float min_candidate;
    if(num <= 0)
    {
        return -1;
    }
    min_candidate = array[0];
    for(int i = 1; i < num;i++)
    {
        if(array[i] < min_candidate)
        {
            min_candidate = array[i];
        }
    }
    return min_candidate;
}

static float calculate_mediana(float array[], int num, int already_sorted){
    if(already_sorted == 0)
    {
        merge_sort(array, 0, (num - 1));
    }
    if(num == 0)
    {
        return -1.0;
    }
    if(num == 1)
    {
        return array[num];
    }
    if(num % 2)
    {
        return array[(num/2)];
    }
    else
    {
        return (array[num/2] + array[num/2 - 1]) / 2;
    }
}

static void merge(float array[], int left, int mid, int right)
{
    int n1 = mid - left + 1; // Size of the left subarray
    int n2 = right - mid;    // Size of the right subarray

    // Temporary arrays to hold the subarrays
    int* leftArray = (int*)malloc(n1 * sizeof(int));
    int* rightArray = (int*)malloc(n2 * sizeof(int));

    // Copy data to temporary arrays
    for (int i = 0; i < n1; i++)
        leftArray[i] = array[left + i];
    for (int j = 0; j < n2; j++)
        rightArray[j] = array[mid + 1 + j];

    // Merge the temporary arrays back into the original array
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArray[i] <= rightArray[j]) {
            array[k] = leftArray[i];
            i++;
        } else {
            array[k] = rightArray[j];
            j++;
        }
        k++;
    }

    // Copy any remaining elements of leftArray
    while (i < n1) {
        array[k] = leftArray[i];
        i++;
        k++;
    }

    // Copy any remaining elements of rightArray
    while (j < n2) {
        array[k] = rightArray[j];
        j++;
        k++;
    }

    // Free the temporary arrays
    free(leftArray);
    free(rightArray);
}

static void merge_sort(float array[], int left, int right)
{    
    if(left < right){
        int mid = (left + (left - right) + 1) / 2;
        merge_sort(array, left, mid);
        merge_sort(array, mid + 1, right);

        merge(array, left, mid, right);
    }
}

static u_int32_t pitch_cerpstrum();

static u_int32_t pitch_autocorelation(PitchData *pitch_data);

int analyse_init_bpm_data(BPMData *bpm_data, BPMDataAlgo bpm_algo)
{
    if(bpm_data == NULL)
    {
        return -1;
    }
    bpm_data->algo = bpm_algo;
    return 0;
}


float analyse_get_song_bpm(gchar * song_path)
{
    
    return -1.0;
}