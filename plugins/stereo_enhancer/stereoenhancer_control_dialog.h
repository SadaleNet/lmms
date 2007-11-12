/*
 * stereoenhancer_control_dialog.h - control-dialog for stereo-enhancer-effect
 *
 * Copyright (c) 2006 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _STEREOENHANCER_CONTROL_DIALOG_H
#define _STEREOENHANCER_CONTROL_DIALOG_H

#include "effect_control_dialog.h"

class knob;
class stereoEnhancerEffect;


class stereoEnhancerControlDialog : public effectControlDialog
{
	Q_OBJECT
public:
	stereoEnhancerControlDialog( QWidget * _parent, stereoEnhancerEffect * _eff );
	virtual ~stereoEnhancerControlDialog()
	{
	}

	virtual void FASTCALL saveSettings( QDomDocument & _doc, 
							QDomElement & _parent );
	virtual void FASTCALL loadSettings( const QDomElement & _this );
	inline virtual QString nodeName( void ) const
	{
		return( "stereoenhancercontrols" );
	}

	virtual ch_cnt_t getControlCount( void )
	{
		return( 1 );
	}


private slots:
	void changeWideCoeff( void );


private:
	stereoEnhancerEffect * m_effect;
	knob * m_widthKnob;

} ;

#endif