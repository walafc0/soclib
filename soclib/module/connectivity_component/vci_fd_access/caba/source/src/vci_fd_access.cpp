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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 */

#include <stdint.h>
#include "register.h"
#include "../include/vci_fd_access.h"
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>

#include <dpp/fstring>

#define CHUNCK_SIZE (1<<(vci_param::K-1))

namespace soclib { namespace caba {

namespace {

inline int soclib_open_sl2os(int soclib_mode)
{
    int ret = 0;
#define do_mode(y, x) if ( soclib_mode & (y) ) ret |= (x)
	do_mode(FD_ACCESS_O_RDONLY, O_RDONLY);
	do_mode(FD_ACCESS_O_WRONLY, O_WRONLY);
	do_mode(FD_ACCESS_O_RDWR, O_RDWR);
	do_mode(FD_ACCESS_O_CREAT, O_CREAT);
	do_mode(FD_ACCESS_O_EXCL, O_EXCL);
	do_mode(FD_ACCESS_O_NOCTTY, O_NOCTTY);
	do_mode(FD_ACCESS_O_TRUNC, O_TRUNC);
	do_mode(FD_ACCESS_O_APPEND, O_APPEND);
	do_mode(FD_ACCESS_O_NONBLOCK, O_NONBLOCK);
#ifdef O_SYNC
	do_mode(FD_ACCESS_O_SYNC, O_SYNC);
#endif
    //	do_mode(FD_ACCESS_O_DIRECT, O_DIRECT);
    //	do_mode(FD_ACCESS_O_LARGEFILE, O_LARGEFILE);
#ifdef O_DIRECTORY
	do_mode(FD_ACCESS_O_DIRECTORY, O_DIRECTORY);
#endif
	do_mode(FD_ACCESS_O_NOFOLLOW, O_NOFOLLOW);
    //	do_mode(FD_ACCESS_O_NOATIME, O_NOATIME);
	do_mode(FD_ACCESS_O_NDELAY, O_NDELAY);
#undef do_mode
    return ret;
}

inline int soclib_stat_os2sl(int stmode)
{
    int ret = stmode & 0777;
#define do_mode(y, x) if ( stmode & (x) ) ret |= (y)
	do_mode(FD_ACCESS_S_IFMT, S_IFMT);
	do_mode(FD_ACCESS_S_IFSOCK, S_IFSOCK);
	do_mode(FD_ACCESS_S_IFLNK, S_IFLNK);
	do_mode(FD_ACCESS_S_IFREG, S_IFREG);
	do_mode(FD_ACCESS_S_IFBLK, S_IFBLK);
	do_mode(FD_ACCESS_S_IFDIR, S_IFDIR);
	do_mode(FD_ACCESS_S_IFCHR, S_IFCHR);
	do_mode(FD_ACCESS_S_IFIFO, S_IFIFO);
	do_mode(FD_ACCESS_S_ISUID, S_ISUID);
	do_mode(FD_ACCESS_S_ISGID, S_ISGID);
	do_mode(FD_ACCESS_S_ISVTX, S_ISVTX);
#undef do_mode
    return ret;
}

}

#ifdef SOCLIB_MODULE_DEBUG
# define FD_ACCESS_MSG(lvl, msg) do { std::cerr << "vci_fd_access:" << __func__ << "(): " << msg << std::endl; } while (0)
#else
# define FD_ACCESS_MSG(lvl, msg) do { if (lvl == 0) std::cerr << "vci_fd_access:" << __func__ << "(): " << msg << std::endl; } while (0)
#endif

#define tmpl(t) template<typename vci_param> t VciFdAccess<vci_param>

tmpl(/**/)::VciFdAccess::Fd::Fd(VciFdAccess *fda, const char *name)
  : _has_tmp(false),
    _type(node_dir),
    _fda(fda),
    _path(name)
{
    _fd = open(name, O_RDONLY);

    if (_fd < 0)
        FD_ACCESS_MSG(0, "unable to open root node " << _path);

    fda->_ino_map[0] = this;
}

tmpl(/**/)::VciFdAccess::Fd::Fd(VciFdAccess *fda, enum node_type_e t, uint32_t &id)
  : _has_tmp(true),
    _type(t),
    _fda(fda)
{
    dpp::fstring path("%s/%u", fda->_tmp_path.c_str(), fda->_tmp_id++);

    FD_ACCESS_MSG(1, "node create " << path);

    switch (t) {
    case node_dir:
        mkdir(path, 0777);
        _fd = open(path, O_RDONLY);
        break;
    case node_file:
        _fd = open(path, O_CREAT | O_RDWR, 0666);
        break;
    }

    if (_fd < 0) {
        FD_ACCESS_MSG(0, "unable to create " << path);
        throw std::exception();
    }

    struct stat st;

    if (::fstat(_fd, &st)) {
        FD_ACCESS_MSG(0, "fstat failed " << path);
        ::close(_fd);
        throw std::exception();
    }

    id = st.st_ino;
    fda->_ino_map[id] = this;
    _path = path;
}

tmpl(/**/)::VciFdAccess::Fd::Fd(VciFdAccess *fda, Fd *parent, const char *name, uint32_t &size, uint32_t &mode, uint32_t &id)
  : _has_tmp(false),
    _fda(fda)
{
    std::string path(name);

    FD_ACCESS_MSG(1, "node lookup " << path);

    if (parent->_type != node_dir) {
        FD_ACCESS_MSG(0, "cant lookup in non-directory parent node " << path);
        throw std::exception();
    }

    struct stat st;

    if (::lstat(path.c_str(), &st)) {
        FD_ACCESS_MSG(1, "entry not found " << path);
        throw std::exception();
    }

    if (S_ISDIR(st.st_mode)) {
        _type = node_dir;
        _fd = open(path.c_str(), O_RDONLY);
        mode = FD_ACCESS_S_IFDIR;
    } else if (S_ISREG(st.st_mode)) {
        _type = node_file;
        _fd = open(path.c_str(), O_RDWR, 0666);
        mode = FD_ACCESS_S_IFREG;
        size = st.st_size;
    } else {
        FD_ACCESS_MSG(0, "unsupported file type " << path);
        throw std::exception();
    }

    if (_fd < 0) {
        FD_ACCESS_MSG(0, "unable to open " << path);
        throw std::exception();
    }

    id = st.st_ino;
    fda->_ino_map[id] = this;
    _path = path;
}

tmpl(/**/)::VciFdAccess::Fd::~Fd()
{
    FD_ACCESS_MSG(1, "node free " << _path);

    if (_has_tmp) {
        int res = 0;

        switch(_type) {
        case node_dir:
            res = ::rmdir(_path.c_str());
            break;
        case node_file:
            res = ::unlink(_path.c_str());
            break;
        }

        if (res)
            FD_ACCESS_MSG(0, "unable to delete " << _path << " temp file");
    }

    ::close(_fd);

    _fda->_ino_map.erase(_fd);
}

tmpl(int)::VciFdAccess::Fd::link(Fd *child, const char *name)
{
    std::string src(child->_path);

    FD_ACCESS_MSG(1, "node link " << src);

    if (_type != node_dir) {
        FD_ACCESS_MSG(0, "cant link in non-directory parent node " << src);
        return -1;
    }

    std::string dest(_path + "/" + name);

    struct stat st;

    if (::lstat(dest.c_str(), &st) == 0) {
        FD_ACCESS_MSG(0, "entry already exists " << dest);
        return -1;
    }

    switch (child->_type) {

    case node_dir:
        if (!_has_tmp) {
            FD_ACCESS_MSG(0, "cant link directory node " << src);
            return -1;
        }
        if (::rename(src.c_str(), dest.c_str())) {
            FD_ACCESS_MSG(0, "unable to rename directory " << src << " to " << dest);
            return -1;
        }
        child->_path = dest;
        child->_has_tmp = false;
        break;

    case node_file:
        if (::link(src.c_str(), dest.c_str())) {
            FD_ACCESS_MSG(0, "unable to link file " << src);
            return -1;
        }
        break;
    }

    return 0;
}

tmpl(int)::VciFdAccess::Fd::unlink(const char *name)
{
    Fd *child;

    std::string src(_path + "/" + name);

    FD_ACCESS_MSG(1, "node unlink " << src);

    if (_type != node_dir) {
        FD_ACCESS_MSG(0, "cant unlink in non-directory parent node " << src);
        return -1;
    }

    struct stat st;

    if (::lstat(src.c_str(), &st) != 0) {
        FD_ACCESS_MSG(0, "entry not found " << src);
        return -1;
    }

    typename std::map<uint32_t, Fd*>::iterator i = _fda->_ino_map.find((uint32_t)st.st_ino);
    if (i == _fda->_ino_map.end())
        child = NULL;
    else
        child = i->second;

    if (S_ISDIR(st.st_mode)) {

        if (child == NULL) {
            if (::rmdir(src.c_str())) {
                FD_ACCESS_MSG(0, "unable to rmdir " << src);
                return -1;
            }

        } else {
            dpp::fstring dest("%s/%u", _fda->_tmp_path.c_str(), _fda->_tmp_id++);
            if (::rename(src.c_str(), dest.c_str())) {
                FD_ACCESS_MSG(0, "unable to rename " << src << " to " << dest);
                return -1;
            }
            child->_path = dest;
            child->_has_tmp = true;
        }

    } else if (S_ISREG(st.st_mode)) {
        if (::unlink(src.c_str())) {
            FD_ACCESS_MSG(0, "unable to unlink " << src);
            return -1;
        }

    } else {
        FD_ACCESS_MSG(0, "unsupported node type " << src);
        return -1;        
    }

    return 0;
}

tmpl(ssize_t)::VciFdAccess::Fd::read(off_t offset, size_t size, void *buf)
{
    ssize_t r;

    FD_ACCESS_MSG(2, "node read " << _path);

    r = ::lseek(_fd, offset, SEEK_SET);

    if (r >= 0)
        r = ::read(_fd, buf, size);

    if (r < 0) {
        FD_ACCESS_MSG(0, "unable to read from " << _path);
        return -1;
    }

    return r;
}

tmpl(ssize_t)::VciFdAccess::Fd::write(off_t offset, size_t size, const void *buf)
{
    ssize_t r;

    FD_ACCESS_MSG(2, "node write " << _path);

    r = ::lseek(_fd, offset, SEEK_SET);

    if (r >= 0)
        r = ::write(_fd, buf, size);

    if (r < 0) {
        FD_ACCESS_MSG(0, "unable to write to " << _path);
        return -1;
    }

    return r;
}

tmpl(int)::VciFdAccess::Fd::readdir(off_t index, char *name, uint32_t &mode)
{
    FD_ACCESS_MSG(2, "node readdir " << _path);

    FD_ACCESS_MSG(0, "not implemented yet");
    return -1;
}

tmpl(int)::VciFdAccess::Fd::stat(uint32_t &size, uint32_t &mode)
{
    struct stat st;

    FD_ACCESS_MSG(2, "node stat " << _path);

    if (::fstat(_fd, &st)) {
        FD_ACCESS_MSG(0, "unable to stat " << _path);
        return -1;
    }

    size = st.st_size;
    mode = soclib_stat_os2sl(st.st_mode);

    return 0;
}

tmpl(void)::ended()
{
	if ( m_irq_enabled )
		r_irq = true;
	m_current_op = m_op = FD_ACCESS_NOOP;
    FD_ACCESS_MSG(2, "op end ");
}

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be)
{
    int cell = (int)addr / vci_param::B;

	switch ((enum SoclibFdAccessRegisters)cell) {
	case FD_ACCESS_FD:
		m_fd = data;
		return true;
    case FD_ACCESS_BUFFER:
		m_buffer = data;
		return true;
    case FD_ACCESS_SIZE:
		m_size = data;
		return true;
    case FD_ACCESS_HOW:
		m_how = data;
		return true;
    case FD_ACCESS_WHENCE:
		m_whence = data;
		return true;
    case FD_ACCESS_OP:
        FD_ACCESS_MSG(2, "operation " << data << " started");
		m_op = data;
		return true;
    case FD_ACCESS_IRQ_ENABLE:
		m_irq_enabled = data;
		return true;
    case FD_ACCESS_RETVAL:
    case FD_ACCESS_ERRNO:
		return false;
	};
	return false;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data)
{
    int cell = (int)addr / vci_param::B;

	switch (cell) {
	case FD_ACCESS_FD:
		data = m_fd;
		return true;
    case FD_ACCESS_BUFFER:
		data = m_buffer;
		return true;
    case FD_ACCESS_SIZE:
		data = m_size;
		return true;
    case FD_ACCESS_HOW:
		data = m_how;
		return true;
    case FD_ACCESS_WHENCE:
		data = m_whence;
		return true;
    case FD_ACCESS_OP:
		data = m_current_op;
		return true;
    case FD_ACCESS_IRQ_ENABLE:
		data = r_irq;
		r_irq = false;
		return true;
    case FD_ACCESS_RETVAL:
		data = m_retval;
		return true;
    case FD_ACCESS_ERRNO:
		data = m_errno;
		return true;
	}
	return false;
}

tmpl(void)::read_done( req_t *req )
{
    if ( ! req->failed() && m_chunck_offset < m_size ) {
        next_req();
        return;
    }

	if ( req->failed() ) {
        FD_ACCESS_MSG(0, "operation " << m_current_op << " dma write error");
		m_retval = -1;
		m_errno = EINVAL;
	}
    delete [] m_data;
	delete req;
    m_data = NULL;
    req = NULL;
	ended();
}

tmpl(void)::write_finish( req_t *req )
{
    if ( ! req->failed() && m_chunck_offset < m_size+1 ) {
        next_req();
        return;
    }

	if ( req->failed() ) {
        FD_ACCESS_MSG(0, "operation " << m_current_op << " dma read error");
		m_retval = -1;
		m_errno = EINVAL;
    } else {

        m_retval = -1;
        switch ((enum SoclibFdOp)m_current_op) {

        case FD_ACCESS_WRITE:
            m_retval = ::write( m_fd, (char *)m_data, m_size );
            m_errno = errno;
            break;

        case FD_ACCESS_OPEN: {
            int how = soclib_open_sl2os(m_how);
            m_retval = ::open( (char *)m_data, how, m_whence );
            m_errno = errno;
            if ( m_retval < 0 )
                FD_ACCESS_MSG(0, "open failed with errno=" << errno);
            break;
        }

        case FD_ACCESS_NODE_LOOKUP: {
            typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
            if (i == _ino_map.end()) {
                FD_ACCESS_MSG(0, "bad lookup parent " << m_fd);
                break;
            }

            try {
                new Fd(this, i->second, (char*)m_data, m_size, m_whence, m_fd);
                m_retval = 0;
            } catch (...) {
            }
            break;
        }

        case FD_ACCESS_NODE_LINK: {
            typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_how);
            if (i == _ino_map.end()) {
                FD_ACCESS_MSG(0, "bad link node " << m_fd);
                break;
            }
            typename std::map<uint32_t, Fd*>::iterator j = _ino_map.find(m_fd);
            if (j == _ino_map.end()) {
                FD_ACCESS_MSG(0, "bad link parent " << m_fd);
                break;
            }
            m_retval = j->second->link(i->second, (char*)m_data);
            break;
        }

        case FD_ACCESS_NODE_UNLINK: {
            typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
            if (i == _ino_map.end()) {
                FD_ACCESS_MSG(0, "bad unlink parent " << m_fd);
                break;
            }

            m_retval = i->second->unlink((char*)m_data);
            break;
        }

        case FD_ACCESS_NODE_WRITE: {
            typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
            if (i == _ino_map.end()) {
                FD_ACCESS_MSG(0, "bad write node " << m_fd);
                break;
            }

            m_retval = i->second->write(m_whence, m_size, m_data);
            break;

        }
        default:
            abort();

        }
    }

