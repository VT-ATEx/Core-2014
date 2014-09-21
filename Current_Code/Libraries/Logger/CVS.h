#ifndef CVS_H 
#define CVS_H

#include <string>
#include <string.h>
#include <sstream>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdint.h>

using std::string;
using std::ostringstream;

class CVS {
private:
	

	string path;          ///string to hold log file path.
	FILE* file;           ///FILE pointer to store file address.
	bool open;            ///bool to let class know if file is open.
	time_t rawtime;       ///time_t to get system time for logging.
	struct tm* timeinfo;  ///struct tm pointer to convert time to readable format.

	int32_t makeLog(bool print, const char *fmt, va_list args);
	int32_t fileHelper(string directory, string file);
public:
	/**
	 * @brief Error codes returned by class methods.
	 */
	enum log_err {
		LOG_OK = 0,   ///Method executed correctly.
		LOG_ERR = -1  ///Method did not execute correctly.
	};

	/************/
	/* Logger() */
	/************/
	/**
     * @brief Constructor for the class. Sets bool value to let
     * class know that log file is not open.
     */
	CVS();

	/*************/
	/* ~Logger() */
	/*************/
	/**
	 * @brief Deconstructor for the class. Closes log file if file
	 * is open.
	 */
	~CVS();

	/*********************************************************************************************************************/
	/* log_err openLog(const bool create = true, const string filename = "logfile", const string directory = "../logs/") */
	/*********************************************************************************************************************/
	/**
	 * @brief Open or create log file to write to. Once file is opened, the open bool is set
	 * to true.
	 *
	 * @param	create 		- If set to true, a new file with the provided name will be
	 * opened. If the name already exists, an integer representing the number of similar
	 * names in the directory will be appended to the end of the filename. If false is passed
	 * , the specified file will be opened and the log messages will be appended to the end.
	 * If the file does not exist, it will be created. If no value is specified, it will be
	 * assumed to be true.
	 *
	 * @param	filename	- Name of file to write log messages to. If no filename
	 * is provided, logfile is set as default.
	 *
	 * @param	directory	- Directory that you would like the file stored in.
	 * If no directory is provided, ../logs/ is set as default.
	 *
	 * @return @b log_err - LOG_OK will return if file opened successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err openCVS(const bool create = true, const string filename = "logfile", const string directory = "../logs/");
	
	/**********************/
	/* log_err closeLog() */
	/**********************/
	/**
	 * @brief Close the currently opened log file. Once file is closed, the open bool is set
	 * to false. Will do nothing if file was not open.
	 *
	 * @return @b log_err - LOG_OK will return if file closed successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err closeLog();
	

	
	/****************************************************/
	/* log_err debug(bool print, const string fmt, ...) */
	/****************************************************/
	/**
	 * @brief Enter a debug log in file as [<current date and time>] DEBUG: <TAG> - <msg>.
	 *
	 * @param	print		- Print log to stdout.
	 * @param	TAG			- Section log was made.
	 * @param 	fmt			- Message string to log.
	 *
	 * @return @b log_err - LOG_OK will return if log wrote successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err write(bool print, const string fmt, ...);
	
	
};

#endif