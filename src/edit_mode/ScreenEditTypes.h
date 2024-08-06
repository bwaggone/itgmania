#ifndef SCREEN_EDIT_TYPES_H
#define SCREEN_EDIT_TYPES

#include "ActorFrame.h"
#include "FadingBanner.h"
#include "TextBanner.h"
#include "GameConstantsAndTypes.h"
#include "StepsDisplay.h"
#include "RageSound.h"
#include "EnumHelper.h"
#include "ThemeMetric.h"

#include <vector>

// This file contains all the enums/newly defined types used by
// edit mode.

/** @brief What type of row is needed for the EditMenu? */
enum EditMenuRow
{
	ROW_GROUP,
	ROW_SONG,
	ROW_STEPS_TYPE,
	ROW_STEPS,
	ROW_SOURCE_STEPS_TYPE,
	ROW_SOURCE_STEPS,
	ROW_ACTION,
	NUM_EditMenuRow /**< The number of EditMenuRows available. */
};

/** @brief The different actions one can take on a step. */
enum EditMenuAction
{
	EditMenuAction_Edit, /**< Modify the current step for the Song. */
	EditMenuAction_Delete, /**< Remove the current step from the Song. */
	EditMenuAction_Create, /**< Create a new step for the Song. */
	EditMenuAction_Practice, /**< Practice the current step for the Song. */
	EditMenuAction_LoadAutosave,
	NUM_EditMenuAction, /**< The number of MenuActions available to choose from. */
	EditMenuAction_Invalid
};

/** @brief What is going on with the Editor? */
enum EditState
{
	STATE_EDITING, /**< The person is making adjustments to the Steps. */
	STATE_RECORDING, /**< The person is recording some Steps live. */
	STATE_RECORDING_PAUSED, /**< The person has temporarily paused the recording of Steps. */
	STATE_PLAYING, /**< The person is just trying out the Steps. */
	NUM_EditState,
	EditState_Invalid
};

enum EditButton
{
	// Add to the name_to_edit_button list when adding to this enum. -Kyz
	EDIT_BUTTON_COLUMN_0,
	EDIT_BUTTON_COLUMN_1,
	EDIT_BUTTON_COLUMN_2,
	EDIT_BUTTON_COLUMN_3,
	EDIT_BUTTON_COLUMN_4,
	EDIT_BUTTON_COLUMN_5,
	EDIT_BUTTON_COLUMN_6,
	EDIT_BUTTON_COLUMN_7,
	EDIT_BUTTON_COLUMN_8,
	EDIT_BUTTON_COLUMN_9,

	// These are modifiers to EDIT_BUTTON_COLUMN_*.
	EDIT_BUTTON_RIGHT_SIDE,
	EDIT_BUTTON_LAY_ROLL,
	EDIT_BUTTON_LAY_TAP_ATTACK,
	EDIT_BUTTON_REMOVE_NOTE,

	// These are modifiers to change the present tap note.
	EDIT_BUTTON_CYCLE_TAP_LEFT, /**< Rotate the available tap notes once to the "left". */
	EDIT_BUTTON_CYCLE_TAP_RIGHT, /**< Rotate the available tap notes once to the "right". */

	EDIT_BUTTON_CYCLE_SEGMENT_LEFT, /**< Select one segment to the left for jumping. */
	EDIT_BUTTON_CYCLE_SEGMENT_RIGHT, /**< Select one segment to the right for jumping. */

	EDIT_BUTTON_SCROLL_UP_LINE,
	EDIT_BUTTON_SCROLL_UP_PAGE,
	EDIT_BUTTON_SCROLL_UP_TS,
	EDIT_BUTTON_SCROLL_DOWN_LINE,
	EDIT_BUTTON_SCROLL_DOWN_PAGE,
	EDIT_BUTTON_SCROLL_DOWN_TS,
	EDIT_BUTTON_SCROLL_NEXT_MEASURE,
	EDIT_BUTTON_SCROLL_PREV_MEASURE,
	EDIT_BUTTON_SCROLL_HOME,
	EDIT_BUTTON_SCROLL_END,
	EDIT_BUTTON_SCROLL_NEXT,
	EDIT_BUTTON_SCROLL_PREV,

