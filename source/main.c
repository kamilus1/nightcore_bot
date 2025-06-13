#include "nightcore.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <glib-2.0/glib.h>

typedef enum _modes{
    MODE_FILE_TO_FILE,
    MODE_URI_TO_FILE,
    MODE_FILE_TO_THUMBNAIL_VIDEO,
    MODE_FILE_TO_SPEEDUP_VIDEO
}MODES;


static gchar * input_file = NULL;
static gchar * output_file = NULL;
static gchar * ai_data_dir = DEFAULT_AI_DATA_DIR;
static gdouble pitch_val = PITCH_DEFAULT;
static gdouble tempo_val = TEMPO_DEFAULT;
static gdouble bass_boost_val = BASS_BOOST_DEFAULT;
static guint64 reverb_delay_ms_val = REVERB_DELAY_MS_DEFAULT;
static gdouble reverb_intensity_val = REVERB_INTENSITY_DEFAULT;
static gdouble reverb_feedback_val = REVERB_FEEDBACK_DEFAULT;
static gboolean ai_save_data = FALSE;
static gchar * input_thumbnail = NULL;
static gint mode = 0;
static gchar *config_path = "./";
static gint save_config = 0;

static GOptionEntry entries[] =
{
    {"input", 'i', 0, G_OPTION_ARG_FILENAME, &input_file, "Input file", NULL},
    {"output", 'o', 0, G_OPTION_ARG_FILENAME, &output_file, "Output file", NULL},
    {"pitch", 'p', 0, G_OPTION_ARG_DOUBLE, &pitch_val, "Value of pitch. P >= 1.0", "P"},
    {"speed", 's', 0, G_OPTION_ARG_DOUBLE, &tempo_val, "Value of speed. S >= 1.0", "S"},
    {"bass", 'b', 0, G_OPTION_ARG_DOUBLE, &bass_boost_val, "Value in dB of boost of bass frequency, 12.0 B >= 0.0", "B"},
    {"reverb_delay", 'd', 0, G_OPTION_ARG_INT64, &reverb_delay_ms_val, "Value of reverb delay in ms, 500 >  D >= 0", "D"},
    {"reverb_intensity", 'r', 0, G_OPTION_ARG_DOUBLE, &reverb_intensity_val, "Value of reverb instensity.  R >= 0.0", "R"},
    {"reverb_feedback", 'f', 0, G_OPTION_ARG_DOUBLE, &reverb_feedback_val, "Value of feedback instensity.  F >= 0.0", "F"},
    {"thumbnail", 't', 0, G_OPTION_ARG_FILENAME, &input_thumbnail, "Input thumbnail file", "T"},
    {"mode", 'm', 0, G_OPTION_ARG_INT, &mode, "Mode. 0: Standard file to file\n1: URL to file\n2: File to thumbnail video file\nFile to sped up video", "M"},
    {"ai_save", 'a', 0, G_OPTION_ARG_NONE, &ai_save_data, "Allow to save parameters", NULL},
    {"ai_dir", 0, 0, G_OPTION_ARG_STRING, &ai_data_dir, "Parameters data directory", NULL},
    G_OPTION_ENTRY_NULL
};


int main(int argc, char *argv[])
{
    /*Initialize Gstreamer*/
    gst_init(&argc, &argv);
    /*Parse arguments*/
    GError *error = NULL;
    GOptionContext *context;
    context = g_option_context_new ("Nightcore creator");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        printf ("[ERR] option parsing failed: %s\n", error->message);
        g_option_context_free(context);
        g_clear_error(&error);
        return -1;
    }

    NightcoreData *nightcore_data = malloc(sizeof(NightcoreData));
    NightcoreErrorCodes nightcore_error = SUCCESS;
    
    nightcore_error = nightcore_init(nightcore_data, bass_boost_val, tempo_val, pitch_val, reverb_delay_ms_val, reverb_intensity_val, reverb_feedback_val);
    if(nightcore_error != SUCCESS)
    {
        printf("[ERR] Error during nightcore data initialization\n");
        printf("[ERR] Error code: %d\n", nightcore_error);
        printf("[ERR] Error name: %s\n", nightcore_get_error_name(nightcore_error));
        g_option_context_free(context);
        g_clear_error(&error);
        free(nightcore_data);
        return -1;
    }
    if(ai_save_data)
    {
        printf("[LOG] Saving nightcore config to: %s\n", ai_data_dir);

    }
    switch(mode)
    {
        case(MODE_FILE_TO_FILE):
            nightcore_error = nightcore_process_file(nightcore_data, input_file, output_file);
            break;
        case(MODE_URI_TO_FILE):
            break;
        case(MODE_FILE_TO_THUMBNAIL_VIDEO):
            nightcore_error = nightcore_process_file_to_thumbnail_video(nightcore_data, input_file, input_thumbnail, output_file);  
            break;  
        case(MODE_FILE_TO_SPEEDUP_VIDEO):

            break;    
        default:
            break;
    };
    if(nightcore_error != SUCCESS)
    {
        printf("\nError during processing\n");
        printf("Error code: %d\n", nightcore_error);
        printf("Error name: %s\n", nightcore_get_error_name(nightcore_error));
        g_option_context_free(context);
        g_clear_error(&error);
        free(nightcore_data);
        return -1;
    }
    free(nightcore_data);
    return 0;
}