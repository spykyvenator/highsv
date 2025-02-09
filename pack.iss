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
Source: "build/highsv.exe"; DestDir: "{app}"
Source: "build/COMCTL32.dll"; DestDir: "{app}"
Source: "build/libfreetype-6.dll"; DestDir: "{app}"
Source: "build/libpango-1.0-0.dll"; DestDir: "{app}"
Source: "build/libfribidi-0.dll"; DestDir: "{app}"
Source: "build/libpangocairo-1.0-0.dll"; DestDir: "{app}"
Source: "build/gdiplus.dll"; DestDir: "{app}"
Source: "build/libgdk_pixbuf-2.0-0.dll"; DestDir: "{app}"
Source: "build/libpangoft2-1.0-0.dll"; DestDir: "{app}"
Source: "build/libgio-2.0-0.dll"; DestDir: "{app}"
Source: "build/libpangowin32-1.0-0.dll"; DestDir: "{app}"
Source: "build/libLerc.dll"; DestDir: "{app}"
Source: "build/libglib-2.0-0.dll"; DestDir: "{app}"
Source: "build/libpcre2-8-0.dll"; DestDir: "{app}"
Source: "build/libbrotlicommon.dll"; DestDir: "{app}"
Source: "build/libgmodule-2.0-0.dll"; DestDir: "{app}"
Source: "build/libpixman-1-0.dll"; DestDir: "{app}"
Source: "build/libbrotlidec.dll"; DestDir: "{app}"
Source: "build/libgobject-2.0-0.dll"; DestDir: "{app}"
Source: "build/libpng16-16.dll"; DestDir: "{app}"
Source: "build/libbz2-1.dll"; DestDir: "{app}"
Source: "build/libgraphene-1.0-0.dll"; DestDir: "{app}"
Source: "build/libsharpyuv-0.dll"; DestDir: "{app}"
Source: "build/libc++.dll"; DestDir: "{app}"
Source: "build/libgraphite2.dll"; DestDir: "{app}"
Source: "build/libthai-0.dll"; DestDir: "{app}"
Source: "build/libcairo-2.dll"; DestDir: "{app}"
Source: "build/libgtk-4-1.dll"; DestDir: "{app}"
Source: "build/libtiff-6.dll"; DestDir: "{app}"
Source: "build/libcairo-gobject-2.dll"; DestDir: "{app}"
Source: "build/libharfbuzz-0.dll"; DestDir: "{app}"
Source: "build/libunwind.dll"; DestDir: "{app}"
Source: "build/libcairo-script-interpreter-2.dll"; DestDir: "{app}"
Source: "build/libharfbuzz-subset-0.dll"; DestDir: "{app}"
Source: "build/libwebp-7.dll"; DestDir: "{app}"
Source: "build/libdatrie-1.dll"; DestDir: "{app}"
Source: "build/libiconv-2.dll"; DestDir: "{app}"
Source: "build/libzstd.dll"; DestDir: "{app}"
Source: "build/libdeflate.dll"; DestDir: "{app}"
Source: "build/libintl-8.dll"; DestDir: "{app}"
Source: "build/msys-highs-1.dll"; DestDir: "{app}"
Source: "build/libepoxy-0.dll"; DestDir: "{app}"
Source: "build/libjbig-0.dll"; DestDir: "{app}"
Source: "build/libexpat-1.dll"; DestDir: "{app}"
Source: "build/libjpeg-8.dll"; DestDir: "{app}"
Source: "build/vulkan-1.dll"; DestDir: "{app}"
Source: "build/libffi-8.dll"; DestDir: "{app}"
Source: "build/liblzma-5.dll"; DestDir: "{app}"
Source: "build/zlib1.dll"; DestDir: "{app}"
Source: "build/libfontconfig-1.dll"; DestDir: "{app}"
Source: "build/liblzo2-2.dll"; DestDir: "{app}"
Source: "build/schemas/gschemas.compiled"; DestDir: "{app}/share/glib-2.0/schemas/"
Source: "build/schemas/org.gtk.gtk4.Settings.FileChooser.gschema.xml"; DestDir: "{app}/share/glib-2.0/schemas/"
Source: "data/ui/icon.ico"; DestDir: "{app}"

[Icons]
Name: "{userdesktop}\highsv"; Filename: "{app}/{#AppExe}"; \
  IconFilename: "{app}/icon.ico"; Tasks: desktopicon
