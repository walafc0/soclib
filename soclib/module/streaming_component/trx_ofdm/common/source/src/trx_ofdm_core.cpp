/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors :
 * 
 * History :
 *
 * Comment :
 *
 */

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "trx_ofdm_core.h"

/*------------------------------------------------------------------------------
 * CLASS: trx_ofdm_core
 *----------------------------------------------------------------------------*/

// first compute step : framing
void trx_ofdm_core::compute() {

  trx_ofdm_core_config *p_cfg;

  // core of computation
  while(true) {
    wait(load_cfg_event) ;
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
    wait(clk_period,STEP_SIM_UNIT);
#endif

    // ##########
    // compute thread
 
    EXPRINT(trxofdm,1,"Start framing step");

    p_cfg = dynamic_cast<trx_ofdm_core_config*>(cfg_core->get_config(current_slot_id));

    if(p_cfg==NULL) {
      EXPRINT(trxofdm, 0,"ERROR : configuration slot " << current_slot_id << " not found");
      exit(0);
    }
    
    // save the current slot id configuration
    framing_slot_id = current_slot_id ;
    nb_buf_in_free--;

    // do framing phase
    framing(p_cfg->fft_type, p_cfg->log2_size_fft, p_cfg->floc, p_cfg->mask_data, p_cfg->mask_pilot);
    
    fft_slot_id->write(framing_slot_id);
    buf_fft->write(cur_buf_in);
    cur_buf_in = (cur_buf_in+1)%NB_BUF; // compute next buffer

    EXPRINT(trxofdm, 1,"End of framing computation");

    // check the availability of a new buffer and wait if not
    if (nb_buf_in_free == 0)
      wait(buf_in_free);

    // write status 0
    write_status(0);
    // write eoc
    write_eoc();

    // end
    // ##########
    
  }

};

void trx_ofdm_core::framing(fft_t fft_type, t_uint16 log2_size_fft, framing_loc floc, t_uint32 * mask_data, t_uint32 * mask_pilot) {
  
  // fft parameters
  int fft_size;
  // for framing purpose
  int ind_mask = 0;
  t_uint32 mask_comp;
  //
  int nb_data_read = 0;


  /***************************************/
  // take useful configuration elements
  /***************************************/ 
  fft_size = (int)pow((double)2,log2_size_fft) ; // compute the number of points for the fft 2**log2_size_fft
  EXPRINT(trxofdm,1,"size_fft=" << fft_size << ", fft_type =" << fft_type << ", floc =" << floc);
  
  // framing for ifft
  if (fft_type == ifft) {
    /***************************************/
    // framing computation
    // only in ifft mode
    /***************************************/
    
    EXPRINT(trxofdm,1, "begin framing computation : framing loc = " << floc);
    // compare the masks to decide the data to put in the input buffer of the fft, depending on location of pilots and data
    switch (floc) {
    case dp1fifo: // data and pilots are in the same FIFO 0 : all the data are first
      // first phase for data
      ind_mask = 0;
      mask_comp = 0x1;
      for (int i=0; i<fft_size; i++) {
	if (i!=0 && i%32==0) {
	  ind_mask++;
	  mask_comp = 0x1;
	}
	//EXPRINT(trxofdm,0,"mask_data[" << ind_mask << "] = " << hex << (int)p_cfg->mask_data[ind_mask] << " , mask_comp = " << (int)mask_comp << " and result = " << (int)(p_cfg->mask_data[ind_mask]&mask_comp)); 
	if ((mask_data[ind_mask]&mask_comp) == mask_comp) {
	  fft_in_buf[cur_buf_in][i] = read_fifo_in(0); // read is blocking
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
	  wait(clk_period,STEP_SIM_UNIT);
#endif
	  nb_data_read++;
	  EXPRINT(trxofdm,2, "trx_ofdm read the data number " << nb_data_read << " : " << hex << fft_in_buf[cur_buf_in][i] << dec << " from fifo 0");
	}
	else {
	  fft_in_buf[cur_buf_in][i] = 0x0;
	}
	mask_comp = mask_comp << 1; // shift one place
      } // end for data phase
	// second phase for pilots
      ind_mask = 0;
      mask_comp = 0x1;
      for (int i=0; i<fft_size; i++) {
	if (i!=0 && i%32==0) {
	  ind_mask++;
	  mask_comp = 0x1;
	}
	if ((mask_pilot[ind_mask]&mask_comp) == mask_comp) {
	  fft_in_buf[cur_buf_in][i] = read_fifo_in(0); // read is blocking
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
	  wait(clk_period,STEP_SIM_UNIT);
#endif
	  EXPRINT(trxofdm,2, "trx_ofdm read the pilot " << hex << fft_in_buf[cur_buf_in][i] << dec << " from fifo 0");
	}
	mask_comp = mask_comp << 1; // shift one place
      } // end for pilot phase
      break;
      // end of framing dp1fifo
      /******************************/ 
    case dp2fifo: // data are in FIFO 0 and pilots in FIFO 1
      // first phase for data
      ind_mask = 0;
      mask_comp = 0x1;
      for (int i=0; i<fft_size; i++) {
	if (i!=0 && i%32==0) {
	  ind_mask++;
	  mask_comp = 0x1;
	}
	if ((mask_data[ind_mask]&mask_comp) == mask_comp) {
	  fft_in_buf[cur_buf_in][i] = read_fifo_in(0); // read is blocking
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
	  wait(clk_period,STEP_SIM_UNIT);
#endif
	  nb_data_read++;
	  EXPRINT(trxofdm,2, "trx_ofdm read the data number " << nb_data_read << " : " << hex << fft_in_buf[cur_buf_in][i] << dec << " from fifo 0");
	}
	else {
	  fft_in_buf[cur_buf_in][i] = 0x0;
	}
	mask_comp = mask_comp << 1; // shift one place
      } // end for data phase
	// second phase for pilots
      ind_mask = 0;
      mask_comp = 0x1;
      for (int i=0; i<fft_size; i++) {
	if (i!=0 && i%32==0) {
	  ind_mask++;
	  mask_comp = 0x1;
	}
	if ((mask_pilot[ind_mask]&mask_comp) == mask_comp) {
	  fft_in_buf[cur_buf_in][i] = read_fifo_in(1); // read is blocking
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
	  wait(clk_period,STEP_SIM_UNIT);
#endif
	  EXPRINT(trxofdm,2, "trx_ofdm read the pilot " << hex << fft_in_buf[cur_buf_in][i] << dec << " from fifo 1");
	}
	mask_comp = mask_comp << 1; // shift one place
      } // end for pilot phase
      break;
    default:
      EXPRINT(trxofdm,0, "ERROR : location for framing is not correctly determined !");
      break;
    }
  }
  // no framing for fft, just get "fft_size" data in fifo 0
  else if (fft_type == fft){
    for (int i=0; i<fft_size; i++) {
      fft_in_buf[cur_buf_in][i] = read_fifo_in(0);
#ifdef TLM_DVFS_MODELISATION
      lpm->wait_n_cycles(1);
#else
      wait(clk_period,STEP_SIM_UNIT);
#endif
    }
  }
  else {
    EXPRINT(trxofdm, 0,"WARNING : fft_type " << fft_type << " is not allowed");
  }
  // end of framing phase  
}

