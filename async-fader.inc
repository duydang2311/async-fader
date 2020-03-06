// fader.inc by Gammix

#if !defined _PawnPlus_included
	#error PawnPlus is required.
#endif

#if defined fader_included
	#endinput
#endif
#define fader_included

#if !defined MAX_TEXT_DRAW_FADES
	#define MAX_TEXT_DRAW_FADES 16
#endif

#define TEXT_FADE 0
#define BOX_FADE 1

//------------------
// Functions
TextDrawFadeForPlayer(playerid, Text:text, from_color, to_color, type = TEXT_FADE, timer_interval = 50, update_rate = 2);
TextDrawStopFadeForPlayer(playerid, Text:text, type = TEXT_FADE);
Task:PlayerTextDrawFade(playerid, PlayerText:playertext, from_color, to_color, type = TEXT_FADE, timer_interval = 50, update_rate = 2);
PlayerTextDrawStopFade(playerid, PlayerText:playertext, type = TEXT_FADE);

// Callbacks
forward OnTextDrawFade(playerid, idx, update_rate, initial_color);
forward OnPlayerTextDrawFade(playerid, idx, update_rate, initial_color);
forward OnPlayerTextDrawFaded(playerid, PlayerText:playertext, fade_type, initial_color, to_color);
public OnPlayerTextDrawFaded(playerid, PlayerText:playertext, fade_type, initial_color, to_color) {
	return 1;
}
//------------------

enum E_TEXTDRAW_FADE_DATA {
    bool:TEXTDRAW_FADE_VALID,
    TEXTDRAW_FADE_ID,
    TEXTDRAW_FADE_TYPE,
	TEXTDRAW_FADE_FROM_COLOR,
    TEXTDRAW_FADE_TO_COLOR,
    TEXTDRAW_FADE_TIMER,
	Task:TEXTDRAW_FADE_TASK
};

stock static textDrawFader[MAX_PLAYERS][MAX_TEXT_DRAW_FADES][E_TEXTDRAW_FADE_DATA];
stock static playerTextDrawFader[MAX_PLAYERS][MAX_TEXT_DRAW_FADES][E_TEXTDRAW_FADE_DATA];

stock static RGBA(color, &rr, &gg, &bb, &aa) {
	rr = color >>> 24;
	gg = (color >>> 16) & 0xFF;
	bb = (color >>> 8) & 0xFF;
	aa = color & 0xFF;
}

stock static HEX(&color, rr, gg, bb, aa) {
	color = (aa & 0xFF) | ((bb & 0xFF) << 8) | ((gg & 0xFF) << 16) | (rr << 24);
}

stock Task:TextDrawFadeForPlayer(playerid, Text:text, from_color, to_color, type = TEXT_FADE, timer_interval = 50, update_rate = 2) {
	if (playerid < 0 || playerid >= MAX_PLAYERS) {
		return 0;
	}

	if (text < Text:0 || text >= Text:MAX_TEXT_DRAWS) {
	    return 0;
	}

	new idx = -1;
	for (new i = 0; i < MAX_TEXT_DRAW_FADES; i++) {
		if (textDrawFader[playerid][i][TEXTDRAW_FADE_VALID] == true) {
			if (textDrawFader[playerid][i][TEXTDRAW_FADE_ID] == _:text && textDrawFader[playerid][i][TEXTDRAW_FADE_TYPE] == type) {
				TextDrawStopFadeForPlayer(playerid, text);
				idx = i;
				break;
			}
		} else {
		    idx = i;
		}
	}

	if (idx == -1) {
		return -1;
	}

	textDrawFader[playerid][idx][TEXTDRAW_FADE_VALID] = true;
	textDrawFader[playerid][idx][TEXTDRAW_FADE_ID] = _:text;
	textDrawFader[playerid][idx][TEXTDRAW_FADE_TYPE] = type;
	textDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR] = from_color;
	textDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR] = to_color;
	textDrawFader[playerid][idx][TEXTDRAW_FADE_TIMER] = SetTimerEx("OnTextDrawFade", timer_interval, true, "iiii", playerid, idx, update_rate, from_color);

	if(task_valid(textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]))
	{
		task_set_result(textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK], false);
		task_delete(textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);
	}

	textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK] = task_new();
	task_keep(textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);

	return 1;
}

