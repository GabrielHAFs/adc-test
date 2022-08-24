#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

static int adc_read(char *dev_dir)
{
	int adc_sample;

	adc_sample = read_dir(dev_dir);

	return adc_sample;
}

int read_dir(const char *basedir)
{
	int ret;
	FILE  *adcdir;
	char *temp = malloc(strlen(basedir) + 2);

	if (!temp) {
		fprintf(stderr, "Memory allocation failed");
		return -ENOMEM;
	}

	ret = sprintf(temp, "%s", basedir);
	if (ret < 0)
		goto error_free;

	adcdir = fopen(temp, "r");
	if (!adcdir) {
		ret = -errno;
		goto error_free;
	}

	errno = 0;
	if (fscanf(adcdir, "%d\n", &ret) != 1) {
		ret = errno ? -errno : -ENODATA;
		if (fclose(adcdir))
			perror("read_dir(): Failed to close dir");

		goto error_free;
	}

	if (fclose(adcdir))
		ret = -errno;

error_free:
	free(temp);

	return ret;
}

int main(int argc, char **argv)
{
	int sample_val;
	float voltage_scale;
	char *dev_dir;
	int dev_num = 0;
    int channel = 0;
	const char *family = "apalis"; //colibri or verdin

	if (&family == "colibri") {
		if ( asprintf(&dev_dir, "/dev/%s-ain%d", family ,channel) < 0 ) 
		{
			printf("%d: Failed to allocate memory\n", __LINE__);
			free(dev_dir);
			return -1;
		}
	}else{
		if ( asprintf(&dev_dir, "/dev/%s-adc%d", family ,channel) < 0 ) 
		{
			printf("%d: Failed to allocate memory\n", __LINE__);
			free(dev_dir);
			return -1;
		}
	}


	sample_val = adc_read(dev_dir);
	if (sample_val < 0)
	{
		free(dev_dir);
		return -1;
	}

	printf("Raw Sample Value: 0x%04x\n", (unsigned int) sample_val);

	free(dev_dir);

    return 0;
}