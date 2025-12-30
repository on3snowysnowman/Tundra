#!/usr/bin/bash
set -euo pipefail

usage() 
{
  cat <<'EOF'
    Usage: gen_Array.sh --type <type> --typename <NAME> [-o DIR] [-f]

        Required:
            --type "<C type>"     The actual C type expression 
            -- cap <capacity>     Capacity of the Array in elements.
            --typename <NAME>     A header-guard-friendly name for the type (must be an identifier)
                                    Example: u32, MyStruct_ptr, float
            
        Options:
            -o, --out DIR         Output directory (default: ../include/tundra/containers)
            -f, --force           Overwrite existing file
            -h, --help            Show this help

        Examples:
            gen_Array.sh --type uint32 --typename u32
            gen_Array.sh --type MyStruct* --typename MyStruct_ptr -o include/containers    
EOF
}

FORCE=0
OUT_DIR="../include/tundra/containers"
TYPE=""
TYPENAME=""
CAPACITY=""

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
    --capacity)
      [[ $# -ge 2 ]] || { echo "Error: --capacity requires an argument." >&2; usage; exit 2; }
      CAPACITY="$2"
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

if [[ -z "$CAPACITY" ]]; then
  echo "Error: Missing required --capacity." >&2
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

OUT_PATH="${OUT_DIR%/}/Array${CAPACITY}${TYPENAME}.h"

if [[ -e "$OUT_PATH" && "$FORCE" -ne 1 ]]; then
  echo "Error: '$OUT_PATH' already exists. Use -f/--force to overwrite." >&2
  exit 1
fi

GENERATED_OUTPUT="#ifndef TUNDRA_ARRAY${CAPACITY}${TYPENAME}_H
#define TUNDRA_ARRAY${CAPACITY}${TYPENAME}_H

// Type flags for the template
#define TUNDRA_TYPE ${TYPE}
#define TUNDRA_CAPACITY ${CAPACITY}
#define TUNDRA_TYPENAME ${TYPENAME}

// Create specialization for the given type
#include \"tundra/internal/container_templates/Array.h\"

// Clean up 
#undef TUNDRA_TYPE
#undef TUNDRA_CAPACITY
#undef TUNDRA_TYPENAME

#endif // TUNDRA_ARRAY${CAPACITY}${TYPENAME}_H"

# Output the generated output to the file
echo "$GENERATED_OUTPUT" > "$OUT_PATH"

echo "Generated: $OUT_PATH"
