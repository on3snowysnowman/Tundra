/**
 * @file GenDynamicArray.cpp
 * @author Joel name (you@domain.com)
 * @brief Generates DynamicArray header files for specified types.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>

int main(int argc, char** argv) 
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <type>\n";
        return -1;
    }

    const std::string OUT_DIR = "../../include/tundra/containers";
    std::string type = argv[1];
    std::string type_upper = type;
    std::transform(
        type_upper.begin(), type_upper.end(),
        type_upper.begin(),
        [](unsigned char c){ return std::toupper(c); }
    );
    std::string header_path = OUT_DIR + "/DynamicArray" + type + ".h";
    std::ofstream out_stream(header_path);

    if (!out_stream.is_open()) {
        std::cerr << "Failed to open file: " << header_path << '\n';
        return -1;
    }

    out_stream << "\n#include \"tundra/internal/MacroHelper.h\"\n\n";
    out_stream << "#ifndef TUNDRA_DYNAMICARRAY" << type_upper << "_H\n";
    out_stream << "#define TUNDRA_DYNAMICARRAY" << type_upper << "_H\n\n";
    out_stream << "#define TYPE " << type << "\n";
    out_stream << "#define NAME TUNDRA_CONCAT(Tundra_DynamicArray, TYPE)\n";
    out_stream << "#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, "
        "TYPE, _##name)\n";
    out_stream << "#define INT_FUNC_NAME(name) "
        "TUNDRA_CONCAT3(InTundra_DynArr, TYPE, _##name)\n";
    out_stream << "#include \"tundra/internal/DynamicArrayTemplate.h\"\n";
    out_stream << "#undef TYPE\n";
    out_stream << "#undef NAME\n";
    out_stream << "#undef FUNC_NAME\n";
    out_stream << "#undef INT_FUNC_NAME\n\n";
    out_stream << "#endif\n";

    out_stream.close();

    return 0;
}
