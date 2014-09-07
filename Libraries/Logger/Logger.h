/******************************/
/* Author: Jacob Yacovelli    */
/* Date: 7-22-2014            */
/* Rev: 1                     */
/* File: Logger.h             */
/******************************/

#ifndef LOGGER_H 
#define LOGGER_H

#include <string>
#include <string.h>
#include <sstream>
#include <dirent.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdint.h>

using std::string;
using std::ostringstream;
using std::cout;

class Logger {
private:
	/*! @enum level
	 * Logging levels
	 */
	enum level {
		ERROR,    ///<An error occured
		WARN,     ///<A warning occured
		INFO,     ///<Log information
		DEBUG,    ///<Log debug messages
		TRACE     ///<Log everything
	};

	string path;          //string to hold log file path.
	FILE* file;           //FILE pointer to store file address.
	bool open;            //bool to let class know if file is open.
	bool loggingSwitch;   //turn on and off logging

	/*************************************************************************************************/
	/* int32_t makeLog(bool print, const string TAG, const level lvl, const char *fmt, va_list args) */
	/*************************************************************************************************/
	/**
	 * @brief Function that makes the log message.
	 *
	 * @param	print	Print log to stdout.
	 * @param	TAG		String to append to front of log message.
	 * @param	lvl		Level to log message as.
	 * @param	*fmt	String format of message.
	 * @param	args	Items to place in message format.
	 *
	 * @return @b int32_t - Returns 0 if completed successfully and -1 otherwise.
	 */
	int32_t makeLog(bool print, const string TAG, const level lvl, const char *fmt, va_list args);

	/*****************************************************/
	/* int32_t fileHelper(string directory, string file) */
	/*****************************************************/
	/**
	 * @brief Function to help find filenames. Looks in directory and counts the number of files
	 * that have the same name as file.
	 *
	 * @param	directory	Directory to search in.
	 * @param	file		Filenames to count.
	 *
	 * @return @b int32_t - Count of similar filenames.
	 */
	int32_t fileHelper(string directory, string file);
public:
	///Error codes returned by class methods.
	enum log_err {
		LOG_OK = 0,   ///<Method executed correctly.
		LOG_ERR = -1  ///<Method did not execute correctly.
	};

	/************/
	/* Logger() */
	/************/
	/**
     * @brief Constructor for the class. Sets bool value to let class know that log file
     * is not open.
     */
	Logger();

	/*************/
	/* ~Logger() */
	/*************/
	/**
	 * @brief Deconstructor for the class. Closes log file if file is open.
	 */
	~Logger();

	/*********************************************************************************************************************/
	/* log_err openLog(const bool create = true, const string filename = "logfile", const string directory = "../logs/") */
	/*********************************************************************************************************************/
	/**
	 * @brief Open or create log file to write to. Once file is opened, the open bool is set
	 * to true.
	 *
	 * @param	create 		If set to true, a new file with the provided name will be
	 * 						opened. If the name already exists, an integer representing
	 * 						the number of similar names in the directory will be appended
	 * 						to the end of the filename. If false is passed, the specified
	 * 						file will be opened and the log messages will be appended to
	 * 						the end. If the file does not exist, it will be created. If no
	 * 						value is specified, it will be assumed to be true.
	 * @param	filename	Name of file to write log messages to. If no filename is
	 *						provided, logfile is set as default.
	 * @param	directory	Directory that you would like the file stored in. If no
	 *						directory is provided, ../logs/ is set as default.
	 *
	 * @return @b log_err - LOG_OK will return if file opened successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err openLog(const bool create = true, const string filename = "logfile", const string directory = "../logs/");
	
	/****************************/
	/* void make_log(bool make) */
	/****************************/
	/**
	  * @brief Used to turn on/off logging functionality.
	  *
	  * @param	make	When set to true, logs will be taken.
	  */
	void make_log(bool make);

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
	/* log_err error(bool print, const string fmt, ...) */
	/****************************************************/
	/**
	 * @brief Enter an error log in file as [<current date and time>] ERROR: <TAG> - <msg>.
	 *
	 * @param	print	Print log to stdout.
	 * @param	TAG		Section log was made.
	 * @param 	fmt 	Message string to log.
	 *
	 * @return @b log_err - LOG_OK will return if log wrote successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err error(bool print, const string TAG, const string fmt, ...);
	
	/***************************************************/
	/* log_err warn(bool print, const string fmt, ...) */
	/***************************************************/
	/**
	 * @brief Enter a warning log in file as [<current date and time>] WARN : <TAG> - <msg>.
	 *
	 * @param	print	Print log to stdout.
	 * @param	TAG		Section log was made.
	 * @param 	fmt		Message string to log.
	 *
	 * @return @b log_err - LOG_OK will return if log wrote successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err warn(bool print, const string TAG, const string fmt, ...);
	
	/***************************************************/
	/* log_err info(bool print, const string fmt, ...) */
	/***************************************************/
	/**
	 * @brief Enter an info log in file as [<current date and time>] INFO : <TAG> - <msg>.
	 *
	 * @param	print	Print log to stdout.
	 * @param	TAG		Section log was made.
	 * @param 	fmt		Message string to log.
	 *
	 * @return @b log_err - LOG_OK will return if log wrote successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err info(bool print, const string TAG, const string fmt, ...);
	
	/****************************************************/
	/* log_err debug(bool print, const string fmt, ...) */
	/****************************************************/
	/**
	 * @brief Enter a debug log in file as [<current date and time>] DEBUG: <TAG> - <msg>.
	 *
	 * @param	print	Print log to stdout.
	 * @param	TAG		Section log was made.
	 * @param 	fmt		Message string to log.
	 *
	 * @return @b log_err - LOG_OK will return if log wrote successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err debug(bool print, const string TAG, const string fmt, ...);
	
	/****************************************************/
	/* log_err trace(bool print, const string fmt, ...) */
	/****************************************************/
	/**
	 * @brief Enter a trace log in file as [<current date and time>] TRACE: <TAG> - <msg>.
	 *
	 * @param	print	Print log to stdout.
	 * @param	TAG		Section log was made.
	 * @param 	fmt		Message string to log.
	 *
	 * @return @b log_err - LOG_OK will return if log wrote successfully, and LOG_ERR will
	 * return otherwise.
	 */
	log_err trace(bool print, const string TAG, const string fmt, ...);
};

#endif