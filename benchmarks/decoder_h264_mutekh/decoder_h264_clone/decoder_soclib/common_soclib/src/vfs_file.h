/*****************************************************************************

  vfs_file.h  -- File performing the mount of the VFS file system

  Thales Author:
  Pierre-Edouard Beaucamps THALES COM - AAL, 2009

*****************************************************************************/

#ifndef __VFS_FILE_H__
#define __VFS_FILE_H__

/****************************************************************************
  Include section
****************************************************************************/
#include <stdio.h>

#if defined(CONFIG_ARCH_EMU)
  #include <drivers/device/block/file-emu/block-file-emu.h>
#else
  #include <drivers/device/block/ramdisk/block-ramdisk.h>
#endif

#include <drivers/fs/fat/fat.h>
#include <drivers/fs/ramfs/ramfs.h>
#include <drivers/fs/iso9660/iso9660.h>
#include <vfs/vfs.h>
#include "soclib_addresses.h"


/****************************************************************************
  Non static functions
****************************************************************************/
void input_vfs_init();

#endif /*__INPUT_H__*/
