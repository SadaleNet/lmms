#ifndef SINGLE_SOURCE_COMPILE

/*
 * song.cpp - root of the model-tree
 *
 * Copyright (c) 2004-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#include "song.h"


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>


#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>


#include "automation_track.h"
#include "automatable_model_templates.h"
#include "automation_editor.h"
#include "bb_editor.h"
#include "bb_track.h"
#include "config_mgr.h"
#include "embed.h"
#include "envelope_and_lfo_parameters.h"
#include "export_project_dialog.h"
#include "fx_mixer.h"
#include "import_filter.h"
#include "instrument_track.h"
#include "main_window.h"
#include "midi_client.h"
#include "mmp.h"
#include "note_play_handle.h"
#include "pattern.h"
#include "piano_roll.h"
#include "project_journal.h"
#include "project_notes.h"
#include "rename_dialog.h"
#include "song_editor.h"
#include "templates.h"
#include "text_float.h"
#include "timeline.h"




song::song( void ) :
	trackContainer(),
	m_automationTrack( track::create( track::AutomationTrack, this ) ),
	m_tempoModel( DEFAULT_BPM, MIN_BPM, MAX_BPM, intModel::defaultRelStep(),
									this ),
	m_masterVolumeModel( 100, 0, 200, 1, this ),
	m_masterPitchModel( 0, -12, 12, 1, this ),
	m_fileName(),
	m_oldFileName(),
	m_modified( FALSE ),
	m_exporting( FALSE ),
	m_playing( FALSE ),
	m_paused( FALSE ),
	m_loadingProject( FALSE ),
	m_playMode( Mode_PlaySong ),
	m_trackToPlay( NULL ),
	m_patternToPlay( NULL ),
	m_loopPattern( FALSE )
{
	m_tempoModel.setTrack( m_automationTrack );
	connect( &m_tempoModel, SIGNAL( dataChanged() ),
						this, SLOT( setTempo() ) );
	connect( &m_tempoModel, SIGNAL( dataUnchanged() ),
						this, SLOT( setTempo() ) );


	connect( engine::getMixer(), SIGNAL( sampleRateChanged() ), this,
					SLOT( updateFramesPerTact64th() ) );


	m_masterVolumeModel.setTrack( m_automationTrack );
	m_masterPitchModel.setTrack( m_automationTrack );

	connect( &m_masterVolumeModel, SIGNAL( dataChanged() ),
			this, SLOT( masterVolumeChanged() ) );
/*	connect( &m_masterPitchModel, SIGNAL( dataChanged() ),
			this, SLOT( masterPitchChanged() ) );*/

}




song::~song()
{
	m_playing = FALSE;
	delete m_automationTrack;
}




void song::masterVolumeChanged( void )
{
	engine::getMixer()->setMasterGain( m_masterVolumeModel.value() /
								100.0f );
}




void song::setTempo( void )
{
	const bpm_t tempo = m_tempoModel.value();
	playHandleVector & phv = engine::getMixer()->playHandles();
	for( playHandleVector::iterator it = phv.begin(); it != phv.end();
									++it )
	{
		notePlayHandle * nph = dynamic_cast<notePlayHandle *>( *it );
		if( nph && !nph->released() )
		{
			nph->resize( tempo );
		}
	}

//	m_bpmSpinBox->setInitValue( _new_bpm );
	engine::updateFramesPerTact64th();
	emit tempoChanged( tempo );
}




