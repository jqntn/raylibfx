$name = ($args[0] -replace '\W', '_').ToLower()
$bytes = [System.IO.File]::ReadAllBytes($args[0])
$hex = ($bytes | ForEach-Object { '0x{0:x2}' -f $_ }) -join ', '
"unsigned char $name[] = { $hex };"
"unsigned int ${name}_len = $($bytes.Length);"