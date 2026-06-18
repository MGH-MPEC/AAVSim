#include "include.h"

#if not defined(_WIN32)
	#include <unistd.h>
#endif

#if __APPLE__
	#include <mach-o/dyld.h>
#endif


/** \brief Empty constructor, should never create an instance of this class */
AavUtil::AavUtil(void){
    //empty
}

/** \brief Empty destructor*/
AavUtil::~AavUtil(void){
    //empty
}

/* Constant string values for AAV version and file/directory information*/
/** The AAVSim Input Version to match that from the input file */
const char *AavUtil::AAVSim_INPUT_VERSION = "43";
/** AAVSim version string */
const char *AavUtil::AAVSim_VERSION_STRING = "43";
/** The compile date of the most recent release */
const char *AavUtil::AAVSim_EXECUTABLE_COMPILED_DATE = "2025-10-01";
/** .tmp */
const char *AavUtil::FILE_EXTENSION_FOR_TEMP = ".tmp";
/** .txt */
const char *AavUtil::FILE_EXTENSION_FOR_TRACE = ".txt";
/** .out */
const char *AavUtil::FILE_EXTENSION_FOR_OUTPUT = ".out";
/** .in */
const char *AavUtil::FILE_EXTENSION_FOR_INPUT = ".in";
/** *.in */
const char *AavUtil::FILE_EXTENSION_INPUT_SEARCH_STR = "*.in";
/** popstats.out */
const char *AavUtil::FILE_NAME_SUMMARIES = "popstats.out";

/** Vector of the file names to be run */

/** Vector of the file names to be run*/
std::vector<std::string> AavUtil::filesToRun;
/** The inputs directory path */
std::string AavUtil::inputsDirectory;
/** The output directory path */
std::string AavUtil::resultsDirectory;
/** True if we're using random seed, false for fixed seed */
bool AavUtil::useRandomSeedByTime;

/** \brief Random number generator class */
MTRand AavUtil::mtRand;

/** \brief useCurrentDirectoryForInputs determines the current directory and sets as inputs directory */
void AavUtil::useCurrentDirectoryForInputs() {
#if defined(_WIN32)
	char buffer[512];
	_getcwd(buffer, 512);
	inputsDirectory = buffer;
#elif __APPLE__
	char buffer[512];
	uint32_t size = sizeof(buffer);
	_NSGetExecutablePath(buffer, &size);
	inputsDirectory = buffer;
	inputsDirectory = inputsDirectory.substr(0, inputsDirectory.find_last_of("\\/"));
#else
	char buffer[512];
	getcwd(buffer, 512);
	inputsDirectory = buffer;
#endif
} /* end useCurrentDirectoryForInputs */

/** \brief findInputFiles locates all the .in files in the current directory and adds them
	to the filesToRun vector */
void AavUtil::findInputFiles() {
#if defined(_WIN32)
	intptr_t hFile; // using intptr_t instead of long type for hFile
	struct _finddata_t tFileInfo;
	hFile = _findfirst( FILE_EXTENSION_INPUT_SEARCH_STR, &tFileInfo );
	int nInputFiles = 0;
	string fileName;

	//get the list of files that we have to process
	filesToRun.clear();
	do {
		fileName = (char *) tFileInfo.name;
		filesToRun.push_back(fileName);
		nInputFiles++;
	} while ( _findnext ( hFile, &tFileInfo ) == 0 );
	_findclose( hFile );
	cout << "Number of files found: " << filesToRun.size() << endl;
#else
	glob_t files;
	glob(FILE_EXTENSION_INPUT_SEARCH_STR, GLOB_ERR, NULL, &files);
	int nInputFiles = 0;
	string fileName;

	filesToRun.clear();
	//get the list of files that we have to process
	int i;
	for( i = 0; i < files.gl_pathc; i++) {
		fileName = (char *) files.gl_pathv[i];
		filesToRun.push_back(fileName);
		++nInputFiles;
	}
	globfree( &files);
#endif
} /* end findInputFiles */

/** \brief createResultsDirectory creates the directory "results" as a subdirectory of the inputs one */
void AavUtil::createResultsDirectory() {
#if defined(_WIN32)
	resultsDirectory = inputsDirectory;
	resultsDirectory.append("\\");
	resultsDirectory.append("results");
	_mkdir(resultsDirectory.c_str());
#else
	resultsDirectory = inputsDirectory;
	resultsDirectory.append("/");
	resultsDirectory.append("results");
	mkdir(resultsDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
} /* end createResultsDirectory */

/** \brief changeDirectoryToResults changes the working directory to the results one */
void AavUtil::changeDirectoryToResults() {
#if defined(_WIN32)
	_chdir(resultsDirectory.c_str());
#else
	chdir(resultsDirectory.c_str());
#endif
} /* end changeDirectoryToResults */

/**
void AavUtil::createResultsSubDirectory(string subDirectory) {
#if defined(_WIN32)
	resultsSubDirectory = resultsDirectory + "\\" + subDirectory;
	resultsSubDirectory.append("\\");
	resultsSubDirectory.append(subDirectory);
	_mkdir(resultsSubDirectory.c_str());
#else
	resultsSubdirectory = resultsDirectory;
	resultsSubDirectory.append("/");
	resultsSubDirectory.append(subDirectory);
	mkdir(resultsSubDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

void AavUtil::changeDirectoryToResultsSubDirectory() {
#if defined(_WIN32)
	_chdir(resultsSubDirectory.c_str());
#else
	chdir(resultsSubDirectory.c_str());
#endif
}

void AavUtil::createTraceFilesDirectory() {
#if defined(_WIN32)
	traceDirectory = resultsSubDirectory;
	traceDirectory.append("\\");
	traceDirectory.append("trace");
	_mkdir(traceDirectory.c_str());
#else
	traceDirectory = resultsSubDirectory;
	traceDirectory.append("/");
	traceDirectory.append("trace");
	mkdir(traceDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

void AavUtil::changeDirectoryToTraceFilesDirectory() {
#if defined(_WIN32)
	_chdir(traceDirectory.c_str());
#else
	chdir(traceDirectory.c_str());
#endif
}
*/

/** \brief changeDirectoryToInputs changes the working directory to the inputs one */
void AavUtil::changeDirectoryToInputs() {
#if defined(_WIN32)
	_chdir(inputsDirectory.c_str());
#else
	chdir(inputsDirectory.c_str());
#endif
} /* end changeDirectoryToInputs */




/** \brief fileExists returns true if the specified file exists, false otherwise
 * \param filename a pointer to a character array representing the name of the file
 **/
bool AavUtil::fileExists(const char *filename) {
	FILE *file;
	//fopen_s(&file, filename, "r");
	file = fopen(filename, "r");
	if (!file)
		return false;
	fclose(file);
	return true;
} /* end fileExists */

/** \brief openFile opens the specified file in the given mode
 * \param filename a pointer to a character array representing the name of the file
 * \param mode a pointer to a character array representing the mode to open the file in: "r" for read, "w" for write, "a" for append, "r+" for reading and writing an existing file, "w+" for reading and writing an empty file, "a+" for reading and appending to a file
 **/
FILE *AavUtil::openFile(const char *filename, const char *mode) {
	FILE *file = fopen(filename, mode);
	return file;
} /* end openFile */

/** \brief closeFile closes the specified file
 * \param filename a pointer to a character array representing the name of the file
 **/
void AavUtil::closeFile(FILE *file) {
	fclose(file);
} /* end closeFile */
