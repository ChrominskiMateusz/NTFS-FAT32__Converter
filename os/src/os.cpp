#include "Converter.h"

int main ()
{
	Converter *f = new Converter ("\\\\.\\I:");
	f->readPartitionBootSector ();

	return 0;
}