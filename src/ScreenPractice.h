#ifndef SCREEN_EDIT_H
#define SCREEN_EDIT_H

#include "ScreenWithMenuElements.h"
#include "BitmapText.h"
#include "Player.h"
#include "RageSound.h"
#include "SnapDisplay.h"
#include "Background.h"
#include "Foreground.h"
#include "NoteField.h"
#include "NoteTypes.h"
#include "Song.h"
#include "Steps.h"
#include "ThemeMetric.h"
#include "PlayerState.h"
#include "GameInput.h"
#include "GameplayAssist.h"
#include "AutoKeysounds.h"
#include "EnumHelper.h"

#include <map>
#include <vector>


class ScreenPractice : public ScreenWithMenuElements
{
public:
	virtual void Init();
	virtual void BeginScreen();
	virtual void EndScreen();

	virtual ~ScreenEdit();
	virtual void Update( float fDeltaTime );
	virtual void DrawPrimitives();
	virtual bool Input( const InputEventPlus &input );
	bool InputPlay( const InputEventPlus &input, EditButton EditB );
	virtual void HandleMessage( const Message &msg );
	virtual void HandleScreenMessage( const ScreenMessage SM );

	// Lua
	virtual void PushSelf( lua_State *L );

protected:

// for MODE_PLAY
	void SetupCourseAttacks();
	PlayerPlus		m_Player;
	Background		m_Background;
	Foreground		m_Foreground;
	bool			m_bReturnToRecordMenuAfterPlay;

// for MODE_RECORD and MODE_PLAY
	int				m_iStartPlayingAt, m_iStopPlayingAt;
	float			m_fBeatToReturnTo;

	AutoKeysounds	m_AutoKeysounds;
	RageSound		*m_pSoundMusic;
	GameplayAssist	m_GameplayAssist;

	ThemeMetric<EditMode> EDIT_MODE;

public:
	/** @brief What are the choices that one can make on the main menu? */
	enum MainMenuChoice
	{
		play_selection,
		set_selection_start,
		set_selection_end,
		edit_steps_information,
		play_whole_song, /**< Play the entire chart from the beginning. */
		play_selection_start_to_end,
		play_current_beat_to_end,
		save, /**< Save the current chart to disk. */
		revert_to_last_save,
		revert_from_disk,
		options, /**< Modify the PlayerOptions and SongOptions. */
		edit_song_info, /**< Edit some general information about the song. */
		edit_timing_data, /**< Edit the chart's timing data. */
		view_steps_data, /**< View step statistics. */
		play_preview_music, /**< Play the song's preview music. */
		exit,
		save_on_exit,
		NUM_MAIN_MENU_CHOICES,
		MAIN_MENU_CHOICE_INVALID
	};
	int GetSongOrNotesEnd();
	void HandleMainMenuChoice( MainMenuChoice c, const std::vector<int> &iAnswers );
	void HandleMainMenuChoice( MainMenuChoice c ) { const std::vector<int> v; HandleMainMenuChoice( c, v ); }
	MainMenuChoice m_CurrentAction;

	enum AreaMenuChoice
	{
		paste_at_current_beat, /**< Paste note data starting at the current beat. */
		paste_at_begin_marker, /**< Paste note data starting at the first market. */
		insert_and_shift,
		delete_and_shift,
		shift_pauses_forward, /**< Shift all timing changes forward one beat. */
		shift_pauses_backward, /**< Shift all timing changes backward one beat. */
		convert_pause_to_beat,
		convert_delay_to_beat,
		last_second_at_beat,
		undo,
		clear_clipboard, /**< Clear the clipboards. */
		modify_attacks_at_row, /**< Modify the chart attacks at this row. */
		modify_keysounds_at_row, /**< Modify the keysounds at this row. */
		NUM_AREA_MENU_CHOICES
	};
	void HandleArbitraryRemapping(RString const& mapstr);
	void HandleAlterMenuChoice(AlterMenuChoice choice,
		const std::vector<int> &answers, bool allow_undo= true,
		bool prompt_clear= true);
	void HandleAlterMenuChoice(AlterMenuChoice c, bool allow_undo= true,
		bool prompt_clear= true)
	{
		const std::vector<int> v; HandleAlterMenuChoice(c, v, allow_undo, prompt_clear);
	}

