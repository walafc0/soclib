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

#include "fht_core.h"

/*------------------------------------------------------------------------------
 * CLASS: fht_core
 *----------------------------------------------------------------------------*/

// Main compute
void fht_core::compute() {

   int  i,j ;  // controles de boucles
  t_uint32 symb_in ;  // flit d entree venant de la FIFO
  t_uint32 symb_out ;// flit de sortie en cours d elaboration
  //fht_unit_config* p_cfg ;
  int sat ;
  int sat_cnt = 0 ; ;
  int fht_cnt = 0 ; ;
  sc_int<20> in_I [32] ;
  sc_int<20> in_Q [32] ;
  sc_int<20> out_I ;
  sc_int<20> out_Q ;
  bool sat_it = 0 ;
  //bool run = true;
  
  //t_uint32 data;

  fht_core_config *p_cfg;

  while(true) {
    
    #ifdef TLM_POWER_ESTIMATION
    //new_power_phase(Power::tlm_power_core::WAIT, lpm->get_frequency());
    #endif
    wait(load_cfg_event) ;

    #ifndef TLM_DVFS_MODELISATION
     wait(clk_period,STEP_SIM_UNIT);
    #else
    lpm->wait_n_cycles(1);
    #endif 
      
    EXPRINT(core,  0, "Start new computation"); // FIXPRINT
    
  
    p_cfg = dynamic_cast<fht_core_config*>(cfg_core->get_config(current_slot_id));

    if (p_cfg==NULL) {
      EXPRINT(core, 0, "ERROR: configuration slot " << current_slot_id << " not found!");
      exit(0);
    }

    if(p_cfg->sat_rst_at_load) {
      sat_cnt =0 ;
      fht_cnt =0 ;
      sat_it = 0 ;
    }

    p_cfg->print_config();
    
    // TRACE
    PRINT(1,"-------------------------TRACE OF 1 DATA BLOCK");
    PRINT(1,"p_cfg->nb_fht = " << p_cfg->nb_fht);
    PRINT(1,"p_cfg->fht_sz = " << p_cfg->fht_sz);
    PRINT(1,"p_cfg->nb_shift = " << p_cfg->nb_shift);

    for(int fht_idx=0; fht_idx<p_cfg->nb_fht; fht_idx++){ // traitement d une FHT
      PRINT(1, "fht_idx = " << fht_idx);

      for(int point_idx=0;point_idx<p_cfg->fht_sz;point_idx++){ // acquisition des complexes
        if(p_cfg->mask_user0[point_idx]){
//          anoc_ni<fht_unit_config>::read_fifo(0,symb_in); // read is blocking
          symb_in = read_fifo_in(0);
          EXPRINT(core, 1, "FHT read the data " << point_idx << " : " << hex << symb_in << dec << " from fifo 0");
        } else if(p_cfg->mask_user1[point_idx]){
//          anoc_ni<fht_unit_config>::read_fifo(1,symb_in); // read is blocking
          symb_in = read_fifo_in(1);
          EXPRINT(core, 1, "FHT read the data " << point_idx << " : " << hex << symb_in << dec << " from fifo 1");
        } else {
          symb_in = 0 ;
          EXPRINT(core, 1, "FHT read the null data " << point_idx << " : " << hex << symb_in << dec );
        }
        sc_int<32> data_in(symb_in);
        in_I[point_idx]= (sc_int<20>) data_in.range(31,16);
        in_Q[point_idx]= (sc_int<20>) data_in.range(15,0);
      }

      #ifdef TLM_POWER_ESTIMATION
      new_power_phase(Power::tlm_power_core::COMPUTE);
      #endif
            
      #ifndef TLM_DVFS_MODELISATION
      //about 3*fht_size cycles
      //std::cout << "wait for 2.5* " << p_cfg->fht_sz << " cycles , T = " << clk_period << std::endl;
      wait( 2.5*p_cfg->fht_sz*clk_period, STEP_SIM_UNIT);
      #else
      //std::cout << "wait for 2.5* " << p_cfg->fht_sz << " cycles , T = " << 1/lpm->get_frequency() << std::endl;
      lpm->wait_n_cycles(static_cast<int>(2.5*p_cfg->fht_sz));
      #endif 

      #ifdef TLM_POWER_ESTIMATION
      new_power_phase(Power::tlm_power_core::WAIT);
      #endif

      for(i=0;i<p_cfg->fht_sz;i++){ // balayage des sorties
        out_I = 0 ;
        out_Q = 0 ;
        for(j=0;j<p_cfg->fht_sz;j++){ // balayage des entrees
          out_I += in_I[j] * C[i][j] ;
          out_Q += in_Q[j] * C[i][j] ;
        } // for j
        out_I = out_I >> p_cfg->nb_shift;
        out_Q = out_Q >> p_cfg->nb_shift;
        // detection de saturation sur I
        sat = 0 ;
        for(j=16;j<20;j++) if(out_I[j]!=out_I[15])sat=1;
        sat_cnt += sat ;
        // detection de saturation sur Q
        sat = 0 ;
        for(j=16;j<20;j++) if(out_Q[j]!=out_Q[15])sat=1;
        sat_cnt += sat ;

        // sortie du resultat
        symb_out = (out_I.range(15,0),out_Q.range(15,0)) ;
        EXPRINT(core, 1," WRITE FIFO with symb_out = " << hex << symb_out << dec);
//        anoc_ni<fht_unit_config>::write_fifo(0,symb_out);  // write is blocking 
        write_fifo_out(0, symb_out);
      } // for i

      fht_cnt++ ;
      if((sat_it==0)&&(sat_cnt>0)){
        sat_it = 1 ;
//         sc_uint<32> data_tmp(0);
//         data_tmp = 2 ; // code IT core
//         data_tmp = data_tmp<<5;
//         data_tmp = data_tmp + rb_config_IT->get_srcid_occ();
//         res_fifo_IT->write((t_uint32)data_tmp.to_uint());
        //write_status(2);    
        write_status(0); //go on anyway
      } // envoi d'une IT
    } // for FHT idx 

  
//     // read data in fifo_in 0
//     data=read_fifo_in(0);
// 
//     // write data in fifo_out 0
//     write_fifo_out(0,data);

    EXPRINT(core, 0,"End of computation");
    
    // write status 0
    write_status(p_cfg->end_status);
    write_eoc();
    
  }

};