void trx_ofdm_core::compute_fft() {
  
  trx_ofdm_core_config *p_cfg;
  int cur_slot_id;

  // fft parameters
  fft_t fft_type;
  int fft_size;
  int fft_time;
  
  // structures for using "FixedFftRadix_4_2" function (Dimitri Ktenas)
  FFT_RADIX_4_CTRL fft_ctrl;
  FixC *ptVectIn;
  FixC *ptVectOut;
  // initialize the size of these arrays
  ptVectIn  = new FixC[MAX_SIZE_FFT*2];
  ptVectOut = new FixC[MAX_SIZE_FFT*2];
  double value;
  short value_sh;
  float Rvalue;
  float Ivalue;
  
  // init pt_vect_in and pt_vect_out
  InitFixC1d(ptVectIn, MAX_SIZE_FFT*2, "ptVectIn", 16, 7, "sr");
  InitFixC1d(ptVectOut, MAX_SIZE_FFT*2, "ptVectOut", 16, 7, "sr");

  // core of computation
  while(true) {

    // wait for a buffer free at input
    cur_buf_fft = buf_fft->read(); // read is blocking
    // check if the corresponding output buffer is free
    while (!buf_out_free_flag[cur_buf_fft])
      wait(buf_out_free);
    // lock the output buffer
    buf_out_free_flag[cur_buf_fft] = false;
    EXPRINT(trxofdm,0,"begin fft operation on buffer " << cur_buf_fft);

#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
    wait(clk_period,STEP_SIM_UNIT);
#endif

    // take buffer in value + slotid value for computation
    cur_slot_id = fft_slot_id->read();
    EXPRINT(trxofdm,0,"begin fft operation with the configuration from slotid " << cur_slot_id);
        
    p_cfg = dynamic_cast<trx_ofdm_core_config*>(cfg_core->get_config(cur_slot_id));
    
    if(p_cfg==NULL) {
      EXPRINT(trxofdm, 0,"ERROR : configuration slot " << cur_slot_id << " not found");
      exit(0);
    }
    
    /***************************************/
    // fft core
    /***************************************/

    // take useful configuration elements
    EXPRINT(trxofdm, 1,"Start fft step");

    fft_type = p_cfg->fft_type;
    
    EXPRINT(trxofdm,2,"log2(size_fft)=" << p_cfg->log2_size_fft);
    fft_size = (int)pow((double)2,p_cfg->log2_size_fft) ; // compute the number of points for the fft 2**log2_size_fft
    EXPRINT(trxofdm,1,"size_fft=" << fft_size);

    // load fft control structure useful for "FixedFftRadix_4_2" function
    if (fft_type == fft)
      fft_ctrl.FftType=1;
    else
      fft_ctrl.FftType=0;
    fft_ctrl.FftSize=fft_size;
    if (p_cfg->norm_power==true)
      fft_ctrl.NormalizationPower = 1;
    else
      fft_ctrl.NormalizationPower = 0;
    fft_ctrl.OverflowTest = 1; // overflow test must be done
    
    /***************************************/
    // fft computation
    /***************************************/
    // load the number of points needed for the fft
    EXPRINT(trxofdm,2, "input of the fft :");
    for (int i=0; i<fft_size; i++) {      
      // transform the received words : 16 bits msb = Im, 16 bits lsb = Re
      // Im part
      value_sh = (short)(fft_in_buf[cur_buf_fft][i] & 0xffff); // take 16 bits 
      value = (double) (value_sh / EXP2(8));            // shift 8 bits on the right
      ptVectIn[2 * i + 1] = FixC(value,16,7);
      fft_in_buf[cur_buf_fft][i] = fft_in_buf[cur_buf_fft][i] >> 16;
      // Real part
      value_sh = (short)(fft_in_buf[cur_buf_fft][i] & 0xffff);
      value = (double)(value_sh / EXP2(8)); 
      ptVectIn[2 * i] = FixC(value,16,7);     
      EXPRINT(trxofdm,2, "Re[" << i << "] = " << (double)ptVectIn[2 * i] << " and Im[" << i << "] = " << (double)ptVectIn[2 * i + 1]);
      
      // NEW !!! shift carrier and shift parity for half-band shifting BEFORE FFT only
      if (fft_type == fft)
	if (p_cfg->shift_carrier)
	  if ((p_cfg->shift_parity && !(i%2)) || (!p_cfg->shift_parity && (i%2))) {
	    ptVectIn[2 * i] = -ptVectIn[2 * i];
	    ptVectIn[2 * i + 1] = -ptVectIn[2 * i + 1];
	  }
      

      EXPRINT(trxofdm,2, "après correction carrier : Re[" << i << "] = " << (double)ptVectIn[2 * i] << " and Im[" << i << "] = " << (double)ptVectIn[2 * i + 1]);
    }
    
    EXPRINT(trxofdm,1, "begin computation");
    // compute fft
    FixedFftRadix_4_2(ptVectIn,ptVectOut,&fft_ctrl);
    
    // wait time for fft
    fft_time = (int)(fft_size/2*log((float)fft_size)/log((float)4));
    EXPRINT(trxofdm,1, "fft time = " << fft_time);
    //for (int i=0; i<fft_time; i++)
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(fft_time*2);
#else
    wait(clk_period*fft_time*2, STEP_SIM_UNIT);
#endif

    // NEW !!! shift carrier and shift parity for half-band shifting AFTER IFFT only
   if (fft_type == ifft)
     if (p_cfg->shift_carrier)
       for (int i=0; i<fft_size; i++) {
	 if ((p_cfg->shift_parity && !(i%2)) || (!p_cfg->shift_parity && (i%2))) {
	  ptVectOut[2 * i] = -ptVectOut[2 * i];
	  ptVectOut[2 * i + 1] = -ptVectOut[2 * i + 1];
	}
       }

    
    // put fft results in output buffer
    EXPRINT(trxofdm,2, "result of the fft :");
    for (int i=0; i<fft_size; i++) {
      Rvalue = (float)ptVectOut[2*i+1];
      Ivalue = (float)ptVectOut[2*i];
      
      fft_out_buf[cur_buf_fft][i] = (t_uint32)((((short)(Ivalue*pow((double)2,8))) & 0xffff) << 16) + (t_uint32)(((short)(Rvalue*pow((double)2,8))) & 0xffff);
      EXPRINT(trxofdm,2, "Re[" << i << "] = " << Rvalue << ",Im[" << i << "] = " << Ivalue << " and result = " << hex << fft_out_buf[cur_buf_fft][i]<< dec );
    }

    /***************************************/
    // end of fft core
    /***************************************/

    // free one input buffer
    nb_buf_in_free++;
    buf_in_free.notify();
    // send slotid and numero buf information to deframing. 
    deframing_slot_id->write(cur_slot_id);
    buf_deframing->write(cur_buf_fft);

    EXPRINT(trxofdm,0,"end of fft for buffer " << cur_buf_fft << " and configuration slot " << cur_slot_id );
  } 

}

