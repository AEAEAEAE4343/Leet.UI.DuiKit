@echo off

mkdir .\package
copy .\x64\Release\Leet.UI.DuiKit.Notepad.exe .\package\
copy .\Compiler\bin\Release\netstandard2.0\Leet.UI.DuiKit.Compiler.dll .\package\
copy .\Compiler.WinForms\bin\Release\Leet.UI.DuiKit.Compiler.WinForms.exe .\package\

mkdir .\package\nuget
copy .\Compiler\bin\Release\*.nupkg .\package\nuget\
