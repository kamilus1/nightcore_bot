#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

static const char * video_files_ext[] = {"mp4", "mov"};
static const char * audio_files_ext[] = {"mp3", "wav", "flac", "opus"};
static const char * input_files_ext[] = {"mp3", "flac", "wav", "mp4", "mov", "webm"};
static const char * thumbnail_files_ext[] = {"jpg", "jpeg", "png"};

int utils_file_valid_path(const char *file_name)
{
    if(file_name == NULL)
    {
        return -1;
    }
    FILE *file = fopen(file_name, "r");
    if(file == NULL)
    {
        return -1;
    }
    fclose(file);
    return 0;
}

float utils_float_mean(float array[], unsigned int n)
{
    if(n == 0)
    {
        return 0.0;
    }
    float sum = 0.0;
    for(unsigned int i = 0; i < n; i++)
    {
        sum += array[i];
    }
    return sum / n;
}
float utils_float_mediana(float array[], unsigned int n){
    if(n == 0)
    {
        return 0.0;
    }
    utils_float_sort(array, n);
    if(n % 2 == 0)
    {
        return (array[n/2 - 1] + array[n/2]) / 2.0;
    }
    return array[n/2];
}

static void utils_float_swap(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

static int utils_float_partition(float array[], int low, int high) {
    float pivot = array[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (array[j] < pivot) {
            i++;
            utils_float_swap(&array[i], &array[j]);
        }
    }
    utils_float_swap(&array[i + 1], &array[high]);
    return (i + 1);
}

static void utils_float_quicksort(float array[], int low, int high) {
    if (low < high) {
        int pivot_index = utils_float_partition(array, low, high);
        
        utils_float_quicksort(array, low, pivot_index - 1);
        utils_float_quicksort(array, pivot_index + 1, high);
    }
}

void utils_float_sort(float array[], unsigned int n){
    if (n <= 1) {
        return;
    }
    utils_float_quicksort(array, 0, (int)n - 1);
}

int utils_int_mean(int array[], unsigned int n){
    if(n == 0)
    {
        return 0;
    }
    int sum = 0;
    for(unsigned int i = 0; i < n; i++)
    {
        sum += array[i];
    }
    return sum / n;
}

int utils_int_mediana(int array[], unsigned int n)
{
    if(n == 0)
    {
        return 0;
    }
    utils_int_sort(array, n);
    if(n % 2 == 0)
    {
        return (array[n/2 - 1] + array[n/2]) / 2;
    }
    return array[n/2];
}

static void utils_int_swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static int utils_int_partition(int array[], int low, int high) {
    int pivot = array[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (array[j] < pivot) {
            i++;
            utils_int_swap(&array[i], &array[j]);
        }
    }
    utils_int_swap(&array[i + 1], &array[high]);
    return (i + 1);
}

static void utils_int_quicksort(int array[], int low, int high) {
    if (low < high) {
        int pivot_index = utils_int_partition(array, low, high);
        
        utils_int_quicksort(array, low, pivot_index - 1);
        utils_int_quicksort(array, pivot_index + 1, high);
    }
}

void utils_int_sort(int array[], unsigned int n)
{
    if (n <= 1) {
        return;
    }
    utils_int_quicksort(array, 0, (int)n - 1);
}

AudioExt utils_get_audio_extension(const char *file_name)
{
    if(file_name == NULL)
    {
        return INVALID;
    }
    for(int i = 0; i < AUDIO_EXTENSIONS_NUM; i++)
    {
        if(strstr(file_name, audio_files_ext[i]) != NULL)
        {
            return (AudioExt)i;
        }
    }
    return INVALID;
}

VideoExt utils_get_video_extension(const char *file_name)
{
    if(file_name == NULL)
    {
        return VIDEO_INVALID;
    }
    for(int i = 0; i < VIDEO_EXTENSIONS_NUM; i++)
    {
        if(strstr(file_name, video_files_ext[i]) != NULL)
        {
            return (VideoExt)i;
        }
    }
    return VIDEO_INVALID;
}

ThumbnailExt utils_get_thumbnail_extension(const char *file_name)
{
    if(file_name == NULL)
    {
        return THUMBNAIL_INVALID;
    }
    for(int i = 0; i < THUMBNAIL_EXTENSIONS_NUM; i++)
    {
        if(strstr(file_name, thumbnail_files_ext[i]) != NULL)
        {
            return (ThumbnailExt)i;
        }
    }
    return THUMBNAIL_INVALID;
}

InputExt utils_get_input_extension(const char *file_name)
{
    if(file_name == NULL)
    {
        return INPUT_INVALID;
    }
    for(int i = 0; i < INPUT_EXTENSIONS_NUM; i++)
    {
        if(strstr(file_name, input_files_ext[i]) != NULL)
        {
            return (InputExt)i;
        }
    }
    return INPUT_INVALID;
}
