#ifdef INTERFACE
CLASS(InputBox) EXTENDS(Label)
	METHOD(InputBox, configureInputBox, void(entity, string, float, float, string))
	METHOD(InputBox, draw, void(entity))
	METHOD(InputBox, setText, void(entity, string))
	METHOD(InputBox, enterText, void(entity, string))
	METHOD(InputBox, keyDown, float(entity, float, float, float))
	METHOD(InputBox, mouseRelease, float(entity, vector))
	METHOD(InputBox, mousePress, float(entity, vector))
	METHOD(InputBox, mouseDrag, float(entity, vector))
	METHOD(InputBox, showNotify, void(entity))

	ATTRIB(InputBox, src, string, NULL)

	ATTRIB(InputBox, cursorPos, float, 0) // characters
	ATTRIB(InputBox, scrollPos, float, 0) // widths

	ATTRIB(InputBox, focusable, float, 1)
	ATTRIB(InputBox, disabled, float, 0)
	ATTRIB(InputBox, lastChangeTime, float, 0)
	ATTRIB(InputBox, dragScrollTimer, float, 0)
	ATTRIB(InputBox, dragScrollPos, vector, '0 0 0')
	ATTRIB(InputBox, pressed, float, 0)
	ATTRIB(InputBox, editColorCodes, float, 1)
	ATTRIB(InputBox, forbiddenCharacters, string, "")
	ATTRIB(InputBox, color, vector, '1 1 1')
	ATTRIB(InputBox, colorF, vector, '1 1 1')
	ATTRIB(InputBox, maxLength, float, 255)
ENDCLASS(InputBox)
void InputBox_Clear_Click(entity btn, entity me);
#endif

#ifdef IMPLEMENTATION
void configureInputBoxInputBox(entity me, string theText, float theCursorPos, float theFontSize, string gfx)
{
	configureLabelLabel(me, theText, theFontSize, 0.0);
	me.src = gfx;
	me.cursorPos = theCursorPos;
}

void setTextInputBox(entity me, string txt)
{
	if(me.text)
		strunzone(me.text);
	setTextLabel(me, strzone(txt));
}

void InputBox_Clear_Click(entity btn, entity me)
{
	me.setText(me, "");
}

float mouseDragInputBox(entity me, vector pos)
{
	float p;
	me.dragScrollPos = pos;
	p = me.scrollPos + pos_x - me.keepspaceLeft;
	me.cursorPos = gui_draw_text_length_up_to_width(me.text, p / me.realFontSize_x, 0);
	me.lastChangeTime = time;
	return 1;
}

float mousePressInputBox(entity me, vector pos)
{
	me.dragScrollTimer = time;
	me.pressed = 1;
	return mouseDragInputBox(me, pos);
}

float mouseReleaseInputBox(entity me, vector pos)
{
	me.pressed = 0;
	return mouseDragInputBox(me, pos);
}

void enterTextInputBox(entity me, string ch)
{
	float i;
	for(i = 0; i < strlen(ch); ++i)
		if(strstrofs(me.forbiddenCharacters, substring(ch, i, 1), 0) > -1)
			return;
	if(strlen(ch) + strlen(me.text) > me.maxLength)
		return;
	me.setText(me, strcat(substring(me.text, 0, me.cursorPos), ch, substring(me.text, me.cursorPos, strlen(me.text) - me.cursorPos)));
	me.cursorPos = me.cursorPos + strlen(ch);
}

float keyDownInputBox(entity me, float key, float ascii, float shift)
{
	me.lastChangeTime = time;
	me.dragScrollTimer = time;
	if(ascii >= 32 && ascii != 127)
	{
		me.enterText(me, chr(ascii));
		return 1;
	}
	switch(key)
	{
		case K_LEFTARROW:
			me.cursorPos -= 1;
			return 1;
		case K_RIGHTARROW:
			me.cursorPos += 1;
			return 1;
		case K_HOME:
			me.cursorPos = 0;
			return 1;
		case K_END:
			me.cursorPos = strlen(me.text);
			return 1;
		case K_BACKSPACE:
			if(me.cursorPos > 0)
			{
				me.cursorPos -= 1;
				me.setText(me, strcat(substring(me.text, 0, me.cursorPos), substring(me.text, me.cursorPos + 1, strlen(me.text) - me.cursorPos - 1)));
			}
			return 1;
		case K_DEL:
			if(shift & GUI_KEY_STATE_CTRL)
				me.setText(me, "");
			else
				me.setText(me, strcat(substring(me.text, 0, me.cursorPos), substring(me.text, me.cursorPos + 1, strlen(me.text) - me.cursorPos - 1)));
			return 1;
	}
	return 0;
}