	delete req;
    delete [] m_data;
    req = NULL;
    m_data = NULL;
	ended();
}

tmpl(void)::next_req()
{
    switch ((enum SoclibFdOp)m_current_op) {
    case FD_ACCESS_NOOP: 
        ended();
        break;

    case FD_ACCESS_NODE_CREATE: {
        try {
            enum node_type_e t;

            m_retval = -1;
            m_errno = ENOTSUP;

            if (m_whence & FD_ACCESS_S_IFDIR)
                t = node_dir;
            else if (m_whence & FD_ACCESS_S_IFREG)
                t = node_file;
            else {
                ended();
                break;
            }

            new Fd(this, t, m_fd);
            m_retval = 0;
        } catch (...) {
            m_errno = errno;
        }
        ended();
        break;
    }

	case FD_ACCESS_NODE_FREE: {
        typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
        m_retval = -1;
        if (i == _ino_map.end()) {
            FD_ACCESS_MSG(0, "bad freed node " << m_fd);
            ended();
            break;
        }
        delete i->second;
        m_retval = 0;
        ended();
        break;
    }

	case FD_ACCESS_NODE_STAT: {
        typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
        m_retval = -1;
        if (i == _ino_map.end()) {
            FD_ACCESS_MSG(0, "bad stat node " << m_fd);
            ended();
            break;
        }
        m_retval = i->second->stat(m_size, m_whence);
        ended();
        break;
    }

    case FD_ACCESS_CLOSE:
        m_retval = ::close(m_fd);
        m_errno = errno;
        ended();
        break;

    case FD_ACCESS_NODE_LOOKUP:
	case FD_ACCESS_NODE_LINK:
	case FD_ACCESS_NODE_UNLINK:
	case FD_ACCESS_NODE_WRITE:
    case FD_ACCESS_OPEN:
    case FD_ACCESS_WRITE: {
        if ( m_chunck_offset == 0 ) {
            m_data = new uint8_t[m_size + 1];
            std::memset(m_data, 0, m_size + 1);
        }
        size_t chunck_size = m_size-m_chunck_offset;
        if ( chunck_size > CHUNCK_SIZE )
            chunck_size = CHUNCK_SIZE;
        VciInitSimpleReadReq<vci_param> *req =
            new VciInitSimpleReadReq<vci_param>(m_data+m_chunck_offset, m_buffer+m_chunck_offset, chunck_size );
        m_chunck_offset += CHUNCK_SIZE;
        req->setDone( this, ON_T(write_finish) );
        m_vci_init_fsm.doReq( req );
        break;
    }

    case FD_ACCESS_READ:
    case FD_ACCESS_NODE_READ:
    case FD_ACCESS_NODE_READDIR: {

        if ( m_chunck_offset == 0 ) {
            m_data = new uint8_t[m_size];
            std::memset(m_data, 0, m_size);

            switch ((enum SoclibFdOp)m_current_op) {
            case FD_ACCESS_NODE_READ: {
                typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
                if (i == _ino_map.end()) {
                    FD_ACCESS_MSG(0, "bad read node " << m_fd);
                    break;
                }
                m_retval = i->second->read(m_whence, m_size, m_data);
                break;
            }

            case FD_ACCESS_NODE_READDIR: {
                typename std::map<uint32_t, Fd*>::iterator i = _ino_map.find(m_fd);
                if (i == _ino_map.end()) {
                    FD_ACCESS_MSG(0, "bad readdir node " << m_fd);
                    break;
                }
                m_retval = i->second->readdir(m_size, (char*)m_data, m_whence);
                break;
            }

            case FD_ACCESS_READ: {
                m_retval = ::read(m_fd, m_data, m_size);
                break;
            }

            default:
                abort();
            }

            if ( m_retval < 0 ) {
                m_errno = errno;
                delete [] m_data;
                m_data = NULL;
                ended();
                break;
            }
        }
         
        size_t chunck_size = m_size-m_chunck_offset;
        if ( chunck_size > CHUNCK_SIZE )
            chunck_size = CHUNCK_SIZE;
        VciInitSimpleWriteReq<vci_param> *req =
            new VciInitSimpleWriteReq<vci_param>(m_buffer+m_chunck_offset, m_data+m_chunck_offset, chunck_size );
        m_chunck_offset += CHUNCK_SIZE;
        req->setDone( this, ON_T(read_done) );
        m_vci_init_fsm.doReq( req );
        break;
    }

    case FD_ACCESS_LSEEK:
        m_retval = ::lseek(m_fd, m_size, m_whence);
        m_errno = errno;
        ended();
        break;
    }
}

