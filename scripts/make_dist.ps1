# Empaqueta UE_Editor.exe + recursos en un ZIP portable listo para descargar.
# Uso:
#   powershell -ExecutionPolicy Bypass -File scripts/make_dist.ps1 -BuildDir build -Config Release
param(
    [string]$BuildDir = "",   # carpeta de CMake (por defecto: <raiz>/build)
    [string]$Config   = "Release",
    [string]$Version  = ""    # versión (por defecto: última etiqueta git o "0.1.0")
)

$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if (-not $BuildDir) { $BuildDir = Join-Path $Root "build" }

# Localiza UE_Editor.exe dentro de la carpeta de compilación.
$exe = Get-ChildItem -Path $BuildDir -Recurse -Filter "UE_Editor.exe" -ErrorAction SilentlyContinue |
       Where-Object { $_.FullName -match $Config } | Select-Object -First 1
if (-not $exe) {
    $exe = Get-ChildItem -Path $BuildDir -Recurse -Filter "UE_Editor.exe" -ErrorAction SilentlyContinue |
           Select-Object -First 1
}
if (-not $exe) { throw "No se encontro UE_Editor.exe en '$BuildDir'." }

if (-not $Version) {
    $Version = & git -C $Root describe --tags --abbrev=0 2>$null
    if (-not $Version) { $Version = "0.1.0" }
    $Version = $Version.TrimStart("v")
}

$distName = "UniversalEngine-Editor-$Version-win-x64"
$distDir  = Join-Path $Root ("dist\" + $distName)
$zipPath  = Join-Path $Root ("dist\" + $distName + ".zip")

$distAssets = Join-Path $distDir "assets"

# --- Preparar la carpeta de distribución -------------------------------
if (Test-Path $distDir) { Remove-Item $distDir -Recurse -Force }
New-Item -ItemType Directory -Path $distDir -Force | Out-Null
New-Item -ItemType Directory -Path $distAssets -Force | Out-Null

Copy-Item $exe.FullName (Join-Path $distDir "UE_Editor.exe")

$templates = Join-Path $Root "assets\templates"
$branding  = Join-Path $Root "assets\branding"
if (Test-Path $templates) { Copy-Item $templates $distAssets -Recurse -Force }
if (Test-Path $branding)  { Copy-Item $branding  $distAssets -Recurse -Force }

Copy-Item (Join-Path $Root "README.md") $distDir -Force
if (Test-Path (Join-Path $Root "docs")) { Copy-Item (Join-Path $Root "docs") (Join-Path $distDir "docs") -Recurse -Force }

# Instrucciones breves para el usuario final (ASCII para evitar problemas de codificacion).
$instructions = @"
UNIVERSAL ENGINE - EDITOR  (version $Version)
==========================================

QUE ES:
  Herramienta de creacion 3D: escenas, objetos, materiales, animacion por
  fotogramas, captura de imagenes BMP y grabacion de video, con interfaz en espanol.

COMO SE USA:
  1. Descomprima este ZIP en una carpeta (por ejemplo C:\Pepe\UniversalEngine).
  2. Ejecute UE_Editor.exe (doble clic). No necesita instalar nada mas.
  3. Al abrir, use "Menú > Nuevo proyecto" o "Menú > Abrir proyecto"
     (la escena demo esta en compilar), o el boton "Plantillas" (Escena demo / Vacia).

CONTROLES (Ventana de vista 3D):
  - Rueda del raton ......... Zoom
  - Boton medio (MMB) ....... Orbitar la camara
  - Shift + MMB ............. Mover (pan)
  - 1 / 3 / 7 ............... Vistas front, lateral y superior
  - F ....................... Enfocar la camara en la seleccion
  - Shift + F ............... Modo vuelo (WASD + E/Q, raton para mirar)
  - Q / W / E / R / T ....... Herramientas: mover, girar, escalar, seleccionar
  - Supr .................... Eliminar la seleccion
  - Ctrl+G / Ctrl+A / Ctrl+S  Rejilla / Ejes / Sombreado (alambrico)
  - Ctrl+R .................. Grabar animacion (archivos BMP en Mis Documentos)

PROYECTOS Y SALIDAS (se crean solos en Mis Documentos\UNIVERSAL ENGINE):
  - Proyectos\   escenas en formato .ueproj (JSON)
  - Capturas\    imagenes BMP del viewport
  - Recordings\  secuencias BMP de video

REQUISITOS:
  - Windows 10/11 de 64 bits
  - Tarjeta grafica compatible con OpenGL 4.3 o superior
  - Internet no es necesario (no requiere backend a la nube)

SOLUCION DE PROBLEMAS:
  - Si la ventana no abre o la grafica es antigua, actualice los drivers
    de la tarjeta grafica.
  - Las plantillas de la demo se leen de la carpeta "assets\templates" que
    esta junto al ejecutable: no la borre.
"@

Set-Content -Path (Join-Path $distDir "INSTRUCCIONES.txt") -Value $instructions -Encoding Ascii

# --- Generar ZIP --------------------------------------------------------
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Compress-Archive -Path ($distDir + "\*") -DestinationPath $zipPath -CompressionLevel Optimal

Write-Host ""
Write-Host "Empaquetado correcto:"
Write-Host "  ZIP:      $zipPath"
Write-Host "  Carpeta:  $distDir"
Write-Host ""