void drawInputBox(entity me)
{
#define CURSOR "_"
	float cursorPosInWidths, totalSizeInWidths;

	if(me.pressed)
		me.mouseDrag(me, me.dragScrollPos); // simulate mouseDrag event

	me.focusable = !me.disabled;
	if(me.disabled)
		gui_draw_alpha *= me.disabledAlpha;

	if(me.src)
	{
		if(me.focused && !me.disabled)
			gui_draw_button_picture('0 0 0', strcat(me.src, "_f"), '1 1 0', me.colorF, 1);
		else
			gui_draw_button_picture('0 0 0', strcat(me.src, "_n"), '1 1 0', me.color, 1);
	}

	me.cursorPos = bound(0, me.cursorPos, strlen(me.text));
	cursorPosInWidths = gui_text_width(substring(me.text, 0, me.cursorPos), 0) * me.realFontSize_x;
	totalSizeInWidths = gui_text_width(strcat(me.text, CURSOR), 0) * me.realFontSize_x;

	if(me.dragScrollTimer < time)
	{
		float save;
		save = me.scrollPos;
		me.scrollPos = bound(cursorPosInWidths - (0.875 - me.keepspaceLeft - me.keepspaceRight), me.scrollPos, cursorPosInWidths - 0.125);
		if(me.scrollPos != save)
			me.dragScrollTimer = time + 0.2;
	}
	me.scrollPos = min(me.scrollPos, totalSizeInWidths - (1 - me.keepspaceRight - me.keepspaceLeft));
	me.scrollPos = max(0, me.scrollPos);

	gui_draw_set_clip_rect(eX * me.keepspaceLeft, eX * (1 - me.keepspaceLeft - me.keepspaceRight) + eY);
	if(me.editColorCodes)
	{
		string ch, ch2;
		float i, n;
		vector theColor;
		float theAlpha;    //float theVariableAlpha;
		vector p;
		vector theTempColor;
		float component;
		
		p = me.realOrigin - eX * me.scrollPos;
		theColor = '1 1 1';
		theAlpha = 1;    //theVariableAlpha = 1; // changes when ^ax found
		
		n = strlen(me.text);
		for(i = 0; i < n; ++i)
		{
			ch = substring(me.text, i, 1);
			if(ch == "^")
			{
				float w;
				ch2 = substring(me.text, i+1, 1);
				w = gui_text_width(strcat(ch, ch2), 0) * me.realFontSize_x;
				if(ch2 == "^")
				{
					gui_draw_fill(p, eX * w + eY * me.realFontSize_y, '1 1 1', 0.5);
					gui_draw_text(p + eX * 0.25 * w, "^", me.realFontSize, theColor, theAlpha, 0);
				}
				else if(ch2 == "0" || stof(ch2)) // digit?
				{
					switch(stof(ch2))
					{
						case 0: theColor = '0 0 0'; theAlpha = 1; break;
						case 1: theColor = '1 0 0'; theAlpha = 1; break;
						case 2: theColor = '0 1 0'; theAlpha = 1; break;
						case 3: theColor = '1 1 0'; theAlpha = 1; break;
						case 4: theColor = '0 0 1'; theAlpha = 1; break;
						case 5: theColor = '0 1 1'; theAlpha = 1; break;
						case 6: theColor = '1 0 1'; theAlpha = 1; break;
						case 7: theColor = '1 1 1'; theAlpha = 1; break;
						case 8: theColor = '1 1 1'; theAlpha = 0.5; break;
						case 9: theColor = '0.5 0.5 0.5'; theAlpha = 1; break;
					}
					gui_draw_fill(p, eX * w + eY * me.realFontSize_y, '1 1 1', 0.5);
					gui_draw_text(p, strcat(ch, ch2), me.realFontSize, theColor, theAlpha, 0);
				}
				else if(ch2 == "x") // ^x found
				{
					theColor = '1 1 1';
					theTempColor = '0 0 0';
					
					component = DIGIT_HEX_TO_DEC(substring(me.text, i+2, 1));
					if (component >= 0) // ^xr found
					{
						theTempColor_x = component/15;
						
						component = DIGIT_HEX_TO_DEC(substring(me.text, i+3, 1));
						if (component >= 0) // ^xrg found
						{
							theTempColor_y = component/15;
							
							component = DIGIT_HEX_TO_DEC(substring(me.text, i+4, 1));
							if (component >= 0) // ^xrgb found
							{
								theTempColor_z = component/15;
								theColor = theTempColor;
								w = gui_text_width(substring(me.text, i, 5), 0) * me.realFontSize_x;
								
								gui_draw_fill(p, eX * w + eY * me.realFontSize_y, '1 1 1', 0.5);
								gui_draw_text(p, substring(me.text, i, 5), me.realFontSize, theColor, 1, 0);    // theVariableAlpha instead of 1 using alpha tags ^ax
								i += 3;
							}
							else
							{
								// blue missing
								w = gui_text_width(substring(me.text, i, 4), 0) * me.realFontSize_x;
								gui_draw_fill(p, eX * w + eY * me.realFontSize_y, eZ, 0.5);
								gui_draw_text(p, substring(me.text, i, 4), me.realFontSize, '1 1 1', theAlpha, 0);
								i += 2;
							}
						}
						else
						{
							// green missing
							w = gui_text_width(substring(me.text, i, 3), 0) * me.realFontSize_x;
							gui_draw_fill(p, eX * w + eY * me.realFontSize_y, eY, 0.5);
							gui_draw_text(p, substring(me.text, i, 3), me.realFontSize, '1 1 1', theAlpha, 0);
							i += 1;
						}
					}
					else
					{
						// red missing
						//w = gui_text_width(substring(me.text, i, 2), 0) * me.realFontSize_x;
						gui_draw_fill(p, eX * w + eY * me.realFontSize_y, eX, 0.5);
						gui_draw_text(p, substring(me.text, i, 2), me.realFontSize, '1 1 1', theAlpha, 0);
					}
				}
				/*else if(ch2 == "a") // ^a found
				{
					gui_draw_fill(p, eX * w + eY * me.realFontSize_y, '1 1 1', 0.5);
					gui_draw_text(p, substring(me.text, i, 2), me.realFontSize, theColor, 0.8, 0);
					
					component = str2chr(me.text, i+2);
					if (component >= '0' && component <= '9')
						component = component - '0';
					else if (component >= 'a' && component <= 'f')
						component = component - 87;
					else if (component >= 'A' && component <= 'F')
						component = component - 55;
					else
						component = -1;
					
					if (component >= 0) // ^ah found
					{
						// FIX ME: overflow here
						if (component == 20 && theVariableAlpha <= 0.97)
							theVariableAlpha = theVariableAlpha + 0.0625;
						else if (component == 30 && theVariableAlpha >= 0.03)
							theVariableAlpha = theVariableAlpha - 0.0625;
						else
							theVariableAlpha = component*0.0625;
						
						gui_draw_fill(p, eX * gui_text_width(substring(me.text, i, 3), 0) * me.realFontSize_x + eY * me.realFontSize_y, '0.8 0.8 0.8', 0.5);
						gui_draw_text(p, strcat(ch, ch2), me.realFontSize, theColor, 0.8, 0);
					}
				}*/
				else
				{
					gui_draw_fill(p, eX * w + eY * me.realFontSize_y, '1 1 1', 0.5);
					gui_draw_text(p, strcat(ch, ch2), me.realFontSize, theColor, theAlpha, 0);
				}
				p += w * eX;
				++i;
				continue;
			}
			gui_draw_text(p, ch, me.realFontSize, theColor, theAlpha, 0); // TODO theVariableAlpha
			p += eX * gui_text_width(ch, 0) * me.realFontSize_x;
		}
	}
	else
		gui_draw_text(me.realOrigin - eX * me.scrollPos, me.text, me.realFontSize, '1 1 1', 1, 0);
		// skipping drawLabel(me);
	if(!me.focused || (time - me.lastChangeTime) < floor(time - me.lastChangeTime) + 0.5)
		gui_draw_text(me.realOrigin + eX * (cursorPosInWidths - me.scrollPos), CURSOR, me.realFontSize, '1 1 1', 1, 0);

	gui_draw_clip_clear();
}

void showNotifyInputBox(entity me)
{
	me.focusable = !me.disabled;
}
#endif
