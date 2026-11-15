#!/bin/sh
# Fetches the single-header MP3 decoder for the DSi build (same minimp3.h
# the 3DS build vendors - plain C99, nothing devkitARM/BlocksDS-specific).
# FLAC support is phase 4+ (see ../PORT_PLAN.md) so dr_flac.h isn't
# fetched here yet.
set -e
cd "$(dirname "$0")/.."

MINIMP3_REF="${MINIMP3_REF:-master}"

fetch() {
    url="$1"; dest="$2"; marker="$3"
    if [ -s "$dest" ]; then echo "have $dest"; return; fi
    echo "downloading $dest"
    tmp="$dest.part.$$"
    trap 'rm -f "$tmp"' EXIT INT TERM
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL --retry 3 "$url" -o "$tmp"
    elif command -v wget >/dev/null 2>&1; then
        wget -q -t 3 "$url" -O "$tmp"
    else
        echo "Need curl or wget, or download $url to $dest by hand." >&2
        exit 1
    fi
    if [ ! -s "$tmp" ] || ! grep -q "$marker" "$tmp"; then
        echo "Download of $dest failed (unexpected content from $url)" >&2
        exit 1
    fi
    mv -f "$tmp" "$dest"
    trap - EXIT INT TERM
}

fetch "https://raw.githubusercontent.com/lieff/minimp3/$MINIMP3_REF/minimp3.h" source/minimp3.h mp3dec_decode_frame
echo "dependencies ready"
