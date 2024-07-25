using System.Collections.Generic;

namespace Leet.UI.DuiKit.Compiler
{
    /// <summary>
    /// Data structure for storing DUI data from binary or XML files.
    /// This structure is only meant as an intermediary storage structure and therefore exposes no public members.
    /// </summary>
    public class DuiBinaryData 
    {
        internal class DuiBinaryResource
        {
            public uint IdIndex;
            public uint AtEntry;
        }

        internal class DuiBinaryEntry
        {
            public enum EntryType : ushort
            {
                StartElement = 0x0, // Entry has child elements/entries/xml nodes
                StartElementEmpty = 0x1, // Entry doesn't have child elements/entries/xml nodes, entries of this type do not need to be closed with an entry of type EndElement
                EndElement = 0x2 // Ends a StartElement entry
            }

            public class EntryProperty
            {
                public ushort NameIndex;
                public ushort ValueIndex;
            }

            public EntryType Type;
            public ushort NameIndex;
            public List<EntryProperty> Properties = new List<EntryProperty>();
        }

        internal List<DuiBinaryEntry> Entries = new List<DuiBinaryEntry>();
        internal List<string> Strings = new List<string>();
        internal List<DuiBinaryResource> Resources = new List<DuiBinaryResource>();
    }
}