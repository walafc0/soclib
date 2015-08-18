///////////////////////////////////////////////////////////////////////////////////
// File     : fifo_virtual_copro_wrapper.h
// Date     : 15/01/2013
// Author   : Quentin Meunier
// Copyright (c) UPMC-LIP6
///////////////////////////////////////////////////////////////////////////////////

#include "../include/fifo_virtual_copro_wrapper.h"

#include "alloc_elems.h"
#include "soclib_endian.h"

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <list>
#include <vector>


namespace dsx { namespace caba {

#define tmpl(...) __VA_ARGS__ FifoVirtualCoprocessorWrapper

#define MAX_MESSAGES 50

using namespace std;


tmpl(vector<string>)::stringArray(const char * first, ... ) {
    vector<string> ret;
    va_list arg;
    va_start(arg, first);
    const char * s = first;
    while (s) {
        ret.push_back(string(s));
        s = va_arg(arg, const char *);
    }
    va_end(arg);
    return ret;
}


tmpl(vector<int>)::intArray(const int length, ... ) {
    int i;
    vector<int> ret;
    va_list arg;
    va_start(arg, length);

    for (i = 0; i < length; i++) {
        ret.push_back(va_arg(arg, int));
    }
    va_end(arg);
    return ret;
}



tmpl(/**/)::~FifoVirtualCoprocessorWrapper() {
    soclib::common::dealloc_elems(p_to_ctrl, m_n_fifo_to_ctrl);
    soclib::common::dealloc_elems(p_from_ctrl, m_n_fifo_from_ctrl);
    kill_thread();
    while (!m_mwmrs.empty()) {
        delete m_mwmrs.back();
        m_mwmrs.pop_back();
    }
}


tmpl(/**/)::FifoVirtualCoprocessorWrapper(
        sc_core::sc_module_name insname,
        const vector<string> &fifos_out,
        const vector<int>    &fifos_out_width,
        const vector<string> &fifos_in,
        const vector<int>    &fifos_in_width)
: soclib::caba::BaseModule(insname),
    m_n_fifo_to_ctrl(fifos_out.size()),
    m_n_fifo_from_ctrl(fifos_in.size()),
    p_clk("clk"),
    p_resetn("resetn"),
    p_to_ctrl(soclib::common::alloc_elems<soclib::caba::FifoOutput<uint32_t> >("to_ctrl", m_n_fifo_to_ctrl)),
    p_from_ctrl(soclib::common::alloc_elems<soclib::caba::FifoInput<uint32_t> >("from_ctrl", m_n_fifo_from_ctrl)),
    m_names_to_ctrl(fifos_out),
    m_names_from_ctrl(fifos_in),
    thread_created(false)
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();


    for (uint32_t i = 0; i < fifos_in.size(); i++) {
        m_mwmrs.push_back(new srl_mwmr_s(fifos_in_width[i], fifos_in[i].c_str(), i, MWMR_WAY_READ));
    }

    for (uint32_t i = 0; i < fifos_out.size(); i++) {
        m_mwmrs.push_back(new srl_mwmr_s(fifos_out_width[i], fifos_out[i].c_str(), i, MWMR_WAY_WRITE));
    }


    pthread_cond_init(&wrapper_cond, NULL);
    pthread_cond_init(&task_cond, NULL);
    pthread_mutex_init(&messages_lock, NULL);