tmpl(void)::transition()
{
	if (!p_resetn) {
		m_vci_target_fsm.reset();
		m_vci_init_fsm.reset();
		r_irq = false;
		m_irq_enabled = false;
		m_op = FD_ACCESS_NOOP;
		m_current_op = FD_ACCESS_NOOP;
        m_chunck_offset = 0;
		return;
	}

	if ( m_current_op == FD_ACCESS_NOOP &&
		 m_op != FD_ACCESS_NOOP ) {
        m_chunck_offset = 0;
		m_current_op = m_op;
        m_op = FD_ACCESS_NOOP;
        next_req();
	}

	m_vci_target_fsm.transition();
	m_vci_init_fsm.transition();
}

tmpl(void)::genMoore()
{
	m_vci_target_fsm.genMoore();
	m_vci_init_fsm.genMoore();

	p_irq = r_irq && m_irq_enabled;
}

tmpl(/**/)::VciFdAccess(
                        sc_module_name name,
                        const MappingTable &mt,
                        const IntTab &srcid,
                        const IntTab &tgtid )
        : caba::BaseModule(name),
          m_vci_target_fsm(p_vci_target, mt.getSegmentList(tgtid)),
          m_vci_init_fsm(p_vci_initiator, mt.indexForId(srcid)),
           _tmp_id(0),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci_target("vci_target"),
      p_vci_initiator("vci_initiator"),
      p_irq("irq")
{
    _tmp_path = dpp::fstring(".vci_fd_access_%p_%u", this, getpid());

    /* create directory for unlinked nodes */
    if (mkdir(_tmp_path.c_str(), 0777)) {
        FD_ACCESS_MSG(0, "error unable to create " << _tmp_path << " directory");
    }

    /** creates node 0 */
    new Fd(this, ".");

	m_vci_target_fsm.on_read_write(on_read, on_write);

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

tmpl(/**/)::~VciFdAccess()
{
    for (typename std::map<uint32_t, Fd*>::iterator i = _ino_map.begin(); i != _ino_map.end(); i++)
        delete i->second;

    rmdir(_tmp_path.c_str());
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

