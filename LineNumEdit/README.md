# LineNumEdit by katahiromz

LineNumEdit is a C++/Win32 library that implements "textbox with line numbers".

![Screenshot](screenshot.png)

- Fully compatible to the Win32 EDIT control.
- Easily usable in C++.
- DLL and static supported.
- Under MIT license.

## How to use this library?

As static:

```c
#define LINENUMEDIT_IMPL
#include "LineNumEdit.hpp"
...
LineNumEdit::SuperclassWindow(); // Initialize the library
```

And create the `"LineNumEdit"` window by
`CreateWindow`/`CreateWindowEx`/`DialogBox` functions
with specifying the window class `"LineNumEdit"`.
See also [LineNumEdit_test.cpp](LineNumEdit_test.cpp) and [LineNumEdit_test_res.rc](LineNumEdit_test_res.rc).

You can also use this library even as a DLL:

```c
LoadLibraryA("LineNumEdit");
```

And create the `"LineNumEdit"` window.

## Messages

### `LNEM_SETLINENUMFORMAT` message

Specifies the format of the line number label. For example, it's `TEXT("%d")`.

- `wParam`: Ignored.
- `lParam`: The format of `LPCTSTR`.
- Return value: zero.

### `LNEM_SETNUMOFDIGITS` message

Sets the number of digits to indicate the width of the line number column.

- `wParam`: The number of digits.
- `lParam`: Ignored.
- Return value: zero.

### `LNEM_SETLINEMARK` message

Sets the color of the line number label.

- `wParam`: The line number to set a mark.
- `lParam`: The `COLORREF` value or `CLR_INVALID`. Use `RGB` macro to get the `COLORREF` value.
- Return value: zero.

### `LNEM_CLEARLINEMARKS` message

Clears the color settings of the line number labels.

- `wParam`: Ignored.
- `lParam`: Ignored.
- Return value: zero.

### `LNEM_SETLINEDELTA` message

Specifies the line number of the top line.

- `wParam`: The line number.
- `lParam`: Ignored.
- Return value: zero.

### `LNEM_SETCOLUMNWIDTH` message

Sets the width of the line number column.

- `wParam`: The width of the column, in pixels.
- `lParam`: Ignored.
- Return value: zero.

### `LNEM_GETCOLUMNWIDTH` message

Gets the width of the line number column.

- `wParam`: Ignored.
- `lParam`: Ignored.
- Return value: The column width in pixels.

### `LNEM_GETLINEMARK` message

Gets the color setting of a line number.

- `wParam`: The line number.
- `lParam`: Ignored.
- Return value: The `COLORREF` value or `CLR_INVALID`.

## Contact us

Please send e-mail to katayama.hirofumi.mz@gmail.com .
