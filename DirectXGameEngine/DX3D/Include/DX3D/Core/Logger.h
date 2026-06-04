#pragma once

#include <DX3D/Core/Core.h>

namespace dx3d
{
	class Logger final
	{
		dx3d_disable_copy_and_move(Logger);
	public:
		enum class LogLevel
		{
			Error = 0,
			Warning,
			Info
		};
		explicit Logger(LogLevel logLevel = LogLevel::Error);
		~Logger();
		void log(LogLevel level, const char* message);

	private:
		LogLevel m_logLevel = LogLevel::Error;
	};

}

#define DX3DLog(logger, type, message)\
logger.log((type), message)

#define DX3DLogThrow(logger, exception, type, message)\
{\
DX3DLog(logger,type,message);\
throw exception(message);\
}

#define DX3DLogInfo(message)\
	DX3DLog(getLogger(), Logger::LogLevel::Info, message)

#define DX3DLogWarning(message)\
	DX3DLog(getLogger(), Logger::LogLevel::Warning, message)

#define DX3DLogError(message)\
	DX3DLog(getLogger(), Logger::LogLevel::Error, message)

#define DX3DLogThrowError(message)\
	DX3DLogThrow(getLogger(), std::runtime_error, Logger::LogLevel::Error, message)

#define DX3DLogThrowInvalidArg(message)\
	DX3DLogThrow(getLogger(), std::invalid_argument, Logger::LogLevel::Error, message)