#include "machine_learning.h"
#include <json-glib/json-glib.h>

int ml_nightcore_data_init(MLNightcoreData *ml_nightcore_data, float origin_bpm, float output_bpm, 
                            float pitch, float output_speed, float input_bass, float output_bass, 
                            float bass_boost, bool is_reverb, 
                            uint32_t reverb_delay, float reverb_feedback, float reverb_value) 
    {
        if(ml_nightcore_data == NULL) 
        {
            g_printerr("ml_nightcore_data pointer cannot be NULL\n");
            return -1;
        }
        if(origin_bpm <= 0 || output_bpm <= 0 || pitch <= 0 || output_speed <= 0 || 
           input_bass < 0 || output_bass < 0 || bass_boost < 0 || reverb_delay < 0 ||
           reverb_feedback < 0 || reverb_value < 0) 
        {
            g_printerr("Invalid parameter values. The values should be positive\n");
            return -1;
        }
        if(reverb_delay > 5000) // Assuming reverb delay is in milliseconds
        {
            g_printerr("Reverb delay cannot be greater than 5000 ms\n");
            return -1;
        }   

                       
        ml_nightcore_data->origin_bpm = origin_bpm;
        ml_nightcore_data->output_bpm = output_bpm;
        ml_nightcore_data->pitch = pitch;
        ml_nightcore_data->output_speed = output_speed;
        ml_nightcore_data->input_bass = input_bass;
        ml_nightcore_data->output_bass = output_bass;
        ml_nightcore_data->bass_boost = bass_boost;
        ml_nightcore_data->is_reverb = is_reverb;
        ml_nightcore_data->reverb_delay = reverb_delay;
        ml_nightcore_data->reverb_feedback = reverb_feedback;
        ml_nightcore_data->reverb_value = reverb_value;
        return 0;
    
    }

    gchar* ml_nightcore_data_to_json(MLNightcoreData *ml_nightcore_data)
    {
        if(ml_nightcore_data == NULL) 
        {
            g_printerr("Invalid parameters for JSON conversion\n");
            return NULL;
        }

        JsonObject *json_object = json_object_new();
        json_object_set_double_member(json_object, "origin_bpm", ml_nightcore_data->origin_bpm);
        json_object_set_double_member(json_object, "output_bpm", ml_nightcore_data->output_bpm);
        json_object_set_double_member(json_object, "pitch", ml_nightcore_data->pitch);
        json_object_set_double_member(json_object, "output_speed", ml_nightcore_data->output_speed);
        json_object_set_double_member(json_object, "input_bass", ml_nightcore_data->input_bass);
        json_object_set_double_member(json_object, "output_bass", ml_nightcore_data->output_bass);
        json_object_set_double_member(json_object, "bass_boost", ml_nightcore_data->bass_boost);
        json_object_set_boolean_member(json_object, "is_reverb", ml_nightcore_data->is_reverb);
        json_object_set_int_member(json_object, "reverb_delay", ml_nightcore_data->reverb_delay);
        json_object_set_double_member(json_object, "reverb_feedback", ml_nightcore_data->reverb_feedback);
        json_object_set_double_member(json_object, "reverb_value", ml_nightcore_data->reverb_value);

        JsonGenerator *generator = json_generator_new();
        json_generator_set_root(generator, JSON_NODE(json_object));
        
        gchar *json_string = json_generator_to_data(generator, NULL);
        
        g_object_unref(generator);
        g_object_unref(json_object);
        return json_string;
    }

    int ml_nightcore_data_from_json(MLNightcoreData *ml_nightcore_data, const char *json_buffer)
    {
        if(ml_nightcore_data == NULL || json_buffer == NULL) 
        {
            g_printerr("Invalid parameters for JSON parsing\n");
            return -1;
        }

        JsonParser *parser = json_parser_new();
        GError *error = NULL;

        if(!json_parser_load_from_data(parser, json_buffer, -1, &error)) 
        {
            g_printerr("Error parsing JSON: %s\n", error->message);
            g_error_free(error);
            return -1;
        }

        JsonNode *root = json_parser_get_root(parser);
        JsonObject *json_object = json_node_get_object(root);

        ml_nightcore_data->origin_bpm = json_object_get_double_member(json_object, "origin_bpm");
        ml_nightcore_data->output_bpm = json_object_get_double_member(json_object, "output_bpm");
        ml_nightcore_data->pitch = json_object_get_double_member(json_object, "pitch");
        ml_nightcore_data->output_speed = json_object_get_double_member(json_object, "output_speed");
        ml_nightcore_data->input_bass = json_object_get_double_member(json_object, "input_bass");
        ml_nightcore_data->output_bass = json_object_get_double_member(json_object, "output_bass");
        ml_nightcore_data->bass_boost = json_object_get_double_member(json_object, "bass_boost");
        ml_nightcore_data->is_reverb = json_object_get_boolean_member(json_object, "is_reverb");
        ml_nightcore_data->reverb_delay = json_object_get_int_member(json_object, "reverb_delay");
        ml_nightcore_data->reverb_feedback = json_object_get_double_member(json_object, "reverb_feedback");
        ml_nightcore_data->reverb_value = json_object_get_double_member(json_object, "reverb_value");

        g_clear_object(&parser);
        return 0;
    }

    int ml_nightcore_data_json_to_file(MLNightcoreData *ml_nightcore_data, const char *file_path)
    {
        if(ml_nightcore_data == NULL || file_path == NULL) 
        {
            g_printerr("Invalid parameters for JSON file writing\n");
            return -1;
        }

        gchar *json_string = ml_nightcore_data_to_json(ml_nightcore_data);
        if(json_string == NULL) 
        {
            g_printerr("Failed to convert data to JSON string\n");
            return -1;
        }

        GError *error = NULL;
        if(!g_file_set_contents(file_path, json_string, -1, &error)) 
        {
            g_printerr("Error writing JSON to file: %s\n", error->message);
            g_error_free(error);
            g_free(json_string);
            return -1;
        }

        g_free(json_string);
        return 0;
    }
    int ml_nightcore_data_free(MLNightcoreData *ml_nightcore_data)
    {
        if(ml_nightcore_data == NULL) 
        {
            g_printerr("ml_nightcore_data pointer cannot be NULL\n");
            return -1;
        }
        // Free the structure itself if it was dynamically allocated
        free(ml_nightcore_data);
        return 0;
    }