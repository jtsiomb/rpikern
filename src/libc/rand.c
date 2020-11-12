/* random number generator, based on this description of the algorithm
 * used by the GNU libc: https://www.mathstat.dal.ca/~selinger/random
 */
#include <stdlib.h>
#include <inttypes.h>

static int init_done;
static int32_t rng[34];
static int32_t *ptr0, *ptr1;

int rand(void)
{
	int res;

	if(!init_done) {
		srand(1);
	}

	*ptr1 += *ptr0;
	res = (uint32_t)*ptr1 >> 1;
	if(++ptr0 >= rng + 34) ptr0 = rng;
	if(++ptr1 >= rng + 34) ptr1 = rng;

	return res;
}

void srand(unsigned int seed)
{
	int i;

	init_done = 1;
	if(seed == 0) seed = 1;

	rng[0] = seed;
	for(i=1; i<31; i++) {
		rng[i] = (16807 * rng[i - 1]) % RAND_MAX;
		if(rng[i] < 0) rng[i] += RAND_MAX;
	}
	for(i=31; i<34; i++) {
		rng[i] = rng[i - 31];
	}
	ptr0 = rng + 3;
	ptr1 = rng + 31;

	for(i=34; i<344; i++) {
		rand();
	}
}
