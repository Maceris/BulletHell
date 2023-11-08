#include "Debugging/Logger.h"

#include <list>
#include <map>

#include "Memory/CriticalSection.h"

static const char* ERROR_LOG_FILENAME = "log.txt";

#ifdef DEBUG
const LogFlag DEFAULT_FLAG_ERROR = FLAG_WRITE_TO_DEBUGGER;
const LogFlag DEFAULT_FLAG_WARNING = FLAG_WRITE_TO_DEBUGGER;
const LogFlag DEFAULT_FLAG_INFO = FLAG_WRITE_TO_DEBUGGER;
#else
const LogFlag DEFAULT_FLAG_ERROR = 0;
const LogFlag DEFAULT_FLAG_WARNING = 0;
const LogFlag DEFAULT_FLAG_INFO = 0;
#endif

#pragma region LogManager declaration
/// <summary>
/// Manages logging, tracking where logs go, and cleaning up log resources.
/// </summary>
class LogManager
{
public:
	/// <summary>
	/// The possible results from an error dialog.
	/// </summary>
	enum ErrorDialogResult
	{
		LOG_MANAGER_ERROR_ABORT,
		LOG_MANAGER_ERROR_RETRY,
		LOG_MANAGER_ERROR_IGNORE
	};

	using Tags = std::map<std::string, LogFlag>;
	using ErrorLoggerList = std::list<Logger::ErrorLogger*>;

	/// <summary>
	/// A set of log flags for various tags, determining if and where they
	/// get logged.
	/// </summary>
	Tags tags;

	/// <summary>
	/// All the erorr loggers that got allocated by macros.
	/// </summary>
	ErrorLoggerList error_loggers;

	/// <summary>
	/// Used to ensure thread safety when adding or reading tags.
	/// </summary>
	CriticalSection tag_critical_section;

	/// <summary>
	/// Used to ensure thread safety when adding or reading the list of 
	/// error loggers.
	/// </summary>
	CriticalSection error_critical_section;

	/// <summary>
	/// Set up the log manager.
	/// </summary>
	LogManager();

	/// <summary>
	/// Clean up the log manager.
	/// </summary>
	~LogManager();

	/// <summary>
	/// Builds up a log string and outputs it to various places depending on 
	/// the log flags.
	/// </summary>
	/// <param name="tag">The tag we are logging.</param>
	/// <param name="message">The message to log.</param>
	/// <param name="function_name">The name of the function calling log.</param>
	/// <param name="source_file">The source file log is called from.</param>
	/// <param name="line_number">The line log is called from.</param>
	void log(const std::string& tag, const std::string& message,
		const char* function_name, const char* source_file,
		unsigned int line_number);

	/// <summary>
	/// Set the log flags for a tag. Passing a flag of 0 will remove any 
	/// flags for that tag.
	/// </summary>
	/// <param name="tag">The tag to set.</param>
	/// <param name="flags">Flags specifying where that tag gets logged.
	/// </param>
	void set_display_flags(const std::string& tag, unsigned char flags);

	/// <summary>
	/// Track a new error logger.
	/// </summary>
	/// <param name="logger">The logger to track.</param>
	void add_error_logger(Logger::ErrorLogger* logger);

	/// <summary>
	/// Log an error, and show an error dialog to the user.
	/// </summary>
	/// <param name="error_message">The error message to show.</param>
	/// <param name="fatal">Whether the error is fatal.</param>
	/// <param name="function_name">The function where this was called from.
	/// </param>
	/// <param name="source_file">The source file this was called from.</param>
	/// <param name="line_number">The line number this was called from.</param>
	/// <returns>The result of the user's choice on the dialog.</returns>
LogManager::ErrorDialogResult error(const std::string& error_message,
	bool fatal, const char* function_name, const char* source_file,
	unsigned int line_number);

private:
	/// <summary>
	/// Outputs the supplied buffer to the appropriate place(s), based on the
	/// supplied flags.
	/// </summary>
	/// <param name="final_buffer">The final log outputting.</param>
	/// <param name="flags">The flags indicating where to log.</param>
	void output_buffer_to_logs(const std::string& final_buffer,
		unsigned char flags);

	/// <summary>
	/// Write to a log file.
	/// </summary>
	/// <param name="data">The data to write.</param>
	void write_to_log_file(const std::string& data);

	/// <summary>
	/// Format a message and return it back out in the first parameter.
	/// </summary>
	/// <param name="out_output_buffer">The buffer to write the formatted
	/// message to, is an out parameter.</param>
	/// <param name="tag">The tag we are logging.</param>
	/// <param name="message">The message to log, pre-formatting.</param>
	/// <param name="function_name">The function that log was called from.
	/// </param>
	/// <param name="source_file">The file that log was called from.</param>
	/// <param name="line_number">The line number that log was called from.
	/// </param>
	void format_message(std::string& output_buffer,
		const std::string& tag, const std::string& message,
		const char* function_name, const char* source_file,
		unsigned int line_number);
};

static LogManager* log_manager = NULL;

#pragma endregion

#pragma region LogManager definition

LogManager::LogManager()
{
	set_display_flags("ERROR", DEFAULT_FLAG_ERROR);
	set_display_flags("WARNING", DEFAULT_FLAG_WARNING);
	set_display_flags("INFO", DEFAULT_FLAG_INFO);
}

LogManager::~LogManager()
{
	error_critical_section.lock();
	for (auto it = error_loggers.begin(); it != error_loggers.end(); ++it)
	{
		Logger::ErrorLogger* logger = *it;
		delete logger;
	}
	error_loggers.clear();
	error_critical_section.unlock();
}

