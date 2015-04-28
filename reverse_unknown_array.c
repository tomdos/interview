/*
 * Reverse an array of unknown data type. The array is filled with items 
 * (static) and not pointers to items.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define	SIZE	10

struct item
{
	int a;
	float b;
	void *c;
};

void
swap(void *a, void *b, void *tmp, size_t size)
{
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
}

void 
reverse(void *array, size_t nmemb, size_t size)
{
	void *tmp;
	int i,j;

	tmp = malloc(size);

	i = 0;
	j = nmemb - 1;
	while (i < j) {
		swap(array + i*size, array + j*size, tmp, size);
		i++;
		j--;
	}

	free(tmp);
}

/* Debug print */
void
print_array(struct item *array, size_t size)
{
	int i;
	
	for (i = 0; i < size; i++)
		printf("%d ", array[i].a);

	printf("\n");
}

int
main(int argc, char *argv[])
{
	int i;
	struct item array[SIZE];

	//fill it with some content
	for (i = 0; i < SIZE; i++)
		array[i].a = i;

	print_array(array, SIZE);
	reverse(array, SIZE, sizeof(struct item));	
	print_array(array, SIZE);

	return 0;
}
