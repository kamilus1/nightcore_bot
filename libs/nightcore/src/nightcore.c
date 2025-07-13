#include "nightcore.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define MS_TO_NS 1000000
#define AUDIO_EXTENSIONS_NUM 6
#define THUMBNAIL_EXTENSIONS_NUM 3
#define VIDEO_EXTENSIONS_NUM 2
#define REVERB_DELAY_MAX_MS 500
//aac
static const char * audio_files_ext[] = {"mp3", "flac", "wav", "mp4", "mov", "webm"};
static const char * video_files_ext = {"mp4", "mov"};
static const char * thumbnail_files_ext[] = {"jpg", "jpeg", "png"};
static const char * night_error_names[] = { "Success", 
                                            "Invalid input file path",
                                            "Invalid output file path",
                                            "Invalid input extension",
                                            "Invalid output extension",
                                            "Unable to create all GstElements",
                                            "Unable to link all elements",
                                            "Null pointer",
                                            "Invalid value range",
                                            "Cant start pipeline to play",
                                            "Invalid Thumbnail extension"};

typedef enum _AudioExt
{
    MP3,
    FLAC,
    WAV,
    MP4,
    MOV,
    WEBM,
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

typedef struct _NightcorePipeline
{
    GstElement *pipeline;
    GstElement *audio_src;
    GstElement *audio_src_dec;
    GstElement *pitch;
    GstElement *bass_boost;
    GstElement *reverb;
    GstElement *audio_convert;
    GstElement *audio_flac_convert;
    GstElement *audio_resample;
    GstElement *audio_sink_enc;
    GstElement *audio_sink;
}NightcorePipeline;

typedef struct _NightcoreThumbnailPipeline
{
    GstElement *pipeline;
    GstElement *audio_src;
    GstElement *audio_src_dec;
    GstElement *pitch;
    GstElement *bass_boost;
    GstElement *reverb;
    GstElement *audio_convert;
    
    GstElement *audio_resample;
    GstElement *audio_sink_enc;
    GstElement *audio_queue;
    /*Video elements*/
    GstElement *image_src;
    GstElement *image_src_dec;
    GstElement *image_src_enc;
    GstElement *image_convert;
    GstElement *image_freeze;
    GstElement *video_queue;
    /**/
    GstElement *muxer_mp4;
    GstElement *file_sink;
}NightcoreThumbnailPipeline;


typedef struct _NightcoreVideoSpeedUpPipeline
{
    GstElement *pipeline;
    GstElement *video_src;
    GstElement *demux;

    GstElement *audio_queue;
    GstElement *audio_src;
    GstElement *audio_src_dec;
    GstElement *pitch;
    GstElement *bass_boost;
    GstElement *reverb;
    GstElement *audio_convert;
    GstElement *audio_flac_convert;
    GstElement *audio_resample;
    GstElement *audio_sink_enc;
    GstElement *audio_sink;

    GstElement *video_queue;

    GstElement *file_sink;
}NightcoreVideoSpeedUpPipeline;


static AudioExt get_audio_extension(const gchar *file_name);

static VideoExt get_video_extension(const gchar *file_name);

static ThumbnailExt get_thumbnail_extension(const gchar *file_name);

static void pad_added_handler(GstElement *src, GstPad *new_pad, NightcorePipeline *data);

static void pad_thumbnail_added_handler(GstElement *src, GstPad *new_pad, NightcoreThumbnailPipeline *pipeline);


static void
link_to_multiplexer (GstPad * tolink_pad, GstElement * mux);

static int file_valid_path(const char *file_name);

#ifdef NIGHTCORE_DEBUG
    #define DEBUG_PRINT(X) X;
#else
    #define DEBUG_PRINT(X)
#endif


NightcoreErrorCodes nightcore_init( NightcoreData *nightcore_data, 
                    gfloat bass_boost_val, 
                    gfloat speed_val, 
                    gfloat pitch_val,
                    guint64 reverb_delay_ms,
                    gfloat reverb_intensity,
                    gfloat reverb_feedback)
{
    
    if(nightcore_data == NULL)
    {
        DEBUG_PRINT(g_print("Pointer to nightcore data cant be null. It must be allocated before"))
        return ERROR_NULL_POINTER;
    }

    if(bass_boost_val < 0.0)
    {
        DEBUG_PRINT(g_print("Value should be highert than 1.0"))
        return ERROR_INVALID_VALUE_RANGE;
    }
    if(speed_val < 1.0)
    {
        DEBUG_PRINT(g_print("Value should be higher than 1.0"))
        return ERROR_INVALID_VALUE_RANGE;
    }
    if(pitch_val < 1.0)
    {
        
        DEBUG_PRINT(g_print("Value should be higher than 1.0"))
        return ERROR_INVALID_VALUE_RANGE;           
    }    
    if(reverb_delay_ms > REVERB_DELAY_MAX_MS)
    {
        DEBUG_PRINT(g_print("Value should be between 0 and 500"));
        return ERROR_INVALID_VALUE_RANGE;
    }
    if(reverb_intensity < 0.0)
    {
        DEBUG_PRINT(g_print("Value should be higher than 0.0"))
        return ERROR_INVALID_VALUE_RANGE;
    }
    if(reverb_feedback < 0.0)
    {
        DEBUG_PRINT(g_print("Value should be higher than 0.0"));
        return ERROR_INVALID_VALUE_RANGE;
    }

    nightcore_data->bass_boost_val = bass_boost_val;
    nightcore_data->pitch_val = pitch_val;
    nightcore_data->speed_val = speed_val;
    nightcore_data->reverb_delay_ms = reverb_delay_ms;
    nightcore_data->reverb_intensity = reverb_intensity;
    nightcore_data->reverb_feedback = reverb_feedback;
    return SUCCESS;
}

NightcoreErrorCodes nightcore_process_file(NightcoreData *nightcore_data, gchar *input_file, gchar *output_file)
{
    AudioExt input_extension, output_extension;
    NightcorePipeline nightcore_pipeline;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    gboolean terminate = FALSE;
    if(input_file == NULL)
    {
        DEBUG_PRINT(g_printerr("Input file is null"))
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(output_file == NULL)
    {
        DEBUG_PRINT(g_printerr("Output file is null"))
        return ERROR_INVALID_OUTPUT_FILE_PATH;
    }

    if(access((const char *)input_file, F_OK) != 0)
    {
        DEBUG_PRINT(g_printerr("Cant access input file."))
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(file_valid_path((const char*)output_file) != 0)
    {
        return ERROR_INVALID_OUTPUT_FILE_PATH;
    }

    
    
    output_extension = get_audio_extension(output_file);
    if(output_extension == INVALID || output_extension == MP4)
    {
        DEBUG_PRINT(g_printerr("Invalid output extension"))
        return ERROR_INVALID_OUTPUT_EXTENSION;
    }
    /*Create pipeline*/
    nightcore_pipeline.pipeline = gst_pipeline_new("nightcore_pipeline");
    /*Create processing elements*/
    nightcore_pipeline.audio_src = gst_element_factory_make("filesrc", "file_src");
    nightcore_pipeline.audio_src_dec = gst_element_factory_make("decodebin", "source_decoder");
    nightcore_pipeline.audio_convert = gst_element_factory_make("audioconvert", "audio_converter");
    nightcore_pipeline.audio_flac_convert = gst_element_factory_make("audioconvert", "audio_flac_converter");
    nightcore_pipeline.audio_resample = gst_element_factory_make("audioresample", "audio_resampler");
    nightcore_pipeline.pitch = gst_element_factory_make("pitch", "nightcore_pitch");
    nightcore_pipeline.bass_boost = gst_element_factory_make("equalizer-10bands", "equalizer_bass_boost");
    nightcore_pipeline.reverb = gst_element_factory_make("audioecho", "reverb");
    nightcore_pipeline.audio_sink = gst_element_factory_make("filesink", "output_sink");
    if(output_extension == MP3)
    {
        nightcore_pipeline.audio_sink_enc = gst_element_factory_make("lamemp3enc", "mp3_encoder");
    }
    else if(output_extension == FLAC)
    {
        nightcore_pipeline.audio_sink_enc = gst_element_factory_make("flacenc", "flac_encoder");
    }
    else
    {
        nightcore_pipeline.audio_sink_enc = gst_element_factory_make("wavenc", "wav_encoder");
    }
    if( !nightcore_pipeline.pipeline || !nightcore_pipeline.audio_src || 
        !nightcore_pipeline.audio_src_dec || !nightcore_pipeline.audio_convert || 
        !nightcore_pipeline.audio_flac_convert ||
        !nightcore_pipeline.audio_resample || !nightcore_pipeline.pitch || 
        !nightcore_pipeline.bass_boost || !nightcore_pipeline.audio_sink || 
        !nightcore_pipeline.audio_sink_enc)
    {
        return ERROR_CANT_CREATE_ALL_ELEMENTS;
    }

    gst_bin_add_many(GST_BIN(nightcore_pipeline.pipeline), nightcore_pipeline.audio_src, 
                    nightcore_pipeline.audio_src_dec, nightcore_pipeline.audio_convert, nightcore_pipeline.audio_flac_convert,
                    nightcore_pipeline.audio_resample, nightcore_pipeline.pitch, nightcore_pipeline.reverb,
                    nightcore_pipeline.bass_boost, nightcore_pipeline.audio_sink_enc, 
                    nightcore_pipeline.audio_sink, NULL);
    if(!gst_element_link(nightcore_pipeline.audio_src, nightcore_pipeline.audio_src_dec))
    {
        DEBUG_PRINT(g_printerr("Cannot link elements starting from audio source"))
        return ERROR_CANT_LINK_ALL_ELEMENTS;
    }
    if(output_extension == WAV)
    {
        if(!gst_element_link_many(nightcore_pipeline.audio_convert, nightcore_pipeline.audio_resample, 
                             nightcore_pipeline.pitch, nightcore_pipeline.reverb, nightcore_pipeline.bass_boost, 
                             nightcore_pipeline.audio_sink_enc, nightcore_pipeline.audio_sink, NULL))
        {
            DEBUG_PRINT(g_printerr("Cannot link elements starting from audio convert"))
            return ERROR_CANT_LINK_ALL_ELEMENTS;
        }
    }
    if(output_extension == FLAC)
    {
      if(!gst_element_link_many(nightcore_pipeline.audio_convert, nightcore_pipeline.audio_resample, 
                             nightcore_pipeline.pitch, nightcore_pipeline.reverb, nightcore_pipeline.bass_boost, nightcore_pipeline.audio_flac_convert,
                             nightcore_pipeline.audio_sink_enc, nightcore_pipeline.audio_sink, NULL))
        {
            DEBUG_PRINT(g_printerr("Cannot link elements starting from audio convert"))
            return ERROR_CANT_LINK_ALL_ELEMENTS;
        }  
    }

    /**setting elements parameterss */
    g_object_set(nightcore_pipeline.audio_src, "location", input_file, NULL);
    g_object_set(nightcore_pipeline.pitch, "pitch", nightcore_data->pitch_val, "tempo", nightcore_data->speed_val, NULL);        
    g_object_set(nightcore_pipeline.bass_boost, "band1", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.bass_boost, "band2", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.bass_boost, "band3", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.bass_boost, "band4", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.reverb, "delay", (nightcore_data->reverb_delay_ms*MS_TO_NS), NULL);
    g_object_set(nightcore_pipeline.reverb, "intensity", (nightcore_data->reverb_intensity), NULL);
    g_object_set(nightcore_pipeline.reverb, "feedback", (nightcore_data->reverb_feedback), NULL);
    g_object_set(nightcore_pipeline.audio_sink, "location", output_file, NULL);
    /** Connect to the pad-added signal  */
    g_signal_connect(nightcore_pipeline.audio_src_dec, "pad-added", G_CALLBACK(pad_added_handler), &nightcore_pipeline);

    /* Start playing */
    ret = gst_element_set_state (nightcore_pipeline.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        DEBUG_PRINT( g_printerr("Unable to set the pipeline to the playing state.\n"))
        gst_object_unref (nightcore_pipeline.pipeline);
        return ERROR_CANT_SET_PIPELINE_PLAYING;
    }

    bus = gst_element_get_bus (nightcore_pipeline.pipeline);
    do {
        msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
            GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

        /* Parse message */
        if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error (msg, &err, &debug_info);
                g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error (&err);
                g_free (debug_info);
                terminate = TRUE;
                break;
            case GST_MESSAGE_EOS:
                g_print ("End-Of-Stream reached.\n");
                terminate = TRUE;
                break;
            case GST_MESSAGE_STATE_CHANGED:
                /* We are only interested in state-changed messages from the pipeline */
                if (GST_MESSAGE_SRC (msg) == GST_OBJECT (nightcore_pipeline.pipeline)) {
                    GstState old_state, new_state, pending_state;
                    gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                    g_print ("Pipeline state changed from %s to %s:\n",
                        gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                }
                break;
            default:
                /* We should not reach here */
                g_printerr("Get message of name %s\n", GST_MESSAGE_TYPE_NAME(msg));
                g_printerr ("Unexpected message received.\n");
                break;
        }
        gst_message_unref (msg);
        }
    } while (!terminate);



    gst_object_unref(bus);
    gst_element_set_state(nightcore_pipeline.pipeline, GST_STATE_NULL);
    gst_object_unref(nightcore_pipeline.pipeline);
    return SUCCESS;
}


NightcoreErrorCodes nightcore_process_file_to_thumbnail_video(  NightcoreData *nightcore_data, 
                                                    gchar *input_audio_file, 
                                                    gchar *input_thumbnail, 
                                                    gchar *output_file
                                                    )
{
    AudioExt input_audio_extension, output_extension;
    ThumbnailExt thumbnail_extension;
    NightcoreThumbnailPipeline nightcore_pipeline;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    GstPad *mux_audio_pad, *mux_video_pad;
    GstPad *queue_audio_pad, *queue_video_pad;
    gboolean terminate = FALSE;
    if(input_audio_file == NULL)
    {
        DEBUG_PRINT(g_printerr("Input "))
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(input_thumbnail == NULL)
    {
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(output_file == NULL)
    {
        return ERROR_INVALID_OUTPUT_FILE_PATH;
    }

    if(access((const char *)input_audio_file, F_OK) != 0)
    {
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(access((const char *) input_thumbnail, F_OK) != 0)
    {
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(file_valid_path((const char*)output_file) != 0)
    {
        return ERROR_INVALID_OUTPUT_FILE_PATH;
    }
    input_audio_extension = get_audio_extension((const gchar*)input_audio_file);
    if(input_audio_extension == INVALID)
    {
        return ERROR_INVALID_INPUT_EXTENSION;
    }
    output_extension = get_video_extension((const gchar *)output_file);
    if(output_extension != V_MOV)
    {
        return ERROR_INVALID_OUTPUT_EXTENSION;
    }
    thumbnail_extension = get_thumbnail_extension((const gchar *)input_thumbnail);
    if(thumbnail_extension == T_INVALID)
    {
        return ERROR_INVALID_THUMBNAIL_EXTENSION;
    }
    nightcore_pipeline.pipeline = gst_pipeline_new("nightcore_pipeline");
    /*Create processing elements*/
    nightcore_pipeline.audio_src = gst_element_factory_make("filesrc", "audio_file_src");
    nightcore_pipeline.audio_src_dec = gst_element_factory_make("decodebin", "source_decoder");
    nightcore_pipeline.audio_convert = gst_element_factory_make("audioconvert", "audio_converter");
    nightcore_pipeline.audio_resample = gst_element_factory_make("audioresample", "audio_resampler");
    nightcore_pipeline.pitch = gst_element_factory_make("pitch", "nightcore_pitch");
    nightcore_pipeline.bass_boost = gst_element_factory_make("equalizer-10bands", "equalizer_bass_boost");
    nightcore_pipeline.reverb = gst_element_factory_make("audioecho", "reverb");
    nightcore_pipeline.audio_sink_enc = gst_element_factory_make("audioconvert", "audio_enc_convert");

    nightcore_pipeline.audio_queue = gst_element_factory_make("queue", "audioqueue");


    nightcore_pipeline.image_src = gst_element_factory_make("filesrc", "image_file_src");
    nightcore_pipeline.video_queue = gst_element_factory_make("queue", "videoqueue");
    nightcore_pipeline.image_src_enc = gst_element_factory_make("videoconvert", "image_enc");
    nightcore_pipeline.image_convert = gst_element_factory_make("videoconvert", "image_converter");
    if(thumbnail_extension == JPEG || thumbnail_extension == JPG)
    {
        nightcore_pipeline.image_src_dec = gst_element_factory_make("jpegdec", "image_dec");
        
    }
    else if(thumbnail_extension == PNG)
    {
        nightcore_pipeline.image_src_dec = gst_element_factory_make("pngdec", "image_dec");
    }
    nightcore_pipeline.image_freeze = gst_element_factory_make("imagefreeze", "img_freezer");

    nightcore_pipeline.muxer_mp4 = gst_element_factory_make("qtmux", "muxer");
    nightcore_pipeline.file_sink = gst_element_factory_make("filesink", "mov_file_sink");
    if( !nightcore_pipeline.pipeline || !nightcore_pipeline.audio_src || 
        !nightcore_pipeline.audio_src_dec || !nightcore_pipeline.audio_convert || 
        !nightcore_pipeline.audio_resample || !nightcore_pipeline.pitch || 
        !nightcore_pipeline.bass_boost || !nightcore_pipeline.audio_sink_enc ||
        !nightcore_pipeline.audio_queue || 
        !nightcore_pipeline.image_src || !nightcore_pipeline.image_src_dec || 
        !nightcore_pipeline.image_src_enc || !nightcore_pipeline.video_queue || !nightcore_pipeline.image_freeze ||
        !nightcore_pipeline.muxer_mp4 || !nightcore_pipeline.file_sink || !nightcore_pipeline.image_convert)
    {
        return ERROR_CANT_CREATE_ALL_ELEMENTS;
    }
    gst_bin_add_many(GST_BIN(nightcore_pipeline.pipeline), nightcore_pipeline.audio_src, 
                    nightcore_pipeline.audio_src_dec, nightcore_pipeline.audio_convert, 
                    nightcore_pipeline.audio_resample, nightcore_pipeline.pitch, nightcore_pipeline.reverb,
                    nightcore_pipeline.bass_boost, nightcore_pipeline.audio_sink_enc,
                    nightcore_pipeline.audio_queue, 
                    nightcore_pipeline.image_src, nightcore_pipeline.image_src_dec, 
                    nightcore_pipeline.image_src_enc, nightcore_pipeline.image_freeze,
                    nightcore_pipeline.video_queue, 
                    nightcore_pipeline.muxer_mp4, nightcore_pipeline.file_sink, 
                    NULL);
    if(!gst_element_link(nightcore_pipeline.audio_src, nightcore_pipeline.audio_src_dec))
    {
        DEBUG_PRINT(g_printerr("Cannot link elements starting from audio source"))
        return ERROR_CANT_LINK_ALL_ELEMENTS;
    }
    if(!gst_element_link_many(nightcore_pipeline.audio_convert, nightcore_pipeline.audio_resample, 
                             nightcore_pipeline.pitch, nightcore_pipeline.reverb, nightcore_pipeline.bass_boost, 
                             nightcore_pipeline.audio_sink_enc,
                             nightcore_pipeline.audio_queue, NULL))
    {
        DEBUG_PRINT(g_printerr("Cannot link elements starting from audio convert"))
        return ERROR_CANT_LINK_ALL_ELEMENTS;
    }
    if(!gst_element_link_many(nightcore_pipeline.image_src, nightcore_pipeline.image_src_dec, nightcore_pipeline.image_src_enc,
                            nightcore_pipeline.image_freeze,
                            nightcore_pipeline.video_queue, NULL))
    {
        DEBUG_PRINT(g_printerr("Cannot link elements starting from image source"))
        return ERROR_CANT_LINK_ALL_ELEMENTS;
    }
    queue_audio_pad = gst_element_get_static_pad(nightcore_pipeline.audio_queue, "src");
    link_to_multiplexer(queue_audio_pad, nightcore_pipeline.muxer_mp4);

    queue_video_pad = gst_element_get_static_pad(nightcore_pipeline.video_queue, "src");
    link_to_multiplexer(queue_video_pad, nightcore_pipeline.muxer_mp4);
    
    gst_object_unref (queue_audio_pad);
    gst_object_unref (queue_video_pad);
    if(!gst_element_link_many(nightcore_pipeline.muxer_mp4, nightcore_pipeline.image_convert ,nightcore_pipeline.file_sink))
    {
        DEBUG_PRINT(g_printerr("Cannot link muxer mp4 to file sink"))
        return ERROR_CANT_LINK_ALL_ELEMENTS;
    }
    /**setting elements parameterss */
    g_object_set(nightcore_pipeline.audio_src, "location", input_audio_file, NULL);
    g_object_set(nightcore_pipeline.pitch, "pitch", nightcore_data->pitch_val, "tempo", nightcore_data->speed_val, NULL);        
    g_object_set(nightcore_pipeline.bass_boost, "band1", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.bass_boost, "band2", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.bass_boost, "band3", nightcore_data->bass_boost_val, NULL);
    g_object_set(nightcore_pipeline.reverb, "delay", (nightcore_data->reverb_delay_ms*MS_TO_NS), NULL);
    g_object_set(nightcore_pipeline.reverb, "intensity", (nightcore_data->reverb_intensity), NULL);
    g_object_set(nightcore_pipeline.reverb, "feedback", (nightcore_data->reverb_feedback), NULL);

    g_object_set(nightcore_pipeline.image_src, "location", input_thumbnail, NULL);
    g_object_set(nightcore_pipeline.image_freeze, "num-buffers", 2000, NULL);

    g_object_set(nightcore_pipeline.file_sink, "location", output_file, NULL);
    /*Connect signale*/
    g_signal_connect(nightcore_pipeline.audio_src_dec, "pad-added", G_CALLBACK(pad_thumbnail_added_handler), &nightcore_pipeline);
    /* Start playing */
    ret = gst_element_set_state (nightcore_pipeline.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        DEBUG_PRINT( g_printerr("Unable to set the pipeline to the playing state.\n"))
        gst_object_unref (nightcore_pipeline.pipeline);
        return ERROR_CANT_SET_PIPELINE_PLAYING;
    }

    bus = gst_element_get_bus (nightcore_pipeline.pipeline);
    do {
        msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
            GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

        /* Parse message */
        if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error (msg, &err, &debug_info);
                g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error (&err);
                g_free (debug_info);
                terminate = TRUE;
                break;
            case GST_MESSAGE_EOS:
                g_print ("End-Of-Stream reached.\n");
                terminate = TRUE;
                break;
            case GST_MESSAGE_STATE_CHANGED:
                /* We are only interested in state-changed messages from the pipeline */
                if (GST_MESSAGE_SRC (msg) == GST_OBJECT (nightcore_pipeline.pipeline)) {
                    GstState old_state, new_state, pending_state;
                    gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                    g_print ("Pipeline state changed from %s to %s:\n",
                        gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                }
                break;
            default:
                /* We should not reach here */
                g_printerr("Get message of name %s\n", GST_MESSAGE_TYPE_NAME(msg));
                g_printerr ("Unexpected message received.\n");
                break;
        }
        gst_message_unref (msg);
        }
    } while (!terminate);



    gst_object_unref(bus);
    gst_element_set_state(nightcore_pipeline.pipeline, GST_STATE_NULL);
    gst_object_unref(nightcore_pipeline.pipeline);

    return SUCCESS;
}

NightcoreErrorCodes nightcore_process_video_to_speed_up_video(  NightcoreData *nightcore_data, 
                                                    gchar *input_video_file, 
                                                    gchar *output_video_file
                                                    )
{
    AudioExt input_extension, output_extension;
    NightcoreVideoSpeedUpPipeline nightcore_pipeline;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    gboolean terminate = FALSE;
    if(input_video_file == NULL)
    {
        DEBUG_PRINT(g_printerr("Input file is null"))
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(output_video_file == NULL)
    {
        DEBUG_PRINT(g_printerr("Output file is null"))
        return ERROR_INVALID_OUTPUT_FILE_PATH;
    }

    if(access((const char *)input_video_file, F_OK) != 0)
    {
        DEBUG_PRINT(g_printerr("Cant access input file."))
        return ERROR_INVALID_INPUT_FILE_PATH;
    }
    if(file_valid_path((const char*)output_video_file) != 0)
    {
        DEBUG_PRINT(g_printerr("Cant access input file."))
        return ERROR_INVALID_OUTPUT_FILE_PATH;
    }

    input_extension = get_audio_extension(input_video_file);
    if(input_extension == INVALID || input_extension != MP4 || input_extension != MOV || input_extension != WEBM)
    {
        DEBUG_PRINT(g_printerr("Invalid input extension"))
        return ERROR_INVALID_INPUT_EXTENSION;
    }

    output_extension = get_audio_extension(output_video_file);
    if(output_extension == INVALID || output_extension == MP4)
    {
        DEBUG_PRINT(g_printerr("Invalid output extension"))
        return ERROR_INVALID_OUTPUT_EXTENSION;
    }
    /*Create pipeline*/
    nightcore_pipeline.pipeline = gst_pipeline_new("nightcore_pipeline");
    /*Create processing elements*/
    nightcore_pipeline.audio_src = gst_element_factory_make("filesrc", "file_src");
    nightcore_pipeline.audio_src_dec = gst_element_factory_make("decodebin", "source_decoder");
    nightcore_pipeline.audio_convert = gst_element_factory_make("audioconvert", "audio_converter");
    nightcore_pipeline.audio_flac_convert = gst_element_factory_make("audioconvert", "audio_flac_converter");
    nightcore_pipeline.audio_resample = gst_element_factory_make("audioresample", "audio_resampler");
    nightcore_pipeline.pitch = gst_element_factory_make("pitch", "nightcore_pitch");
    nightcore_pipeline.bass_boost = gst_element_factory_make("equalizer-10bands", "equalizer_bass_boost");
    nightcore_pipeline.reverb = gst_element_factory_make("audioecho", "reverb");
    nightcore_pipeline.audio_sink = gst_element_factory_make("filesink", "output_sink");
    gst_object_unref(bus);
    gst_element_set_state(nightcore_pipeline.pipeline, GST_STATE_NULL);
    gst_object_unref(nightcore_pipeline.pipeline);
    return SUCCESS;
}


const char * nightcore_get_error_name(NightcoreErrorCodes error_code)
{
    return night_error_names[error_code];
}

static AudioExt get_audio_extension(const gchar *file_name)
{
    gchar *file_name_copy = g_strdup(file_name);
    gchar *token = (gchar *)strtok((char *)file_name_copy, ".");
    gchar *extenison_result;
    while(token)
    {
        extenison_result = token;
        token = (gchar *)strtok(NULL, ".");
    }
    AudioExt result = INVALID;
    for(guint8 i = 0; i < AUDIO_EXTENSIONS_NUM; i++)
    {
        if(strcmp(extenison_result, audio_files_ext[i]) == 0)
        {
            result = (AudioExt)i;
        }
    }
    g_free(file_name_copy);
    return result;
}

static VideoExt get_video_extension(const gchar *file_name)
{
    gchar *file_name_copy = g_strdup(file_name);
    gchar *token = (gchar *)strtok((char *)file_name_copy, ".");
    gchar *extension_result;
    while(token)
    {
        extension_result = token;
        token = (gchar *)strtok(NULL, ".");
    }
    VideoExt result = V_INVALID;
    for(guint8 i=0;i < VIDEO_EXTENSIONS_NUM; i++)
    {
        if(strcmp(extension_result, video_files_ext[i]) == 0)
        {
            result = (VideoExt)i;
        }
    }
    g_free(file_name_copy);
    return result;
}

static ThumbnailExt get_thumbnail_extension(const gchar *file_name)
{
    gchar *file_name_copy = g_strdup(file_name);
    gchar *token = (gchar *)strtok((char *)file_name_copy, ".");
    gchar *extenison_result;
    while(token)
    {
        extenison_result = token;
        token = (gchar *)strtok(NULL, ".");
    }
    ThumbnailExt result = T_INVALID;
    for(guint8 i = 0; i < THUMBNAIL_EXTENSIONS_NUM; i++)
    {
        if(strcmp(extenison_result, thumbnail_files_ext[i]) == 0)
        {
            result = (ThumbnailExt)i;
        }
    }
    g_free(file_name_copy);
    return result;
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, NightcorePipeline *pipeline)
{   
    GstPad *sink_pad = gst_element_get_static_pad (pipeline->audio_convert, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    DEBUG_PRINT( g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src)))

     /* If our converter is already linked, we have nothing to do here */

    if (gst_pad_is_linked(sink_pad))
    {
        DEBUG_PRINT(g_print ("We are already linked. Ignoring.\n"))
        goto exit;
    }

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps (new_pad);
    new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
    new_pad_type = gst_structure_get_name (new_pad_struct);
    if (!g_str_has_prefix (new_pad_type, "audio/x-raw")) {
        DEBUG_PRINT(g_print ("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type))
        goto exit;
     }
    /* Attempt the link */
    ret = gst_pad_link (new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)) {
       DEBUG_PRINT(g_print ("Type is '%s' but link failed.\n", new_pad_type))
    } else {
        DEBUG_PRINT(g_print ("Link succeeded (type '%s').\n", new_pad_type))
    }

exit:
    /* Unreference the new pad's caps, if we got them */
    if (new_pad_caps != NULL)
        gst_caps_unref (new_pad_caps);

    /* Unreference the sink pad */
    gst_object_unref (sink_pad);

}

static void pad_thumbnail_added_handler(GstElement *src, GstPad *new_pad, NightcoreThumbnailPipeline *pipeline)
{
    GstPad *sink_pad = gst_element_get_static_pad (pipeline->audio_convert, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    DEBUG_PRINT( g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src)))

     /* If our converter is already linked, we have nothing to do here */

    if (gst_pad_is_linked(sink_pad))
    {
        DEBUG_PRINT(g_print ("We are already linked. Ignoring.\n"))
        goto exit;
    }

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps (new_pad);
    new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
    new_pad_type = gst_structure_get_name (new_pad_struct);
    if (!g_str_has_prefix (new_pad_type, "audio/x-raw")) {
        DEBUG_PRINT(g_print ("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type))
        goto exit;
     }
    /* Attempt the link */
    ret = gst_pad_link (new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)) {
       DEBUG_PRINT(g_print ("Type is '%s' but link failed.\n", new_pad_type))
    } else {
        DEBUG_PRINT(g_print ("Link succeeded (type '%s').\n", new_pad_type))
    }

exit:
    /* Unreference the new pad's caps, if we got them */
    if (new_pad_caps != NULL)
        gst_caps_unref (new_pad_caps);

    /* Unreference the sink pad */
    gst_object_unref (sink_pad);
}

static int file_valid_path(const char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "w+");
    if(fp == NULL)
    {
        return 1;
    }
    fclose(fp);
    return 0;
}

static void
link_to_multiplexer (GstPad * tolink_pad, GstElement * mux)
{
  GstPad *pad;
  gchar *srcname, *sinkname;

  srcname = gst_pad_get_name (tolink_pad);
  pad = gst_element_get_compatible_pad (mux, tolink_pad, NULL);
  gst_pad_link (tolink_pad, pad);
  sinkname = gst_pad_get_name (pad);
  gst_object_unref (GST_OBJECT (pad));

  g_print ("A new pad %s was created and linked to %s\n", sinkname, srcname);
  g_free (sinkname);
  g_free (srcname);
}
