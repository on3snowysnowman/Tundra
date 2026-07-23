/**
 * @file generate.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generation program for creating specific type instance files of the
 * templated Tundra containers.
 * @version 0.1
 * @date 2026-04-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <iostream>
#include <cstdint>
#include <limits>
#include <fstream>
#include <filesystem>

void pause()
{
    std::cout << "Press enter to continue...";
    std::cin.get();
}

void clear_screen()
{
    std::cout << "\033[2J\033[H" << std::endl;
}

void clear_input()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_invalid_sel()
{
    clear_screen();
    std::cout << "Invalid input\n";
    pause();
}

template<typename T>
T prompt_type(const std::string& msg)
{
    while(true)
    {
        clear_screen();
        std::cout << msg;

        std::string line;
        std::getline(std::cin, line);

        std::stringstream ss(line);

        T value;

        if(ss >> value)
            return value;

        std::cerr << "INVALID\n";

        print_invalid_sel();
    }
}

std::string prompt_directory(const std::string &new_file_name)
{
    static constexpr const char *DEF_PATH = "../include/tundra/containers";
    
    std::string in;

    while(true)
    {
        clear_screen();
        std::cout << "Directory to place generated file? "
            "(Enter \'0\' for default: " << DEF_PATH <<  " )\n\n >> ";

        std::getline(std::cin, in);

        if(!std::cin)
        {
            clear_input();
            print_invalid_sel();
            continue;
        }

        if(in == "0") in = DEF_PATH;

        std::string full_path = in + '/' + new_file_name;

        if(!std::filesystem::exists(full_path))
        {
            return full_path;
        }

        bool continue_loop = false;

        while(true)
        {
            char c = prompt_type<char>("The file: " + full_path + " already exists, "
            "overwrite it (y/n)? WARNING: This cannot be undone.\n\n >> ");

            if(c == 'y') return full_path;

            if(c == 'n') 
            {
                continue_loop = true;
                break;
            }

            print_invalid_sel();
            continue;
        }

        if(continue_loop) continue;

        return full_path;
    }
}

struct TypeInfo
{
    bool custom_copy = false;
    bool custom_move = false;
    bool custom_free = false;
    bool custom_init = false;
};

TypeInfo prompt_type_info()
{
    TypeInfo info;

    const char *msg = "Does type need custom copy handling? (y/n)\n\n >> ";

    while(true)
    {
        char selected = prompt_type<char>(msg);

        if(selected != 'y' && selected != 'n')
        {
            clear_input();
            print_invalid_sel();
            continue;
        }

        info.custom_copy = selected == 'y';
        break;
    }

    msg = "Does type need custom move handling? (y/n)\n\n >> ";

    while(true)
    {
        char selected = prompt_type<char>(msg);

        if(selected != 'y' && selected != 'n')
        {
            clear_input();
            print_invalid_sel();
            continue;
        }

        info.custom_move = selected == 'y';
        break;
    }

    msg = "Does type need custom free handling? (y/n)\n\n >> ";

    while(true)
    {
        char selected = prompt_type<char>(msg);

        if(selected != 'y' && selected != 'n')
        {
            clear_input();
            print_invalid_sel();
            continue;
        }

        info.custom_free = selected == 'y';
        break;
    }

    msg = "Does type need custom default initialization? (y/n)\n\n >> ";

    while(true)
    {
        char selected = prompt_type<char>(msg);

        if(selected != 'y' && selected != 'n')
        {
            clear_input();
            print_invalid_sel();
            continue;
        }

        info.custom_init = selected == 'y';
        break;
    }

    return info;
}

void output_type_defines(std::ostream &out_stream, const TypeInfo &t_info, 
    const std::string &type_lit, const std::string &type_name)
{
    out_stream << 
    "// Type flags for the template \n"
    "#define TUNDRA_NEEDS_CUSTOM_COPY " << t_info.custom_copy << "\n"
    "#define TUNDRA_NEEDS_CUSTOM_FREE " << t_info.custom_free << "\n"
    "#define TUNDRA_NEEDS_CUSTOM_MOVE " << t_info.custom_move << "\n"
    "#define TUNDRA_NEEDS_CUSTOM_INIT " << t_info.custom_init << "\n"
    "#define TUNDRA_TYPE " << type_lit << "\n"
    "#define TUNDRA_TYPENAME " << type_name << "\n";
}

void output_type_cleanup(std::ostream &out_stream, const TypeInfo &t_info)
{
    out_stream << 
      "// Clean up\n"
    "#undef TUNDRA_TYPE\n"
    "#undef TUNDRA_TYPENAME\n"
    "#undef TUNDRA_NEEDS_CUSTOM_COPY\n"
    "#undef TUNDRA_NEEDS_CUSTOM_FREE\n"
    "#undef TUNDRA_NEEDS_CUSTOM_MOVE\n"
    "#undef TUNDRA_NEEDS_CUSTOM_INIT\n";

    if(t_info.custom_copy)
    {
        out_stream << "#undef TUNDRA_COPY_INIT_CALL_SIG\n";
        out_stream << "#undef TUNDRA_COPY_ASSIGN_CALL_SIG\n";
    }
    if(t_info.custom_free)
    {
        out_stream << "#undef TUNDRA_FREE_CALL_SIG\n";
    }
    if(t_info.custom_move)
    {
        out_stream << "#undef TUNDRA_MOVE_INIT_CALL_SIG\n";
        out_stream << "#undef TUNDRA_MOVE_ASSIGN_CALL_SIG\n";
    }
    if(t_info.custom_init)
    {
        out_stream << "#undef TUNDRA_DEF_INIT_CALL_SIG\n";
    }
}

void output_type_functions(std::ostream &out_stream, const TypeInfo &t_info)
{
    if(t_info.custom_copy)
    {
        out_stream << 
        "\n// COPY BEHAVIOR ---------------------------------------------------------------\n\n"

        "// Macro for per element copy call from a source object to an uninitialized \n"
        "// object. Change the signature as needed, but macro name must remain the same.\n"
        "#define TUNDRA_COPY_INIT_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n\n"
    
        "// Macro for per element copy call from a source object to an initialized \n"
        "// object. Change the signature as needed, but macro name must remain the same.\n"
        "#define TUNDRA_COPY_ASSIGN_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n";
    }

    if(t_info.custom_free)
    {
        out_stream << 
        "\n// FREE BEHAVIOR ---------------------------------------------------------------\n"
        "// Macro for per element free call. Change the signature as needed, but macro \n"
        "// name must remain the same. `elem_ptr` points to the element to free.\n"
        "#define TUNDRA_FREE_CALL_SIG(elem_ptr) // User defines func call.\n";
    }

    if(t_info.custom_move)
    {
        out_stream << 
        "\n// MOVE BEHAVIOR ---------------------------------------------------------------\n\n"
        
        "// Macro for per element move call from a source object to an uninitialized \n"
        "// object. Change the signature as needed, but macro name must remain the same.\n"
        "#define TUNDRA_MOVE_INIT_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n\n"
    
        "// Macro for per element move call from a source object to an initialized \n"
        "// object. Change the signature as needed, but macro name must remain the same.\n"
        "#define TUNDRA_MOVE_ASSIGN_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n";
    }

    if(t_info.custom_init)
    {
        out_stream << 
        "\n// INIT BEHAVIOR ---------------------------------------------------------------\n\n"
        "// Macro for per element default init call on an uninitialized object.\n"
        "// Change the signature as needed, but macro name must remain the same.\n"
        "#define TUNDRA_DEF_INIT_CALL_SIG(elem_ptr) // User defines func call\n";
    }
}

void generic_container_generate_menu(const std::string &full_cont_name, 
    const std::string &abrev_cont_name, const std::string &header_guard_name)
{
    // const char * msg = "C type the Array contains (ie. MyStruct*)?\n\n >> ";

    std::string msg = "C type the " + abrev_cont_name + " contains "
        "(ie. MyStruct*)?\n\n >> ";

    std::string type_lit = prompt_type<std::string>(msg);

    msg = "Header-guard friendly name for this type "
        "(ie. MyStruct_ptr)?\n\n >> ";

    std::string type_name = prompt_type<std::string>(msg);

    TypeInfo t_info = prompt_type_info();

    std::string new_file_name = full_cont_name + type_name + ".h";

    std::cerr << "New file name: " << new_file_name << '\n';

    std::string out_directory = prompt_directory(new_file_name);

    std::ofstream out_file(out_directory);

    if(!out_file)
    {
        std::cerr << "Failed to open output directory: " << out_directory << 
            '\n';
        exit(1);
    }

    out_file << "\n#ifndef " << header_guard_name << type_name << "_H\n"
        "#define " << header_guard_name << type_name << "_H\n\n"
        "#include \"tundra/internal/MacroHelper.h\"\n\n";
        
    output_type_defines(out_file, t_info, type_lit, type_name);

    output_type_functions(out_file, t_info);

    // if(t_info.custom_copy)
    // {
    //     out_file << 
    //     "\n// COPY BEHAVIOR ---------------------------------------------------------------\n\n"

    //     "// Macro for per element copy call from a source object to an uninitialized \n"
    //     "// object. Change the signature as needed, but macro name must remain the same.\n"
    //     "#define TUNDRA_COPY_INIT_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n\n"
    
    //     "// Macro for per element copy call from a source object to an initialized \n"
    //     "// object. Change the signature as needed, but macro name must remain the same.\n"
    //     "#define TUNDRA_COPY_ASSIGN_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n";
    // }

    // if(t_info.custom_free)
    // {
    //     out_file << 
    //     "\n// FREE BEHAVIOR ---------------------------------------------------------------\n"
    //     "// Macro for per element free call. Change the signature as needed, but macro \n"
    //     "// name must remain the same. `elem_ptr` points to the element to free.\n"
    //     "#define TUNDRA_FREE_CALL_SIG(elem_ptr) // User defines func call.\n";
    // }

    // if(t_info.custom_move)
    // {
    //     out_file << 
    //     "\n// MOVE BEHAVIOR ---------------------------------------------------------------\n\n"
        
    //     "// Macro for per element move call from a source object to an uninitialized \n"
    //     "// object. Change the signature as needed, but macro name must remain the same.\n"
    //     "#define TUNDRA_MOVE_INIT_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n\n"
    
    //     "// Macro for per element move call from a source object to an initialized \n"
    //     "// object. Change the signature as needed, but macro name must remain the same.\n"
    //     "#define TUNDRA_MOVE_ASSIGN_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n";
    // }

    // if(t_info.custom_init)
    // {
    //     out_file << 
    //     "\n// INIT BEHAVIOR ---------------------------------------------------------------\n\n"
    //     "// Macro for per element default init call. Change the signature as needed, but \n"
    //     "// macro name must remain the same. `elem_ptr` points to the " << type_lit << " to \n"
    //     "// default initialize. Assume `elem_ptr` does not point to a valid object.\n"
    //     "#define TUNDRA_DEF_INIT_CALL_SIG(elem_ptr) // User defines func call\n";
    // }

    out_file << 
    "\n// -----------------------------------------------------------------------------\n\n"
    "// Create specialization for the given type\n"
    "#include \"tundra/internal/container_templates/" << 
        full_cont_name << ".h\"\n\n";
  
    output_type_cleanup(out_file, t_info);

    out_file << 
    "#endif // " << header_guard_name << type_name << "_H\n";

    out_file.close();
}

void arr_generate_menu()
{
    const char * msg = "C type the Array contains (ie. MyStruct*)?\n\n >> ";

    clear_screen();
    std::cout << msg;

    // std::string type_lit = prompt_type<std::string>(msg);
    std::string type_lit;
    std::getline(std::cin, type_lit);

    msg = "Header-guard friendly name for this type "
        "(ie. MyStruct_ptr)?\n\n >> ";

    std::string type_name = prompt_type<std::string>(msg);

    msg = "Capacity of the Array (Must be non negative)?\n\n >> ";

    size_t arr_cap = prompt_type<size_t>(msg);

    std::string new_file_name = "Array" + std::to_string(arr_cap) +  
        type_name + ".h";

    std::string out_directory = prompt_directory(new_file_name);

    std::ofstream out_file(out_directory);

    if(!out_file)
    {
        std::cerr << "Failed to open output directory: " << out_directory << 
            '\n';
        exit(1);
    }

    out_file << "\n#ifndef TUNDRA_ARRAY" << type_name << arr_cap << "_H\n"
        "#define TUNDRA_DYNAMICARRAY" << type_name << arr_cap << "_H\n\n"
       "#include \"tundra/internal/MacroHelper.h\"\n\n";

    out_file << "#define TUNDRA_TYPE " << type_lit << "\n"
    "#define TUNDRA_TYPENAME " << type_name << "\n" << 
    "#define TUNDRA_CAPACITY " << arr_cap << '\n';

    out_file << 
    "\n// Create specialization for the given type\n"
    "#include \"tundra/internal/container_templates/Array.h\"\n\n";

    out_file << 
    "#endif // TUNDRA_ARR" << type_name << arr_cap << "_H\n";

    out_file.close();
}

void dynstk_generate_menu()
{
    std::string msg = "C type the Stack contains "
        "(ie. MyStruct*)?\n\n >> ";

    std::string type_lit = prompt_type<std::string>(msg);

    msg = "Header-guard friendly name for this type "
        "(ie. MyStruct_ptr)?\n\n >> ";

    std::string type_name = prompt_type<std::string>(msg);

    TypeInfo t_info = prompt_type_info();

    std::string new_file_name = "DynamicStack" + type_name + ".h";

    std::cerr << "New file name: " << new_file_name << '\n';

    std::string out_directory = prompt_directory(new_file_name);

    std::ofstream out_file(out_directory);

    if(!out_file)
    {
        std::cerr << "Failed to open output directory: " << out_directory << 
            '\n';
        exit(1);
    }

    const char * HEADER_GUARD_NAME = "TUNDRA_DYNAMICSTACK";

    out_file << "\n#ifndef " << HEADER_GUARD_NAME << type_name << "_H\n"
        "#define " << HEADER_GUARD_NAME << type_name << "_H\n\n"
        "#include \"tundra/internal/MacroHelper.h\"\n\n";
        
    out_file << "// Create dependant container" 
    "\n// -----------------------------------------------------------------------------\n" 
    "#ifndef TUNDRA_DYNAMICARRAY" << type_name << "_H\n"
    "#define TUNDRA_DYNAMICARRAY" << type_name << "_H\n#define TUNDRA_TYPE " <<
    type_lit << "\n#define TUNDRA_TYPENAME " << type_name << 
    "\n#include \"tundra/internal/container_templates/DynamicArray.h\"\n"
    "#undef TUNDRA_TYPE\n#undef TUNDRA_TYPENAME\n#endif\n\n";

    output_type_defines(out_file, t_info, type_lit, type_name);

    output_type_functions(out_file, t_info);

    // if(t_info.custom_copy)
    // {
    //     out_file << 
    //     "\n// COPY BEHAVIOR ---------------------------------------------------------------\n"

    //     "// Macro for per element copy call. Change the signature as needed, but macro \n"
    //     "// name must remain the same. `src_ptr` points to the " << type_lit << " to copy from, \n"
    //     "// `dst_ptr` points to the " << type_lit << " to copy to. Assume `dst_ptr` does not \n"
    //     "// point to a valid object.\n"
    //     "#define TUNDRA_COPY_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n";
    // }

    // if(t_info.custom_free)
    // {
    //     out_file << 
    //     "\n// FREE BEHAVIOR ---------------------------------------------------------------\n"
    //     "// Macro for per element free call. Change the signature as needed, but macro \n"
    //     "// name must remain the same. `elem_ptr` points to the element to free.\n"
    //     "#define TUNDRA_FREE_CALL_SIG(elem_ptr) // User defines func call.\n";
    // }

    // if(t_info.custom_move)
    // {
    //     out_file << 
    //     "\n// MOVE BEHAVIOR ---------------------------------------------------------------\n"
    //     "// Macro for per element move call. Change the signature as needed, but macro \n"
    //     "// name must remain the same. `src_ptr` points to the " << type_lit << " to move from,\n"
    //     "// `dst_ptr` points to the " << type_lit << " to move to. Assume `dst_ptr` does not \n"
    //     "// point to a valid object.\n"
    //     "#define TUNDRA_MOVE_CALL_SIG(dst_ptr, src_ptr) // User defines func call.\n";
    // }

    // if(t_info.custom_init)
    // {
    //     out_file << 
    //     "\n// INIT BEHAVIOR ---------------------------------------------------------------\n"
    //     "// Macro for per element default init call. Change the signature as needed, but \n"
    //     "// macro name must remain the same. `elem_ptr` points to the " << type_lit << " to \n"
    //     "// default initialize. Assume `elem_ptr` does not point to a valid object.\n"
    //     "#define TUNDRA_DEF_INIT_CALL_SIG(elem_ptr) // User defines func call\n";
    // }

    out_file << 
    "\n// -----------------------------------------------------------------------------\n"
    "// Create specialization for the given type\n"
    "#include \"tundra/internal/container_templates/DynamicStack.h\"\n\n";
  
    output_type_cleanup(out_file, t_info);

    out_file << 
    "#endif // " << HEADER_GUARD_NAME << type_name << "_H\n";

    out_file.close();
}

void container_selection_menu()
{
    enum SELECTION_TYPE
    {
        ARRAY = 1,
        DYN_ARR,
        STACK,
        DYN_STK,
        LNK_LST,
        QUIT
    };

    while(true)
    {
        clear_screen();

        int selected = prompt_type<int>("Generate which container?\n\n1: Array\n2: "
            "DynamicArray\n3: Stack\n4: DynamicStack\n5: LinkedList\n"
            "6: Quit\n\n   >> ");

        if(selected < 1 || selected > 6)
        {
            clear_input();
            print_invalid_sel();
            continue;
        }

        switch(selected)
        {
            case ARRAY:

                arr_generate_menu();
                break;

            case DYN_ARR:

                // dyn_arr_generate_menu();
                generic_container_generate_menu("DynamicArray", 
                    "Array", "TUNDRA_DYNAMICARRAY");
                break;

            case DYN_STK:

                dynstk_generate_menu();
                break;

            case LNK_LST:

                generic_container_generate_menu("LinkedList",
                    "List", "TUNDRA_LINKEDLIST");
                break;

            case QUIT:

                exit(0);
        }
    }
}

int main()
{
    container_selection_menu();
    return 0;
}