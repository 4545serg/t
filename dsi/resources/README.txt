icon.bmp: a placeholder 32x32 icon for the DSi menu/TWiLight Menu++ icon
(generated automatically, 8-bit indexed). ndstool wants a 4-bit/16-color
32x32 BMP for the icon - if the CI build fails on the `ndstool` step with
a complaint about the icon format, that's the first thing to fix: re-save
resources/icon.bmp as 4bpp/16-color (any image editor's "16 colors" BMP
export, or `convert icon.bmp -colors 16 -depth 4 icon.bmp` with
ImageMagick). Real artwork (carried over from the 3DS boot_art) is phase 5
- see ../PORT_PLAN.md.
