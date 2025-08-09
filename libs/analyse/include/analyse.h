#ifndef _ANALYSE_H_
#define _ANALYSE_H_

#include <gst/gst.h>
#include <glib-2.0/glib.h>

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
    u_int32_t     audio_frequency;
    u_int32_t     spect_bands;
}PitchData;

typedef struct _FrequencyVolData
{
    GstElement *pipeline;
    GstElement *audio_source;
    GstElement *audio_convert;
    GstElement *audiosink;
    gdouble low_freq_start;
    gdouble medium_freq_start;
    gdouble high_freq_start;
    u_int32_t audio_frequency;
    u_int32_t spect_bands;
    
}FrequencyVolData;

int analyse_init_bpm_data(BPMData *bpm_data, BPMDataAlgo bpm_algo);

int analyse_init_pitch_data(PitchData *pitch_data, 
                            PitchDataAlgo pitch_algo, 
                            u_int32_t audio_frequency,
                            u_int32_t spect_bands);

int analyse_init_freq_vol_data(FrequencyVolData *freq_vol_data, 
                                gdouble low_freq_start, 
                                gdouble medium_freq_start, 
                                gdouble high_freq_start,
                                u_int32_t audio_frequency,
                                u_int32_t spect_bands);

float analyse_get_song_bpm(BPMData *bpm_data, gchar * song_path);


float analyse_get_song_pitch(PitchData *pitch_data, gchar * song_path);

float analyse_get_song_frequency_volume(gchar *song_path, float start_freq, float end_freq);

#endif