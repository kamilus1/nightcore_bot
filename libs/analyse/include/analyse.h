#ifndef _ANALYSE_H_
#define _ANALYSE_H_

#include <gst/gst.h>

#define PROB_NUM 100

#define AUDIO_FREQUENCY 44100

typedef enum _BPMDataAlgo
{
    LAST = 0,
    MEDIUM = 1,
    MEDIANA = 2
}BPMDataAlgo;

typedef enum _PitchDataAlgo
{
    CEPSTRUM = 0,
    AUTOCORELATION = 1
}PitchDataAlgo;




typedef struct _BPMData
{
    BPMDataAlgo algo;
    GstElement *pipeline;
    GstElement *audio_source;
    GstElement *audio_convert;
    GstElement *caps_filter;
    GstElement *bpm_detector;
    GstElement *fakesink;
    gfloat bpm_data[PROB_NUM];
}BPMData; 

typedef struct _PitchData
{
    PitchDataAlgo algo;
    GstElement   *pipeline;
    GstElement   *audio_source;
    GstElement   *audio_convert;
    GstElement   *fft_spectrum;
    GstElement   *fakesink;
    guint32_t     audio_frequency;
}PitchData;

typedef struct _FrequencyVolData
{
    GstElement *pipeline;
    GstElement *audio_source;
    GstElement *audio_convert;
    GstElement *audiosink;
    guint32_t low_freq_start;
    guint32_t medium_freq_start;
    guint32_t high_freq_start;
    guint32_t audio_frequency;
}FrequencyVolData;

int analyse_init_bpm_data(BPMData *bpm_data, BPMDataAlgo bpm_algo);

int analyse_init_pitch_data(PitchData, )

float analyse_get_song_bpm(gchar * song_path);

float analyse_get_song_pitch(gchar *song_path);

float analyse_get_song_frequency_volume(gchar *song_path, float start_freq, float end_freq);

#endif