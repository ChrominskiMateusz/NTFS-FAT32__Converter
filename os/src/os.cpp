#include "Fat.h"


int main ()
{
	Fat *f = new Fat ("wf.img");
	f->readPartition ();

	return 0;
}