#include "global.h"
/*
-----------------------------------------------------------------------------
 Class: ScreenSelectStyle

 Desc: See header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "ScreenSelectStyle.h"
#include "GameManager.h"
#include "RageSounds.h"
#include "ThemeManager.h"
#include "PrefsManager.h"
#include "ScreenManager.h"
#include "GameState.h"
#include "AnnouncerManager.h"
#include "ActorUtil.h"
#include "LightsManager.h"


#define ICON_GAIN_FOCUS_COMMAND		THEME->GetMetric (m_sName,"IconGainFocusCommand")
#define ICON_LOSE_FOCUS_COMMAND		THEME->GetMetric (m_sName,"IconLoseFocusCommand")
#define DISABLED_COLOR				THEME->GetMetricC(m_sName,"DisabledColor")


ScreenSelectStyle::ScreenSelectStyle( CString sClassName ) : ScreenSelect( sClassName )
{
	m_iSelection = 0;

	LIGHTSMAN->SetLightsMode( LIGHTSMODE_MENU );

	unsigned i;
	for( i=0; i<m_aModeChoices.size(); i++ )
	{
		const ModeChoice& mc = m_aModeChoices[i];

		//
		// Load icon
		//
		CString sIconElementName = ssprintf("%s icon%d",m_sName.c_str(), i+1);
		CString sIconPath = THEME->GetPathToG(sIconElementName);

		m_textIcon[i].SetName( ssprintf("Icon%d",i+1) );
		m_sprIcon[i].SetName( ssprintf("Icon%d",i+1) );

		if( sIconPath.empty() )	// element doesn't exist
		{
			m_textIcon[i].LoadFromFont( THEME->GetPathToF("Common normal") );
			m_textIcon[i].SetText( mc.m_sName );
			m_textIcon[i].SetZoom(0.5f);
			this->AddChild( &m_textIcon[i] );
		}
		else
		{
			m_sprIcon[i].Load( sIconPath );
			this->AddChild( &m_sprIcon[i] );
		}
	

		//
		// Load Picture
		//
		CString sPictureElementName = ssprintf("%s picture%d",m_sName.c_str(),i+1);
		CString sPicturePath = THEME->GetPathToG(sPictureElementName);
		if( sPicturePath != "" )
		{
			m_sprPicture[i].SetName( ssprintf("Picture") );
			m_sprPicture[i].Load( sPicturePath );
			m_sprPicture[i].SetDiffuse( RageColor(1,1,1,0) );
			this->AddChild( &m_sprPicture[i] );
		}


		//
		// Load info
		//
		CString sInfoElementName = ssprintf("%s info%d",m_sName.c_str(),i+1);
		CString sInfoPath = THEME->GetPathToG(sInfoElementName);
		if( sInfoPath != "" )
		{
			m_sprInfo[i].SetName( ssprintf("Info") );
			m_sprInfo[i].Load( sInfoPath );
			m_sprInfo[i].SetDiffuse( RageColor(1,1,1,0) );
			this->AddChild( &m_sprInfo[i] );
		}
	}


	m_sprWarning.SetName( "Warning" );
	m_sprWarning.Load( THEME->GetPathToG(m_sName+" warning") );
	this->AddChild( &m_sprWarning );
		
	m_sprExplanation.SetName( "Explanation" );
	m_sprExplanation.Load( THEME->GetPathToG(m_sName + " explanation") );
	this->AddChild( &m_sprExplanation );
		


	// fix Z ordering of Picture and Info so that they draw on top
	for( i=0; i<this->m_aModeChoices.size(); i++ )
		this->MoveToTail( &m_sprPicture[i] );
	for( i=0; i<this->m_aModeChoices.size(); i++ )
		this->MoveToTail( &m_sprInfo[i] );


	this->UpdateSelectableChoices();

	m_sprPremium.SetName( "Premium" );

	switch( PREFSMAN->m_Premium )
	{
	case PrefsManager::DOUBLES_PREMIUM:
		m_sprPremium.Load( THEME->GetPathToG(m_sName + " doubles premium") );
		this->AddChild( &m_sprPremium );
		break;
	case PrefsManager::JOINT_PREMIUM:
		m_sprPremium.Load( THEME->GetPathToG(m_sName + " joint premium") );
		this->AddChild( &m_sprPremium );
		break;
	}


	m_soundChange.Load( THEME->GetPathToS(m_sName + " change") );
	m_soundSelect.Load( THEME->GetPathToS("Common start") );


	//
	// TweenOnScreen
	//
	for( i=0; i<m_aModeChoices.size(); i++ )
	{
		SET_XY_AND_ON_COMMAND( m_textIcon[i] );
		SET_XY_AND_ON_COMMAND( m_sprIcon[i] );
	}
	SET_XY_AND_ON_COMMAND( m_sprExplanation );
	SET_XY_AND_ON_COMMAND( m_sprWarning );
	SET_XY_AND_ON_COMMAND( m_sprPremium );

	// let AfterChange tween Picture and Info
	this->SortByZ();
}

void ScreenSelectStyle::MenuLeft( PlayerNumber pn )
{
	int iSwitchToIndex = -1;	// -1 means none found
	for( int i=m_iSelection-1; i>=0; i-- )
	{
		if( m_aModeChoices[i].IsPlayable() )
		{
			iSwitchToIndex = i;
			break;
		}
	}

	if( iSwitchToIndex == -1 )
		return;

	BeforeChange();
	m_iSelection = iSwitchToIndex;
	m_soundChange.Play();
	AfterChange();
}

void ScreenSelectStyle::MenuRight( PlayerNumber pn )
{
	int iSwitchToIndex = -1;	// -1 means none found
	for( unsigned i=m_iSelection+1; i<m_aModeChoices.size(); i++ )	
	{
		if( m_aModeChoices[i].IsPlayable() )
		{
			iSwitchToIndex = i;
			break;
		}
	}

	if( iSwitchToIndex == -1 )
		return;

	BeforeChange();
	m_iSelection = iSwitchToIndex;
	m_soundChange.Play();
	AfterChange();
}

void ScreenSelectStyle::MenuStart( PlayerNumber pn )
{
	m_soundSelect.Play();
	SCREENMAN->SendMessageToTopScreen( SM_AllDoneChoosing );

	const ModeChoice& mc = m_aModeChoices[GetSelectionIndex(pn)];
	SOUND->PlayOnceFromDir( ANNOUNCER->GetPathTo(ssprintf("%s comment %s",m_sName.c_str(),mc.m_sName.c_str())) );

	//
	// TweenOffScreen
	//
	unsigned i;

	/* Stop all tweens where they are, since we might have selected before
	 * we finished tweening in. */
	for( i=0; i<m_SubActors.size(); i++ )
		m_SubActors[i]->StopTweening();

	for( i=0; i<m_aModeChoices.size(); i++ )
	{
		OFF_COMMAND( m_sprIcon[i] );
		OFF_COMMAND( m_textIcon[i] );
	}
	OFF_COMMAND( m_sprExplanation );
	OFF_COMMAND( m_sprWarning );
	OFF_COMMAND( m_sprPicture[m_iSelection] );
	OFF_COMMAND( m_sprInfo[m_iSelection] );
	OFF_COMMAND( m_sprPremium );
}

