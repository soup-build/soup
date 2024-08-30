
#pragma once
#include "Helpers.h"

namespace Monitor
{
	class ConnectionManagerBase
	{
	private:
		std::mutex pipeMutex;

	public:
		ConnectionManagerBase() :
			pipeMutex()
		{
			DebugTrace("ConnectionManagerBase::ConnectionManagerBase");
		}

		void Initialize(int32_t traceProcessId)
		{
			DebugTrace("ConnectionManagerBase::Initialize");
			{
				auto lock = std::lock_guard<std::mutex>(pipeMutex);
				Connect(traceProcessId);
			}

			// Notify that we are connected
			Message message;
			message.Type = MessageType::Initialize;
			message.ContentSize = 0;
			WriteMessage(message);
		}

		void Shutdown()
		{
			DebugTrace("ConnectionManagerBase::Shutdown");
			auto lock = std::lock_guard<std::mutex>(pipeMutex);
			Message message;
			message.Type = MessageType::Shutdown;
			message.ContentSize = 0;
			UnsafeWriteMessage(message);

			Disconnect();
		}

		void WriteMessage(const Message& message)
		{
			auto lock = std::lock_guard<std::mutex>(pipeMutex);
			UnsafeWriteMessage(message);
		}

		void DebugError(std::string_view message, uint32_t value)
		{
			printf("DETOUR-CLIENT-ERROR: %s %u\n", message.data(), value);
		}

		void DebugError(std::string_view message)
		{
			printf("DETOUR-CLIENT-ERROR: %s\n", message.data());
		}

	#ifdef TRACE_DETOUR_CLIENT
		template<typename... Args>
		static void DebugTrace(std::string_view message, Args&&... args)
		{
			auto result = std::vformat(message, std::make_format_args(args...))
			printf("DETOUR-CLIENT: %s\n", result.c_str());
		}
	#else
		template<typename... Args>
		static void DebugTrace(std::string_view, Args&&...)
		{
			// NO-OP
		}
	#endif

	protected:
		virtual void Connect(int32_t traceProcessId) = 0;
		virtual void Disconnect() = 0;
		virtual void UnsafeWriteMessage(const Message& message) = 0;
	};
}