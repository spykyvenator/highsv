#define AppName "highsv"
#define AppExe "highsv.exe"

[Setup]
AppName=highsv
AppVersion=alpha
WizardStyle=modern
DefaultDirName={autopf}\highsv
DefaultGroupName=highsv
UninstallDisplayIcon={app}\uninstall.exe
Compression=zip
SolidCompression=yes

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
  GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

  [Run]
  Filename: "{app}/{#AppExe}"; Description: "Launch Application"; Flags: postinstall skipifsilent
[Files]
Source: "highsv.exe"; DestDir: "{app}"
Source: "COMCTL32.dll"; DestDir: "{app}"
Source: "libfreetype-6.dll"; DestDir: "{app}"
Source: "libpango-1.0-0.dll"; DestDir: "{app}"
Source: "libfribidi-0.dll"; DestDir: "{app}"
Source: "libpangocairo-1.0-0.dll"; DestDir: "{app}"
Source: "gdiplus.dll"; DestDir: "{app}"
Source: "libgdk_pixbuf-2.0-0.dll"; DestDir: "{app}"
Source: "libpangoft2-1.0-0.dll"; DestDir: "{app}"
Source: "libgio-2.0-0.dll"; DestDir: "{app}"
Source: "libpangowin32-1.0-0.dll"; DestDir: "{app}"
Source: "libLerc.dll"; DestDir: "{app}"
Source: "libglib-2.0-0.dll"; DestDir: "{app}"
Source: "libpcre2-8-0.dll"; DestDir: "{app}"
Source: "libbrotlicommon.dll"; DestDir: "{app}"
Source: "libgmodule-2.0-0.dll"; DestDir: "{app}"
Source: "libpixman-1-0.dll"; DestDir: "{app}"
Source: "libbrotlidec.dll"; DestDir: "{app}"
Source: "libgobject-2.0-0.dll"; DestDir: "{app}"
Source: "libpng16-16.dll"; DestDir: "{app}"
Source: "libbz2-1.dll"; DestDir: "{app}"
Source: "libgraphene-1.0-0.dll"; DestDir: "{app}"
Source: "libsharpyuv-0.dll"; DestDir: "{app}"
Source: "libc++.dll"; DestDir: "{app}"
Source: "libgraphite2.dll"; DestDir: "{app}"
Source: "libthai-0.dll"; DestDir: "{app}"
Source: "libcairo-2.dll"; DestDir: "{app}"
Source: "libgtk-4-1.dll"; DestDir: "{app}"
Source: "libtiff-6.dll"; DestDir: "{app}"
Source: "libcairo-gobject-2.dll"; DestDir: "{app}"
Source: "libharfbuzz-0.dll"; DestDir: "{app}"
Source: "libunwind.dll"; DestDir: "{app}"
Source: "libcairo-script-interpreter-2.dll"; DestDir: "{app}"
Source: "libharfbuzz-subset-0.dll"; DestDir: "{app}"
Source: "libwebp-7.dll"; DestDir: "{app}"
Source: "libdatrie-1.dll"; DestDir: "{app}"
Source: "libiconv-2.dll"; DestDir: "{app}"
Source: "libzstd.dll"; DestDir: "{app}"
Source: "libdeflate.dll"; DestDir: "{app}"
Source: "libintl-8.dll"; DestDir: "{app}"
Source: "msys-highs-1.dll"; DestDir: "{app}"
Source: "libepoxy-0.dll"; DestDir: "{app}"
Source: "libjbig-0.dll"; DestDir: "{app}"
Source: "libexpat-1.dll"; DestDir: "{app}"
Source: "libjpeg-8.dll"; DestDir: "{app}"
Source: "vulkan-1.dll"; DestDir: "{app}"
Source: "libffi-8.dll"; DestDir: "{app}"
Source: "liblzma-5.dll"; DestDir: "{app}"
Source: "zlib1.dll"; DestDir: "{app}"
Source: "libfontconfig-1.dll"; DestDir: "{app}"
Source: "liblzo2-2.dll"; DestDir: "{app}"
Source: "schemas/gschemas.compiled"; DestDir: "{app}/share/glib-2.0/schemas/"
Source: "schemas/org.gtk.gtk4.Settings.FileChooser.gschema.xml"; DestDir: "{app}/share/glib-2.0/schemas/"
Source: "../data/ui/icon.ico"; DestDir: "{app}"

[Icons]
Name: "{userdesktop}\highsv"; Filename: "{app}/{#AppExe}"; \
  IconFilename: "{app}/icon.ico"; Tasks: desktopicon