void song::doActions( void )
{
	while( !m_actions.empty() )
	{
		switch( m_actions.front() )
		{
			case ActionStop:
			{
				timeLine * tl =
					m_playPos[m_playMode].m_timeLine;
				m_playing = FALSE;
				if( tl != NULL )
				{

		switch( tl->behaviourAtStop() )
		{
			case timeLine::BackToZero:
				m_playPos[m_playMode].setTact( 0 );
				m_playPos[m_playMode].setTact64th( 0 );
				break;

			case timeLine::BackToStart:
				if( tl->savedPos() >= 0 )
				{
					m_playPos[m_playMode].setTact(
						tl->savedPos().getTact() );
					m_playPos[m_playMode].setTact64th(
						tl->savedPos().getTact64th() );
					tl->savePos( -1 );
				}
				break;

			case timeLine::KeepStopPosition:
			default:
				break;
		}

				}
				else
				{
					m_playPos[m_playMode].setTact( 0 );
					m_playPos[m_playMode].setTact64th( 0 );
				}

				m_playPos[m_playMode].setCurrentFrame( 0 );

				// remove all note-play-handles that are active
				engine::getMixer()->clear();

				break;
			}

			case ActionPlaySong:
				m_playMode = Mode_PlaySong;
				m_playing = TRUE;
				break;

			case ActionPlayTrack:
				m_playMode = Mode_PlayTrack;
				m_playing = TRUE;
				break;

			case ActionPlayBB:
				m_playMode = Mode_PlayBB;
				m_playing = TRUE;
				break;

			case ActionPlayPattern:
				m_playMode = Mode_PlayPattern;
				m_playing = TRUE;
				break;

			case ActionPause:
				m_playing = FALSE;// just set the play-flag
				m_paused = TRUE;
				break;

			case ActionResumeFromPause:
				m_playing = TRUE;// just set the play-flag
				m_paused = FALSE;
				break;
		}

		// a second switch for saving pos when starting to play
		// anything (need pos for restoring it later in certain
		// timeline-modes)
		switch( m_actions.front() )
		{
			case ActionPlaySong:
			case ActionPlayTrack:
			case ActionPlayBB:
			case ActionPlayPattern:
			{
				timeLine * tl =
					m_playPos[m_playMode].m_timeLine;
				if( tl != NULL )
				{
					tl->savePos( m_playPos[m_playMode] );
				}
				break;
			}

			// keep GCC happy...
			default:
				break;
		}

		m_actions.erase( m_actions.begin() );

	}

}