	void HandleAreaMenuChoice( AreaMenuChoice c, const std::vector<int> &iAnswers, bool bAllowUndo = true );
	void HandleAreaMenuChoice( AreaMenuChoice c, bool bAllowUndo = true )
	{
		const std::vector<int> v; HandleAreaMenuChoice( c, v, bAllowUndo );
	}
	/** @brief How should the selected notes be transformed? */
	enum TurnType
	{
		left, /**< Turn the notes as if you were facing to the left. */
		right, /**< Turn the notes as if you were facing to the right. */
		mirror, /**< Flip the notes vertically. */
		lrmirror, /**< Swap the left and right columns. */
		udmirror, /**< Swap the up and down columns. */
		turn_backwards, /**< Turn the notes as if you were facing away from the machine. */
		shuffle, /**< Replace one column with another column. */
		super_shuffle, /**< Replace each note individually. */
		hyper_shuffle, /**< Replace each note individually but more fairly. */
		NUM_TURN_TYPES
	};
	enum TransformType
	{
		noholds,
		nomines,
		little,
		wide,
		big,
		quick,
		skippy,
		add_mines,
		echo,
		stomp,
		planted,
		floored,
		twister,
		nojumps,
		nohands,
		noquads,
		nostretch,
		NUM_TRANSFORM_TYPES
	};
	enum AlterType
	{
		autogen_to_fill_width,
		backwards,
		swap_sides,
		copy_left_to_right,
		copy_right_to_left,
		clear_left,
		clear_right,
		collapse_to_one,
		collapse_left,
		shift_left,
		shift_right,
		swap_up_down,
		arbitrary_remap,
		NUM_ALTER_TYPES
	};
	enum TempoType
	{
		compress_2x,
		compress_3_2,
		compress_4_3,
		expand_4_3,
		expand_3_2,
		expand_2x,
		NUM_TEMPO_TYPES
	};

	enum StepsInformationChoice
	{
		difficulty, /**< What is the difficulty of this chart? */
		meter, /**< What is the numerical rating of this chart? */
		predict_meter, /**< What does the game think this chart's rating should be? */
		chartname, /**< What is the name of this chart? */
		description, /**< What is the description of this chart? */
		chartstyle, /**< How is this chart meant to be played? */
		step_credit, /**< Who wrote this individual chart? */
		step_display_bpm,
		step_min_bpm,
		step_max_bpm,
		step_music,
		NUM_STEPS_INFORMATION_CHOICES
	};
	void HandleStepsInformationChoice( StepsInformationChoice c, const std::vector<int> &iAnswers );

	enum StepsDataChoice
	{
		tap_notes,
		jumps,
		hands,
		quads,
		holds,
		mines,
		rolls,
		lifts,
		fakes,
		stream,
		voltage,
		air,
		freeze,
		chaos,
		NUM_STEPS_DATA_CHOICES
	};
	void HandleStepsDataChoice(StepsDataChoice c, const std::vector<int> &answers);

	enum SongInformationChoice
	{
		main_title,
		sub_title,
		artist,
		genre,
		credit,
		preview,
		main_title_transliteration,
		sub_title_transliteration,
		artist_transliteration,
		last_second_hint,
		preview_start,
		preview_length,
		display_bpm,
		min_bpm,
		max_bpm,
		NUM_SONG_INFORMATION_CHOICES
	};
	void HandleSongInformationChoice( SongInformationChoice c, const std::vector<int> &iAnswers );

	enum TimingDataInformationChoice
	{
		beat_0_offset,
		bpm,
		stop,
		delay,
//		time_signature,
		label,
		tickcount,
		combo,
		warp,
//		speed,
		speed_percent,
		speed_wait,
		speed_mode,
		scroll,
		fake,
		shift_timing_in_region_down,
		shift_timing_in_region_up,
		copy_timing_in_region,
		clear_timing_in_region,
		paste_timing_from_clip,
		copy_full_timing,
		paste_full_timing,
		erase_step_timing,
		NUM_TIMING_DATA_INFORMATION_CHOICES
	};
	void HandleTimingDataInformationChoice (TimingDataInformationChoice c,
						const std::vector<int> &iAnswers );

