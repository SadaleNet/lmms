/*
 *
 * Copyright (c) 2008 Danny McRae <khjklujn/at/users.sourceforge.net>
 * 
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
#ifndef _MOOG_MODEL_H
#define _MOOG_MODEL_H

#include "stk_model.h"


class moogModel: public stkModel
{
public:
	moogModel( bool _monophonic = FALSE,
				StkFloat _portamento = 0.0f,
				StkFloat _bend = 0.0f,
				StkFloat _bend_range = 2.0f,
				bool _velocity_sensitive_lpf = TRUE,
				StkFloat _velocity_sensitive_q = 0.5f,
				StkFloat _volume = 1.0f,
				StkFloat _pan = 0.0f,
				StkFloat _spread = 0.0f,
				StkFloat _filter_q = 64.0f,
				StkFloat _filter_sweep_rate = 64.0f,
				StkFloat _vibrato_frequency = 64.0f,
				StkFloat _vibrato_gain = 64.0f );
	~moogModel();
	
	inline floatModel * filterQ( void ) const
	{
		return( m_filterQ );
	}
		
	inline floatModel * filterSweepRate( void ) const
	{
		return( m_filterSweepRate );
	}
	
	inline floatModel * vibratoFrequency( void ) const
	{
		return( m_vibratoFrequency );
	}
	
	inline floatModel * vibratoGain( void ) const
	{
		return( m_vibratoGain );
	}
	
	void FASTCALL saveSettings( QDomDocument & _doc, QDomElement & _parent );
	
	void FASTCALL loadSettings( const QDomElement & _this );

private:
	floatModel * m_filterQ;
	floatModel * m_filterSweepRate;
	floatModel * m_vibratoFrequency;
	floatModel * m_vibratoGain;
};

#endif