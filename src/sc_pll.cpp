/** @file sc_pll.cpp
 *  @brief Phase Locked Loop top level module implementation
 *
 *  @author Americo Dias
 *  @date 06/02/2018
 */
#include "sc_pll.h"

/**
 * PLL implementation
 */
sc_pll::sc_pll (sc_module_name name_,
				// Phase detector
				double tr_,
				double tf_,
				// Charge pump
				double tstep_,
				double vcc_,
				double current_up_,
				double current_dn_,
				double current_leak_,
				double mosfet_vth_,
				// Loop filter
				int order_,
				double r1_value_,
				double c1_value_,
				double c2_value_,
				double r3_value_,
				double c3_value_,
				// Voltage controlled oscillator
				//double tstep_,
				//double vcc_,
				double vcm_,
				double kvo_,
				double fmin_,
				// Divider
				//double vcm_,
				double factor_):
				sc_module(name_) {

	// Declaration of the building blocks
	sc_pf 		= new sc_pfdetector("sc_pf", tr_, tf_);
	sc_lf 		= new sc_lfilter("sc_lf", order_, r1_value_, c1_value_, c2_value_, r3_value_, c3_value_);
	sca_tdf_div = new sca_tdf_divider("sca_tdf_div", vcm_, factor_);
	sca_tdf_cp 	= new sca_tdf_chargepump("sca_tdf_cp", tstep_, vcc_, current_up_, current_dn_,current_leak_, mosfet_vth_);
	sca_tdf_vco = new sca_tdf_vcoscillator("sca_tdf_vco", tstep_, vcc_, vcm_, kvo_, fmin_);

	// Phase detector:
	sc_in_fref = &sc_pf->sc_in_fref;
	sc_pf->sc_out_up(sc_sig_up);
	sc_pf->sc_out_dn(sc_sig_dn);
	sc_pf->sc_in_fdiv(sc_sig_fdiv);

	// Charge pump:
	sca_tdf_cp->sc_in_up(sc_sig_up);
	sca_tdf_cp->sc_in_dn(sc_sig_dn);
	sca_tdf_cp->sca_tdf_in_vctrl(sca_tdf_sig_vctrl);
	sca_tdf_cp->sca_tdf_out_ictrl(sca_tdf_sig_ictrl);

	// Loop filter:
	sc_lf->sca_tdf_in_ictrl(sca_tdf_sig_ictrl);
	sc_lf->sca_tdf_out_vctrl(sca_tdf_sig_vctrl);

	// Voltage controlled oscillator:
	sca_tdf_vco->sca_tdf_in_vctrl(sca_tdf_sig_vctrl);
	sca_tdf_vco->sca_tdf_out_fout(sca_tdf_sig_fout);
	//sca_tdf_out_fout = &sca_tdf_vco->sca_tdf_out_fout;
	sca_tdf_out_fout_freq = &sca_tdf_vco->sca_tdf_out_fout_freq;

	// Divider
	sca_tdf_div->sca_tdf_in_fin(sca_tdf_sig_fout);
	sca_tdf_div->sc_out_fout(sc_sig_fdiv);

}
