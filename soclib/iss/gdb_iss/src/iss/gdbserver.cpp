/*
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
 * Copyright (c) UPMC, Lip6, SoC
 *         Alexandre Becoulet <alexandre.becoulet@lip6.fr>, 2007
 *
 * Maintainers: becoulet nipo
 */

#include <cstring>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#include <stdint.h>

#include "gdbserver.h"
#include "exception.h"

#include "soclib_endian.h"

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

namespace soclib { namespace common {

template<typename CpuIss> int GdbServer<CpuIss>::socket_ = -1;
template<typename CpuIss> int GdbServer<CpuIss>::asocket_ = -1;
template<typename CpuIss> int GdbServer<CpuIss>::poll_timeout_ = 100;
template<typename CpuIss> typename GdbServer<CpuIss>::State GdbServer<CpuIss>::init_state_ = Running;
template<typename CpuIss> std::vector<GdbServer<CpuIss> *> GdbServer<CpuIss>::list_;
template<typename CpuIss> unsigned int GdbServer<CpuIss>::current_id_ = 0;
template<typename CpuIss> unsigned int GdbServer<CpuIss>::step_id_ = 0;
template<typename CpuIss> bool GdbServer<CpuIss>::ctrl_c_ = false;
template<typename CpuIss> bool GdbServer<CpuIss>::debug_ = false;
template<typename CpuIss> std::map<uint32_t, bool> GdbServer<CpuIss>::break_exec_;
template<typename CpuIss> typename GdbServer<CpuIss>::address_set_t GdbServer<CpuIss>::break_read_access_;
template<typename CpuIss> typename GdbServer<CpuIss>::address_set_t GdbServer<CpuIss>::break_write_access_;
template<typename CpuIss> Loader* GdbServer<CpuIss>::loader_ = 0;

template<typename CpuIss> uint16_t GdbServer<CpuIss>::port_ = 2346;

template<typename CpuIss>
inline uint32_t GdbServer<CpuIss>::debug_reg_swap(uint32_t val)
{
    if ( CpuIss::s_endianness == CpuIss::ISS_LITTLE_ENDIAN )
        return soclib::endian::uint32_swap(val);
    return val;
}

template<typename CpuIss>
void GdbServer<CpuIss>::global_init()
{
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_ < 0)
        throw soclib::exception::RunTimeError("GdbServer: Unable to create socket");

    {
        int tmp = 1;
        setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
    }

    struct sockaddr_in    addr;

    for (int i = 0; ; i++)
        {
            if (i == 10)
                throw soclib::exception::RunTimeError("GdbServer: Unable to bind()");

            memset(&addr, 0, sizeof(addr));
            addr.sin_port = htons(port_ + i);
            addr.sin_family = AF_INET;

            if (bind(socket_, (struct sockaddr*)&addr, sizeof (struct sockaddr_in)) >= 0)
                {
                    fprintf(stderr, "[GDB] listening on port %i\n", port_ + i);
                    break;
                }
        }

