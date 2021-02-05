#include <gtest/gtest.h> 
#include "ghyper.h" 

//#define DEBUG

int main(int argc, char **argv){
	#ifdef DEBUG
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	exit(1);
	#endif

	uint8_t dummy[10];
	Ghyper *ghv = new Ghyper();
	return ghv->hv(dummy);
}
