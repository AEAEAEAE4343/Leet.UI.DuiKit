using System.Collections.Generic;

namespace Leet.UI.DuiKit
{
    public class DuiBinaryData 
    {
        public class DuiBinaryResource
        {
            public uint IdIndex;
            public uint AtEntry;
        }

        public class DuiBinaryEntry
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

        public List<DuiBinaryEntry> Entries = new List<DuiBinaryEntry>();
        public List<string> Strings = new List<string>();
        public List<DuiBinaryResource> Resources = new List<DuiBinaryResource>();

    }
}