    cout << "Coprocessor Wrapper " << name() << " created" << endl;
}



tmpl(void)::transition() {
    if (!p_resetn.read()) {
        m_state = VIRT_COPRO_WRAPPER_RUNNING;
        m_cycle = 0;

        kill_thread();
        
        m_buffer_index = 0;
        m_words_left = 0;
        m_cycles_left = 0;
        m_current_mwmr = 0;
        
        srl_log_printf(DEBUG, "Creating thread for copro 0x%x (%s)\n", this, name().c_str());
        pthread_create(&task_thread, NULL, FifoVirtualCoprocessorWrapper::call_task_func, this);
        thread_created = true;

        return;
    }

    m_cycle++;

    switch (m_state) {
        case VIRT_COPRO_WRAPPER_RUNNING:
            assert(m_cycles_left == 0);

            copro_message cmd;
            get_message(&cmd);
            switch (cmd.type) {
                case VIRT_COPRO_WRAPPER_BUSY:
                    m_state = VIRT_COPRO_WRAPPER_WAITING;
                    m_cycles_left = cmd.nb_busy;
                    break;
                case VIRT_COPRO_WRAPPER_READ:
                    m_state = VIRT_COPRO_WRAPPER_READING;
                    m_words_left = cmd.nb_words;
                    m_current_mwmr = cmd.mwmr_id;
                    m_buffer = cmd.buffer;
                    m_buffer_index = 0;
                    break;
                case VIRT_COPRO_WRAPPER_WRITE:
                    srl_log_printf(DEBUG, "CoproWrapper %s: WRITE\n", name().c_str());
                    m_state = VIRT_COPRO_WRAPPER_WRITING;
                    m_words_left = cmd.nb_words;
                    m_current_mwmr = cmd.mwmr_id;
                    m_buffer = cmd.buffer;
                    m_buffer_index = 0;
                    break;
                case VIRT_COPRO_WRAPPER_GET_CYCLE: // ?
                    break;
                case VIRT_COPRO_WRAPPER_END_SIMULATION:
                    sc_core::sc_stop();
                    break;
                case VIRT_COPRO_WRAPPER_SET_STATUS:
                    break;
                case VIRT_COPRO_WRAPPER_READ_CONTROL: // ?
                    break;
                default:
                    assert(0 && "Impossible value read for command");
            }
            break;

        case VIRT_COPRO_WRAPPER_WRITING:
            //srl_log_printf(DEBUG, "CoproWrapper %s: WRITING (m_words_left = %d)\n", name().c_str(), m_words_left);
            if (p_to_ctrl[m_current_mwmr].wok.read()) {
                m_buffer_index++;
                m_words_left--;
                if (m_words_left == 0) {
                    m_state = VIRT_COPRO_WRAPPER_RUNNING;
                    free(m_buffer);
                }
            }
            break;

        case VIRT_COPRO_WRAPPER_READING:
            m_buffer[m_buffer_index] = machine_to_le((uint32_t) p_from_ctrl[m_current_mwmr].data.read());
            if (p_from_ctrl[m_current_mwmr].rok.read()) {
                m_buffer_index++;
                m_words_left--;
                if (m_words_left == 0) {
                    copro_message msg;
                    msg.type = VIRT_COPRO_WRAPPER_RSP_READ;
                    msg.mwmr_id = m_current_mwmr;
                    put_message(msg);
                    m_state = VIRT_COPRO_WRAPPER_RUNNING;
                }
            }
            break;

        case VIRT_COPRO_WRAPPER_WAITING:
            if (m_cycles_left != 0) {
                m_cycles_left--;
            }
            else {
                m_state = VIRT_COPRO_WRAPPER_RUNNING;
            }
            break;
    }
}

tmpl(void)::genMoore() {
    for (size_t i = 0; i < m_n_fifo_from_ctrl; i++) {
        p_from_ctrl[i].r = (m_state == VIRT_COPRO_WRAPPER_READING && m_current_mwmr == i);
    }
    for (size_t i = 0; i < m_n_fifo_to_ctrl; i++) {
        if (m_state == VIRT_COPRO_WRAPPER_WRITING && m_current_mwmr == i) {
            p_to_ctrl[i].data = machine_to_le(m_buffer[m_buffer_index]);
            p_to_ctrl[i].w = true;
        }
        else {
            p_to_ctrl[i].w = false;
        }
    }
}



// Default task code: it should never be executed
tmpl(void *)::task_func() {
    cout << "Dans le task_func par défaut" << endl;
    srl_log_printf(NONE, "Virtual Coprocessor Wrapper %s Error : No function implemented", name().c_str());
    return 0;
}


/*** Cmd/Rsp fifos wrapper methods ***/

tmpl(void)::get_message(copro_message * msg) {
    pthread_mutex_lock(&messages_lock);
    srl_log_printf(DEBUG, "CoproWrapper %s: in get_message\n", name().c_str());
    while (in_messages.elems.empty()) {
        srl_log_printf(DEBUG, "CoproWrapper %s in get_message: fifo in_message is empty : signaling task_cond and waiting for wrapper_cond\n", name().c_str());

        // We signal the task_cond condition in case the task thread is asleep because it has filled the in_messages fifo (too many writes or busy requests) 
        pthread_cond_signal(&task_cond);
        pthread_cond_wait(&wrapper_cond, &messages_lock);

        srl_log_printf(DEBUG, "CoproWrapper %s in get_message: woken up on wrapper_cond\n", name().c_str());
    }
    *msg = in_messages.elems.front();
    in_messages.elems.pop_front();
    pthread_mutex_unlock(&messages_lock);
}

tmpl(void)::put_message(copro_message msg) {
    srl_log_printf(DEBUG, "CoproWrapper %s in put_message: we put a message and signal task_cond\n", name().c_str());
    pthread_mutex_lock(&messages_lock);
    out_messages.elems.push_back(msg);
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&messages_lock);
}


tmpl(void)::kill_thread() {
    if (thread_created) {
        srl_log_printf(DEBUG, "Canceling existing thread for copro %s\n", name().c_str());
        pthread_cancel(task_thread); // we kill the previous thread before creating a new one
        pthread_join(task_thread, NULL);
        copro_message msg;
        // Emptying the fifos, and deallocating when necessary
        while (!in_messages.elems.empty()) {
            msg = in_messages.elems.front();
            in_messages.elems.pop_front();
            if (msg.type == VIRT_COPRO_WRAPPER_WRITE) {
                free(msg.buffer);
            }
        }
        out_messages.elems.clear();

        pthread_mutex_init(&messages_lock, NULL);
    }
}


/**************** MWMR functions *********************/

tmpl(srl_mwmr_t)::srl_get_mwmr(const char * name) {
    string s_name(name);
    for (uint32_t i = 0; i < m_mwmrs.size(); i++) {
        if (!s_name.compare(m_mwmrs[i]->name)) {
            return m_mwmrs[i];
        }
    }
    srl_log_printf(NONE, "Error: mwmr not found: %s\n", name);
    assert(0);
}


