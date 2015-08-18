///////////////////////////////////////////////////////////////////////////////////
// File     : fifo_virtual_copro_wrapper.cpp
// Date     : 15/01/2013
// Author   : Quentin Meunier
// Copyright (c) UPMC-LIP6
///////////////////////////////////////////////////////////////////////////////////

#ifndef FIFO_TASK_H_
#define FIFO_TASK_H_

#include <systemc>
#include <stdarg.h>
#include <vector>
#include <list>

#include "caba_base_module.h"

#include "fifo_ports.h"

#include "srl_log.h"
#include "srl_hw_helpers.h"
#include "srl_private_types.h"

namespace dsx { namespace caba {

typedef enum {
    VIRT_COPRO_WRAPPER_BUSY,
    VIRT_COPRO_WRAPPER_READ,
    VIRT_COPRO_WRAPPER_RSP_READ,
    VIRT_COPRO_WRAPPER_WRITE,
    VIRT_COPRO_WRAPPER_GET_CYCLE,
    VIRT_COPRO_WRAPPER_END_SIMULATION,
    VIRT_COPRO_WRAPPER_SET_STATUS,
    VIRT_COPRO_WRAPPER_READ_CONTROL,
} virtual_copro_wrapper_cmd_type;

typedef struct _copro_message {
    virtual_copro_wrapper_cmd_type type;
    uint32_t nb_words;
    uint32_t nb_busy;
    uint32_t mwmr_id;
    uint32_t * buffer;
} copro_message;

typedef struct _messages {
    std::list<copro_message> elems;
} messages;



class FifoVirtualCoprocessorWrapper
    : public soclib::caba::BaseModule
{

    private:
    const size_t m_n_fifo_to_ctrl;
    const size_t m_n_fifo_from_ctrl;

    public:
    sc_core::sc_in<bool> p_clk;
    sc_core::sc_in<bool> p_resetn;

    soclib::caba::FifoOutput<uint32_t> * p_to_ctrl;
    soclib::caba::FifoInput<uint32_t> * p_from_ctrl;


    private:
    size_t m_cycle;
    size_t m_cycles_left;
    size_t m_words_left;
    uint32_t m_current_mwmr;

    std::vector<srl_mwmr_t> m_mwmrs;

    const std::vector<std::string> m_names_to_ctrl;
    const std::vector<std::string> m_names_from_ctrl;

    enum {
        VIRT_COPRO_WRAPPER_RUNNING,
        VIRT_COPRO_WRAPPER_READING,
        VIRT_COPRO_WRAPPER_WRITING,
        VIRT_COPRO_WRAPPER_WAITING,
    } m_state;


    uint32_t * m_buffer;
    size_t m_buffer_index;

    messages in_messages;
    messages out_messages;

    pthread_t task_thread;
    bool thread_created;

    pthread_cond_t wrapper_cond;
    pthread_cond_t task_cond;

    pthread_mutex_t messages_lock;

    protected:
    SC_HAS_PROCESS(FifoVirtualCoprocessorWrapper);

    public:
    ~FifoVirtualCoprocessorWrapper();

    FifoVirtualCoprocessorWrapper(sc_core::sc_module_name insname,
            const std::vector<std::string> &fifos_out,
            const std::vector<int> &fifos_out_width,
            const std::vector<std::string> &fifos_in,
            const std::vector<int> &fifos_in_width
            );

    void transition();
    void genMoore();


    static std::vector<int> intArray(const int length, ... );
    static std::vector<std::string> stringArray(const char * first, ... );

    // Task function
    virtual void * task_func();


    /**** MWMR ****/
    #define SRL_GET_MWMR(name) srl_get_mwmr(#name)
    srl_mwmr_t srl_get_mwmr(const char * name);

    ssize_t srl_mwmr_try_write(srl_mwmr_t fifo, void * mem, size_t len);
    ssize_t srl_mwmr_try_read(srl_mwmr_t fifo, void * mem, size_t len);
    void srl_mwmr_write(srl_mwmr_t fifo, void * mem, size_t len);
    void srl_mwmr_read(srl_mwmr_t fifo, void * mem, size_t len);


    /*** hw_helpers ***/
    size_t srl_cycle_count();
    void srl_busy_cycles(uint32_t);
    void srl_abort();


    // Let this method public?
    static void * call_task_func(void * arg) {
        return ((FifoVirtualCoprocessorWrapper *) arg)->task_func();
    }


    private:
    void get_message(copro_message * msg);
    void put_message(copro_message msg);
    void kill_thread();
    
};

}}

#endif /* FIFO_TASK_H_ */

/*
# Local Variables:
# tab-width: 4;
# c-basic-offset: 4;
# c-file-offsets:((innamespace . 0)(inline-open . 0));
# indent-tabs-mode: nil;
# End:
#
# vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
*/

