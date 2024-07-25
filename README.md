![Leet's DirectUI Toolkit](https://github.com/AEAEAEAE4343/Leet.UI.DuiKit/blob/master/screenshots/bannernofonts.svg?raw=true)
---
Set of tools for working with the undocumented DirectUI user interface library found in Windows.

## Leet's DirectUI Notepad (Leet.UI.DuiKit.Notepad)
GUI tool for developing and previewing Direct UI XML files, written in C++ using DirectUI.

![Leet's DirectUI Notepad](https://github.com/AEAEAEAE4343/Leet.UI.DuiKit/blob/master/screenshots/notepad.png?raw=true)

Traditionally DirectUI modifications or custom programs were made by trial and error. There was no easy way to get direct feedback from code. Painstakingly editing executable resources over and over again was common. With DirectUI Notepad this is no longer the case.

DUI XML code can be entered into the left pane and it will be compiled as you type it (Automatic compilation can be disabled if you prefer pressing a hotkey). The result is then rendered onto the right side of the window. This tool is designed as a playground for learning DUI XML. With immediate feedback and detailed error messages, it becomes very easy to build an intuition for DUI.

## Leet's DirectUI Compiler (Leet.UI.DuiKit.Compiler.WinForms)
Simple tool for converting DUI Binary data into DUI XML and reverse, written using .NET WinForms. 
The interface is very easy to use and quite self explanatory:

![Leet's DirectUI Compiler](https://github.com/AEAEAEAE4343/Leet.UI.DuiKit/blob/master/screenshots/winformscompiler.png?raw=true)

## Leet.UI.DuiKit.Compiler
.NET library for compiling and decompiling DUI binary (DUIB) and DUI XML files. This serves as the backend for *Leet's DUI Compiler*.
The library exposes three classes:
- `DuiBinaryData`: Intermediary format used in conversion.
- `DuiBinarySerializer`: Can read and write DUIB data to and from any .NET streams.
- `DuiXmlSerializer`: Can read and write DUI XML data to and from any .NET streams.

Doing conversion using the library is extremely simple. Here's all that is required to convert DUIB into DUI XML:
```csharp
using Leet.UI.DuiKit.Compiler;

DuiBinarySerializer binSerializer = new DuiBinarySerializer();
DuiXmlSerializer xmlSerializer = new DuiXmlSerializer();
DuiBinaryData duiData = binSerializer.Deserialize(inputStream);
xmlSerializer.Serialize(duiData, outputStream);
```

# License
The entire DirectUI Toolkit project is licensed under the Simple Classic Theme license, version 1.0. For more information, see `LICENSE.MD`.