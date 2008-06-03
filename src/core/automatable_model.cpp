/*
 * automatable_model.cpp - some implementations of automatableModel-class
 *
 * Copyright (c) 2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#include <Qt/QtXml>


#include "automatable_model.h"
#include "automation_pattern.h"
#include "automation_editor.h"


float automatableModel::__copiedValue = 0;



template<>
inline float automatableModel::minEps<float>( void )
{
	return( 1.0e-10 );
}




automatableModel::automatableModel( DataType _type,
						const float _val,
						const float _min,
						const float _max,
						const float _step,
						::model * _parent,
						bool _default_constructed ) :
	model( _parent, _default_constructed ),
	m_dataType( _type ),
	m_value( _val ),
	m_initValue( _val ),
	m_minValue( _min ),
	m_maxValue( _max ),
	m_step( _step ),
	m_range( _max - _min ),
	m_journalEntryReady( FALSE ),
	m_controllerConnection( NULL ),
	m_automationPattern( NULL ),
	m_track( NULL )
{
}




automatableModel::~automatableModel()
{
	delete m_automationPattern;
	while( m_linkedModels.empty() == FALSE )
	{
		m_linkedModels.last()->unlinkModel( this );
		m_linkedModels.erase( m_linkedModels.end() - 1 );
	}

	if( m_controllerConnection )
	{
		delete m_controllerConnection;
	}
}



void automatableModel::saveSettings( QDomDocument & _doc, QDomElement & _this,
							const QString & _name )
{
	if( m_automationPattern && m_automationPattern->getTimeMap().size()
									> 1 )
	{
		QDomElement pattern_element;
		QDomNode node = _this.namedItem(
					automationPattern::classNodeName() );
		if( node.isElement() )
		{
			pattern_element = node.toElement();
		}
		else
		{
			pattern_element = _doc.createElement(
					automationPattern::classNodeName() );
			_this.appendChild( pattern_element );
		}
		QDomElement element = _doc.createElement( _name );
		m_automationPattern->saveSettings( _doc, element );
		pattern_element.appendChild( element );
	}
	else
	{
		_this.setAttribute( _name, m_value );
	}

	if( m_controllerConnection )
	{
		QDomElement controller_element;
		QDomNode node = _this.namedItem( "connection" );
		if( node.isElement() )
		{
			controller_element = node.toElement();
		}
		else
		{
			controller_element = _doc.createElement( "connection" );
			_this.appendChild( controller_element );
		}
		QDomElement element = _doc.createElement( _name );
		m_controllerConnection->saveSettings( _doc, element );
		controller_element.appendChild( element );
	}
}




void automatableModel::loadSettings( const QDomElement & _this,
						const QString & _name )
{
	QDomNode node = _this.namedItem( automationPattern::classNodeName() );
	if( node.isElement() && getAutomationPattern() )
	{
		node = node.namedItem( _name );
		if( node.isElement() )
		{
			m_automationPattern->loadSettings( node.toElement() );
			setValue( m_automationPattern->valueAt( 0 ) );
			return;
		}
	}

	node = _this.namedItem( "connection" );
	if( node.isElement() )
	{
		node = node.namedItem( _name );
		if( node.isElement() )
		{
			//m_controllerConnection = new controllerConnection( (controller*)NULL );
			setControllerConnection( new controllerConnection( (controller*)NULL ) );
			m_controllerConnection->loadSettings( node.toElement() );
		}
	}

	setInitValue( _this.attribute( _name ).toFloat() );
}




void automatableModel::setValue( const float _value )
{
	const float old_val = m_value;

	m_value = fittedValue( _value );
	if( old_val != m_value )
	{
		// add changes to history so user can undo it
		addJournalEntry( journalEntry( 0, m_value - old_val ) );

		// notify linked models

		// doesn't work because of implicit typename T
		for( autoModelVector::iterator it =
		 			m_linkedModels.begin();
				it != m_linkedModels.end(); ++it )
		{
			if( value<float>() != (*it)->value<float>() &&
				(*it)->fittedValue( value<float>() )
						!= (*it)->value<float>() )
			{
				bool journalling = (*it)->testAndSetJournalling(
							isJournalling() );
				(*it)->setValue( value<float>() );
				(*it)->setJournalling( journalling );
			}
		}
		setFirstValue();
		emit dataChanged();
	}
	else
	{
		emit dataUnchanged();
	}
}




void automatableModel::setRange( const float _min, const float _max,
							const float _step )
{
        if( ( m_maxValue != _max ) || ( m_minValue != _min ) )
	{
		m_minValue = _min;
		m_maxValue = _max;
		if( m_minValue > m_maxValue )
		{
			qSwap<float>( m_minValue, m_maxValue );
		}
		m_range = m_maxValue - m_minValue;
		setStep( _step );

		// re-adjust value
		setInitValue( value<float>() );

		emit propertiesChanged();
	}
}




void automatableModel::setStep( const float _step )
{
	if( m_step != _step )
	{
		m_step = _step;
		emit propertiesChanged();
	}
}




float automatableModel::fittedValue( float _value ) const
{
	_value = tLimit<float>( _value, m_minValue, m_maxValue );

	if( m_step != 0 )
	{
		_value = roundf( _value / m_step ) * m_step;
	}
	else
	{
		_value = m_minValue;
	}

	// correct rounding error at the border
	if( tAbs<float>( _value - m_maxValue ) <
				minEps<float>() * tAbs<float>( m_step ) )
	{
		_value = m_maxValue;
	}

	// correct rounding error if value = 0
	if( tAbs<float>( _value ) < minEps<float>() * tAbs<float>( m_step ) )
	{
		_value = 0;
	}

	return( _value );
}





void automatableModel::redoStep( journalEntry & _je )
{
	bool journalling = testAndSetJournalling( FALSE );
	setValue( value<float>() + _je.data().toDouble() );
	setJournalling( journalling );
}




void automatableModel::undoStep( journalEntry & _je )
{
	journalEntry je( _je.actionID(), -_je.data().toDouble() );
	redoStep( je );
}




void automatableModel::prepareJournalEntryFromOldVal( void )
{
	m_oldValue = value<float>();
	saveJournallingState( FALSE );
	m_journalEntryReady = TRUE;
}




void automatableModel::addJournalEntryFromOldToCurVal( void )
{
	if( m_journalEntryReady )
	{
		restoreJournallingState();
		if( value<float>() != m_oldValue )
		{
			addJournalEntry( journalEntry( 0, value<float>() -
								m_oldValue ) );
		}
		m_journalEntryReady = FALSE;
	}
}




void automatableModel::linkModel( automatableModel * _model )
{
	if( qFind( m_linkedModels.begin(), m_linkedModels.end(), _model )
						== m_linkedModels.end() )
	{
		m_linkedModels.push_back( _model );
	}
}




void automatableModel::unlinkModel( automatableModel * _model )
{
	if( qFind( m_linkedModels.begin(), m_linkedModels.end(), _model )
						!= m_linkedModels.end() )
	{
		m_linkedModels.erase( qFind( m_linkedModels.begin(),
							m_linkedModels.end(),
							_model ) );
	}
}






void automatableModel::linkModels( automatableModel * _model1,
						automatableModel * _model2 )
{
	_model1->linkModel( _model2 );
	_model2->linkModel( _model1 );

	if( _model1->m_automationPattern != _model2->m_automationPattern )
	{
		delete _model2->m_automationPattern;
		_model2->m_automationPattern = _model1->m_automationPattern;
	}
}




void automatableModel::unlinkModels( automatableModel * _model1,
						automatableModel * _model2 )
{
	_model1->unlinkModel( _model2 );
	_model2->unlinkModel( _model1 );

	if( _model1->m_automationPattern && _model1->m_automationPattern
					== _model2->m_automationPattern )
	{
		_model2->m_automationPattern = new automationPattern(
				*_model1->m_automationPattern, _model2 );
	}
}





void automatableModel::initAutomationPattern( void )
	{
		m_automationPattern = new automationPattern( NULL, this );
	}

automationPattern * automatableModel::getAutomationPattern( void )
{
	if( !m_automationPattern )
	{
		m_automationPattern = new automationPattern( m_track, this );
		setFirstValue();
//		syncAutomationPattern();
	}
	return( m_automationPattern );
}


void automatableModel::setFirstValue( void )
{
	if( m_automationPattern && m_automationPattern->updateFirst() )
	{
		m_automationPattern->putValue( 0, m_value, FALSE );
		if( engine::getAutomationEditor() &&
				engine::getAutomationEditor()->currentPattern()
						== m_automationPattern )
		{
			engine::getAutomationEditor()->update();
		}
	}
}




void automatableModel::setInitValue( const float _value )
{
	m_initValue = _value;
	bool journalling = testAndSetJournalling( FALSE );
	setValue( _value );
	if( m_automationPattern )
	{
		setFirstValue();
	}
	setJournalling( journalling );
}



void automatableModel::reset( void )
{
	setValue( initValue<float>() );
}




void automatableModel::copyValue( void )
{
	__copiedValue = value<float>();
}




void automatableModel::pasteValue( void )	
{
	setValue( __copiedValue );
}




#include "automatable_model.moc"
