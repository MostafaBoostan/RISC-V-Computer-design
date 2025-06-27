[Setup]
AppName=RISC-V Computer
AppVersion=1.0
DefaultDirName={pf}\RISC-V Computer
DefaultGroupName=RISC-V Computer
OutputBaseFilename=RISC-V_Installer
SetupIconFile="G:\IUT\code\RISC-V-Computer-design\src\RISC-V-Computer-design-code\build\Desktop-Release\release\monitoring.ico"
Compression=lzma
SolidCompression=yes

[Files]
Source: "G:\IUT\code\RISC-V-Computer-design\src\RISC-V-Computer-design-code\build\Desktop-Release\release\*"; DestDir: "{app}"; Flags: recursesubdirs
Source: "G:\IUT\code\RISC-V-Computer-design\src\RISC-V-Computer-design-code\build\Desktop-Release\release\monitoring.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\RISC-V Computer"; Filename: "{app}\RISC-V-Computer-design-code.exe"; IconFilename: "{app}\monitoring.ico"
Name: "{commondesktop}\RISC-V Computer"; Filename: "{app}\RISC-V-Computer-design-code.exe"; IconFilename: "{app}\monitoring.ico"
