// See https://aka.ms/new-console-template for more information
using Leet.UI.DuiKit;

DuiBinarySerializer binarySerializer = new DuiBinarySerializer();
DuiXmlSerializer xmlSerializer = new DuiXmlSerializer();

using FileStream input = File.Open("output.xml", FileMode.Open);
using FileStream output = File.Open("output.bin", FileMode.Create);
DuiBinaryData data2 = xmlSerializer.Deserialize(input);
binarySerializer.Serialize(data2, output);