void song::processNextBuffer( void )
{
	doActions();

	if( m_playing == FALSE )
	{
		return;
	}

	QList<track *> trackList;
	Sint16 tco_num = -1;

	switch( m_playMode )
	{
		case Mode_PlaySong:
			trackList = tracks();
			// at song-start we have to reset the LFOs
			if( m_playPos[Mode_PlaySong] == 0 )
			{
				envelopeAndLFOParameters::resetLFO();
			}
			break;

		case Mode_PlayTrack:
			trackList.push_back( m_trackToPlay );
			break;

		case Mode_PlayBB:
			if( engine::getBBTrackContainer()->numOfBBs() > 0 )
			{
				tco_num = engine::getBBTrackContainer()->
								currentBB();
				trackList.push_back( bbTrack::findBBTrack(
								tco_num ) );
			}
			break;

		case Mode_PlayPattern:
			if( m_patternToPlay != NULL )
			{
				tco_num = m_patternToPlay->getTrack()->
						getTCONum( m_patternToPlay );
				trackList.push_back(
						m_patternToPlay->getTrack() );
			}
			break;

		default:
			return;

	}

	if( trackList.empty() == TRUE )
	{
		return;
	}

	// check for looping-mode and act if neccessary
	timeLine * tl = m_playPos[m_playMode].m_timeLine;
	bool check_loop = tl != NULL && m_exporting == FALSE &&
				tl->loopPointsEnabled() &&
				!( m_playMode == Mode_PlayPattern &&
					m_patternToPlay->freezing() == TRUE );
	if( check_loop )
	{
		if( m_playPos[m_playMode] < tl->loopBegin() ||
					m_playPos[m_playMode] >= tl->loopEnd() )
		{
			m_playPos[m_playMode].setTact(
						tl->loopBegin().getTact() );
			m_playPos[m_playMode].setTact64th(
						tl->loopBegin().getTact64th() );
		}
	}

	f_cnt_t total_frames_played = 0;
	float frames_per_tact64th = engine::framesPerTact64th();

	while( total_frames_played
				< engine::getMixer()->framesPerPeriod() )
	{
		f_cnt_t played_frames = engine::getMixer()
				->framesPerPeriod() - total_frames_played;

		float current_frame = m_playPos[m_playMode].currentFrame();
		// did we play a 64th of a tact?
		if( current_frame >= frames_per_tact64th )
		{
			int tact64th = m_playPos[m_playMode].getTact64th()
				+ (int)( current_frame / frames_per_tact64th );
			// did we play a whole tact?
			if( tact64th >= 64 )
			{
				// per default we just continue playing even if
				// there's no more stuff to play
				// (song-play-mode)
				int max_tact = m_playPos[m_playMode].getTact()
									+ 2;

				// then decide whether to go over to next tact
				// or to loop back to first tact
				if( m_playMode == Mode_PlayBB )
				{
					max_tact = engine::getBBTrackContainer()
							->lengthOfCurrentBB();
				}
				else if( m_playMode == Mode_PlayPattern &&
					m_loopPattern == TRUE &&
					tl != NULL &&
					tl->loopPointsEnabled() == FALSE )
				{
					max_tact = m_patternToPlay->length()
								.getTact();
				}
				if( m_playPos[m_playMode].getTact() + 1
								< max_tact )
				{
					// next tact
					m_playPos[m_playMode].setTact(
						m_playPos[m_playMode].getTact()
									+ 1 );
				}
				else
				{
					// first tact
					m_playPos[m_playMode].setTact( 0 );
				}
			}
			m_playPos[m_playMode].setTact64th( tact64th % 64 );

			if( check_loop )
			{
				if( m_playPos[m_playMode] >= tl->loopEnd() )
				{
					m_playPos[m_playMode].setTact(
						tl->loopBegin().getTact() );
					m_playPos[m_playMode].setTact64th(
						tl->loopBegin().getTact64th() );
				}
			}

			current_frame = fmodf( current_frame,
							frames_per_tact64th );
			m_playPos[m_playMode].setCurrentFrame( current_frame );
		}

		f_cnt_t last_frames = (f_cnt_t)frames_per_tact64th
						- (f_cnt_t)current_frame;
		// skip last frame fraction
		if( last_frames == 0 )
		{
			++total_frames_played;
			m_playPos[m_playMode].setCurrentFrame( current_frame
								+ 1.0f );
			continue;
		}
		// do we have some samples left in this tact64th but this are
		// less then samples we have to play?
		if( last_frames < played_frames )
		{
			// then set played_samples to remaining samples, the
			// rest will be played in next loop
			played_frames = last_frames;
		}

		if( (f_cnt_t)current_frame == 0 )
		{
			if( m_playMode == Mode_PlaySong )
			{
				m_automationTrack->play( m_playPos[m_playMode],
						played_frames,
						total_frames_played, tco_num );
			}

			// loop through all tracks and play them
			for( int i = 0; i < trackList.size(); ++i )
			{
				trackList[i]->play( m_playPos[m_playMode],
						played_frames,
						total_frames_played, tco_num );
			}
		}

		// update frame-counters
		total_frames_played += played_frames;
		m_playPos[m_playMode].setCurrentFrame( played_frames +
								current_frame );
	}
}




bool song::realTimeTask( void ) const
{
	return( !( m_exporting == TRUE || ( m_playMode == Mode_PlayPattern &&
		  	m_patternToPlay != NULL &&
			m_patternToPlay->freezing() == TRUE ) ) );
}




void song::play( void )
{
	if( m_playing == TRUE )
	{
		if( m_playMode != Mode_PlaySong )
		{
			// make sure, bb-editor updates/resets it play-button
			engine::getBBTrackContainer()->stop();
			//pianoRoll::inst()->stop();
		}
		else
		{
			pause();
			return;
		}
	}
	m_actions.push_back( ActionPlaySong );
}




void song::playTrack( track * _trackToPlay )
{
	if( m_playing == TRUE )
	{
		stop();
	}
	m_trackToPlay = _trackToPlay;

	m_actions.push_back( ActionPlayTrack );
}




void song::playBB( void )
{
	if( m_playing == TRUE )
	{
		stop();
	}
	m_actions.push_back( ActionPlayBB );
}




