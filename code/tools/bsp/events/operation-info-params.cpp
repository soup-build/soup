module;
#include <optional>
#include <stdexcept>
#include <string>
export module BSP:OperationInfoParams;
import json11;
import Opal;

namespace BSP {
	export class OperationInfoParams {
	public:
		/**
		 * The path to the file that is being requested.
		 */
		Opal::Path File;

	public:
		static OperationInfoParams Parse(json11::Json &value) {
			auto result = OperationInfoParams();

			if (!value.is_object())
				throw std::runtime_error("operationInfo params must be an object");

			auto &object = value.object_items();

			if (!object.contains("file"))
				throw std::runtime_error("operationInfo params missing 'file'");

			result.File = Opal::Path(object.at("file").string_value());

			return result;
		}
	};
}
