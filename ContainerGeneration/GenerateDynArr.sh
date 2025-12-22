#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage:
  gen_dynamic_array_header.sh --type "<C type>" --typename <NAME> [-o DIR] [-f]
                             [--custom-copy] [--custom-free] [--custom-move]
                             [--custom-all]

Required:
  --type "<C type>"     The actual C type expression 
  --typename <NAME>     A header-guard-friendly name for the type (must be an identifier)
                        Example: u32, MyStruct_ptr, float

Options:
  -o, --out DIR         Output directory (default: ../include/tundra/containers)
  -f, --force           Overwrite existing file
  -h, --help            Show this help

Hook flags:
  --custom-copy         Generate copy hook + enable compile-time copy path
  --custom-free         Generate free hook + enable compile-time free path
  --custom-move         Generate move hook + enable compile-time move path
  --custom-all          Equivalent to --custom-copy --custom-free --custom-move

Examples:
  gen_dynamic_array_header.sh --type uint32_t --typename u32
  gen_dynamic_array_header.sh --type MyStruct* --typename MyStruct_ptr -o include/containers
  gen_dynamic_array_header.sh --type Foo --typename Foo --custom-copy --custom-free -f
  gen_dynamic_array_header.sh --type Bar --typename Bar --custom-all
EOF
}

FORCE=0
OUT_DIR="../include/tundra/containers"
TYPE=""
TYPENAME=""

NEEDS_COPY=0
NEEDS_FREE=0
NEEDS_MOVE=0

# Manual argument parsing to support long options
while [[ $# -gt 0 ]]; do
  case "$1" in
    --type)
      [[ $# -ge 2 ]] || { echo "Error: --type requires an argument." >&2; usage; exit 2; }
      TYPE="$2"
      shift 2
      ;;
    --typename)
      [[ $# -ge 2 ]] || { echo "Error: --typename requires an argument." >&2; usage; exit 2; }
      TYPENAME="$2"
      shift 2
      ;;
    -o|--out)
      [[ $# -ge 2 ]] || { echo "Error: $1 requires an argument." >&2; usage; exit 2; }
      OUT_DIR="$2"
      shift 2
      ;;
    -f|--force)
      FORCE=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    --custom-copy)
      NEEDS_COPY=1
      shift
      ;;
    --custom-free)
      NEEDS_FREE=1
      shift
      ;;
    --custom-move)
      NEEDS_MOVE=1
      shift
      ;;
    --custom-all)
      NEEDS_COPY=1
      NEEDS_FREE=1
      NEEDS_MOVE=1
      shift
      ;;
    *)
      echo "Error: Unknown argument: $1" >&2
      usage
      exit 2
      ;;
  esac
done

# Validate required args
if [[ -z "$TYPE" ]]; then
  echo "Error: Missing required --type." >&2
  usage
  exit 2
fi

if [[ -z "$TYPENAME" ]]; then
  echo "Error: Missing required --typename." >&2
  usage
  exit 2
fi

# Validate typename is a valid preprocessor identifier component
# Rules: first char [A-Za-z_], remaining [A-Za-z0-9_]
if [[ ! "$TYPENAME" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]; then
  echo "Error: --typename must match ^[A-Za-z_][A-Za-z0-9_]*$ (identifier-safe)." >&2
  exit 2
fi

mkdir -p "$OUT_DIR"

OUT_PATH="${OUT_DIR%/}/DynamicArray${TYPENAME}.h"

if [[ -e "$OUT_PATH" && "$FORCE" -ne 1 ]]; then
  echo "Error: '$OUT_PATH' already exists. Use -f/--force to overwrite." >&2
  exit 1
fi

GENERATED_OUTPUT="#ifndef TUNDRA_DYNAMICARRAY${TYPENAME}_H
#define TUNDRA_DYNAMICARRAY${TYPENAME}_H

// Element handling flags for the template 
#define TUNDRA_NEEDS_NEEDS_CUSTOM_COPY ${NEEDS_COPY}
#define TUNDRA_NEEDS_CUSTOM_FREE ${NEEDS_FREE}
#define TUNDRA_NEEDS_CUSTOM_MOVE ${NEEDS_MOVE}
#define TUNDRA_TYPE ${TYPE}
"


# Custom copy function stub
if [[ "$NEEDS_COPY" -eq 1 ]]; then
GENERATED_OUTPUT+=\
"
/**
 * User-defined copy function for ${TYPE}.
 */
static inline void Tundra_DynArr${TYPENAME}_custom_elem_copy(const ${TYPE}* src, ${TYPE}* dst)
{
    // User implements copy logic here.
}

// Macro for per element copy call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_COPY_FUNC_SIG(dst_ptr, src_ptr) \\
  Tundra_DynArr${TYPENAME}_custom_elem_copy((const ${TYPE}*)(src_ptr), (${TYPE}*)(dst_ptr))
"
fi

# Custom free function stub
if [[ "$NEEDS_FREE" -eq 1 ]]; then
GENERATED_OUTPUT+=\
"
/**
 * User-defined free function for ${TYPE}.
 */
static inline void Tundra_DynArr${TYPENAME}_custom_elem_free(${TYPE}* array)
{
    // User implements free logic here.
}

// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_FREE_FUNC_SIG(array_ptr) \\
    Tundra_DynArr${TYPENAME}_custom_elem_free((${TYPE}*)(array_ptr))
"
fi

# Custom move function stub
if [[ "$NEEDS_MOVE" -eq 1 ]]; then
GENERATED_OUTPUT+=\
"
/** 
 * User-defined move function for ${TYPE}.
 */
static inline void Tundra_DynArr${TYPENAME}_custom_elem_move(${TYPE}* dst, ${TYPE}* src)
{
    // User implements move logic here.
}

// Macro for per element move call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_MOVE_FUNC_SIG(dst_ptr, src_ptr) \\
    Tundra_DynArr${TYPENAME}_custom_elem_move((${TYPE}*)(dst_ptr), (${TYPE}*)(src_ptr))
"
fi

GENERATED_OUTPUT+=\
"
// Create specialization for the given type
#include \"tundra/internal/container_templates/DynamicArray.h\"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_NEEDS_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
"

if [[ "$NEEDS_COPY" -eq 1 ]]; then
GENERATED_OUTPUT+=\
"
#ifdef TUNDRA_COPY_FUNC_SIG
#undef TUNDRA_COPY_FUNC_SIG
#endif
"
fi

if [[ "$NEEDS_FREE" -eq 1 ]]; then
GENERATED_OUTPUT+=\
"
#ifdef TUNDRA_FREE_FUNC_SIG
#undef TUNDRA_FREE_FUNC_SIG
#endif
"
fi

if [[ "$NEEDS_MOVE" -eq 1 ]]; then
GENERATED_OUTPUT+=\
"
#ifdef TUNDRA_MOVE_FUNC_SIG
#undef TUNDRA_MOVE_FUNC_SIG
#endif
"
fi

GENERATED_OUTPUT+=\
"
#endif // TUNDRA_DYNAMICARRAY${TYPENAME}_H
"

# Output the generated output to the file
echo "$GENERATED_OUTPUT" > "$OUT_PATH"

echo "Generated: $OUT_PATH"