void trx_ofdm_core::deframing() {
  
  trx_ofdm_core_config *p_cfg;
  int cur_slot_id;
  fft_t fft_type;
  int fft_size;
  // for deframing purpose
  int ind_mask = 0;
  t_uint32 mask_comp;

  // core of computation
  while(true) {

    // wait for a buffer ready at input
    cur_buf_out = buf_deframing->read(); // read is blocking

#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
    wait(clk_period,STEP_SIM_UNIT);
#endif
    EXPRINT(trxofdm,0,"begin deframing operation for buffer " << cur_buf_out);

    // take slotid value for computation
    cur_slot_id = deframing_slot_id->read();
    EXPRINT(trxofdm,0,"begin deframing operation with configuration from slot id " << cur_slot_id);
        
    p_cfg = dynamic_cast<trx_ofdm_core_config*>(cfg_core->get_config(cur_slot_id));
    
    if(p_cfg==NULL) {
      EXPRINT(trxofdm, 0,"ERROR : configuration slot " << cur_slot_id << " not found");
      exit(0);
    }
    
    /***************************************/
    // deframing core
    /***************************************/  
    fft_type = p_cfg->fft_type;
    EXPRINT(trxofdm,2,"log2(size_fft)=" << p_cfg->log2_size_fft);
    fft_size = (int)pow((double)2,p_cfg->log2_size_fft) ; // compute the number of points for the fft 2**log2_size_fft
    EXPRINT(trxofdm,1,"deframing begin with size_fft=" << fft_size);

    if (fft_type == ifft) {   // no deframing but possible guard insertion
      // guard interval insertion
      if (p_cfg->gi_insertion) {
	for (int i=0; i<p_cfg->gi_size; i++) {
	  write_fifo_out(0,fft_out_buf[cur_buf_out][fft_size-p_cfg->gi_size+i]); // write is blocking
	  EXPRINT(trxofdm,1,"write data " << fft_out_buf[cur_buf_out][fft_size-p_cfg->gi_size+i] << " to fifo_out 0");
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
	  wait(clk_period, STEP_SIM_UNIT);
#endif
	}
      }
      // OFDM symbol writing
      for (int i=0; i<fft_size; i++) {
	write_fifo_out(0,fft_out_buf[cur_buf_out][i]); // write is blocking
	EXPRINT(trxofdm,1,"write data " << fft_out_buf[cur_buf_out][fft_size-p_cfg->gi_size+i] << " to fifo_out 0");
#ifdef TLM_DVFS_MODELISATION
  lpm->wait_n_cycles(1);
#else
	wait(clk_period, STEP_SIM_UNIT);
#endif
      }      
    }
    else if (fft_type == fft) {
      int data_send = 0;
      int pilot_send = 0;

      ind_mask = 0;
      mask_comp = 0x1;

      for (int i=0; i<fft_size; i++) {
	if (i!=0 && i%32==0) {
	  ind_mask++;
	  mask_comp = 0x1;
	}
	if ((p_cfg->mask_data[ind_mask]&mask_comp) == mask_comp) {  // write data to fifo out 0
	  write_fifo_out(0,fft_out_buf[cur_buf_out][i]); // write is blocking
	  data_send++;
	  EXPRINT(trxofdm,1,"write data " << data_send << " = " << hex << fft_out_buf[cur_buf_out][i] << dec << " to fifo_out 0");
#ifdef TLM_DVFS_MODELISATION
    lpm->wait_n_cycles(1);
#else
	  wait(clk_period,STEP_SIM_UNIT);
#endif
	}
	else { // write pilots to fifo out 1
	  if ((p_cfg->mask_pilot[ind_mask]&mask_comp) == mask_comp) {  // write pilots to fifo out 1
	    write_fifo_out(1,fft_out_buf[cur_buf_out][i]); // write is blocking
	    pilot_send++;
	    EXPRINT(trxofdm,1,"write pilot " << pilot_send << " = " << hex << fft_out_buf[cur_buf_out][i] << " to fifo_out 1");
#ifdef TLM_DVFS_MODELISATION
      lpm->wait_n_cycles(1);
#else
	    wait(clk_period,STEP_SIM_UNIT);
#endif
	  }
	}
	mask_comp = mask_comp << 1; // shift one place
      }
    }
    else {
      EXPRINT(trxofdm, 0,"WARNING : fft_type " << fft_type << " is not allowed");
    }
    /***************************************/
    // end of deframing core
    /***************************************/  
    EXPRINT(trxofdm,0,"end of deframing for buffer " << cur_buf_out << " and configuration slot " << cur_slot_id );

    // unlock the output buffer
    buf_out_free_flag[cur_buf_out] = true;
    buf_out_free.notify();
  }

}