void song::playPattern( pattern * _patternToPlay, bool _loop )
{
	if( m_playing == TRUE )
	{
		stop();
	}
	m_patternToPlay = _patternToPlay;
	m_loopPattern = _loop;
	if( m_patternToPlay != NULL )
	{
		m_actions.push_back( ActionPlayPattern );
	}
}




tact song::lengthInTacts( void ) const
{
	tact len = 0;
	const QList<track *> & ctl = tracks();
	for( int i = 0; i < ctl.size(); ++i )
	{
		len = tMax( ctl[i]->length(), len );
	}
	return( len );
}




void song::setPlayPos( tact _tact_num, tact64th _t_64th, PlayModes _play_mode )
{
	m_playPos[_play_mode].setTact( _tact_num );
	m_playPos[_play_mode].setTact64th( _t_64th );
	m_playPos[_play_mode].setCurrentFrame( 0.0f );
}




void song::stop( void )
{
	m_actions.push_back( ActionStop );
}






void song::pause( void )
{
	m_actions.push_back( ActionPause );
}




void song::resumeFromPause( void )
{
	m_actions.push_back( ActionResumeFromPause );
}




void song::startExport( void )
{
	stop();
	doActions();

	play();
	doActions();

	m_exporting = TRUE;
}




void song::stopExport( void )
{
	stop();
	m_exporting = FALSE;
}






void song::insertBar( void )
{
	QList<track *> tl = tracks();
	for( int i = 0; i < tl.size(); ++i )
	{
		tl[i]->insertTact( m_playPos[Mode_PlaySong] );
	}
}




void song::removeBar( void )
{
	QList<track *> tl = tracks();
	for( int i = 0; i < tl.size(); ++i )
	{
		tl[i]->removeTact( m_playPos[Mode_PlaySong] );
	}
}




void song::addBBTrack( void )
{
	track * t = track::create( track::BBTrack, this );
	engine::getBBTrackContainer()->setCurrentBB(
						bbTrack::numOfBBTrack( t ) );
}




void song::addSampleTrack( void )
{
	(void) track::create( track::SampleTrack, this );
}




bpm_t song::getTempo( void )
{
	return( m_tempoModel.value() );
}




automationPattern * song::tempoAutomationPattern( void )
{
	return( m_tempoModel.getAutomationPattern() );
}




void song::clearProject( void )
{
	engine::getProjectJournal()->setJournalling( FALSE );

	if( m_playing )
	{
		stop();
	}

	engine::getMixer()->lock();
	if( engine::getBBEditor() )
	{
		engine::getBBEditor()->clearAllTracks();
	}
	if( engine::getSongEditor() )
	{
		engine::getSongEditor()->clearAllTracks();
	}
	if( engine::getFxMixerView() )
	{
		engine::getFxMixerView()->clear();
	}
	QCoreApplication::sendPostedEvents();
	engine::getBBTrackContainer()->clearAllTracks();
	clearAllTracks();

	engine::getFxMixer()->clear();

	if( engine::getAutomationEditor() )
	{
		engine::getAutomationEditor()->setCurrentPattern( NULL );
	}
	m_tempoModel.getAutomationPattern()->clear();
	m_masterVolumeModel.getAutomationPattern()->clear();
	m_masterPitchModel.getAutomationPattern()->clear();

	engine::getMixer()->unlock();

	if( engine::getProjectNotes() )
	{
		engine::getProjectNotes()->clear();
	}

	engine::getProjectJournal()->clearInvalidJournallingObjects();
	engine::getProjectJournal()->clearJournal();

	engine::getProjectJournal()->setJournalling( TRUE );
}





