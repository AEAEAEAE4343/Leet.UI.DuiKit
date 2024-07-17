#pragma once
class NotepadWindow
{
public:
	static bool Create(NotepadWindow** pNotepadWindow);
	void OnInput();
	void OnEvent();
	void Refresh();
private:
	NotepadWindow();
};