stock TextDrawStopFadeForPlayer(playerid, Text:text, type = TEXT_FADE) {
    if (playerid < 0 || playerid >= MAX_PLAYERS) {
		return 0;
	}

	if (text < Text:0 || text >= Text:MAX_TEXT_DRAWS) {
	    return 0;
	}

	for (new i = 0; i < MAX_TEXT_DRAW_FADES; i++) {
		if (textDrawFader[playerid][i][TEXTDRAW_FADE_VALID] == true) {
			if (textDrawFader[playerid][i][TEXTDRAW_FADE_ID] == _:text && textDrawFader[i][playerid][TEXTDRAW_FADE_TYPE] == type) {

				if(task_valid(textDrawFader[playerid][i][TEXTDRAW_FADE_TASK]))
				{
					task_set_result(textDrawFader[playerid][i][TEXTDRAW_FADE_TASK], false);
					task_delete(textDrawFader[playerid][i][TEXTDRAW_FADE_TASK]);
					textDrawFader[playerid][i][TEXTDRAW_FADE_TASK] = Task:0;
				}

			    KillTimer(textDrawFader[playerid][i][TEXTDRAW_FADE_TIMER]);
			    textDrawFader[playerid][i][TEXTDRAW_FADE_VALID] = false;

			    return 1;
			}
		}
	}

	return 0;
}

stock Task:PlayerTextDrawFade(playerid, PlayerText:playertext, from_color, to_color, type = TEXT_FADE, timer_interval = 50, update_rate = 2) {
	if (playerid < 0 || playerid >= MAX_PLAYERS) {
		return Task:0;
	}

	if (playertext < PlayerText:0 || playertext >= PlayerText:MAX_PLAYER_TEXT_DRAWS) {
	    return Task:0;
	}

	new idx = -1;
	for (new i = 0; i < MAX_TEXT_DRAW_FADES; i++) {
		if (playerTextDrawFader[playerid][i][TEXTDRAW_FADE_VALID] == true) {
			if (playerTextDrawFader[playerid][i][TEXTDRAW_FADE_ID] == _:playertext && playerTextDrawFader[playerid][i][TEXTDRAW_FADE_TYPE] == type) {

				if(task_valid(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]))
				{
					task_set_result(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK], false);
					task_delete(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);
					playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK] = Task:0;
				}

				PlayerTextDrawStopFade(playerid, playertext);
				idx = i;
				break;
			}
		} else {
		    idx = i;
		}
	}

	if (idx == -1) {
		return Task:0;
	}

	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_VALID] = true;
	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_ID] = _:playertext;
	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TYPE] = type;
	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR] = from_color;
	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR] = to_color;
	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TIMER] = SetTimerEx("OnPlayerTextDrawFade", timer_interval, true, "iiii", playerid, idx, update_rate, from_color);
	if(task_valid(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]))
	{
		task_set_result(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK], false);
		task_delete(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);
	}
	playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK] = task_new();
	task_keep(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);

	return playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK];
}

stock PlayerTextDrawStopFade(playerid, PlayerText:playertext, type = TEXT_FADE) {
    if (playerid < 0 || playerid >= MAX_PLAYERS) {
		return 0;
	}

	if (playertext < PlayerText:0 || playertext >= PlayerText:MAX_PLAYER_TEXT_DRAWS) {
	    return 0;
	}

	for (new i = 0; i < MAX_TEXT_DRAW_FADES; i++) {
		if (playerTextDrawFader[playerid][i][TEXTDRAW_FADE_VALID] == true) {
			if (playerTextDrawFader[playerid][i][TEXTDRAW_FADE_ID] == _:playertext && playerTextDrawFader[i][playerid][TEXTDRAW_FADE_TYPE] == type) {

			    KillTimer(playerTextDrawFader[playerid][i][TEXTDRAW_FADE_TIMER]);
			    playerTextDrawFader[playerid][i][TEXTDRAW_FADE_VALID] = false;

			    return 1;
			}
		}
	}

	return 0;
}

