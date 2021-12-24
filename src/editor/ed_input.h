#ifndef EDITOR_ED_INPUT_H
#define EDITOR_ED_INPUT_H

void EdControl_Setup(editor_t *_ed, editor_api_t *_api);
int Mouse_Event(int x, int y, bool lbutton, bool rbutton);
int Keyb_Event(int scancode, bool keydown);

#endif