#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <assert.h>

static inline void
exit_on_failure(const char *const restrict format,
		const int err) __attribute__((noreturn));

static inline void
exit_on_failure(const char *const restrict reason,
		const int err)
{
	(void) fprintf(stderr, "%s: %s\n", reason, snd_strerror(err));
	exit(1);
}

#define BIT_RATE 	44100
#define COUNT_CHANNELS	2
#define BUF_LEN 	128
#define PCM_DEVICE	"hw:1,0"
/* #define PCM_DEVICE	"default" */
/* #define DEBUG(MSG) printf("line: %d -- %s\n", __LINE__, MSG), fflush(stdout) */
#define DEBUG(MSG)


      
int
main (int argc,
      char *argv[])
{
	FILE *output;
	int i;
	int err;
	short buf[BUF_LEN];
	unsigned int bit_rate;

	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;

	err = snd_pcm_open(&capture_handle,
			   PCM_DEVICE,
			   SND_PCM_STREAM_CAPTURE,
			   0);

	if (err < 0)
		exit_on_failure("cannot open audio device",
				err);
	DEBUG("opened");


	err = snd_pcm_hw_params_malloc(&hw_params);
	if (err < 0)
		exit_on_failure("cannot allocate hardware parameter structure ",
				err);
	DEBUG("allocated");
			 
	err = snd_pcm_hw_params_any(capture_handle, hw_params);
	if (err < 0) 
		exit_on_failure("cannot initialize hardware parameter "
				"structure", err);
	DEBUG("hw_params init");


	err = snd_pcm_hw_params_set_access(capture_handle,
					   hw_params,
					   SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0)
		exit_on_failure("cannot set access type", err);
	DEBUG("access type");

	err = snd_pcm_hw_params_set_format(capture_handle,
					   hw_params,
					   SND_PCM_FORMAT_S16_LE);
	if (err < 0)
		exit_on_failure("cannot set sample format", err);
	DEBUG("set sample format");

	bit_rate = BIT_RATE;
	err = snd_pcm_hw_params_set_rate_near(capture_handle,
					      hw_params,
					      &bit_rate,
					      0);
	if ((err < 0) || (bit_rate != BIT_RATE))
		exit_on_failure("cannot set sample rate", err);
	DEBUG("set sample rate");


	err = snd_pcm_hw_params_set_channels(capture_handle,
					     hw_params,
					     COUNT_CHANNELS);
	if (err < 0)
		exit_on_failure("cannot set channel count", err);
	DEBUG("set channel count");


	err = snd_pcm_hw_params(capture_handle,
			       	hw_params);
	if (err < 0)
		exit_on_failure("cannot set parameters", err);
	DEBUG("set parameters");

	snd_pcm_hw_params_free(hw_params);

	err = snd_pcm_prepare(capture_handle);
	if (err < 0)
		exit_on_failure("cannot prepare audio interface for use", err);

	assert(output = fopen("output.raw", "w"));

	for (i = 0; i < 1000; ++i) {
		err = snd_pcm_readi(capture_handle,
				    buf,
				    BUF_LEN);
		if (err != BUF_LEN)
			exit_on_failure("read from audio interface failed", err);

		DEBUG("read from autdio interface");

		assert(fwrite(buf,
			      sizeof(buf[0]),
			      BUF_LEN,
			      output) == BUF_LEN);
	}

	assert(fclose(output) == 0);

	snd_pcm_close(capture_handle);

	DEBUG("closed");

	sleep(0);

	DEBUG("done");

	return 0;
}
