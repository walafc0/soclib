
import struct
import sys

def _terminal_width():
    import termios
    import fcntl

    fd_stdout = sys.stdout.fileno()
    x = fcntl.ioctl(fd_stdout, termios.TIOCGWINSZ, '\x00'*8)
    return struct.unpack("HHHH", x)[1]

def terminal_width():
    try:
        return _terminal_width()
    except:
        return 80