// create new file
void song::createNewProject( void )
{
	QString default_template = configManager::inst()->userProjectsDir()
						+ "templates/default.mpt";
	if( QFile::exists( default_template ) )
	{
		createNewProjectFromTemplate( default_template );
		return;
	}

	default_template = configManager::inst()->factoryProjectsDir()
						+ "templates/default.mpt";
	if( QFile::exists( default_template ) )
	{
		createNewProjectFromTemplate( default_template );
		return;
	}

	m_loadingProject = TRUE;

	clearProject();

	engine::getProjectJournal()->setJournalling( FALSE );

	m_fileName = m_oldFileName = "";

	if( engine::getMainWindow() )
	{
		engine::getMainWindow()->resetWindowTitle();
	}

	track * t;
	t = track::create( track::InstrumentTrack, this );
	dynamic_cast< instrumentTrack * >( t )->loadInstrument(
					"tripleoscillator" );
//	track::create( track::SampleTrack, this );
	t = track::create( track::InstrumentTrack,
						engine::getBBTrackContainer() );
	dynamic_cast< instrumentTrack * >( t )->loadInstrument(
						"tripleoscillator" );
	track::create( track::BBTrack, this );

	m_tempoModel.setInitValue( DEFAULT_BPM );
	m_masterVolumeModel.setInitValue( 100 );
	m_masterPitchModel.setInitValue( 0 );

	engine::getProjectJournal()->setJournalling( TRUE );

	m_loadingProject = FALSE;
}




void FASTCALL song::createNewProjectFromTemplate( const QString &
								_template )
{
	loadProject( _template );
	// clear file-name so that user doesn't overwrite template when
	// saving...
	m_fileName = m_oldFileName = "";
}




// load given song
void FASTCALL song::loadProject( const QString & _file_name )
{
	m_loadingProject = TRUE;

	clearProject();

	engine::getProjectJournal()->setJournalling( FALSE );

	m_fileName = _file_name;
	m_oldFileName = _file_name;

	multimediaProject mmp( m_fileName );
	// if file could not be opened, head-node is null and we create
	// new project
	if( mmp.head().isNull() )
	{
		createNewProject();
		return;
	}

	if( engine::getMainWindow() )
	{
		engine::getMainWindow()->resetWindowTitle();
	}

	// get the header information from the DOM
	m_tempoModel.loadSettings( mmp.head(), "bpm" );
	m_masterVolumeModel.loadSettings( mmp.head(), "mastervol" );
	m_masterPitchModel.loadSettings( mmp.head(), "masterpitch" );

	if( m_playPos[Mode_PlaySong].m_timeLine )
	{
		// reset loop-point-state
		m_playPos[Mode_PlaySong].m_timeLine->toggleLoopPoints( 0 );
	}

	QDomNode node = mmp.content().firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( node.nodeName() == "trackcontainer" )
			{
				( (journallingObject *)( this ) )->
					restoreState( node.toElement() );
			}
			else if( engine::hasGUI() )
			{
				if( node.nodeName() ==
					engine::getFxMixer()->nodeName() )
				{
					engine::getFxMixer()->restoreState(
							node.toElement() );
				}
				else if( node.nodeName() ==
					engine::getPianoRoll()->nodeName() )
				{
					engine::getPianoRoll()->restoreState(
							node.toElement() );
				}
				else if( node.nodeName() ==
					engine::getAutomationEditor()->
								nodeName() )
				{
					engine::getAutomationEditor()->
						restoreState( node.toElement() );
				}
				else if( node.nodeName() ==
						engine::getProjectNotes()->
								nodeName() )
				{
					( (journallingObject *)( engine::
							getProjectNotes() ) )->
						restoreState( node.toElement() );
				}
				else if( node.nodeName() ==
						m_playPos[Mode_PlaySong].
							m_timeLine->nodeName() )
				{
					m_playPos[Mode_PlaySong].
						m_timeLine->restoreState(
							node.toElement() );
				}
			}
		}
		node = node.nextSibling();
	}

	configManager::inst()->addRecentlyOpenedProject( _file_name );

	engine::getProjectJournal()->setJournalling( TRUE );

	m_loadingProject = FALSE;
}




