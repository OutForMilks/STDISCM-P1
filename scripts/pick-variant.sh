#!/usr/bin/env bash
# Asks which variant to run and prints the chosen directory name on stdout.
#
# The menu and the prompt go to stderr, so the caller can capture just the
# answer with $(...). The reply is read from /dev/tty for the same reason.
#
# Usage: pick-variant.sh [preselection]
#   preselection may be "2", "v2", or a full directory name; when it is given
#   (and valid) nothing is asked. An empty argument means "ask".

set -euo pipefail

root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)

# Discover the variants instead of hardcoding them, so adding a fifth folder
# needs no change here.
variants=()
while IFS= read -r dir; do
    variants+=("$dir")
done < <(cd "$root" && printf '%s\n' variant*/ | sed 's:/$::' | sort)

if [ ${#variants[@]} -eq 0 ]; then
    echo "no variant*/ directories found in $root" >&2
    exit 1
fi

label() {
    printf '%s' "$1" | sed -E 's/^variant[0-9]+_//; s/_/ /g'
}

# Accepts "2", "v2", "variant2", or the full directory name. Prints the
# matching directory name and returns 0, or returns 1 if nothing matches.
resolve() {
    local want=${1#v}
    want=${want#ariant}

    if [[ $want =~ ^[0-9]+$ ]] && [ "$want" -ge 1 ] && [ "$want" -le ${#variants[@]} ]; then
        printf '%s\n' "${variants[$((want - 1))]}"
        return 0
    fi
    local v
    for v in "${variants[@]}"; do
        if [ "$1" = "$v" ]; then
            printf '%s\n' "$v"
            return 0
        fi
    done
    return 1
}

preselected=${1:-}
if [ -n "$preselected" ]; then
    if resolve "$preselected"; then
        exit 0
    fi
    echo "not a known variant: $preselected" >&2
    exit 1
fi

if [ ! -t 0 ] && [ ! -e /dev/tty ]; then
    echo "no terminal to ask on; pass the variant instead, e.g. make VARIANT=2" >&2
    exit 1
fi

{
    echo
    echo "Which variant do you want to run?"
    for i in "${!variants[@]}"; do
        printf '  %d) %-34s %s\n' "$((i + 1))" "${variants[i]}" "$(label "${variants[i]}")"
    done
} >&2

while true; do
    printf 'choice [1-%d]: ' "${#variants[@]}" >&2
    if ! IFS= read -r reply < /dev/tty; then
        echo >&2
        echo "cancelled" >&2
        exit 1
    fi
    reply=${reply//[[:space:]]/}
    [ -z "$reply" ] && continue
    if resolve "$reply"; then
        exit 0
    fi
    echo "  '$reply' is not one of the choices" >&2
done
