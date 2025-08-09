#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>

#define AUDIO_EXTENSIONS_NUM 3
#define THUMBNAIL_EXTENSIONS_NUM 3
#define VIDEO_EXTENSIONS_NUM 2
#define INPUT_EXTENSIONS_NUM 6

typedef enum _AudioExt
{
    MP3,
    FLAC,
    WAV,
    INVALID
}AudioExt;

typedef enum _VideoExt
{
    V_MP4,
    V_MOV,
    V_INVALID
}VideoExt;

typedef enum _ThumbnailExt
{
    JPG,
    JPEG,
    PNG,
    T_INVALID
}ThumbnailExt;

typedef enum _InputExt
{
    I_MP3,
    I_FLAC,
    I_WAV, 
    I_MP4,
    I_MOV, 
    I_WEBM,
    I_INVALID
}InputExt;

int utils_file_valid_path(const char *file_name);

float utils_float_mean(float array[], unsigned int n);

float utils_float_mediana(float array[], unsigned int n);

void utils_float_sort(float array[], unsigned int n);

int utils_int_mean(int array[], unsigned int n);

int utils_int_mediana(int array[], unsigned int n);

void utils_int_sort(int array[], unsigned int n);

AudioExt utils_get_audio_extension(const char *file_name);

VideoExt utils_get_video_extension(const char *file_name);

ThumbnailExt utils_get_thumbnail_extension(const char *file_name);

InputExt utils_get_input_extension(const char *file_name);




#endif