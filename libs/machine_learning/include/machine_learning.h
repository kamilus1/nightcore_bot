#ifndef _MACHINE_LEARNING_H_
#define _MACHINE_LEARNING_H_
#include <stdbool.h>
#include <stdint.h>
#include <json-glib/json-glib.h>

typedef enum _MusicGenre{
    NO_GENRE = 0,
    TRAP = 1, //Trap 
    POP_TRAP = 2, // trap subgeneres
    POP = 3,
}MusicGenre;

typedef struct _MLNightcoreData
{
    float origin_bpm;
    float output_bpm;
    float pitch;
    float output_speed;
    float input_bass;
    float output_bass;
    float bass_boost;
    bool  is_reverb;
    uint32_t reverb_delay;
    float reverb_feedback;
    float reverb_value;

}MLNightcoreData;

int ml_nightcore_data_init(MLNightcoreData *ml_nightcore_data, float origin_bpm, float output_bpm, 
                            float pitch, float output_speed, float input_bass, float output_bass, 
                            float bass_boost, bool is_reverb, 
                            uint32_t reverb_delay, float reverb_feedback, float reverb_value);

gchar* ml_nightcore_data_to_json(MLNightcoreData *ml_nightcore_data);

int ml_nightcore_data_from_json(MLNightcoreData *ml_nightcore_data, const char *json_buffer);

int ml_nightcore_data_free(MLNightcoreData *ml_nightcore_data);

int ml_nightcore_data_json_to_file(MLNightcoreData *ml_nightcore_data, const char *file_path);



#endif