	EDIT_BUTTON_SEGMENT_NEXT, /**< Jump to the start of the next segment downward. */
	EDIT_BUTTON_SEGMENT_PREV, /**< Jump to the start of the previous segment upward. */

	// These are modifiers to EDIT_BUTTON_SCROLL_*.
	EDIT_BUTTON_SCROLL_SELECT,

	EDIT_BUTTON_LAY_SELECT,

	EDIT_BUTTON_SCROLL_SPEED_UP,
	EDIT_BUTTON_SCROLL_SPEED_DOWN,

	EDIT_BUTTON_SNAP_NEXT,
	EDIT_BUTTON_SNAP_PREV,

	EDIT_BUTTON_OPEN_EDIT_MENU,
	EDIT_BUTTON_OPEN_TIMING_MENU,
	EDIT_BUTTON_OPEN_ALTER_MENU,
	EDIT_BUTTON_OPEN_AREA_MENU,
	EDIT_BUTTON_OPEN_BGCHANGE_LAYER1_MENU,
	EDIT_BUTTON_OPEN_BGCHANGE_LAYER2_MENU,
	EDIT_BUTTON_OPEN_COURSE_MENU,
	EDIT_BUTTON_OPEN_COURSE_ATTACK_MENU,

	EDIT_BUTTON_OPEN_STEP_ATTACK_MENU, /**< Open up the Step Attacks menu. */
	EDIT_BUTTON_ADD_STEP_MODS, /**< Add a mod attack to the row. */

	EDIT_BUTTON_OPEN_INPUT_HELP,

	EDIT_BUTTON_BAKE_RANDOM_FROM_SONG_GROUP,
	EDIT_BUTTON_BAKE_RANDOM_FROM_SONG_GROUP_AND_GENRE,

	EDIT_BUTTON_PLAY_FROM_START,
	EDIT_BUTTON_PLAY_FROM_CURSOR,
	EDIT_BUTTON_PLAY_SELECTION,
	EDIT_BUTTON_RECORD_FROM_CURSOR,
	EDIT_BUTTON_RECORD_SELECTION,

	EDIT_BUTTON_RECORD_HOLD_RESET,
	EDIT_BUTTON_RECORD_HOLD_OFF,
	EDIT_BUTTON_RECORD_HOLD_MORE,
	EDIT_BUTTON_RECORD_HOLD_LESS,

	EDIT_BUTTON_RETURN_TO_EDIT,

	EDIT_BUTTON_INSERT,
	EDIT_BUTTON_DELETE,
	EDIT_BUTTON_INSERT_SHIFT_PAUSES,
	EDIT_BUTTON_DELETE_SHIFT_PAUSES,

	EDIT_BUTTON_OPEN_NEXT_STEPS,
	EDIT_BUTTON_OPEN_PREV_STEPS,
	EDIT_BUTTON_PLAY_SAMPLE_MUSIC,

	EDIT_BUTTON_BPM_UP,
	EDIT_BUTTON_BPM_DOWN,
	EDIT_BUTTON_STOP_UP,
	EDIT_BUTTON_STOP_DOWN,

	EDIT_BUTTON_DELAY_UP,
	EDIT_BUTTON_DELAY_DOWN,

	EDIT_BUTTON_OFFSET_UP,
	EDIT_BUTTON_OFFSET_DOWN,
	EDIT_BUTTON_SAMPLE_START_UP,
	EDIT_BUTTON_SAMPLE_START_DOWN,
	EDIT_BUTTON_SAMPLE_LENGTH_UP,
	EDIT_BUTTON_SAMPLE_LENGTH_DOWN,

	EDIT_BUTTON_ADJUST_FINE, /**< This button modifies offset, BPM, and stop segment changes. */