void trx_ofdm_core::write_register(t_uint32 addr, t_uint32 data) {
  // ##########
  // FIXME : add here specific core registers
  //if (addr == xxx) {
  // update register xxx	
  //}
  //else if (addr == yyy) {
  // update register yyy
  //} else {
  // update generic register (configuration...)
    core_tlm::write_register(addr,data);
  //}
  // ### FIXME

}


t_uint32 trx_ofdm_core::read_register(t_uint32 addr) {
  // ##########
  // FIXME
  EXPRINT(trxofdm, 0,"CORE TRX_OFDM FIXME : Dump addr: " << addr); //FIXME
  return addr;
}

// Constructor 
trx_ofdm_core::trx_ofdm_core(sc_module_name module_name_,
              int clk_period_,
	      bool multicore_,
              t_uint16 core_id_,
              t_uint16 nb_fifo_in_,
              t_uint16 *size_fifo_in_,
              t_uint16 nb_fifo_out_,
              t_uint16 *size_fifo_out_,
              t_uint16 nb_cfg_core_,
	      bool static_init_,
              string sno_name_) : core_tlm(module_name_,
					   clk_period_,
					   multicore_,
				           core_id_,
					   nb_fifo_in_,
					   size_fifo_in_,
					   nb_fifo_out_,
					   size_fifo_out_,
					   nb_cfg_core_,
					   TRX_OFDM_CORE_CFG_SIZE,
					   static_init_,
	                                   sno_name_)

