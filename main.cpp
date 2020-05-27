#include <soundio/soundio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>

static float seconds_offset = 0.0f;

static void writeCallback(struct SoundIoOutStream *out_stream, int frame_count_min, int frame_count_max)
{

    const struct SoundIoChannelLayout *layout = &out_stream->layout;
    float float_sample_rate = out_stream->sample_rate;
    float seconds_per_frame = 1.0f / float_sample_rate;
    struct SoundIoChannelArea *areas;
    int frames_left = frame_count_max;
    int err;

    while (frames_left > 0)
    {
        int frame_count = frames_left;

        if ((err = soundio_outstream_begin_write(out_stream, &areas, &frame_count)))
        {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count)
            break;

        //Setup Sine Wave
        float pitch = 440.0f;
        float radians_per_second = pitch * 2.0f * M_PI;

        //Write Sine Wave to buffer
        for (int channel = 0; channel < layout->channel_count; channel++)
        {
            for (int frame = 0; frame < frame_count; frame++)
            {
                float sample = sin((seconds_offset + frame * seconds_per_frame) * radians_per_second);
                float *buffer = (float *)(areas[channel].ptr + areas[channel].step * frame);
                *buffer = sample;
            }
        }
        seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);

        if ((err = soundio_outstream_end_write(out_stream)))
        {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
    }
}

int main(int argc, char *argv[])
{
    int err;
    //Create SoundIo object
    SoundIo *soundio = soundio_create();
    if (!soundio)
    {
        fprintf(stderr, "out of memory\n");
        return 1;
    }
    if (err = soundio_connect(soundio))
    {
        fprintf(stderr, "error connecting: %s\n", soundio_strerror(err));
        return 1;
    }

    soundio_flush_events(soundio);

    //Setup output device
    int default_out_device_index = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0)
    {
        fprintf(stderr, "no output device found\n");
        return 1;
    }
    SoundIoDevice *output_device = soundio_get_output_device(soundio, default_out_device_index);
    if (!output_device)
    {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    fprintf(stderr, "Output device: %s\n", output_device->name);

    SoundIoOutStream *out_stream = soundio_outstream_create(output_device);
    if (!out_stream)
    {
        fprintf(stderr, "out of memory\n");
        return 1;
    }
    out_stream->format = SoundIoFormatFloat32NE;
    out_stream->write_callback = writeCallback;

    if ((err = soundio_outstream_open(out_stream)))
    {
        fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
        return 1;
    }

    if (out_stream->layout_error)
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(out_stream->layout_error));

    if ((err = soundio_outstream_start(out_stream)))
    {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        return 1;
    }

    for (;;)
        soundio_wait_events(soundio);

    soundio_outstream_destroy(out_stream);
    soundio_device_unref(output_device);
    soundio_destroy(soundio);
    return 0;
}