int ScreenSelectStyle::GetSelectionIndex( PlayerNumber pn )
{
	return m_iSelection;
}

void ScreenSelectStyle::UpdateSelectableChoices()
{
	unsigned i;
	/* If a player joins during the tween-in, this diffuse change
	 * will be undone by the tween.  Hmm. 
	 *
	 * This is fixed now, since SetDiffuse() will affect the latest
	 * tween, if we're currently tweening. */
	for( i=0; i<m_aModeChoices.size(); i++ )
	{
		/* If the icon is text, use a dimmer diffuse, or we won't be
		 * able to see the glow. */
		if( m_aModeChoices[i].IsPlayable() )
		{
			m_sprIcon[i].SetDiffuse( RageColor(1,1,1,1) );
			m_textIcon[i].SetDiffuse( RageColor(0.5f,0.5f,0.5f,1) );	// gray so glow is visible
		}
		else
		{
			m_sprIcon[i].SetDiffuse( DISABLED_COLOR );
			m_textIcon[i].SetDiffuse( DISABLED_COLOR );
		}
	}

	// Select first enabled choie
	BeforeChange();

	int iSwitchToStyleIndex = -1;	// -1 means none found
	for( i=0; i<m_aModeChoices.size(); i++ )
	{
		const ModeChoice& mc = m_aModeChoices[i];
		if( mc.IsPlayable() )
		{
			iSwitchToStyleIndex = i;
			break;
		}
	}
	if( iSwitchToStyleIndex == -1 )// no styles are enabled.  We're stuck!
	{
		DEBUG_ASSERT(0);
		SCREENMAN->SystemMessage( "No Styles are selectable." );
		SCREENMAN->SetNewScreen( THEME->GetMetric("Common","InitialScreen") );
		return;
	}
	

	m_iSelection = iSwitchToStyleIndex;
	AfterChange();
}

void ScreenSelectStyle::BeforeChange()
{
	// dim/hide old selection
	m_sprIcon[m_iSelection].Command( ICON_LOSE_FOCUS_COMMAND );
	m_textIcon[m_iSelection].Command( ICON_LOSE_FOCUS_COMMAND );
	m_sprPicture[m_iSelection].StopTweening();
	m_sprInfo[m_iSelection].StopTweening();
	m_sprPicture[m_iSelection].SetDiffuse( RageColor(1,1,1,0) );
	m_sprInfo[m_iSelection].SetDiffuse( RageColor(1,1,1,0) );
	m_sprPicture[m_iSelection].SetGlow( RageColor(1,1,1,0) );
	m_sprInfo[m_iSelection].SetGlow( RageColor(1,1,1,0) );
}

void ScreenSelectStyle::AfterChange()
{
	m_sprIcon[m_iSelection].Command( ICON_GAIN_FOCUS_COMMAND );
	m_textIcon[m_iSelection].Command( ICON_GAIN_FOCUS_COMMAND );
	m_sprPicture[m_iSelection].SetDiffuse( RageColor(1,1,1,1) );
	m_sprInfo[m_iSelection].SetDiffuse( RageColor(1,1,1,1) );
	m_sprPicture[m_iSelection].SetZoom( 1 );
	m_sprInfo[m_iSelection].SetZoom( 1 );
	SET_XY_AND_ON_COMMAND( m_sprPicture[m_iSelection] );
	SET_XY_AND_ON_COMMAND( m_sprInfo[m_iSelection] );
}
