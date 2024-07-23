#pragma once

namespace DirectUI
{
	
	struct Fill
	{
		BYTE dType;
		union
		{
			struct
			{
				COLORREF cr;
				COLORREF cr2;
				COLORREF cr3;
			} ref;

			struct
			{
				UINT uType;
				UINT uState;
			} fillDFC;

			struct
			{
				HTHEME hTheme;
				int iPartId;
				int iStateId;
			} fillDTB;
		};
	};

	struct Cursor
	{

	};

	struct Graphic
	{

	};


	struct EnumMap
	{

	};

	struct PropertyInfo
	{
		UCString name;
		UINT64 unk1;
		struct PropCapability {
			ValueType type : 6;
			UINT other : 26;
			UINT unk;
		} *cap;
		struct { UCString str_value; int int_value; } *enum_value_map;
		Value *(*get_default_value)();
		UINT64 *unk2;
	};

	struct DepRecs
	{

	};

	struct UILIB_API NavReference
	{
	public:
		NavReference() = delete;
		NavReference(const NavReference&) = delete;
		~NavReference() = delete;

		void Init(Element *, RECT *);
		NavReference& operator=(NavReference const &);
	};
}