void fht_core::write_register(t_uint32 addr, t_uint32 data) {

//  if (addr == 1) {
//    read_cycles = data;
//    EXPRINT(core, 1, "read_cycles = " << read_cycles);
//  }
//  else if (addr == 2) {
//    write_cycles = data;
//    EXPRINT(core, 1, "write_cycles = " << write_cycles);   
//  }
//  else if (addr == 3) {
//    compute_cycles = data;
//    EXPRINT(core, 1, "compute_cycles = " << compute_cycles);   
//  } else {
//    core_tlm::write_register(addr,data);
//  }
    core_tlm::write_register(addr,data);
}


t_uint32 fht_core::read_register(t_uint32 addr) {
  EXPRINT(core, 0,"CORE FHT FIXME : Dump addr: " << addr); //FIXME
  return addr;
}


// Constructor 
fht_core::fht_core(sc_module_name module_name_,
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
             FHT_CORE_CFG_SIZE,
             static_init_,
                                     sno_name_)
            #ifdef TLM_POWER_ESTIMATION
 ,tlm_power_core(  utils_extract_subname((std::string) name(), 1) + "." + utils_extract_subname((std::string) name(), 0)  , utils_extract_subname((std::string) name(), 1), &lpm)
            #endif // TLM_POWER_ESTIMATION     


{

  write_cycles = 0;
  read_cycles = 0;
  compute_cycles = 0;

  // construction de la matrice
  C[0][0] = 1 ;
  C[0][1] = 1 ;
  C[1][0] = 1 ;
  C[1][1] = -1 ;
  for(int sz=4;sz<=32;sz*=2){ // Kronecker Product
    for(int j=0;j<sz/2;j++){
      for(int k=0;k<sz/2;k++){
        C[j][k+sz/2] = C[j][k] ;
        C[j+sz/2][k] = C[j][k] ;
        C[j+sz/2][k+sz/2] = - C[j][k] ;
      } // for k
    } // for j
  } // for sz

  addr_core_cfg_begin = FHT_CORE_CFG_BEGIN;
  
  // Fill in the core config table
  fht_core_config::init_core_config_table(cfg_core);
  
  if(static_init) {
    
    // initialize configurations to load
    init_config_to_load();
    
    if(nb_cfg_core_to_load!=0) {
      // intialize configurations from catalog file
      fht_core_config::init_config_from_file(0,cfg_core, nb_cfg_core_to_load, num_cfg_core, name());

    } else {
      EXPRINT(core, 0,"No CORE configuration to load");
    }
  } else {
    
    EXPRINT(core, 0,"Dynamic initialization : no configuration file loaded for CORE");
    
  }

};





