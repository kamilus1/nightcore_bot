#include "analyse.h"
#include <string.h>

#ifdef NIGHTCORE_DEBUG
    #define DEBUG_PRINT(X) X;
#else
    #define DEBUG_PRINT(X)
#endif

#define DEFAULT_BPM -1.0

static void merge(float array[], int left, int mid, int right);

static void merge_sort(float array[], int left, int right);

static float calculate_medium(float array[], int num);


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

static u_int32_t pitch_cerpstrum(PitchData *pitch_data);

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

int analyse_init_pitch_data(PitchData *pitch_data, 
                            PitchDataAlgo pitch_algo, 
                            u_int32_t audio_frequency,
                            u_int32_t spect_bands)
{
    if(pitch_data == NULL)
    {
        return -1;
    }
    pitch_data->algo = pitch_algo;
    pitch_data->audio_frequency = audio_frequency;
    pitch_data->spect_bands = spect_bands;
    return 0;
}
int analyse_init_freq_vol_data(FrequencyVolData *freq_vol_data, 
                                gdouble low_freq_start, 
                                gdouble medium_freq_start, 
                                gdouble high_freq_start,
                                u_int32_t audio_frequency,
                                u_int32_t spect_bands)
{
    if(freq_vol_data == NULL)
    {
        return -1;
    }
    if(low_freq_start < 0)
    {
        return -2;
    }
    if(medium_freq_start < 0)
    {
        return -2;
    }
    if(high_freq_start < 0)
    {
        return -2;
    }
    if(low_freq_start >= medium_freq_start)
    {
        return -3;
    }
    if(medium_freq_start >= high_freq_start)
    {
        return -3;
    }
    if(high_freq_start > audio_frequency)
    {
        return -3;
    }
    freq_vol_data->low_freq_start = low_freq_start;
    freq_vol_data->medium_freq_start = medium_freq_start;
    freq_vol_data->high_freq_start = high_freq_start;
    freq_vol_data->audio_frequency = audio_frequency;
    freq_vol_data->spect_bands = spect_bands;
    return 0;
}

float analyse_get_song_bpm(BPMData *bpm_data, gchar * song_path)
{
    GstBus *bus;
    GstCaps *caps;
    GstMessage *msg;
    gboolean terminate = FALSE;

    if(bpm_data == NULL)
    {
        return -1.0;
    }
    if(song_path == NULL)
    {
        g_printerr("Song path is NULL");
        return -1.0;
    }
    bpm_data->pipeline = gst_pipeline_new("BPMPipeline");
    bpm_data->audio_source = gst_element_factory_make("filesrc", "audio_file_src");
    bpm_data->audio_convert = gst_element_factory_make("audioconvert", "audio_converter");
    bpm_data->caps_filter = gst_element_factory_make("capsfilter", "caps_filter");
    bpm_data->bpm_detector = gst_element_factory_make("bpmdetect", "bpm_detector");
    bpm_data->fakesink = gst_element_factory_make("fakesink", "sink");
    if(!bpm_data->pipeline || !bpm_data->audio_source || !bpm_data->audio_convert || !bpm_data->caps_filter || 
        !bpm_data->bpm_detector || !bpm_data->fakesink)
    {
        g_printerr("ERROR: One or more element cant be created!\n");
        return -1.0;
    }
    /*bpmdetect works good only with one channel*/
    caps = gst_caps_from_string("audio/x-raw,channels=1");
    g_object_set(bpm_data->caps_filter, "caps", caps, NULL);
    g_object_set(bpm_data->audio_source, "location", song_path, NULL);

    gst_bin_add_many(GST_BIN(bpm_data->pipeline), bpm_data->audio_source, bpm_data->audio_convert, bpm_data->caps_filter, 
                        bpm_data->bpm_detector, bpm_data->fakesink, NULL);

    if(!gst_element_link_many(bpm_data->audio_source, bpm_data->audio_convert, bpm_data->caps_filter, bpm_data->bpm_detector, 
                                bpm_data->fakesink, NULL))
    {  
        g_printerr("ERROR: One or more element cant be linked!\n");
        return -1.0;
    }

    bus = gst_element_get_bus(bpm_data->pipeline);
    do
    {
        msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
            GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_TAG);
        if(msg!=NULL)
        {
            GError *err;
            gchar *debug_info;
            switch(GST_MESSAGE_TYPE(msg))
            {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error (msg, &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error (&err);
                    g_free (debug_info);
                    terminate = TRUE;
                    break;
                case GST_MESSAGE_TAG:
                    GstTagList *tag_list;
                    gst_message_parse_tag(msg, &tag_list);
                    const GValue *val = gst_tag_list_get_value_index(tag_list, "beats-per-minute", 0);
                    if(val == NULL)
                    {
                        g_printerr("[ERROR] BPM value is NULL");
                        break;
                    }
                    
                    gfloat bpm = g_value_get_float(val);
                    if(bpm > 0)
                    {
                        if(bpm_data->algo == MEDIUM || bpm_data->algo == MEDIANA)
                        {
                            bpm_data->bpm_data[0] = bpm;
                        }
                        else if(bpm_data->algo == LAST)
                        {
                            bpm_data->bpm_data[0] = bpm;
                        }
                    }
                    break;
                case GST_MESSAGE_EOS:
                    g_print ("End-Of-Stream reached.\n");
                    terminate = TRUE;
                    break;
                default:
                    break;
            }
        }
        
    } while (!terminate);
    
    
    return -1.0;
}

float analyse_get_song_pitch(PitchData *pitch_data, gchar * song_path)
{
    if(pitch_data == NULL)
    {
        return -1.0;
    }
    if(song_path == NULL)
    {
        g_printerr("Song path is NULL");
        return -1.0;
    }
    switch(pitch_data->algo)
    {
        case(CEPSTRUM):
            return pitch_cerpstrum(pitch_data);
        case(AUTOCORELATION):
            return pitch_autocorelation(pitch_data);
        default:
            g_printerr("Unknown pitch algorithm\n");
            return -1.0;
    }
    return -1.0;
}

static u_int32_t pitch_cerpstrum(PitchData *pitch_data)
{
    return 0;
}

static u_int32_t pitch_autocorelation(PitchData *pitch_data)
{
    return 0;
}


static void bpm_process_data(BPMData *bpm_data, gfloat bpm_value)
{
    
}


static float calculate_medium(float array[], int num){
    float medium = 0.0;
    for(int i = 0; i<num;i++)
    {
        medium += array[i];
    }
    return medium;
}