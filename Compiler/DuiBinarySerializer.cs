using System.Xml;
using System.IO;
using System.Text;
using System.Diagnostics;

namespace Leet.UI.DuiKit
{
    class DuiBinarySerializer 
    {
        private byte[] signatureBuffer = new byte[4];
        byte[] dataBuffer = new byte[2048];

        private List<DuiBinaryData.DuiBinaryEntry> ReadBinaryEntriesChunk(int offset) 
        {
            int pointer = offset + 4; // Skip the first uint32 (size)
            uint entryCount = BitConverter.ToUInt32(dataBuffer, pointer);
            pointer += 4;

            if (entryCount == 0)
                return [];

            List<DuiBinaryData.DuiBinaryEntry> entries = [];
            for (int i = 0; i < entryCount; i++)
            {
                // The short is built up like this:
                // 0xCCCT
                // Where C is the property count and T is the type flag
                ushort typeAndPropertyCount = BitConverter.ToUInt16(dataBuffer, pointer);
                pointer += 2;
                ushort nameIndex = BitConverter.ToUInt16(dataBuffer, pointer);
                pointer += 2;
                DuiBinaryData.DuiBinaryEntry entry = new DuiBinaryData.DuiBinaryEntry 
                {
                    Type = (DuiBinaryData.DuiBinaryEntry.EntryType)(typeAndPropertyCount & 0xF), // Take the last nibble
                    NameIndex = nameIndex,
                    Properties = [], 
                };

                // Read the properties
                ushort propertyCount = (ushort)(typeAndPropertyCount >> 4); // Take the first three nibbles
                for (int j = 0; j < propertyCount; j++) 
                {
                    entry.Properties.Add(new DuiBinaryData.DuiBinaryEntry.EntryProperty
                    {
                        NameIndex = BitConverter.ToUInt16(dataBuffer, pointer),
                        ValueIndex = BitConverter.ToUInt16(dataBuffer, pointer + 2),
                    });
                    pointer += 4;
                }

                entries.Add(entry);
            }

            return entries;
        }

        private List<string> ReadBinaryStringsChunk(int offset) 
        {
            int pointer = offset + 4; // Skip the first uint32 (size)
            uint stringCount = BitConverter.ToUInt32(dataBuffer, pointer);
            pointer += 4;
            
            if (stringCount == 0)
                return [];

            // The next part contains a list of offsets to the strings
            // We do not need these as the strings themselves follow immediately
            // in the same order
            pointer += (int)(4 * stringCount);

            List<string> strings = [];
            string stringBuffer = string.Empty;
            do    
            {
                // Keep reading bytes until we reach a zero
                char temp = Encoding.Unicode.GetString(dataBuffer, pointer, 2)[0];
                if (temp == '\0')
                {
                    // When we reach a zero, it indicates the end of a string
                    strings.Add(stringBuffer);
                    stringBuffer = string.Empty;
                }
                else stringBuffer += temp;

                pointer += 2;

            // We keep iterating until we have all strings
            } while (strings.Count != stringCount);

            return strings;
        }

        private List<DuiBinaryData.DuiBinaryResource> ReadBinaryResourcesChunk(int offset) 
        {
            uint size = BitConverter.ToUInt32(dataBuffer, offset);
            int pointer = offset + 4; // Skip the first uint32 (size)
            uint resourceCount = BitConverter.ToUInt32(dataBuffer, pointer);
            pointer += 4;
            
            if (resourceCount == 0)
                return [];

            List<DuiBinaryData.DuiBinaryResource> resources = [];
            for (int i = 0; i < resourceCount; i ++)
            {
                DuiBinaryData.DuiBinaryResource resource = new DuiBinaryData.DuiBinaryResource 
                {
                    IdIndex = BitConverter.ToUInt32(dataBuffer, pointer),
                    AtEntry = BitConverter.ToUInt32(dataBuffer, pointer + 4),
                };

                pointer += 8;    
                resources.Add(resource);
            }
            return resources;
        }

