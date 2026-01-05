# PowerShell script to patch ximapng.cpp for libpng 1.6+ compatibility
#
# This script replaces direct struct access (info_ptr->field) with
# accessor functions (png_get_xxx) as per libpng 1.6 API

$file = "c:\Fragua\eMule-Aishor-Forge\CxImage\ximapng.cpp"
$content = Get-Content $file -Raw

# Backup original
$content | Set-Content "$file.backup" -NoNewline

# READ operations - replace with png_get_xxx() functions
# These need to be done in specific order to avoid conflicts

# Width and height
$content = $content -replace 'info_ptr->width(?![a-z_])', 'png_get_image_width(png_ptr, info_ptr)'
$content = $content -replace 'info_ptr->height(?![a-z_])', 'png_get_image_height(png_ptr, info_ptr)'

# Color type and bit depth
$content = $content -replace 'info_ptr->color_type(?![a-z_])', 'png_get_color_type(png_ptr, info_ptr)'
$content = $content -replace 'info_ptr->bit_depth(?![a-z_])', 'png_get_bit_depth(png_ptr, info_ptr)'

# Channels
$content = $content -replace 'info_ptr->channels(?![a-z_])', 'png_get_channels(png_ptr, info_ptr)'

# Pixel depth - calculated field
$content = $content -replace 'info_ptr->pixel_depth(?![a-z_])', '(png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr))'

# Rowbytes
$content = $content -replace 'info_ptr->rowbytes(?![a-z_])', 'png_get_rowbytes(png_ptr, info_ptr)'

# Interlace type
$content = $content -replace 'info_ptr->interlace_type(?![a-z_])', 'png_get_interlace_type(png_ptr, info_ptr)'

# Save patched file
$content | Set-Content $file -NoNewline

Write-Host "Parche aplicado exitosamente. Backup guardado en ximapng.cpp.backup"
