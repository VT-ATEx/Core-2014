#include "CVS.h"

/********************/
/* Logger::Logger() */
/********************/
CVS::CVS() {
	open = false;
}

/*********************/
/* Logger::~Logger() */
/*********************/
CVS::~CVS() {
	closeLog();
}

/*****************************************************************************************************/
/* Logger::log_err Logger::openLog(const bool create, const string filename, const string directory) */
/*****************************************************************************************************/
CVS::log_err CVS::openCVS(const bool create /*= true*/, const string filename /*= "logfile"*/, const string directory /*= "../logs/"*/) {
	ostringstream oss;
	int32_t duplicate;

	mkdir(directory.c_str(), 0755);

	if (create) {
		
		if ((duplicate = fileHelper(directory, filename)) < 0)
			return LOG_ERR;

		if (duplicate > 0)
			oss << directory << filename << duplicate << ".csv";
		else
			oss << directory << filename << ".csv";
		path = oss.str();

		if ((file = fopen(path.c_str(), "w")) == NULL)
			return LOG_ERR;
	} else {
		oss << directory << filename << ".csv";
		path = oss.str();

		if ((file = fopen(path.c_str(), "a")) == NULL)
			return LOG_ERR;
	}
	open = true;

	return LOG_OK;
}

/**************************************/
/* Logger::log_err Logger::closeLog() */
/**************************************/
CVS::log_err CVS::closeLog() {

	if (open == true) {
	
		if (fclose(file) != 0)
			return LOG_ERR;
	}
	open = false;

	return LOG_OK;
}


/**************************************************************************************/
/* Logger::log_err Logger::debug(bool print, const string TAG, const string fmt, ...) */
/**************************************************************************************/
CVS::log_err CVS::write(bool print, const string fmt, ...) {
	va_list args;
	va_start(args, fmt);

	if (makeLog(print, fmt.c_str(), args) == LOG_ERR) {
		va_end(args);
		return LOG_ERR;
	}
	va_end(args);
	return LOG_OK;
}


/*********************************************************************************************************/
/* int32_t Logger::makeLog(bool print, const string TAG, const level lvl, const char *fmt, va_list args) */
/*********************************************************************************************************/
int32_t CVS::makeLog(bool print, const char *fmt, va_list args) {
	ostringstream oss;
	string sTime, logMesg;
	char msg[strlen(fmt)];

	vsprintf(msg, fmt, args);

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	sTime = asctime(timeinfo);
	sTime[sTime.length()-1] = '\0';
	sTime = sTime.c_str();
	oss << "" << sTime << ",";

	

	oss << msg << '\n';
	logMesg = oss.str();

	if (print)
		printf(logMesg.c_str());

	if (fputs(logMesg.c_str(), file) < 0)
		return LOG_ERR;
	fflush(file);

	return LOG_OK;
}

/*************************************************************/
/* int32_t Logger::fileHelper(string directory, string file) */
/*************************************************************/
int32_t CVS::fileHelper(string directory, string file) {
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