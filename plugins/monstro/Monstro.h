/*
 * Monstro.h - a semi-modular 3-osc synth with modulation matrix
 *
 * Copyright (c) 2014 Vesa Kivimäki <contact/dot/diizy/at/nbl/dot/fi>
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


#ifndef MONSTRO_H
#define MONSTRO_H


#include "Instrument.h"
#include "InstrumentView.h"
#include "AutomatableModel.h"
#include "automatable_button.h"
#include "TempoSyncKnob.h"
#include "NotePlayHandle.h"
#include "pixmap_button.h"
#include "combobox.h"
#include "Oscillator.h"
// #include "fastpow.h" // once grejppi's fastpow gets merged


#define makeknob( name, x, y, hint, unit, oname ) 		\
	name = new knob( knobStyled, view ); 				\
	name ->move( x, y );								\
	name ->setHintText( tr( hint ) + " ", unit );		\
	name ->setObjectName( oname );						\
	name ->setFixedSize( 20, 20 );

#define maketsknob( name, x, y, hint, unit, oname ) 		\
	name = new TempoSyncKnob( knobStyled, view ); 				\
	name ->move( x, y );								\
	name ->setHintText( tr( hint ) + " ", unit );		\
	name ->setObjectName( oname );						\
	name ->setFixedSize( 20, 20 );

#define setwavemodel( name ) 												\
	name .addItem( tr( "Sine wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sin" ) ) );		\
	name .addItem( tr( "Triangle wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "tri" ) ) );	\
	name .addItem( tr( "Saw wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "saw" ) ) );			\
	name .addItem( tr( "Ramp wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "ramp" ) ) );		\
	name .addItem( tr( "Square wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sqr" ) ) );		\
	name .addItem( tr( "Soft square wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sqrsoft" ) ) );		\
	name .addItem( tr( "Moog saw wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "moog" ) ) );	\
	name .addItem( tr( "Abs. sine wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sinabs" ) ) );		\
	name .addItem( tr( "Exponential wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "exp" ) ) );	\
	name .addItem( tr( "White noise" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "noise" ) ) );

#define setlfowavemodel( name ) 												\
	name .addItem( tr( "Sine wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sin" ) ) );		\
	name .addItem( tr( "Triangle wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "tri" ) ) );	\
	name .addItem( tr( "Saw wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "saw" ) ) );			\
	name .addItem( tr( "Ramp wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "ramp" ) ) );		\
	name .addItem( tr( "Square wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sqr" ) ) );		\
	name .addItem( tr( "Soft square wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sqrsoft" ) ) );		\
	name .addItem( tr( "Moog saw wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "moog" ) ) );	\
	name .addItem( tr( "Abs. sine wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "sinabs" ) ) );		\
	name .addItem( tr( "Exponential wave" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "exp" ) ) );	\
	name .addItem( tr( "Random" ), static_cast<PixmapLoader*>( new PluginPixmapLoader( "rand" ) ) );


const int O1ROW = 22;
const int O2ROW = 22 + 39;
const int O3ROW = 22 + 39 * 2;
const int LFOROW = 22 + 39 * 3;
const int E1ROW = 22 + 39 * 4;
const int E2ROW = 22 + 39 * 5;

const int KNOBCOL1 = 16;
const int KNOBCOL2 = 16 + 30;
const int KNOBCOL3 = 16 + 30 * 2;
const int KNOBCOL4 = 16 + 30 * 3;
const int KNOBCOL5 = 16 + 30 * 4;
const int KNOBCOL6 = 16 + 30 * 5;
const int KNOBCOL7 = 16 + 30 * 6;

const int LFOCOL1 = KNOBCOL2;
const int LFOCOL2 = KNOBCOL2 + 26;
const int LFOCOL3 = KNOBCOL2 + 26*2;
const int LFOCOL4 = 171;
const int LFOCOL5 = 171 + 26;
const int LFOCOL6 = 171 + 26*2;

const int MATCOL1 = 32;
const int MATCOL2 = 32 + 25;
const int MATCOL3 = 32 + 25*2;
const int MATCOL4 = 32 + 25*3;
const int MATCOL5 = 149;
const int MATCOL6 = 149 + 25;
const int MATCOL7 = 149 + 25*2;
const int MATCOL8 = 149 + 25*3;

const int MATROW1 = 22;
const int MATROW2 = 22 + 39;
const int MATROW3 = 22 + 39*2;
const int MATROW4 = 22 + 39*3;
const int MATROW5 = 22 + 39*4;
const int MATROW6 = 22 + 39*5;

const int OPVIEW = 0;
const int MATVIEW = 1;

const int WAVE_SINE = 0;
const int WAVE_TRI = 1;
const int WAVE_SAW = 2;
const int WAVE_RAMP = 3;
const int WAVE_SQR = 4;
const int WAVE_SQRSOFT = 5;
const int WAVE_MOOG = 6;
const int WAVE_SINABS = 7;
const int WAVE_EXP = 8;
const int WAVE_NOISE = 9;
const int NUM_WAVES = 10;

const int MOD_MIX = 0;
const int MOD_AM = 1;
const int MOD_FM = 2;
const int MOD_PM = 3;
const int NUM_MODS = 4;

const float MODCLIP = 2.0;

const float MIN_FREQ = 18.0f;
const float MAX_FREQ = 48000.0f;

class MonstroInstrument;
class MonstroView;

// use grejppi's once it's merged
inline double fastPow( double a, double b )
{
	union
	{
		double d;
		int32_t x[2];
	} u = { a };
  u.x[1] = static_cast<int32_t>( b * ( u.x[1] - 1072632447 ) + 1072632447 );
  u.x[0] = 0;
  return u.d;
}


class MonstroSynth 
{
public:
	MonstroSynth( MonstroInstrument * _i, NotePlayHandle * _nph,
					const sample_rate_t _samplerate, fpp_t _frames );
	virtual ~MonstroSynth();

	void renderOutput( fpp_t _frames, sampleFrame * _buf );

	inline sample_rate_t samplerate() const
	{
		return m_samplerate;
	}
	
private:

	MonstroInstrument * m_parent;
	NotePlayHandle * m_nph;
	const sample_rate_t m_samplerate;
	fpp_t m_fpp;

	sample_t * m_env1_buf;
	sample_t * m_env2_buf;
	sample_t * m_lfo1_buf;
	sample_t * m_lfo2_buf;

	void renderModulators( fpp_t _frames );

	// linear interpolation
	inline sample_t interpolate( sample_t s1, sample_t s2, float x )
	{
		return s1 + ( s2 - s1 ) * x;
	}

	
	inline sample_t calcSlope( sample_t _s, float _slope )
	{
		if( _slope == 0.0f ) return _s;
		const double exp = fastPow( 10.0, static_cast<double>( _slope * -1.0 ) );
		return fastPow( _s, exp );
	}
	
	inline sample_t oscillate( int _wave, const float _ph )
	{
		switch( _wave )
		{
			case WAVE_SINE:
				return Oscillator::sinSample( _ph );
				break;
			case WAVE_TRI:
				return Oscillator::triangleSample( _ph );
				break;
			case WAVE_SAW:
				return Oscillator::sawSample( _ph );
				break;
			case WAVE_RAMP:
				return Oscillator::sawSample( _ph ) * -1.0;
				break;
			case WAVE_SQR:
				return Oscillator::squareSample( _ph );
				break;
			case WAVE_SQRSOFT:
			{
				const float ph = fraction( _ph );
				if( ph < 0.1 )	return Oscillator::sinSample( ph * 5 + 0.75 );
				else if( ph < 0.5 ) return 1.0f;
				else if( ph < 0.6 ) return Oscillator::sinSample( ph * 5 + 0.75 );
				else return -1.0f;
				break;
			}
			case WAVE_MOOG:
				return Oscillator::moogSawSample( _ph );
				break;
			case WAVE_SINABS:
				return qAbs( Oscillator::sinSample( _ph ) );
				break;
			case WAVE_EXP:
				return Oscillator::expSample( _ph );
				break;
			case WAVE_NOISE:
			default:
				return Oscillator::noiseSample( _ph );
				break;
		}
		return 0.0;
	}
	

	float m_osc1l_phase;
	float m_osc1r_phase;
	float m_osc2l_phase;
	float m_osc2r_phase;
	float m_osc3l_phase;
	float m_osc3r_phase;

	sample_t m_env1_phase;
	sample_t m_env2_phase;
	
	sample_t m_lfo1_last;
	sample_t m_lfo2_last;

};

class MonstroInstrument : public Instrument
{
	Q_OBJECT
public:
	MonstroInstrument( InstrumentTrack * _instrument_track );
	virtual ~MonstroInstrument();

	virtual void playNote( NotePlayHandle * _n,
						sampleFrame * _working_buffer );
	virtual void deleteNotePluginData( NotePlayHandle * _n );

	virtual void saveSettings( QDomDocument & _doc,
							QDomElement & _this );
	virtual void loadSettings( const QDomElement & _this );

	virtual QString nodeName() const;

	virtual f_cnt_t desiredReleaseFrames() const;

	virtual PluginView * instantiateView( QWidget * _parent );

public slots:
	void updateVolumes();
	void updateFreq();
	void updatePO();
	void updateEnvelope1();
	void updateEnvelope2();
	void updateLFOAtts();
	void updateSamplerate();

protected:
	float m_osc1l_vol;
	float m_osc1r_vol;
	float m_osc2l_vol;
	float m_osc2r_vol;
	float m_osc3l_vol;
	float m_osc3r_vol;

	float m_osc1l_freq;
	float m_osc1r_freq;
	float m_osc2l_freq;
	float m_osc2r_freq;
	float m_osc3_freq;
	
	float m_osc1l_po;
	float m_osc1r_po;
	float m_osc2l_po;
	float m_osc2r_po;
	float m_osc3l_po;
	float m_osc3r_po;

	float m_env1_pre;
	float m_env1_att;
	float m_env1_hold;
	float m_env1_dec;
	float m_env1_rel;
	
	float m_env2_pre;
	float m_env2_att;
	float m_env2_hold;
	float m_env2_dec;
	float m_env2_rel;

	f_cnt_t m_env1_len;
	f_cnt_t m_env2_len;
	
	f_cnt_t m_env1_relF;
	f_cnt_t m_env2_relF;
	
	f_cnt_t m_lfo1_att;
	f_cnt_t m_lfo2_att;
	
	sample_rate_t m_samplerate;
	fpp_t m_fpp;

private:
	inline float leftCh( float _vol, float _pan )
	{
		return ( _pan <= 0 ? 1.0 : 1.0 - ( _pan / 100.0 ) ) * _vol / 100.0;
	}

	inline float rightCh( float _vol, float _pan )
	{
		return ( _pan >= 0 ? 1.0 : 1.0 + ( _pan / 100.0 ) ) * _vol / 100.0;
	}

//////////////////////////////////////
//            models of             //
//	    operator view knobs         //
//                                  //
//////////////////////////////////////

	FloatModel	m_osc1Vol;
	FloatModel	m_osc1Pan;
	FloatModel	m_osc1Crs;
	FloatModel	m_osc1Ftl;
	FloatModel	m_osc1Ftr;
	FloatModel	m_osc1Spo;
	FloatModel	m_osc1Pw;

	FloatModel	m_osc2Vol;
	FloatModel	m_osc2Pan;
	FloatModel	m_osc2Crs;
	FloatModel	m_osc2Ftl;
	FloatModel	m_osc2Ftr;
	FloatModel	m_osc2Spo;
	ComboBoxModel	m_osc2Wave;

	FloatModel	m_osc3Vol;
	FloatModel	m_osc3Pan;
	FloatModel	m_osc3Crs;
	FloatModel	m_osc3Spo;
	FloatModel	m_osc3Sub;
	ComboBoxModel	m_osc3Wave1;
	ComboBoxModel	m_osc3Wave2;

	ComboBoxModel	m_lfo1Wave;
	TempoSyncKnobModel	m_lfo1Att;
	TempoSyncKnobModel	m_lfo1Rate;
	FloatModel	m_lfo1Phs;

	ComboBoxModel	m_lfo2Wave;
	TempoSyncKnobModel	m_lfo2Att;
	TempoSyncKnobModel	m_lfo2Rate;
	FloatModel	m_lfo2Phs;

	TempoSyncKnobModel	m_env1Pre;
	TempoSyncKnobModel	m_env1Att;
	TempoSyncKnobModel	m_env1Hold;
	TempoSyncKnobModel	m_env1Dec;
	FloatModel	m_env1Sus;
	TempoSyncKnobModel	m_env1Rel;
	FloatModel	m_env1Slope;

	TempoSyncKnobModel	m_env2Pre;
	TempoSyncKnobModel	m_env2Att;
	TempoSyncKnobModel	m_env2Hold;
	TempoSyncKnobModel	m_env2Dec;
	FloatModel	m_env2Sus;
	TempoSyncKnobModel	m_env2Rel;
	FloatModel	m_env2Slope;

	IntModel	m_o23Mod;

	IntModel	m_selectedView;

//////////////////////////////////////
//          models of               //
//	modulation matrix view knobs    //
//                                  //
//////////////////////////////////////

	FloatModel 	m_vol1env1;
	FloatModel	m_vol1env2;
	FloatModel	m_vol1lfo1;
	FloatModel	m_vol1lfo2;

	FloatModel 	m_vol2env1;
	FloatModel	m_vol2env2;
	FloatModel	m_vol2lfo1;
	FloatModel	m_vol2lfo2;

	FloatModel 	m_vol3env1;
	FloatModel	m_vol3env2;
	FloatModel	m_vol3lfo1;
	FloatModel	m_vol3lfo2;

	FloatModel 	m_phs1env1;
	FloatModel	m_phs1env2;
	FloatModel	m_phs1lfo1;
	FloatModel	m_phs1lfo2;

	FloatModel 	m_phs2env1;
	FloatModel	m_phs2env2;
	FloatModel	m_phs2lfo1;
	FloatModel	m_phs2lfo2;

	FloatModel 	m_phs3env1;
	FloatModel	m_phs3env2;
	FloatModel	m_phs3lfo1;
	FloatModel	m_phs3lfo2;

	FloatModel 	m_pit1env1;
	FloatModel	m_pit1env2;
	FloatModel	m_pit1lfo1;
	FloatModel	m_pit1lfo2;

	FloatModel 	m_pit2env1;
	FloatModel	m_pit2env2;
	FloatModel	m_pit2lfo1;
	FloatModel	m_pit2lfo2;

	FloatModel 	m_pit3env1;
	FloatModel	m_pit3env2;
	FloatModel	m_pit3lfo1;
	FloatModel	m_pit3lfo2;

	FloatModel 	m_pw1env1;
	FloatModel	m_pw1env2;
	FloatModel	m_pw1lfo1;
	FloatModel	m_pw1lfo2;

	FloatModel 	m_sub3env1;
	FloatModel	m_sub3env2;
	FloatModel	m_sub3lfo1;
	FloatModel	m_sub3lfo2;

	friend class MonstroSynth;
	friend class MonstroView;

};

class MonstroView : public InstrumentView
{
	Q_OBJECT
public:
	MonstroView( Instrument * _instrument,
					QWidget * _parent );
	virtual ~MonstroView();

protected slots:
	void updateLayout();

private:
	virtual void modelChanged();

	void setWidgetBackground( QWidget * _widget, const QString & _pic );
	QWidget * setupOperatorsView( QWidget * _parent );
	QWidget * setupMatrixView( QWidget * _parent );

//////////////////////////////////////
//                                  //
//	    operator view knobs         //
//                                  //
//////////////////////////////////////

	knob *	m_osc1VolKnob;
	knob *	m_osc1PanKnob;
	knob *	m_osc1CrsKnob;
	knob *	m_osc1FtlKnob;
	knob *	m_osc1FtrKnob;
	knob *	m_osc1SpoKnob;
	knob *	m_osc1PwKnob;

	knob *	m_osc2VolKnob;
	knob *	m_osc2PanKnob;
	knob *	m_osc2CrsKnob;
	knob *	m_osc2FtlKnob;
	knob *	m_osc2FtrKnob;
	knob *	m_osc2SpoKnob;
	comboBox *	m_osc2WaveBox;

	knob *	m_osc3VolKnob;
	knob *	m_osc3PanKnob;
	knob *	m_osc3CrsKnob;
	knob *	m_osc3SpoKnob;
	knob *	m_osc3SubKnob;
	comboBox *	m_osc3Wave1Box;
	comboBox *	m_osc3Wave2Box;

	comboBox *	m_lfo1WaveBox;
	TempoSyncKnob *	m_lfo1AttKnob;
	TempoSyncKnob *	m_lfo1RateKnob;
	knob *	m_lfo1PhsKnob;

	comboBox *	m_lfo2WaveBox;
	TempoSyncKnob *	m_lfo2AttKnob;
	TempoSyncKnob *	m_lfo2RateKnob;
	knob *	m_lfo2PhsKnob;

	TempoSyncKnob *	m_env1PreKnob;
	TempoSyncKnob *	m_env1AttKnob;
	TempoSyncKnob *	m_env1HoldKnob;
	TempoSyncKnob *	m_env1DecKnob;
	knob *	m_env1SusKnob;
	TempoSyncKnob *	m_env1RelKnob;
	knob *	m_env1SlopeKnob;

	TempoSyncKnob *	m_env2PreKnob;
	TempoSyncKnob *	m_env2AttKnob;
	TempoSyncKnob *	m_env2HoldKnob;
	TempoSyncKnob *	m_env2DecKnob;
	knob *	m_env2SusKnob;
	TempoSyncKnob *	m_env2RelKnob;
	knob *	m_env2SlopeKnob;

	automatableButtonGroup * m_o23ModGroup;

	automatableButtonGroup * m_selectedViewGroup;

	QWidget * m_operatorsView;
	QWidget * m_matrixView;

/////////////////////////////////
//                             //
//    matrix view knobs        //
//                             //
/////////////////////////////////
	
	knob * 	m_vol1env1Knob;
	knob *	m_vol1env2Knob;
	knob *	m_vol1lfo1Knob;
	knob *	m_vol1lfo2Knob;

	knob * 	m_vol2env1Knob;
	knob *	m_vol2env2Knob;
	knob *	m_vol2lfo1Knob;
	knob *	m_vol2lfo2Knob;

	knob * 	m_vol3env1Knob;
	knob *	m_vol3env2Knob;
	knob *	m_vol3lfo1Knob;
	knob *	m_vol3lfo2Knob;

	knob * 	m_phs1env1Knob;
	knob *	m_phs1env2Knob;
	knob *	m_phs1lfo1Knob;
	knob *	m_phs1lfo2Knob;

	knob * 	m_phs2env1Knob;
	knob *	m_phs2env2Knob;
	knob *	m_phs2lfo1Knob;
	knob *	m_phs2lfo2Knob;

	knob * 	m_phs3env1Knob;
	knob *	m_phs3env2Knob;
	knob *	m_phs3lfo1Knob;
	knob *	m_phs3lfo2Knob;

	knob * 	m_pit1env1Knob;
	knob *	m_pit1env2Knob;
	knob *	m_pit1lfo1Knob;
	knob *	m_pit1lfo2Knob;

	knob * 	m_pit2env1Knob;
	knob *	m_pit2env2Knob;
	knob *	m_pit2lfo1Knob;
	knob *	m_pit2lfo2Knob;

	knob * 	m_pit3env1Knob;
	knob *	m_pit3env2Knob;
	knob *	m_pit3lfo1Knob;
	knob *	m_pit3lfo2Knob;

	knob * 	m_pw1env1Knob;
	knob *	m_pw1env2Knob;
	knob *	m_pw1lfo1Knob;
	knob *	m_pw1lfo2Knob;

	knob * 	m_sub3env1Knob;
	knob *	m_sub3env2Knob;
	knob *	m_sub3lfo1Knob;
	knob *	m_sub3lfo2Knob;	

};


#endif