        /// <summary>
        /// Serializes a DuiBinaryData object into DUI binary data and outputs it onto a Stream.
        /// </summary>
        /// <param name="data">The DUI data to be serialized.</param>
        /// <param name="output">The stream to output the DUIB data to.</param>
        public void Serialize(DuiBinaryData data, Stream output) 
        {
            using BinaryWriter writer = new BinaryWriter(output, Encoding.Unicode);
            
            // Calculate chunk sizes

            // each entry structure is 2 two-byte integers
            int entryChunkSize = data.Entries.Count * 4;
            // each property structure is 2 two-byte integers
            entryChunkSize += data.Entries.SelectMany(a => a.Properties).Count() * 4;
            // chunk size + entry count
            entryChunkSize += 8;
            
            // each resource structure is 2 four-byte integers
            int resourceChunkSize = data.Resources.Count * 8;
            // chunk size + entry count
            resourceChunkSize += 8; 

            // total character count * 2 bytes per character
            int stringChunkSize = data.Strings.Aggregate((a, b) => a + b).Length * 2; 
            // offsets (4 bytes) + null terminators (2 bytes)
            stringChunkSize += data.Strings.Count * 6; 
            // chunk size + entry count
            stringChunkSize += 8;

            // We can now determine the offsets
            // This is the order they appear in in almost all DUIB data
            int resourceChunkOffset = 20;
            int entryChunkOffset = resourceChunkOffset + resourceChunkSize;
            int stringChunkOffset = entryChunkOffset + entryChunkSize;

            // Write header
            writer.Write(Encoding.ASCII.GetBytes("duib"));
            writer.Write(5);
            writer.Write((uint)entryChunkOffset); // Entry chunk offset
            writer.Write((uint)stringChunkOffset); // String chunk offset
            writer.Write((uint)resourceChunkOffset); // Resource chunk offset

            // Write resource chunk
            writer.Write((uint)resourceChunkSize); // Chunk size
            writer.Write((uint)data.Resources.Count); // Resource count

            foreach (var resource in data.Resources)
            {
                writer.Write(resource.IdIndex);
                writer.Write(resource.AtEntry);
            }

            // Write entry chunk
            writer.Write((uint)entryChunkSize); // Chunk size
            writer.Write((uint)data.Entries.Count); // Entry count
            
            foreach (var entry in data.Entries)
            {
                writer.Write((ushort)(entry.Properties.Count << 4 | (ushort)entry.Type));
                writer.Write(entry.NameIndex);

                foreach (var property in entry.Properties) 
                {
                    writer.Write(property.NameIndex);
                    writer.Write(property.ValueIndex);
                }
            }

            // Write string chunks
            writer.Write((uint)stringChunkSize); // Chunk size
            writer.Write((uint)data.Strings.Count); // String count

            // Pointer to store string offset. We start at the end of the pointer
            // table and for each string, we move by how big the string is.
            uint stringOffset = (uint)(data.Strings.Count * 4) + 8;
            foreach (var str in data.Strings)
            {
                writer.Write(stringOffset);
                // A string is (its length + a null terminator) * 2 bytes long
                stringOffset += (uint)((str.Length + 1) * 2);
            }

            foreach (var str in data.Strings)
            {
                writer.Write(str.AsSpan());
                writer.Write((ushort)0x0);
            }
        }

        /// <summary>
        /// Deserializes binary data from a Stream into a DuiData object.
        /// </summary>
        /// <param name="input">The stream from which to read in the DUI binary data.</param>
        /// <returns>An DuiData object representing the data read from the stream.</returns>
        public DuiBinaryData Deserialize(Stream input) 
        {
            using BinaryReader reader = new BinaryReader(input);
            int bytesRead = reader.Read(signatureBuffer, 0, 4);
            
            // Sanity checks
            if (Encoding.ASCII.GetString(signatureBuffer, 0, 4) != "duib")
                throw new InvalidDataException("The data in the stream is not valid DUIB data.");
            
            uint version = reader.ReadUInt32();
            if (version != 5)
                throw new InvalidDataException("The DUIB data in the stream is unsupported. Only version 5 DUIB files are supported.");
            
            // Read in offsets
            uint entryChunkOffset = reader.ReadUInt32();
            uint stringChunkOffset = reader.ReadUInt32();
            uint resourceChunkOffset = reader.ReadUInt32();

            // Read in the two out of three chunks to avoid seeking
            uint lastOffset = Math.Max(entryChunkOffset, Math.Max(stringChunkOffset, resourceChunkOffset));
            if (lastOffset > dataBuffer.Length)
                Array.Resize(ref dataBuffer, (int)lastOffset);
            reader.Read(dataBuffer, 20, (int)(lastOffset - 20));

            // Determine how big the last chunk is and read it
            uint lastChunkSize = reader.ReadUInt32();
            if (dataBuffer.Length + lastChunkSize > dataBuffer.Length)
                Array.Resize(ref dataBuffer, (int)(dataBuffer.Length + lastChunkSize));
            reader.Read(dataBuffer, (int)lastOffset + 4, (int)lastChunkSize - 4);

            // Note: Some bytes have been read outside of the buffer.
            //       This is not an issue as these variables (header and chunk size of the last chunk)
            //       are not important while parsing the data, only while receiving it. 
            List<DuiBinaryData.DuiBinaryEntry> entries = ReadBinaryEntriesChunk((int)entryChunkOffset);
            List<string> strings = ReadBinaryStringsChunk((int)stringChunkOffset);
            List<DuiBinaryData.DuiBinaryResource> resources = ReadBinaryResourcesChunk((int)resourceChunkOffset);

            for (int i = 0; i < resources.Count; i++)
            {
                if (entries[(int)resources[i].AtEntry].Properties.Where(p => p.NameIndex == 32770 && p.ValueIndex == resources[i].IdIndex).Count() == 0)
                    Debugger.Break();
            }

            return new DuiBinaryData 
            {
                Entries = entries,
                Strings = strings,
                Resources = resources,
            };
        }
    } 
}