#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2023";
	static const char UBUNTU_VERSION_STYLE[] =  "23.05";
	
	//Software Status
	static const char STATUS[] =  "";
	static const char STATUS_SHORT[] =  "b";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 4;
	static const long BUILD  = 303;
	static const long REVISION  = 1673;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 398;
	#define RC_FILEVERSION 0,4,303,1673
	#define RC_FILEVERSION_STRING "0, 4, 303, 1673\0"
	static const char FULLVERSION_STRING [] = "0.4.303.1673";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;
	

}
#endif //VERSION_H