// save current song
bool song::saveProject( void )
{
	multimediaProject mmp( multimediaProject::SongProject );

	m_tempoModel.saveSettings( mmp, mmp.head(), "bpm" );
	m_masterVolumeModel.saveSettings( mmp, mmp.head(), "mastervol" );
	m_masterPitchModel.saveSettings( mmp, mmp.head(), "masterpitch" );


	( (journallingObject *)( this ) )->saveState( mmp, mmp.content() );

	engine::getFxMixer()->saveState( mmp, mmp.content() );
	engine::getPianoRoll()->saveState( mmp, mmp.content() );
	engine::getAutomationEditor()->saveState( mmp, mmp.content() );
	( (journallingObject *)( engine::getProjectNotes() ) )->saveState( mmp,
								mmp.content() );
	m_playPos[Mode_PlaySong].m_timeLine->saveState( mmp, mmp.content() );

	m_fileName = mmp.nameWithExtension( m_fileName );
	if( mmp.writeFile( m_fileName ) == TRUE )
	{
		textFloat::displayMessage( tr( "Project saved" ),
					tr( "The project %1 is now saved."
							).arg( m_fileName ),
				embed::getIconPixmap( "project_save", 24, 24 ),
									2000 );
		configManager::inst()->addRecentlyOpenedProject( m_fileName );
		engine::getMainWindow()->resetWindowTitle();
	}
	else
	{
		textFloat::displayMessage( tr( "Project NOT saved." ),
				tr( "The project %1 was not saved!" ).arg(
							m_fileName ),
				embed::getIconPixmap( "error" ), 4000 );
		return( FALSE );
	}
	return( TRUE );
}




// save current song in given filename
bool FASTCALL song::saveProjectAs( const QString & _file_name )
{
	QString o = m_oldFileName;
	m_oldFileName = m_fileName;
	m_fileName = _file_name;
	if( saveProject() == FALSE )
	{
		m_fileName = m_oldFileName;
		m_oldFileName = o;
		return( FALSE );
	}
	m_oldFileName = m_fileName;
	return( TRUE );
}




void song::importProject( void )
{
	QFileDialog ofd( NULL, tr( "Import file" ) );
	ofd.setDirectory( configManager::inst()->userProjectsDir() );
	ofd.setFileMode( QFileDialog::ExistingFiles );
	if( ofd.exec () == QDialog::Accepted && !ofd.selectedFiles().isEmpty() )
	{
		importFilter::import( ofd.selectedFiles()[0], this );
	}
}


#warning TODO: move somewhere else
static inline QString baseName( const QString & _file )
{
	return( QFileInfo( _file ).absolutePath() + "/" +
			QFileInfo( _file ).completeBaseName() );
}


void song::exportProject( void )
{
	QString base_filename;

	if( m_fileName != "" )
	{
		base_filename = baseName( m_fileName );
	}
	else
	{
		base_filename = tr( "untitled" );
	}
 	base_filename += fileEncodeDevices[0].m_extension;

	QFileDialog efd( engine::getMainWindow() );
	efd.setFileMode( QFileDialog::AnyFile );

	int idx = 0;
	QStringList types;
	while( fileEncodeDevices[idx].m_fileType != NullFile )
	{
		types << tr( fileEncodeDevices[idx].m_description );
		++idx;
	}
	efd.setFilters( types );
	efd.selectFile( base_filename );
	efd.setWindowTitle( tr( "Select file for project-export..." ) );

	if( efd.exec() == QDialog::Accepted &&
		!efd.selectedFiles().isEmpty() && efd.selectedFiles()[0] != ""
		)
	{
		const QString export_file_name = efd.selectedFiles()[0];
		if( QFileInfo( export_file_name ).exists() == TRUE &&
			QMessageBox::warning( engine::getMainWindow(),
						tr( "File already exists" ),
						tr( "The file \"%1\" already "
							"exists. Do you want "
							"to overwrite it?"
						).arg( QFileInfo(
						export_file_name ).fileName() ),
						QMessageBox::Yes,
							QMessageBox::No |
							QMessageBox::Escape |
							QMessageBox::Default )
			== QMessageBox::No )
		{
			return;
		}
		exportProjectDialog epd( export_file_name,
						engine::getMainWindow() );
		epd.exec();
	}
}




void song::updateFramesPerTact64th( void )
{
	engine::updateFramesPerTact64th();
}




void song::setModified( void )
{
	if( !m_loadingProject )
	{
		m_modified = TRUE;
		if( engine::getMainWindow() )
		{
			engine::getMainWindow()->resetWindowTitle();
		}
	}
}




#include "song.moc"


#endif