#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "netstring.h"

int
main()
{
	char *test_data = "\xab{\xcd}\xef;";
	char *enc_data;
	size_t enc_size;
	char *dec_data;
	size_t dec_size;

	printf("Test Data (%u): '%s'\n", (unsigned int)strlen(test_data), test_data);

	enc_data = netstring_encode(test_data, strlen(test_data), &enc_size);
	if (!enc_data)
	{
		fprintf(stderr, "encoding error\n");
		exit(EXIT_FAILURE);
	}

	printf("Encoded Data (%u): '%s'\n", (unsigned int)enc_size, enc_data);

	dec_data = netstring_decode(enc_data, &dec_size);
	if (!dec_data)
	{
		fprintf(stderr, "decoding error\n");
		exit(EXIT_FAILURE);
	}

	printf("Decoded Data (%u): '%s'\n", (unsigned int)dec_size, dec_data);

	if (0 == strncmp(dec_data, test_data, dec_size))
		printf("TEST SUCCESS\n");
	else
		printf("TEST FAILURE\n");

	free(enc_data);
	free(dec_data);

	return EXIT_SUCCESS;
}
