using System;
using System.IO;
using System.Xml;
using System.Linq;

namespace Leet.UI.DuiKit.Compiler
{
    public class DuiXmlSerializer
    {
        /// <summary>
        /// Serializes a DuiBinaryData object into XML and outputs it onto a Stream.
        /// </summary>
        /// <param name="data">The DUI data to be serialized.</param>
        /// <param name="output">The stream to output the XML data to.</param>
        public void Serialize(DuiBinaryData data, Stream output) 
        {
            using (XmlWriter writer = XmlWriter.Create(output, new XmlWriterSettings() { Indent = true, OmitXmlDeclaration = true, IndentChars = "\t" }))
            {

                foreach (var entry in data.Entries)
                {
                    if (entry.Type == DuiBinaryData.DuiBinaryEntry.EntryType.StartElement
                        || entry.Type == DuiBinaryData.DuiBinaryEntry.EntryType.StartElementEmpty)
                    {
                        string name = (entry.NameIndex & 0x8000) != 0 ? Constants.BDXCommonStringTable[entry.NameIndex & 0x7FFF] : data.Strings[entry.NameIndex];
                        writer.WriteStartElement(name);

                        foreach (var property in entry.Properties)
                        {
                            string propertyName = (property.NameIndex & 0x8000) != 0 ? Constants.BDXCommonStringTable[property.NameIndex & 0x7FFF] : data.Strings[property.NameIndex];
                            string propertyValue = (property.ValueIndex & 0x8000) != 0 ? Constants.BDXCommonStringTable[property.ValueIndex & 0x7FFF] : data.Strings[property.ValueIndex];

                            writer.WriteAttributeString(propertyName, propertyValue);
                        }

                        if (entry.Type == DuiBinaryData.DuiBinaryEntry.EntryType.StartElementEmpty)
                            writer.WriteEndElement();
                    }
                    else if (entry.Type == DuiBinaryData.DuiBinaryEntry.EntryType.EndElement)
                    {
                        writer.WriteEndElement();
                    }
                    else throw new InvalidDataException($"The entry type {entry.Type} is not supported.");
                }
            }
        }

        /// <summary>
        /// Deserializes XML data from a Stream into a DuiBinaryData object.
        /// </summary>
        /// <param name="input">The stream from which to read in the DUI XML data.</param>
        /// <returns>A DuiBinaryData object representing the data read from the stream.</returns>
        public DuiBinaryData Deserialize(Stream input) 
        {
            DuiBinaryData data = new DuiBinaryData();

            // We read through all XML tokens and convert them into DUIB entries
            // The layout of DUIB follows XML structure so it is very easy to do this
            // For storing the actual names we use a string table and indeces to said table.
            ushort GetStringIndex(string text) 
            {
                if (Constants.BDXCommonStringTable.Contains(text))
                    return (ushort)(Constants.BDX_COMMON_STRING | (uint)Array.IndexOf(Constants.BDXCommonStringTable, text));
                
                if (data.Strings.Contains(text))
                    return (ushort)data.Strings.IndexOf(text);

                data.Strings.Add(text);
                return (ushort)(data.Strings.Count - 1);
            }

            using (XmlTextReader reader = new XmlTextReader(input))
            while (reader.MoveToNextAttribute() || reader.Read())
            {
                switch (reader.NodeType)
                {
                    // Start and end of elements are very similar so they are combined here
                    case XmlNodeType.EndElement:
                    case XmlNodeType.Element:
                    {
                        var entry = new DuiBinaryData.DuiBinaryEntry();
                        
                        if (reader.NodeType == XmlNodeType.EndElement)
                            entry.Type = DuiBinaryData.DuiBinaryEntry.EntryType.EndElement;
                        else 
                            entry.Type = reader.IsEmptyElement 
                                ? DuiBinaryData.DuiBinaryEntry.EntryType.StartElementEmpty
                                : DuiBinaryData.DuiBinaryEntry.EntryType.StartElement;

                        // If it is a common name, use that to save space in the string table
                        entry.NameIndex = GetStringIndex(reader.Name);
                        data.Entries.Add(entry);
                    }      
                    break;    
                    case XmlNodeType.Attribute:
                    {
                        var lastEntry = data.Entries.Last();
                        var property = new DuiBinaryData.DuiBinaryEntry.EntryProperty();
                        property.NameIndex = GetStringIndex(reader.Name);
                        property.ValueIndex = GetStringIndex(reader.Value);
                        lastEntry.Properties.Add(property);

                        // If we have a resid attribute and the element is not a <style> element,
                        // we add a reference to the resource chunk of the data
                        if (reader.Name == "resid" && lastEntry.NameIndex != 8)
                        {
                            data.Resources.Add(new DuiBinaryData.DuiBinaryResource() {
                                AtEntry = (uint)(data.Entries.Count - 1),
                                IdIndex = property.ValueIndex,
                            });
                        }
                    }           
                    break;
                }
            }

            return data;
        }
    }
}