public OnTextDrawFade(playerid, idx, update_rate, initial_color) {
	new new_color[4];
	new final_color[4];

    RGBA(textDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR], new_color[0], new_color[1], new_color[2], new_color[3]);
	RGBA(textDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR], final_color[0], final_color[1], final_color[2], final_color[3]);

	for (new i; i < 4; i++) {
		if (new_color[i] > final_color[i]) {
		    new_color[i] -= update_rate;

		    if (new_color[i] <= final_color[i]) {
		    	new_color[i] = final_color[i];
		    }
		} else if (new_color[i] < final_color[i]) {
		    new_color[i] += update_rate;

		    if (new_color[i] >= final_color[i]) {
		    	new_color[i] = final_color[i];
		    }
		}
	}

 	HEX(textDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR], new_color[0], new_color[1], new_color[2], new_color[3]);

	if (textDrawFader[playerid][idx][TEXTDRAW_FADE_TYPE] == TEXT_FADE) {
		TextDrawColor(Text:textDrawFader[playerid][idx][TEXTDRAW_FADE_ID], textDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR]);
	} else {
		TextDrawBoxColor(Text:textDrawFader[playerid][idx][TEXTDRAW_FADE_ID], textDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR]);
	}

	TextDrawShowForPlayer(playerid, Text:textDrawFader[playerid][idx][TEXTDRAW_FADE_ID]);

	if (textDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR] == textDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR]) {
		#if defined OnTextDrawFaded
  			OnTextDrawFaded(playerid, Text:textDrawFader[playerid][idx][TEXTDRAW_FADE_ID], textDrawFader[playerid][idx][TEXTDRAW_FADE_TYPE], initial_color, textDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR]);
		#endif

		KillTimer(textDrawFader[playerid][idx][TEXTDRAW_FADE_TIMER]);
  		textDrawFader[playerid][idx][TEXTDRAW_FADE_VALID] = false;
  		task_set_result(textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK], false);
		task_delete(textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);
		textDrawFader[playerid][idx][TEXTDRAW_FADE_TASK] = Task:0;
	}
}

public OnPlayerTextDrawFade(playerid, idx, update_rate, initial_color) {
	new new_color[4];
	new final_color[4];

    RGBA(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR], new_color[0], new_color[1], new_color[2], new_color[3]);
	RGBA(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR], final_color[0], final_color[1], final_color[2], final_color[3]);

	for (new i; i < 4; i++) {
		if (new_color[i] > final_color[i]) {
		    new_color[i] -= update_rate;

		    if (new_color[i] <= final_color[i]) {
		    	new_color[i] = final_color[i];
		    }
		} else if (new_color[i] < final_color[i]) {
		    new_color[i] += update_rate;

		    if (new_color[i] >= final_color[i]) {
		    	new_color[i] = final_color[i];
		    }
		}
	}

 	HEX(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR], new_color[0], new_color[1], new_color[2], new_color[3]);

	if (playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TYPE] == TEXT_FADE) {
		PlayerTextDrawColor(playerid, PlayerText:playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_ID], playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR]);
	} else {
		PlayerTextDrawBoxColor(playerid, PlayerText:playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_ID], playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR]);
	}

	PlayerTextDrawShow(playerid, PlayerText:playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_ID]);

	if (playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_FROM_COLOR] == playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR]) {
		#if defined OnPlayerTextDrawFaded
  			OnPlayerTextDrawFaded(playerid, PlayerText:playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_ID], playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TYPE], initial_color, playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TO_COLOR]);
		#endif
		
		KillTimer(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TIMER]);
  		playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_VALID] = false;
  		task_set_result(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK], true);
		task_delete(playerTextDrawFader[playerid][idx][TEXTDRAW_FADE_TASK]);
	}
}