	EDIT_BUTTON_SAVE, /**< Save the present changes into the chart. */

	EDIT_BUTTON_UNDO, /**< Undo a recent change. */

	EDIT_BUTTON_ADD_COURSE_MODS,

	EDIT_BUTTON_SWITCH_PLAYERS, /**< Allow entering notes for a different Player. */

	EDIT_BUTTON_SWITCH_TIMINGS, /**< Allow switching between Song and Step TimingData. */

	// Add to the name_to_edit_button list when adding to this enum. -Kyz
	NUM_EditButton, // leave this at the end
	EditButton_Invalid
};


AutoScreenMessage(SM_UpdateTextInfo);
AutoScreenMessage(SM_BackFromMainMenu);
AutoScreenMessage(SM_BackFromAreaMenu);
AutoScreenMessage(SM_BackFromAlterMenu);
AutoScreenMessage(SM_BackFromArbitraryRemap);
AutoScreenMessage(SM_BackFromStepsInformation);
AutoScreenMessage(SM_BackFromStepsData);
AutoScreenMessage(SM_BackFromOptions);
AutoScreenMessage(SM_BackFromSongInformation);
AutoScreenMessage(SM_BackFromBGChange);
AutoScreenMessage(SM_BackFromInsertTapAttack);
AutoScreenMessage(SM_BackFromInsertTapAttackPlayerOptions);
AutoScreenMessage(SM_BackFromAttackAtTime);
AutoScreenMessage(SM_BackFromInsertStepAttack);
AutoScreenMessage(SM_BackFromAddingModToExistingAttack);
AutoScreenMessage(SM_BackFromEditingModToExistingAttack);
AutoScreenMessage(SM_BackFromEditingAttackStart);
AutoScreenMessage(SM_BackFromEditingAttackLength);
AutoScreenMessage(SM_BackFromAddingAttackToChart);
AutoScreenMessage(SM_BackFromInsertStepAttackPlayerOptions);
AutoScreenMessage(SM_BackFromInsertCourseAttack);
AutoScreenMessage(SM_BackFromInsertCourseAttackPlayerOptions);
AutoScreenMessage(SM_BackFromCourseModeMenu);
AutoScreenMessage(SM_BackFromKeysoundTrack);
AutoScreenMessage(SM_BackFromNewKeysound);
AutoScreenMessage(SM_DoRevertToLastSave);
AutoScreenMessage(SM_DoRevertFromDisk);
AutoScreenMessage(SM_ConfirmClearArea);
AutoScreenMessage(SM_BackFromTimingDataInformation);
AutoScreenMessage(SM_BackFromTimingDataChangeInformation);
AutoScreenMessage(SM_BackFromDifficultyMeterChange);
AutoScreenMessage(SM_BackFromBeat0Change);
AutoScreenMessage(SM_BackFromBPMChange);
AutoScreenMessage(SM_BackFromStopChange);
AutoScreenMessage(SM_BackFromDelayChange);
AutoScreenMessage(SM_BackFromTickcountChange);
AutoScreenMessage(SM_BackFromComboChange);
AutoScreenMessage(SM_BackFromLabelChange);
AutoScreenMessage(SM_BackFromWarpChange);
AutoScreenMessage(SM_BackFromSpeedPercentChange);
AutoScreenMessage(SM_BackFromSpeedWaitChange);
AutoScreenMessage(SM_BackFromSpeedModeChange);
AutoScreenMessage(SM_BackFromScrollChange);
AutoScreenMessage(SM_BackFromFakeChange);
AutoScreenMessage(SM_BackFromStepMusicChange);
AutoScreenMessage(SM_DoEraseStepTiming);
AutoScreenMessage(SM_DoSaveAndExit);
AutoScreenMessage(SM_DoExit);
AutoScreenMessage(SM_AutoSaveSuccessful);
AutoScreenMessage(SM_SaveSuccessful);
AutoScreenMessage(SM_SaveSuccessNoSM);
AutoScreenMessage(SM_SaveFailed);


#endif
