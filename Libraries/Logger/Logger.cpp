/******************************/
/* Author: Jacob Yacovelli    */
/* Date: 7-21-2014            */
/* Rev: 1                     */
/* File: Logger.cpp           */
/******************************/

#include "Logger.h"

/********************/
/* Logger::Logger() */
/********************/
Logger::Logger() : open(false), loggingSwitch(true) {
}

/*********************/
/* Logger::~Logger() */
/*********************/
Logger::~Logger() {
	closeLog();
}

/*****************************************************************************************************/
/* Logger::log_err Logger::openLog(const bool create, const string filename, const string directory) */
/*****************************************************************************************************/
Logger::log_err Logger::openLog(const bool create /*= true*/, const string filename /*= "logfile"*/, const string directory /*= "../logs/"*/) {
	ostringstream oss;
	int32_t duplicate;

	mkdir(directory.c_str(), 0755);

	if (create) {
		
		if ((duplicate = fileHelper(directory, filename)) < 0)
			return LOG_ERR;

		if (duplicate > 0)
			oss << directory << filename << duplicate << ".log";
		else
			oss << directory << filename << ".log";
		path = oss.str();

		if ((file = fopen(path.c_str(), "w")) == NULL)
			return LOG_ERR;
	} else {
		oss << directory << filename << ".log";
		path = oss.str();

		if ((file = fopen(path.c_str(), "a")) == NULL)
			return LOG_ERR;
	}
	open = true;

	return LOG_OK;
}

/************************************/
/* void Logger::make_log(bool make) */
/************************************/
void Logger::make_log(bool make) {
	loggingSwitch = make;
}

/**************************************/
/* Logger::log_err Logger::closeLog() */
/**************************************/
Logger::log_err Logger::closeLog() {

	if (open == true) {
	
		if (fclose(file) != 0)
			return LOG_ERR;
	}
	open = false;

	return LOG_OK;
}

/**************************************************************************************/
/* Logger::log_err Logger::error(bool print, const string TAG, const string fmt, ...) */
/**************************************************************************************/
Logger::log_err Logger::error(bool print, const string TAG, const string fmt, ...) {
	
	if (loggingSwitch) {
		va_list args;
		va_start(args, fmt);

		if (makeLog(print, TAG, ERROR, fmt.c_str(), args) == LOG_ERR) {
			va_end(args);
			return LOG_ERR;
		}
		va_end(args);
	}
	return LOG_OK;
}

/*************************************************************************************/
/* Logger::log_err Logger::warn(bool print, const string TAG, const string fmt, ...) */
/*************************************************************************************/
Logger::log_err Logger::warn(bool print, const string TAG, const string fmt, ...) {
	
	if (loggingSwitch) {
		va_list args;
		va_start(args, fmt);

		if (makeLog(print, TAG, WARN, fmt.c_str(), args) == LOG_ERR) {
			va_end(args);
			return LOG_ERR;
		}
		va_end(args);
	}
	return LOG_OK;
}

/*************************************************************************************/
/* Logger::log_err Logger::info(bool print, const string TAG, const string fmt, ...) */
/*************************************************************************************/
Logger::log_err Logger::info(bool print, const string TAG, const string fmt, ...) {
	
	if (loggingSwitch) {
		va_list args;
		va_start(args, fmt);

		if (makeLog(print, TAG, INFO, fmt.c_str(), args) == LOG_ERR) {
			va_end(args);
			return LOG_ERR;
		}
		va_end(args);
	}
	return LOG_OK;
}

/**************************************************************************************/
/* Logger::log_err Logger::debug(bool print, const string TAG, const string fmt, ...) */
/**************************************************************************************/
Logger::log_err Logger::debug(bool print, const string TAG, const string fmt, ...) {
	
	if (loggingSwitch) {
		va_list args;
		va_start(args, fmt);

		if (makeLog(print, TAG, DEBUG, fmt.c_str(), args) == LOG_ERR) {
			va_end(args);
			return LOG_ERR;
		}
		va_end(args);
	}
	return LOG_OK;
}

/**************************************************************************************/
/* Logger::log_err Logger::trace(bool print, const string TAG, const string fmt, ...) */
/**************************************************************************************/
Logger::log_err Logger::trace(bool print, const string TAG, const string fmt, ...) {
	
	if (loggingSwitch) {
		va_list args;
		va_start(args, fmt);

		if (makeLog(print, TAG, TRACE, fmt.c_str(), args) == LOG_ERR) {
			va_end(args);
			return LOG_ERR;
		}
		va_end(args);
	}
	return LOG_OK;
}

/*********************************************************************************************************/
/* int32_t Logger::makeLog(bool print, const string TAG, const level lvl, const char *fmt, va_list args) */
/*********************************************************************************************************/
int32_t Logger::makeLog(bool print, const string TAG, const level lvl, const char *fmt, va_list args) {
	ostringstream oss;
	string sTime;
	char msg[strlen(fmt)];
	bool display = print;
	time_t rawtime;       //time_t to get system time for logging.
	struct tm* timeinfo;  //struct tm pointer to convert time to readable format.

	vsprintf(msg, fmt, args);

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	sTime = asctime(timeinfo);
	oss << "[" << sTime.substr(0,  sTime.size()-1) << "] ";

	switch (lvl) {
		case ERROR:
			oss << "ERROR: ";
			break;
		case WARN:
			oss << "WARN : ";
			break;
		case INFO:
			oss << "INFO : ";
			break;
		case DEBUG:
			oss << "DEBUG: ";
			break;
		case TRACE:
			oss << "TRACE: ";
			break;
	}

	oss << TAG << " - " << msg << '\n';

	if (display)
		cout << msg << '\n';

	if (fputs(oss.str().c_str(), file) < 0)
		return LOG_ERR;
	fflush(file);

	return LOG_OK;
}

/*************************************************************/
/* int32_t Logger::fileHelper(string directory, string file) */
/*************************************************************/
int32_t Logger::fileHelper(string directory, string file) {
	DIR *dir;
	struct dirent *dp;
	int32_t count = 0;

	if ((dir = opendir(directory.c_str())) == NULL) {
		perror("Cannot open path");
		return -1;
	}

	while ((dp = readdir(dir)) != NULL) {
		if (strncmp(dp->d_name, file.c_str(), strlen(file.c_str())) == 0)
			count++;
	}
	closedir(dir);

	return count;
}