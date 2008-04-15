/*
 * stereomatrix_controls.cpp - controls for stereoMatrix-effect
 *
 * Copyright (c) 2008 Paul Giblock <drfaygo/at/gmail/dot/com>
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

#include "stereomatrix_controls.h"
#include "stereo_matrix.h"
#include "automatable_model_templates.h"


stereoMatrixControls::stereoMatrixControls( stereoMatrixEffect * _eff ) :
		effectControls( _eff ),
		m_effect( _eff ),
		m_llModel( 1.0f, 0.0f, 1.0f, 0.01f ),
		m_lrModel( 0.0f, 0.0f, 1.0f, 0.01f ),
		m_rlModel( 0.0f, 0.0f, 1.0f, 0.01f ),
		m_rrModel( 1.0f, 0.0f, 1.0f, 0.01f )
{
	connect( &m_llModel, SIGNAL( dataChanged( void ) ),
			this, SLOT( changeMatrix( void ) ) );
	connect( &m_lrModel, SIGNAL( dataChanged( void ) ),
			this, SLOT( changeMatrix( void ) ) );
	connect( &m_rlModel, SIGNAL( dataChanged( void ) ),
			this, SLOT( changeMatrix( void ) ) );
	connect( &m_rrModel, SIGNAL( dataChanged( void ) ),
			this, SLOT( changeMatrix( void ) ) );

	changeMatrix();
}



void stereoMatrixControls::changeMatrix( void )
{
}



void FASTCALL stereoMatrixControls::loadSettings(
		const QDomElement & _this )
{
	m_llModel.setValue( _this.attribute( "l-l" ).toFloat() );
	m_lrModel.setValue( _this.attribute( "l-r" ).toFloat() );
	m_rlModel.setValue( _this.attribute( "r-l" ).toFloat() );
	m_rrModel.setValue( _this.attribute( "r-r" ).toFloat() );
}




void FASTCALL stereoMatrixControls::saveSettings( QDomDocument & _doc, 
		QDomElement & _this )
{
	_this.setAttribute( "l-l", m_llModel.value() );
	_this.setAttribute( "l-r", m_llModel.value() );
	_this.setAttribute( "r-l", m_llModel.value() );
	_this.setAttribute( "r-r", m_llModel.value() );
}



#include "stereomatrix_controls.moc"