#ifdef TLM_POWER_ESTIMATION
 ,tlm_power_core(  utils_extract_subname((std::string) name(), 1) + "." + utils_extract_subname((std::string) name(), 0)  , utils_extract_subname((std::string) name(), 1), &lpm)
#endif // TLM_POWER_ESTIMATION
{

  // threads for multi-buffers
  SC_THREAD(compute_fft);
  SC_THREAD(deframing);
  
  addr_core_cfg_begin = TRX_OFDM_CORE_CFG_BEGIN;
  
  // Fill in the core config table
  trx_ofdm_core_config::init_core_config_table(cfg_core);
  
  if(static_init) {
    
    // initialize configurations to load
    init_config_to_load();
    
    if(nb_cfg_core_to_load!=0) {
      // intialize configurations from catalog file
      trx_ofdm_core_config::init_config_from_file(0,cfg_core, nb_cfg_core_to_load, num_cfg_core, name());

    } else {
      EXPRINT(trxofdm, 0,"No CORE configuration to load");
    }
  } else {
    
    EXPRINT(trxofdm, 0,"Dynamic initialization : no configuration file loaded for CORE");
    
  }


  // buffers init
  fft_in_buf = new t_uint32*[NB_BUF];
  for (int i=0; i<NB_BUF; i++) 
    fft_in_buf[i] =  new t_uint32[MAX_SIZE_FFT];
  for (int i=0; i<NB_BUF; i++) 
    for (int j=0; j<MAX_SIZE_FFT; j++) 
      fft_in_buf[i][j]=0;
  //
  fft_out_buf = new t_uint32*[NB_BUF];
  for (int i=0; i<NB_BUF; i++) 
    fft_out_buf[i] =  new t_uint32[MAX_SIZE_FFT];
  for (int i=0; i<NB_BUF; i++) 
    for (int j=0; j<MAX_SIZE_FFT; j++) 
      fft_out_buf[i][j]=0;

  // fifo init
  buf_fft = new sc_fifo<int>(NB_BUF);
  buf_deframing = new sc_fifo<int>(NB_BUF);
  fft_slot_id = new sc_fifo<int>(NB_SLOT);
  deframing_slot_id = new sc_fifo<int>(NB_SLOT);

  // flags init
  cur_buf_in = 0;
  cur_buf_out = 0;
  cur_buf_fft = 0;
  framing_slot_id = 0;
  nb_buf_in_free = NB_BUF;
  buf_out_free_flag = new bool[NB_BUF];
  for (int i=0; i< NB_BUF; i++)
    buf_out_free_flag[i]=true;

};
