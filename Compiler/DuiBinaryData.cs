using System.Xml;

namespace Leet.UI.DuiKit
{
    class DuiBinaryData 
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
            public List<EntryProperty> Properties = [];
        }

        public List<DuiBinaryEntry> Entries = [];
        public List<string> Strings = [];
        public List<DuiBinaryResource> Resources = [];

    }
}