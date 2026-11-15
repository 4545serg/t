# PROJECT NOISE - DSi port plan

Phase 0: toolchain + both 256x192 screens up. DONE (superseded by phase 1).
Phase 1: core player - SD browser, MP3 (minimp3) + WAV playback via Maxmod streaming, transport. DONE on BlocksDS, unverified on hardware - see BUILD.md "Status".
Phase 2: UI at 256x192 - bottom browser, top now-playing, settings; keep the existing palette and layout logic. NEXT.
Phase 3: cover thumbnails (pre-scaled), reduced glow, visualizer.
Phase 4: favorites, queue, search, settings persistence, resume; FLAC only if hardware benchmarks allow.
Phase 5: real DSi banner/icon art (carried over from the 3DS boot_art), TWiLight Menu++ check.

Toolchain note: this port moved from devkitARM+libnds to BlocksDS (see
BUILD.md) because devkitPro's own maintainer has said pacman installs
aren't meant to run in CI, and BlocksDS ships an official Docker image
with everything preinstalled instead. BlocksDS's default ARM7 core
already includes Maxmod + libnds, so there's no custom ARM7 binary in
this project (there was in the first, devkitARM-based attempt).

Removed for DSi: citro2d/citro3d GPU rendering (replaced by libnds 2D),
curl/mbedtls HTTPS lyrics (no practical TLS stack on DSi - needs a
plain-HTTP proxy or stays 3DS-only), ARM11-specific code, the 3DS
on-screen keyboard (custom DSi keyboard is phase 4, same as the 3DS
build's 4th round of changes).
