#include "Converter.h"

int main ()
{
	Converter g;
	g.readBlock ();

	NTFS *f = new NTFS ("ntfs.img");		// \\\\.\\I:
	f->readPartitionBootSector ();
	f->getMFTChain ();
	f->readMFT (5, 0);


	return 0;
}