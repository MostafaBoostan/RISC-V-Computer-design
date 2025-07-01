[Setup]
AppName=RISC-V Computer
AppVersion=1.0
DefaultDirName={pf}\RISC-V Computer
DefaultGroupName=RISC-V Computer
OutputBaseFilename=RISC-V_Installer
SetupIconFile="G:\IUT\code\RISC-V-Computer-design\build\Files\computer.ico"
Compression=lzma
SolidCompression=yes

[Files]
Source: "G:\IUT\code\RISC-V-Computer-design\build\Files\*"; DestDir: "{app}"; Flags: recursesubdirs
Source: "G:\IUT\code\RISC-V-Computer-design\build\Files\computer.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\RISC-V Computer"; Filename: "{app}\RISC-V-Computer-design-code.exe"; IconFilename: "{app}\computer.ico"
Name: "{commondesktop}\RISC-V Computer"; Filename: "{app}\RISC-V-Computer-design-code.exe"; IconFilename: "{app}\computer.ico"
