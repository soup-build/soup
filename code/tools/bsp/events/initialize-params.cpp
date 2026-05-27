module;
#include <optional>
#include <string>
export module BSP:InitializeParams;
import json11;

namespace BSP {
	export class ClientInfo {
		/**
		 * The name of the client as defined by the client.
		 */
		std::string Name;

		/**
		 * The client's version as defined by the client.
		 */
		std::optional<std::string> Version;
	};

	export class InitializeParams {
	public:
		/**
		 * The process Id of the parent process that started the server. Is null if
		 * the process has not been started by another process. If the parent
		 * process is not alive then the server should exit (see exit notification)
		 * its process.
		 */
		std::optional<int> ProcessId;

		/**
		 * Information about the client
		 *
		 * @since 3.15.0
		 */
		std::optional<ClientInfo> ClientInfo;

		/**
		 * The locale the client is currently showing the user interface
		 * in. This must not necessarily be the locale of the operating
		 * system.
		 *
		 * Uses IETF language tags as the value's syntax
		 * (See https://en.wikipedia.org/wiki/IETF_language_tag)
		 *
		 * @since 3.16.0
		 */
		std::optional<std::string> Locale;

		/**
		 * The rootPath of the workspace. Is null
		 * if no folder is open.
		 *
		 * @deprecated in favour of `rootUri`.
		 */
		std::optional<std::string> RootPath;

		/**
		 * The rootUri of the workspace. Is null if no
		 * folder is open. If both `rootPath` and `rootUri` are set
		 * `rootUri` wins.
		 *
		 * @deprecated in favour of `workspaceFolders`
		 */
		// rootUri : DocumentUri | null;

		/**
		 * User provided initialization options.
		 */
		// initializationOptions ?: LSPAny;

		/**
		 * The capabilities provided by the client (editor or tool)
		 */
		// capabilities : ClientCapabilities;

		/**
		 * The initial trace setting. If omitted trace is disabled ('off').
		 */
		// trace ?: TraceValue;

		/**
		 * The workspace folders configured in the client when the server starts.
		 * This property is only available if the client supports workspace folders.
		 * It can be `null` if the client supports workspace folders but none are
		 * configured.
		 *
		 * @since 3.6.0
		 */
		// workspaceFolders ?: WorkspaceFolder[] | null;

	public:
		static InitializeParams Parse(json11::Json &value) {
			auto result = InitializeParams();
			return result;
		}
	};
}