	enum TimingDataChangeChoice
	{
		timing_all,
		timing_bpm,
		timing_stop,
		timing_delay,
		timing_time_sig,
		timing_warp,
		timing_label,
		timing_tickcount,
		timing_combo,
		timing_speed,
		timing_scroll,
		timing_fake,
		NUM_TimingDataChangeChoices
	};
	void HandleTimingDataChangeChoice(TimingDataChangeChoice choice,
		const std::vector<int>& answers);

	enum BGChangeChoice
	{
		layer,
		rate,
		transition,
		effect,
		color1,
		color2,
		file1_type,
		file1_song_bganimation,
		file1_song_movie,
		file1_song_still,
		file1_global_bganimation,
		file1_global_movie,
		file1_global_movie_song_group,
		file1_global_movie_song_group_and_genre,
		file2_type,
		file2_song_bganimation,
		file2_song_movie,
		file2_song_still,
		file2_global_bganimation,
		file2_global_movie,
		file2_global_movie_song_group,
		file2_global_movie_song_group_and_genre,
		delete_change,
		NUM_BGCHANGE_CHOICES
	};

	enum SpeedSegmentModes
	{
		SSMODE_Beats,
		SSMODE_Seconds
	};

	/**
	 * @brief Take care of any background changes that the user wants.
	 *
	 * It is important that this is only called in Song Timing mode.
	 * @param c the Background Change style requested.
	 * @param iAnswers the other settings involving the change. */
	void HandleBGChangeChoice( BGChangeChoice c, const std::vector<int> &iAnswers );

	enum CourseAttackChoice
	{
		duration,
		set_mods,
		remove,
		NUM_CourseAttackChoice
	};

	enum StepAttackChoice
	{
		sa_duration,
		sa_set_mods,
		sa_remove,
		NUM_StepAttackChoice
	};

	void InitEditMappings();
	EditButton DeviceToEdit( const DeviceInput &DeviceI ) const;
	EditButton MenuButtonToEditButton( GameButton MenuI ) const;
	bool EditToDevice( EditButton button, int iSlotNum, DeviceInput &DeviceI ) const;
	bool EditPressed( EditButton button, const DeviceInput &DeviceI );
	bool EditIsBeingPressed( EditButton button ) const;
	const MapEditToDI *GetCurrentDeviceInputMap() const;
	const MapEditButtonToMenuButton *GetCurrentMenuButtonMap() const;
	void LoadKeymapSectionIntoMappingsMember(XNode const* section, MapEditToDI& mappings);
	MapEditToDI		m_EditMappingsDeviceInput;
	MapEditToDI		m_PlayMappingsDeviceInput;
	MapEditToDI		m_RecordMappingsDeviceInput;
	MapEditToDI		m_RecordPausedMappingsDeviceInput;
	MapEditButtonToMenuButton m_EditMappingsMenuButton;
	MapEditButtonToMenuButton m_PlayMappingsMenuButton;
	MapEditButtonToMenuButton m_RecordMappingsMenuButton;
	MapEditButtonToMenuButton m_RecordPausedMappingsMenuButton;

	void MakeFilteredMenuDef( const MenuDef* pDef, MenuDef &menu );
	void EditMiniMenu(const MenuDef* pDef,
			  ScreenMessage SM_SendOnOK = SM_None,
			  ScreenMessage SM_SendOnCancel = SM_None );

	int attackInProcess;
	int modInProcess;
private:
	/**
	 * @brief Retrieve the appropriate TimingData based on GAMESTATE.
	 * @return the proper TimingData. */
	const TimingData & GetAppropriateTiming() const;
	/**
	 * @brief Retrieve the appropriate TimingData to use for updating. */
	TimingData & GetAppropriateTimingForUpdate();
	/**
	 * @brief Retrieve the appropriate SongPosition data based on GAMESTATE.
	 * @return the proper SongPosition. */
	SongPosition & GetAppropriatePosition() const;
	void SetBeat(float fBeat);
	float GetBeat();
	int GetRow();

};

#endif

/*
 * (c) 2001-2004 Chris Danford
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
