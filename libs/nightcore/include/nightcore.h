#ifndef _NIGHTCORE_H_
#define _NIGHTCORE_H_

#define NIGHTCORE_DEBUG

#define PITCH_DEFAULT 1.0
#define TEMPO_DEFAULT 1.0
#define BASS_BOOST_DEFAULT 0.0
#define REVERB_DELAY_MS_DEFAULT 0
#define REVERB_INTENSITY_DEFAULT 0.0 
#define REVERB_FEEDBACK_DEFAULT 0.0

#include "night_error_codes.h"
#include <gst/gst.h>

typedef struct _NightcoreData
{
    
    gfloat bass_boost_val;
    gfloat speed_val;
    gfloat pitch_val;
    guint64 reverb_delay_ms;
    gfloat reverb_intensity;
    gfloat reverb_feedback;
    //gboolean reverb_surround;
} NightcoreData;

NightcoreErrorCodes nightcore_init( NightcoreData *nightcore_data, 
                    gfloat bass_boost_val, 
                    gfloat speed_val, 
                    gfloat pitch_val,
                    guint64 reverb_delay_ms,
                    gfloat reverb_intensity,
                    gfloat reverb_feedback);

NightcoreErrorCodes nightcore_process_file(NightcoreData *nightcore_data, gchar *input_file, gchar *output_file);

NightcoreErrorCodes nightcore_process_file_to_thumbnail_video(  NightcoreData *nightcore_data, 
                                                    gchar *input_audio_file, 
                                                    gchar *input_thumbnail, 
                                                    gchar *output_file
                                                    );

//NightcoreErrorCodes nightcore_process_url(NightcoreData *nightcore_data, gchar *input_url, gchar *output_file);

NightcoreErrorCodes nightcore_process_video_to_speed_up_video(  NightcoreData *nightcore_data, 
                                                    gchar *input_video_file, 
                                                    gchar *output_video_file
                                                    );


const char * nightcore_get_error_name(NightcoreErrorCodes error_code);

#endif  