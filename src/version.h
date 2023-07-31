#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{

	//Date Version Types
<<<<<<< HEAD
	static const char DATE[] = "20";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2023";
	static const char UBUNTU_VERSION_STYLE[] =  "23.05";

=======
	static const char DATE[] = "31";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2023";
	static const char UBUNTU_VERSION_STYLE[] =  "23.07";
	
>>>>>>> 3a06434 (add shortcut for reset to defafault settings)
	//Software Status
	static const char STATUS[] =  "";
	static const char STATUS_SHORT[] =  "b";

	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 4;
<<<<<<< HEAD
	static const long BUILD  = 1760;
	static const long REVISION  = 50;

	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2504;
	#define RC_FILEVERSION 0,4,1760,50
	#define RC_FILEVERSION_STRING "0, 4, 1760, 50\0"
	static const char FULLVERSION_STRING [] = "0.4.1760.50";

=======
	static const long BUILD  = 1770;
	static const long REVISION  = 28;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2517;
	#define RC_FILEVERSION 0,4,1770,28
	#define RC_FILEVERSION_STRING "0, 4, 1770, 28\0"
	static const char FULLVERSION_STRING [] = "0.4.1770.28";
	
>>>>>>> 3a06434 (add shortcut for reset to defafault settings)
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;

}
#endif //VERSION_H