    if (listen(socket_, 1) < 0)
        throw soclib::exception::RunTimeError("GdbServer: Unable to listen()");
}

template<typename CpuIss>
GdbServer<CpuIss>::GdbServer(const std::string &name, uint32_t ident)
    : CpuIss(name, ident),
      mem_req_(false),
      mem_count_(0),
      catch_exceptions_(true), // Do not change without prior discussion
      call_trace_(false),
      call_trace_zero_(false),
      wait_on_except_(false),
      exit_on_trap_(false),
      exit_on_fault_(false),
      wait_on_wpoint_(true),
      cur_addr_(0),
      cycles_(0),
      cycles_bp_(0),
      cpu_id_(ident)
    {
        init_state();
        if (list_.empty())
            global_init();

        id_ = list_.size();
        list_.push_back(this);
    }

template<typename CpuIss>
int GdbServer<CpuIss>::write_packet(const char *data_)
{
    unsigned int i, len = strlen(data_);
    char ack, end[4];
    uint8_t chksum = 0;
    char data[len];

    memcpy(data, data_, len + 1);

    // gdb RLE data compression

    unsigned char repeat = 0;
    unsigned int cmplen = len;
    char *cmp = data;
    char *last = 0;

#if 1

    for (i = 0; ; )
        {
            if (i < cmplen && last && (*last == cmp[i]) && (repeat + 29 < 126))
                repeat++;
            else
                {
                    if (repeat > 3)
                        {
                            while (repeat == '#' - 29 ||
                                   repeat == '$' - 29 ||
                                   repeat == '+' - 29 ||
                                   repeat == '-' - 29)
                                {
                                    repeat--;
                                    last++;
                                }
                            last[1] = '*';
                            last[2] = 29 + repeat;
                            memmove(last + 3, cmp + i, cmplen - i + 1);
                            cmp = last + 3;
                            cmplen -= i;
                            i = 0;
                            last = 0;
                            repeat = 0;
                            continue;
                        }
                    else
                        {
                            last = cmp + i;
                            repeat = 0;
                        }

                    if (i == cmplen)
                        break;
                }
            i++;
        }

    cmp[cmplen] = 0;
    len = cmp - data + cmplen;

#endif

    for (i = 0; i < len; i++)
        chksum += data[i];

    sprintf(end, "#%02x", chksum);

    do
        {
            if (debug_)
                fprintf(stderr, "[GDB] sending packet data '%s'\n", data);

            send(asocket_, "$", 1, MSG_DONTWAIT | MSG_NOSIGNAL);
            send(asocket_, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            send(asocket_, end, 3, MSG_DONTWAIT | MSG_NOSIGNAL);

            if (read(asocket_, &ack, 1) < 1)
                {
                    close(asocket_);
                    asocket_ = -1;    
                    return -1;
                }
        }
    while (ack != '+');

    return 0;
}

template<typename CpuIss>
char * GdbServer<CpuIss>::read_packet(char *buffer, size_t size)
{
    int res = read(asocket_, buffer, size);

    if (res <= 0)
        {
            close(asocket_);
            asocket_ = -1;    
            return 0;
        }

    uint8_t sum = 0, chksum = 0;
    char *data = 0;
    char *end = 0;
    int i;

    // find data in packet
    for (i = 0; i < res; i++)
        {
            switch (buffer[i])
                {
                case '$':
                    sum = 0;
                    data = buffer + i + 1;
                    break;

                case '#':
                    chksum = sum;
                    end = buffer + i;
                    *end = 0;
                    goto end;

                default:
                    sum += buffer[i];
                }
        }
 end:

    // malformed packet
    if (!end || data >= end)
        {
            if (debug_)
                fprintf(stderr, "[GDB] malformed packet %i bytes\n", res);

            return 0;
        }

    if (debug_)
        fprintf(stderr, "[GDB] packet with checksum %02x: %s\n", chksum, data);

    // verify checksum
    end[3] = 0;
    if (chksum != strtoul(end + 1, 0, 16))
        {
            send(asocket_, "-", 1, MSG_DONTWAIT | MSG_NOSIGNAL);

            if (debug_)
                fprintf(stderr, "[GDB] bad packet checksum\n");

            return 0;
        }

    send(asocket_, "+", 1, MSG_DONTWAIT | MSG_NOSIGNAL);

    return data;
}

template<typename CpuIss>
void GdbServer<CpuIss>::process_monitor_packet(char *data)
{
    const char *delim = " \t,";
    char *tokens[255], *save;
    unsigned int i = 0;

    for (char *t = strtok_r(data, delim, &save);
         t != NULL; t = strtok_r(NULL, delim, &save))
        {
            tokens[i++] = t;
            if (i == 255)
                break;
        }

    if (i >= 2 && !strcmp(tokens[0], "stepcpu"))
        {
            unsigned int id = atoi(tokens[1]);

            if (id > 0 && id <= list_.size())
                {
                    if (debug_)
                        fprintf(stderr, "[GDB] single step forced on cpu %u\n", id - 1);

                    step_id_ = id;
                    write_packet("OK");
                    return;
                }
        }

    if (i >= 2 && !strcmp(tokens[0], "sleepms"))
        {
            int poll_timeout_ = atoi(tokens[1]);

            if (debug_)
                fprintf(stderr, "[GDB] poll timeout changed to %u\n", poll_timeout_);

            write_packet("OK");
            return;
        }

    if (i >= 2 && !strcmp(tokens[0], "except"))
        {
            bool value = atoi(tokens[1]) != 0;

            if (i == 2)
                {
                    for (unsigned int i = 0; i < list_.size(); i++)
                        list_[i]->catch_exceptions_ = value;
                    write_packet("OK");
                    return;
                }
            else
                {
                    unsigned int id = atoi(tokens[2]) - 1;

                    if (id < list_.size())
                        {
                            list_[id]->catch_exceptions_ = value;
                            write_packet("OK");
                            return;
                        }
                }
        }

    if (i >= 2 && !strcmp(tokens[0], "load"))
    {
        if (! loader_)
        {
            std::cerr << "[GDB] no loader defined !!!" << std::cerr;
        }
        else
        {
            const char *file = tokens[1];
            try {
                loader_->load_file(file);
            } catch (const soclib::exception::Exception &e ) {
                std::cerr << "[GDB] " << e.what() << std::endl;
                write_packet("");
                return;
            }
            write_packet("OK");
            return;
        }
    }

    if (i >= 2 && !strcmp(tokens[0], "calltrace"))
        {
            if (! loader_)
                {
                    std::cerr << "[GDB] no loader defined !!!" << std::cerr;
                }
            else
                {
                    bool value = atoi(tokens[1]) != 0;

                    if (i == 2)
                        {
                            for (unsigned int i = 0; i < list_.size(); i++)
                                list_[i]->call_trace_ = value;
                            write_packet("OK");
                            return;
                        }
                    else
                        {
                            unsigned int id = atoi(tokens[2]) - 1;
                            
                            if (id < list_.size())
                                {
                                    list_[id]->call_trace_ = value;
                                    write_packet("OK");
                                    return;
                                }
                        }
                }
        }

    if (i >= 2 && !strcmp(tokens[0], "cachedebug"))
        {
            unsigned int debug_mask = strtoul(tokens[1], NULL, 0);
            /* prepare XTN request */
            mem_req_ = true;
            mem_type_ = CpuIss::XTN_WRITE;
            mem_addr_ = CpuIss::XTN_DEBUG_MASK * 4;
            mem_len_ = 4;
            mem_data_ = debug_mask;
            state_ = WaitGdbMem;

            return;
        }

    if (i >= 2 && !strcmp(tokens[0], "issdebug"))
        {
            unsigned int debug_mask = strtoul(tokens[1], NULL, 0);
            CpuIss::set_debug_mask(debug_mask);
            write_packet("OK");
            return;
        }

    if (i >= 2 && !strcmp(tokens[0], "debug"))
        {
            debug_ = atoi(tokens[1]) != 0;
            write_packet("OK");
            return;
        }

    if (i >= 1 && !strcmp(tokens[0], "dump"))
        {
            CpuIss::dump();
            write_packet("OK");
            return;
        }

    if (i >= 3 && !strcmp(tokens[0], "watch"))
        {
            const char *flags = tokens[1];
            uint32_t addr = strtoul(tokens[2], 0, 0);
            size_t size = i >= 4 ? strtoul(tokens[3], 0, 0) : 0;

            // default size is cpu register width
            size = size ? size : CpuIss::debugGetRegisterSize(0) / 8;
            address_set_t ival(addr, addr + size - 1);

            if (strchr(flags, '-'))
                {
                    if (strchr(flags, 'r'))
                        break_read_access_ &= ~ival;
                    if (strchr(flags, 'w'))
                        break_write_access_ &= ~ival;
                }
            else
                {
                    if (strchr(flags, 'r'))
                        break_read_access_ |= ival;
                    if (strchr(flags, 'w'))
                        break_write_access_ |= ival;
                }

            write_packet("OK");
            return;
        }

    write_packet("");
}

template<typename CpuIss>
void GdbServer<CpuIss>::process_gdb_packet()
{
    char buffer[1024];

    char *data = read_packet(buffer, 1024);

    if (data)
        {
            switch (data[0])
                {
                case 'k':       // Kill
                    write_packet("OK");
                    cleanup();
                    sc_stop();
                    break;

                case 'D': // Detach
                    write_packet("OK");
                    cleanup();
                    return;

                case 'q': // Query
                    switch (data[1])
                        {
                        case 'T': {
                            if (strncmp(data + 2, "hreadExtraInfo", 14))
                                break;

                            assert(data[16] == ',');

                            unsigned int id = strtoul(data + 17, 0, 16);

                            assert(id > 0 && id <= list_.size());

                            std::string name = std::string("Processor ") + list_[id - 1]->name();
                            
                            if (step_id_ == id)
                                name += " [STEPPING]";

                            char *b = buffer;
                            for (unsigned int i = 0; i < name.size(); i++)
                                b += sprintf(b, "%02x", (int)name[i]);

                            write_packet(buffer);
                            return;
                        }

                        case 'C': // get current thread id
                            sprintf(buffer, "QC%x", current_id_ + 1);
                            write_packet(buffer);
                            return;
                        case 'f': // get thread list first
                            if (!strcmp(data + 2, "ThreadInfo"))
                                {
                                    char *b = buffer;
                                    *b++ = 'm';
                                    for (unsigned int i = 0; i < list_.size(); i++)
                                        b += sprintf(b, "%x%s", i + 1, i == list_.size() - 1 ? "" : ",");
                                    write_packet(buffer);
                                    return;
                                }
                            break;

                        case 's': // get thread list next
                            if (!strcmp(data + 2, "ThreadInfo"))
                                {
                                    write_packet("l"); // end of list
                                    return;
                                }
                            break;

                        case 'R': {
                            unsigned int i;
                            char byte[3] = { 0 };

                            if (strncmp(data + 2, "cmd", 3))
                                break;

                            assert(data[5] == ',');

                            data += 6;

                            for (i = 0; data[i * 2]; i++)
                                {
                                    memcpy(byte, data + i * 2, 2);
                                    data[i] = strtoul(byte, 0, 16);
                                }

                            data[i] = 0;

                            if (debug_)
                                fprintf(stderr, "[GDB] monitor packet: '%s'\n", data);

                            process_monitor_packet(data);
                            return;
                        }

                        }
                    break;

                case '?':       // Indicate the reason the target halted
                    write_packet("S05"); // SIGTRAP
                    return;

                case 'p': {       // read single register
                    unsigned int reg = strtoul(data + 1, 0, 16);
                    char fmt[32];

                    sprintf(fmt, "%%0%ux", (unsigned int)CpuIss::debugGetRegisterSize(reg) / 4);
                    sprintf(buffer, fmt, debug_reg_swap(CpuIss::debugGetRegisterValue(reg)));
                    write_packet(buffer);
                    return;
                }

                case 'P': {     // write single register
                    char *end;
                    unsigned int reg = strtoul(data + 1, &end, 16);
                    assert(*end == '=');
                    uint32_t value = strtoul(end + 1, 0, 16);

                    CpuIss::debugSetRegisterValue(reg, debug_reg_swap(value));
                    write_packet("OK");
                    return;
                }

                case 'g': {      // read all registers
                    char *b = buffer;
                    for (unsigned int i = 0; i < CpuIss::debugGetRegisterCount(); i++)
                        {
                            char fmt[32];
                            sprintf(fmt, "%%0%ux", (unsigned int)CpuIss::debugGetRegisterSize(i) / 4);
                            b += sprintf(b, fmt, debug_reg_swap(
                                             CpuIss::debugGetRegisterValue(i)));
                        }
                    write_packet(buffer);
                    return;
                }

                case 'G': {       // write all registers

                    data++;

                    for (unsigned int i = 0; i < CpuIss::debugGetRegisterCount(); i++)
                        {
                            size_t s = CpuIss::debugGetRegisterSize(i) / 4;
                            char word[s + 1];
                            word[s] = 0;
                            memcpy(word, data, s);
                            if (strlen(word) != s)
                                break;
                            CpuIss::debugSetRegisterValue(i, debug_reg_swap(
                                                              strtoul(word, 0, 16)));
                            data += s;
                        }

                    write_packet("OK");

                    return;
                }

                case 'm': {     // read memory
                    char *end;
                    uint32_t addr = strtoul(data + 1, &end, 16);
                    assert(*end == ',');
                    size_t len = strtoul(end + 1, 0, 16);

                    mem_req_ = true;
                    mem_type_ = CpuIss::DATA_READ;
                    mem_addr_ = addr;
                    mem_len_ = mem_count_ = len;
                    mem_buff_ = mem_ptr_ = (uint8_t*)malloc(len);
                    state_ = WaitGdbMem;

                    return;
                }

                case 'M': {      // write memory
                    char *end;
                    uint32_t addr = strtoul(data + 1, &end, 16);
                    assert(*end == ',');
                    size_t len = strtoul(end + 1, &end, 16);
                    assert(*end == ':');

                    mem_req_ = true;
                    mem_type_ = CpuIss::DATA_WRITE;
                    mem_addr_ = addr;
                    mem_len_ = mem_count_ = len;
                    mem_buff_ = mem_ptr_ = (uint8_t*)malloc(len);

                    char byte[3] = { 0 };

                    for (unsigned int i = 0; i < len; i++)
                        {
                            memcpy(byte, end + 1 + i * 2, 2);
                            mem_buff_[i] = strtoul(byte, 0, 16);
                        }

                    mem_data_ = *mem_ptr_++;
                    state_ = WaitGdbMem;

                    return;
                }

                case 'c': {      // continue [optional resume addr in hex]
                    if (data[1])
                        CpuIss::debugSetRegisterValue(CpuIss::s_pc_register_no, strtoul(data + 1, 0, 16));

                    change_all_states(RunningNoBp);
                    return;
                }

                case 's': {      // continue single step [optional resume addr in hex]
                    uint32_t pc;
                    GdbServer *gs;

                    if (step_id_)       // single step on other processor
                        gs = list_[current_id_ = step_id_ - 1];
                    else
                        gs = this;
 
                    if (data[1]) {        // continue at specified address
                        pc = strtoul(data + 1, 0, 16);
                        gs->CpuIss::debugSetRegisterValue(CpuIss::s_pc_register_no, pc);
                    } else
                        pc = gs->CpuIss::debugGetRegisterValue(CpuIss::s_pc_register_no);

                    gs->state_ = Step;
                    gs->step_pc_ = pc;
                    return;
                }

                case 'H': {     // set current thread
                    int id = strtol(data + 2, 0, 16);

                    switch (id)
                        {
                        case -1: // All threads
                        case 0: // pick any
                            break;
                        default:
                            if ((unsigned)id <= list_.size())
                                current_id_ = id - 1;
                            break;
                        }

                    if (debug_)
                        fprintf(stderr, "[GDB] thread %i selected\n", current_id_);

                    write_packet("OK");

                    return;
                }

                case 'T': {     // check if thread is alive
                    int id = strtol(data + 1, 0, 16);
                    if (id > 0 && (unsigned)id <= list_.size())
                        write_packet("OK");
                    else
                        write_packet("E2");
                    return;
                }

                case 'z':       // set and clean break points
                case 'Z': {
                    char *end;
                    uint32_t addr = strtoul(data + 3, &end, 16);
                    assert(*end == ',');
                    size_t len = strtoul(end + 1, 0, 16);

                    switch (data[1])
                        {
                        case '0':
                        case '1': // execution break point
                            if (data[0] == 'Z')
                                break_exec_[addr] = true;
                            else
                                break_exec_.erase(addr);
                            break;

                        case '2': // write watch point
                            if (data[0] == 'Z')
                                break_write_access_ |= address_set_t(addr, addr + len - 1);
                            else
                                break_write_access_ &= ~address_set_t(addr, addr + len - 1);
                            break;

                        case '3': // read watch point
                            if (data[0] == 'Z')
                                break_read_access_ |= address_set_t(addr, addr + len - 1);
                            else
                                break_read_access_ &= ~address_set_t(addr, addr + len - 1);
                            break;

                        case '4': // access watch point
                            if (data[0] == 'Z') {
                                break_read_access_ |= address_set_t(addr, addr + len - 1);
                                break_write_access_ |= address_set_t(addr, addr + len - 1);
                            } else {
                                break_read_access_ &= ~address_set_t(addr, addr + len - 1);
                                break_write_access_ &= ~address_set_t(addr, addr + len - 1);
                            }
                            break;

                        default:
                            write_packet("");
                            return;
                        }

                    write_packet("OK");
                    return;
                }

                }

            // empty reply if not supported
            write_packet("");
        }
}

template<typename CpuIss>
void GdbServer<CpuIss>::try_accept()
{
    struct pollfd pf;

    pf.fd = socket_;
    pf.events = POLLIN | POLLPRI;

    if (poll(&pf, 1, 0) > 0)
        {
            struct sockaddr_in addr;
            socklen_t addr_size = sizeof(addr);

            asocket_ = accept(socket_, (struct sockaddr*)&addr, &addr_size);

            if (asocket_ >= 0)
                {
                    // freeze all processors on new connections
                    change_all_states(WaitIssMem);
                }
        }
}

template<typename CpuIss>
bool GdbServer<CpuIss>::process_mem_access(
    const struct CpuIss::DataResponse &drsp
    )
{
    if (!mem_req_)
        return false;

    if (!drsp.valid)
        return true;

    if (drsp.error)
        {
            write_packet("E0d");

            free(mem_buff_);
            mem_req_ = false;
            mem_count_ = 0;
            return false;
        }

    switch (mem_type_)
        {
        case CpuIss::DATA_READ: {
            do
                {
                    *mem_ptr_++ = drsp.rdata >> (8 * (mem_addr_ & 3));
                    mem_addr_++;
                    mem_count_--;
                }
            while (mem_count_ && (mem_addr_ & 3));

            if (mem_count_)
                return true;

            char packet[mem_len_ * 2 + 1];
            char *b = packet;

            for (unsigned int i = 0; i < mem_len_ ; i++)
                b += sprintf(b, "%02x", mem_buff_[i]);

            write_packet(packet);

            free(mem_buff_);
            mem_req_ = false;

            break;
        }

        case CpuIss::DATA_WRITE: {
            mem_addr_++;
            mem_count_--;

            if (mem_count_)
                {
                    mem_data_ = *mem_ptr_++;
                    return true;
                }

            write_packet("OK");
            free(mem_buff_);
            mem_req_ = false;

            break;
        }

        case CpuIss::XTN_WRITE: {
            write_packet("OK");
            mem_req_ = false;

            break;
        }


        default:
            assert(0);
        }

    return false;
}

template<typename CpuIss>
void GdbServer<CpuIss>::watch_mem_access()
{
    if (!break_read_access_.empty() || !break_write_access_.empty())
        {
            struct CpuIss::InstructionRequest ireq;
            struct CpuIss::DataRequest dreq;

            CpuIss::getRequests(ireq, dreq);

            if (!dreq.valid)
                return;
            switch(dreq.type)
                {
                default:
                    break;

                case CpuIss::DATA_WRITE:
                case CpuIss::DATA_SC:
                    if (break_write_access_[dreq.addr]) {
                        char buffer[32];

                        std::cerr << *this << "WRITE watchpoint triggered at " << std::hex << dreq.addr << " with value " << dreq.wdata << std::endl;

                        if (!wait_on_wpoint_)
                            break;

                        change_all_states(WaitIssMem); // all processors will end their memory access
                        state_ = Frozen; // except the current processor
                        current_id_ = id_;
                        sprintf(buffer, "T05thread:%x;watch:%x;", id_ + 1, dreq.addr);
                        write_packet(buffer);
                    }
                    break;

                case CpuIss::DATA_READ:
                case CpuIss::DATA_LL:
                    if (break_read_access_[dreq.addr]) {
                        char buffer[32];

                        std::cerr << *this << "READ watchpoint triggered at " << std::hex << dreq.addr << std::endl;

                        if (!wait_on_wpoint_)
                            break;

                        change_all_states(WaitIssMem); // all processors will end their memory access
                        state_ = Frozen; // except the current processor
                        current_id_ = id_;
                        sprintf(buffer, "T05thread:%x;rwatch:%x;", id_ + 1, dreq.addr);
                        write_packet(buffer);
                    }

                    break;
                }

        }
}

template<typename CpuIss>
bool GdbServer<CpuIss>::check_break_points()
{
    char buffer[32];
    uint32_t pc = CpuIss::debugGetRegisterValue(CpuIss::s_pc_register_no);
    int sig;

    if (call_trace_) {
        BinaryFileSymbolOffset sym = loader_->get_symbol_by_addr(pc);
        if (strcmp(sym.symbol().name().c_str(), "Unknown") != 0) {
            uintptr_t symaddr = sym.symbol().address();

            if (symaddr != cur_func_) {
                //std::cerr << *this << "symaddr : " << std::hex << symaddr << " - name : " << sym.symbol().name() << std::endl;
                    cur_func_ = symaddr;
                    if ( (!call_trace_zero_ || pc == symaddr) && (sym.symbol().size() >= 4 || pc != cur_addr_ + 4)) {
                        // avoid display pc+4 when out of symbol
                        std::cerr << *this << "jumped from " << std::hex << cur_addr_ << " to " << sym << std::endl;
                    }
            }
        }
        cur_addr_ = pc;
    }

    if (cycles_bp_ && cycles_bp_ <= cycles_)
        {
            sig = 2;
            cycles_bp_ = 0;
            std::cerr << *this << "reached a cycle breakpoint" << std::endl;
            goto stop;
        }

    if (break_exec_.find(pc) != break_exec_.end())
        {
            sig = 5;
            goto stop;
        }

    if (ctrl_c_)
        {
            ctrl_c_ = false;
            sig = 2;
            goto stop;
        }

    return false;

 stop:
    change_all_states(WaitIssMem);
    current_id_ = id_;

    sprintf(buffer, "T%02xthread:%x;", sig, id_ + 1);
    write_packet(buffer);
    return true;
}

template<typename CpuIss>
void GdbServer<CpuIss>::cleanup()
{
    for (unsigned int i = 0; i < list_.size(); i++)
        {
            GdbServer & gs = *list_[i];

            gs.mem_req_ = false;
            if (gs.mem_count_)
                free(gs.mem_buff_);

            gs.break_exec_.clear();
            gs.break_read_access_.clear();
            gs.break_write_access_.clear();
            gs.state_ = Running;
        }

    close(asocket_);
    asocket_ = -1;
}

template<typename CpuIss>
bool GdbServer<CpuIss>::debugExceptionBypassed( Iss2::ExceptionClass cl, Iss2::ExceptionCause ca )
{
    static const char *str[] = { EXCEPTIONCAUSE_STRINGS };
    int signal = 5; // SIGTRAP by default

    switch ( cl )
        {
        case Iss2::EXCL_FAULT:
            std::cerr << *this << "FAULT: " << str[ca] << std::endl;

            switch ( ca )
                {
                case Iss2::EXCA_BADADDR:
                    signal = 11;
                    break;
                case Iss2::EXCA_ALIGN:
                    signal = 7;
                    break;
                case Iss2::EXCA_ILL:
                    signal = 4;
                    break;
                case Iss2::EXCA_FPU:
                    signal = 8;
                    break;

                case Iss2::EXCA_PAGEFAULT:
                    signal = 11;
                    break;

                case Iss2::EXCA_REGWINDOW:
                    return false;

                default:
                    signal = 5;
                };

            if ( exit_on_fault_ )
                exit(42);
            break;

        case Iss2::EXCL_TRAP:            
            std::cerr << *this << "TRAP" << std::endl;
            if ( exit_on_trap_ )
                exit(41);

            signal = 5;
            break;

        case Iss2::EXCL_SYSCALL:
            return false;

        case Iss2::EXCL_IRQ:
            signal = 2;
            return false;
        }

    // FIXME add configurable check mask

    if ((asocket_ < 0 && !wait_on_except_) || !catch_exceptions_)
        return false;

    if (state_ == WaitIssMem)
        return true;     // An other cpu already froze execution

    char buffer[32];

    sprintf(buffer, "T%02xthread:%x;", signal, id_ + 1);

    write_packet(buffer);
    change_all_states(WaitIssMem);
    current_id_ = id_;
    state_ = Frozen;

    return true;
}

template<typename CpuIss>
uint32_t GdbServer<CpuIss>::executeNCycles( 
    uint32_t ncycle,
    const struct CpuIss::InstructionResponse &irsp,
    const struct CpuIss::DataResponse &drsp,
    uint32_t irq_bit_field )
{
    // check for incoming connection
    if (id_ == 0)
        {
            static unsigned int counter = 0;

            if (!(counter++ % 10000))
                {
                    if (asocket_ < 0)
                        try_accept();
                    else if (state_ == Running)
                        {
                            struct pollfd pf;
                            char buffer[1];

                            pf.fd = asocket_;
                            pf.events = POLLIN | POLLPRI;

                            // try to read CTRL-C code
                            if (poll(&pf, 1, 0) == 1 &&
                                read(asocket_, buffer, 1) == 1 &&
                                buffer[0] == 3)
                                ctrl_c_ = true;
                        }
                }
        }

    bool satisfied = (! pending_ins_request_ || irsp.valid)
        && (! pending_data_request_ || drsp.valid);

#if defined(SOCLIB_MODULE_DEBUG)
    std::cout << CpuIss::name() << " gdb " << state_ << std::endl;
#endif

    switch (state_)
        {
        case WaitGdbMem:

            if (satisfied && !process_mem_access(drsp))
                state_ = Frozen;

            return ncycle;

        case WaitIssMem:
            CpuIss::executeNCycles(0, irsp, drsp, 0);

            if (satisfied)
                state_ = Frozen;

            return 1;

        case Frozen:
            if (id_ != current_id_)
                return 1;

            assert(!mem_req_);

            // process incoming packets
            if (asocket_ >= 0)
                {
                    struct pollfd pf;

                    pf.fd = asocket_;
                    pf.events = POLLIN | POLLPRI;

                    switch (poll(&pf, 1, poll_timeout_))
                        {
                        case 0:         // nothing happened
                            break;
                        case 1:         // need to read data
                            process_gdb_packet();
                            break;
                        default:
                            cleanup();
                            break;
                        }
                }
            return 1;

        case RunningNoBp: {

            uint32_t pc = CpuIss::debugGetRegisterValue(CpuIss::s_pc_register_no);
            size_t ncycles_done = CpuIss::executeNCycles(ncycle, irsp, drsp, irq_bit_field);

            if (pc != CpuIss::debugGetRegisterValue(CpuIss::s_pc_register_no))
                state_ = Running;

            // check memory access break point
            watch_mem_access();

            cycles_ += ncycles_done;
            return ncycles_done;
        }

        case Running: {

            // check execution break point
            if (check_break_points())
                return 1;

            size_t ncycles_done = CpuIss::executeNCycles(ncycle, irsp, drsp, irq_bit_field);

            // check memory access break point
            watch_mem_access();

            cycles_ += ncycles_done;
            return ncycles_done;
        }

        case Step: {
            char buffer[32];
            uint32_t cycles = CpuIss::executeNCycles(ncycle, irsp, drsp, 0);

            if (CpuIss::debugGetRegisterValue(CpuIss::s_pc_register_no) != step_pc_) {
                sprintf(buffer, "T05thread:%x;", id_ + 1);
                write_packet(buffer);
                state_ = WaitIssMem;
                step_id_ = 0;
            }

            cycles_ += cycles;
            return cycles;
        }

        }

    std::abort();
}


template<typename CpuIss>
void GdbServer<CpuIss>::getRequests(struct CpuIss::InstructionRequest &ireq,
                                    struct CpuIss::DataRequest &dreq) const
{
    GdbServer<CpuIss> *_this = const_cast<GdbServer<CpuIss> *>(this);
    switch (state_) {
    case Frozen:
        dreq.valid = false;
        ireq.valid = false;
        break;

    case WaitGdbMem:
        ireq.valid = false;
        dreq.valid = mem_req_;
        dreq.addr = mem_addr_ & ~3;
        dreq.wdata = mem_data_ << (8 * (mem_addr_ & 3));
        dreq.type = mem_type_;
        if ( mem_type_ == CpuIss::DATA_READ )
            dreq.be = 0xf;
        else
            dreq.be = 1 << (mem_addr_ & 3);
        dreq.mode = CpuIss::MODE_HYPER;
        break;

    case WaitIssMem:
    case RunningNoBp:
    case Running:
    case Step:
        CpuIss::getRequests(ireq, dreq);
        break;
    }
    _this->pending_data_request_ = dreq.valid;
    _this->pending_ins_request_ = ireq.valid;
}


template<typename CpuIss>
void GdbServer<CpuIss>::init_state()
{
    const char *env_val = getenv("SOCLIB_GDB");
    size_t id = list_.size();

    if ( id == 0 )
        std::cerr << "[GDB] SOCLIB_GDB env variable may contain the following flag letters: " << std::endl
                  << "  X (dont break on except),      S (wait connect on except),  F (start frozen)" << std::endl
                  << "  C (functions branch trace),    Z (functions entry trace),   D (gdb protocol debug)," << std::endl
                  << "  W (dont break on watchpoints), T (exit simulation on trap), E (exit on fault)" << std::endl
                  << "  => See http://www.soclib.fr/trac/dev/wiki/Tools/GdbServer" << std::endl;

    if ( env_val ) {

        if (!id)
            for (int i = 0; env_val[i]; i++)
                if (!strchr("FCTZSXWDE", env_val[i]))
                    std::cerr << "[GDB] Warning: SOCLIB_GDB variable doesn't support the `" << env_val[i] << "' flag." << std::endl;

        if (strchr( env_val, 'F' ))
            state_ = WaitIssMem;
        else 
            state_ = init_state_;

        if (strchr( env_val, 'D' ))
            debug_ = true;
        if (strchr( env_val, 'C' ))
            call_trace_ = true;
        if (strchr( env_val, 'Z' ))
            call_trace_ = call_trace_zero_ = true;

        if (call_trace_ && !loader_) {
            std::cerr << "[GDB] No loader defined for GdbServer !!! call trace disabled." << std::endl;
            call_trace_ = call_trace_zero_ = false;                
        }

        if (strchr( env_val, 'S' ))
            wait_on_except_ = true;

        if (strchr( env_val, 'T' ))
            exit_on_trap_ = true;

        if (strchr( env_val, 'E' ))
            exit_on_fault_ = true;

        if (strchr( env_val, 'W' ))
            wait_on_wpoint_ = false;

        if (strchr( env_val, 'X' ))
            catch_exceptions_ = false;
    } else {
        state_ = init_state_;
    }

    // Options below this point are handled once for all cpus

    if (( env_val = getenv("SOCLIB_GDB_CYCLEBP") )) {
        
        while (*env_val) {
            uint64_t bp = strtoul(env_val, (char**)&env_val, 0);

            if (*env_val != ',' || strtoul(env_val + 1, (char**)&env_val, 0) == id) {
                std::cerr << "[GDB] cycle breakpoint added at cycle " << std::dec << bp << " for cpu " << id << std::endl;
                cycles_bp_ = bp;
            }

            if (*env_val == ':')
                env_val++;
        }
    }

    if (id)
        return;

    if (( env_val = getenv("SOCLIB_GDB_SLEEPMS") )) {
        poll_timeout_ = atoi(env_val);
    }

    if (( env_val = getenv("SOCLIB_GDB_WATCH") )) {
        size_t size;

        do {
            uint32_t addr = strtoul( env_val, (char**)&env_val, 0 );
            size = 4;

            while ( *env_val && *env_val != ':' ) {
                if ( *env_val == 'w' ) {
                    break_write_access_ |= address_set_t(addr, addr + size - 1);
                    std::cerr << "[GDB] Write watchpoint added [0x" << std::hex << addr << ", 0x" << addr + size - 1<< "]" << std::endl;
                } else if ( *env_val == 'r' ) {
                    break_read_access_ |= address_set_t(addr, addr + size - 1);
                    std::cerr << "[GDB] Read watchpoint added [0x" << std::hex << addr << ", 0x" << addr + size - 1<< "]" << std::endl;
                } else if ( *env_val == ',' ) {
                    size = strtoul( env_val + 1, (char**)&env_val, 0 );
                    continue;
                }

                env_val++;
            }

        } while ( *env_val++ == ':' );
    }
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