void LogManager::log(const std::string& tag, const std::string& message,
	const char* function_name, const char* source_file,
	unsigned int line_number)
{
	tag_critical_section.lock();
	Tags::iterator result = tags.find(tag);
	if (result != tags.end()) {
		tag_critical_section.unlock();

		std::string buffer;
		format_message(buffer, tag, message, function_name, source_file,
			line_number);
		output_buffer_to_logs(buffer, result->second);
	}
	else
	{
		tag_critical_section.unlock();
	}
}

void LogManager::set_display_flags(const std::string& tag, unsigned char flags)
{
	tag_critical_section.lock();

	if (flags == 0)
	{
		tags.erase(tag);
	}
	else
	{
		Tags::iterator result = tags.find(tag);
		if (result != tags.end())
		{
			result->second = flags;
		}
		else
		{
			tags.insert(std::make_pair(tag, flags));
		}
	}

	tag_critical_section.unlock();
}

void LogManager::add_error_logger(Logger::ErrorLogger* logger)
{
	error_critical_section.lock();
	error_loggers.push_back(logger);
	error_critical_section.unlock();
}

LogManager::ErrorDialogResult LogManager::error(
	const std::string& error_message, bool fatal, const char* function_name,
	const char* source_file, unsigned int line_number)
{
	std::string tag = fatal ? "FATAL" : "ERROR";

	std::string buffer;
	format_message(buffer, tag, error_message, function_name, source_file,
		line_number);

	// Log first, dialog later
	tag_critical_section.lock();
	Tags::iterator result = tags.find(tag);
	if (result != tags.end())
	{
		output_buffer_to_logs(buffer, result->second);
	}
	tag_critical_section.unlock();

	// Show a dialog box, with an error icon, defaulting to abort
	int response = MessageBoxA(nullptr, buffer.c_str(), tag.c_str(), 
		MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON1);

	switch (response)
	{
	case IDABORT: 
		__debugbreak();// breaks into the debugger
		return LogManager::LOG_MANAGER_ERROR_RETRY;
	case IDIGNORE:
		return LogManager::LOG_MANAGER_ERROR_IGNORE;
	case IDRETRY:
	default:
		return LogManager::LOG_MANAGER_ERROR_RETRY;
	}

}

void LogManager::output_buffer_to_logs(const std::string& final_buffer,
	unsigned char flags)
{
	if ((flags & FLAG_WRITE_TO_DEBUGGER ) > 0)
	{
		OutputDebugStringA(final_buffer.c_str());
	}
	if ((flags & FLAG_WRITE_TO_LOG_FILE) > 0)
	{
		write_to_log_file(final_buffer.c_str());
	}
}

void LogManager::write_to_log_file(const std::string& data)
{
	FILE* log_file = nullptr;
	// Opens for reading and appending. Creates the file if it doesn't exist.
	fopen_s(&log_file, ERROR_LOG_FILENAME, "a+");
	if (!log_file)
	{
		// Can't open log file, so logging here would be kinda pointless
		return;
	}
	fprintf_s(log_file, data.c_str());
	fclose(log_file);
}

void LogManager::format_message(std::string& output_buffer,
	const std::string& tag, const std::string& message,
	const char* function_name, const char* source_file,
	unsigned int line_number)
{
	if (tag.empty())
	{
		output_buffer = message;
	}
	else
	{
		output_buffer = "[" + tag + "] " + message;
	}

	const bool has_extra_data = function_name != nullptr
		|| source_file != nullptr || line_number != 0;

	if (has_extra_data)
	{
		output_buffer += "\n( ";
	}
	if (function_name != nullptr)
	{
		output_buffer += "Function: ";
		output_buffer += function_name;
	}
	if (line_number != 0)
	{
		output_buffer += " Line: ";
		char line_number_buffer[10+1];
		memset(line_number_buffer, 0, sizeof(char));
		
		output_buffer += _itoa_s(line_number, line_number_buffer, 10);
	}
	if (source_file != nullptr)
	{
		output_buffer += "Function: ";
		output_buffer += source_file;
	}
	if (has_extra_data)
	{
		output_buffer += " )";
	}
	output_buffer += "\n";
}

#pragma endregion

#pragma region ErrorLogger definition

Logger::ErrorLogger::ErrorLogger()
	: enabled(true)
{
	log_manager->add_error_logger(this);
}

void Logger::ErrorLogger::log_error(const std::string& error_message,
	bool fatal, const char* function_name, const char* source_file,
	unsigned int line_number)
{
	if (enabled)
	{
		if (log_manager->error(error_message, fatal, function_name, 
			source_file, line_number))
		{
			enabled = false;
		}
	}
}

#pragma endregion

#pragma region Logger function definitions

namespace Logger
{
	void init()
	{
		if (!log_manager)
		{
			log_manager = ALLOC LogManager;
		}
	}

	void destroy()
	{
		SAFE_DELETE(log_manager);
	}

	void log(const std::string& tag, const std::string& error_message,
		const char* function_name, const char* source_file,
		unsigned int line_number)
	{
		LOG_ASSERT(log_manager);
		log_manager->log(tag, error_message, function_name, source_file,
			line_number);
	}

	void set_display_flags(const std::string& tag, unsigned char flags)
	{
		LOG_ASSERT(log_manager);
		log_manager->set_display_flags(tag, flags);
	}
}

#pragma endregion