tmpl(void)::srl_mwmr_read(srl_mwmr_t fifo, void * mem, size_t len) {
    // len = number of items
    srl_assert(fifo->way == MWMR_WAY_READ);

    srl_log_printf(DEBUG, "Task %s: in srl_mwmr_read (%s, %d words, buffer = %x)\n", name().c_str(), fifo->name, len * fifo->width, mem);

    copro_message msg;
    msg.type = VIRT_COPRO_WRAPPER_READ;
    msg.nb_words = len * fifo->width;
    msg.mwmr_id = fifo->id;
    msg.buffer = (uint32_t *) mem;

    pthread_mutex_lock(&messages_lock);
    in_messages.elems.push_back(msg);
    pthread_cond_signal(&wrapper_cond);

    while (out_messages.elems.empty()) {
        srl_log_printf(DEBUG, "Task %s in srl_mwmr_read: out_message is empty, waiting on task_cond for the response\n", name().c_str());

        pthread_cond_wait(&task_cond, &messages_lock);

        srl_log_printf(DEBUG, "Task %s in srl_mwmr_read: woken up on task_cond\n", name().c_str());
    }
    msg = out_messages.elems.front();
    out_messages.elems.pop_front();
    srl_assert(msg.type == VIRT_COPRO_WRAPPER_RSP_READ);
    srl_assert(msg.mwmr_id == fifo->id);

    pthread_mutex_unlock(&messages_lock);

}


tmpl(void)::srl_mwmr_write(srl_mwmr_t fifo, void * mem, size_t len) {
    bool fifo_full = false;
    uint32_t nb_bytes;
    srl_assert(fifo->way == MWMR_WAY_WRITE);

    srl_log_printf(DEBUG, "Task %s: in srl_mwmr_write (%s, %d words)\n", name().c_str(), fifo->name, len * fifo->width);

    copro_message msg;
    msg.type = VIRT_COPRO_WRAPPER_WRITE;
    msg.nb_words = len * fifo->width;
    msg.mwmr_id = fifo->id;
    nb_bytes = len * fifo->width * sizeof(uint32_t);
    msg.buffer = (uint32_t *) malloc(nb_bytes);
    memcpy(msg.buffer, mem, nb_bytes);

    pthread_mutex_lock(&messages_lock);
    in_messages.elems.push_back(msg);
    if (in_messages.elems.size() > MAX_MESSAGES) {
        fifo_full = true;
    }

    srl_log_printf(DEBUG, "Task %s in srl_mwmr_write: we signal wrapper_cond\n", name().c_str());
    pthread_cond_signal(&wrapper_cond);

    // The cond_wait on task_cond must be done after having done the cond_signal on wrapper_cond
    if (fifo_full) {
        srl_log_printf(DEBUG, "Task %s in srl_mwmr_write: the fifo in_message is full, we wait on task_cond\n", name().c_str());
        pthread_cond_wait(&task_cond, &messages_lock);
        srl_log_printf(DEBUG, "Task %s in srl_mwmr_write: woken up on task_cond\n", name().c_str());
    }

    pthread_mutex_unlock(&messages_lock);
}


tmpl(ssize_t)::srl_mwmr_try_read(srl_mwmr_t fifo, void * mem, size_t len) {
    srl_assert(0 && "srl_mwmr_try_read is not implemented");
}


tmpl(ssize_t)::srl_mwmr_try_write(srl_mwmr_t fifo, void * mem, size_t len) {
    srl_assert(0 && "srl_mwmr_try_write is not implemented");
}



/***** hardware helpers ******/
tmpl(size_t)::srl_cycle_count() {
    // Possibly do a cond_wait() here so as to have a synchronisation point, otherwise it may not take into account a preivous call to srl_busy_cycle.
    return m_cycle;
}


tmpl(void)::srl_busy_cycles(uint32_t cycles) {
    bool fifo_full = false;
    srl_log_printf(DEBUG, "Task %s in srl_busy_cycles (%d cycles)\n", name().c_str(), cycles);
    copro_message msg;
    msg.type = VIRT_COPRO_WRAPPER_BUSY;
    msg.nb_busy = cycles;
    msg.buffer = NULL;

    pthread_mutex_lock(&messages_lock);
    in_messages.elems.push_back(msg);
    if (in_messages.elems.size() > MAX_MESSAGES) {
        srl_log_printf(DEBUG, "Task %s in srl_busy_cycles: the fifo in_message is full, we wait on task_cond\n", name().c_str());
        fifo_full = true;
    }
    pthread_cond_signal(&wrapper_cond);

    if (fifo_full) {
        pthread_cond_wait(&task_cond, &messages_lock);
        srl_log_printf(DEBUG, "Task %s in srl_busy_cycles: woken up on task_cond\n", name().c_str());
    }

    pthread_mutex_unlock(&messages_lock);
}


tmpl(void)::srl_abort() {
    sc_core::sc_stop();
}

}}


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

