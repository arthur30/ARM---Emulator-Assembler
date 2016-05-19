#include <stdlib.h>

int main(int argc, char **argv)
{
	(void)argv;

	/* Check if source and dest files are provided.	 */
	if (argc < 3)
		return 1;

	return EXIT_SUCCESS;
}
