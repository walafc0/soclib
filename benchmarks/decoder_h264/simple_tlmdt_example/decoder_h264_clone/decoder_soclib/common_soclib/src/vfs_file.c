/*****************************************************************************

  vfs_file.c  -- File performing the mount of the VFS file system
   - input_vfs_init

  Thales Author:
  Pierre-Edouard Beaucamps THALES COM - AAL, 2009

*****************************************************************************/

/****************************************************************************
  Include section
****************************************************************************/
#include "vfs_file.h"

#include <stdio.h>

/****************************************************************************
  Variables and structures
****************************************************************************/
static struct device_s bd_dev;


void input_vfs_init()
{
  /*************************************
     VFS management
  *************************************/
  device_init(&bd_dev);
  bd_dev.addr[0] = 0x68200000;
  error_t err = block_ramdisk_init(&bd_dev,NULL);//, (void*) 0x68200000);
    
  //#if defined(CONFIG_VFS_LIBC_STREAM)
  printf("  -- INPUT VFS INIT -- VFS Initialization\n");
  struct vfs_fs_s *root_fs;
  //fat_open(&bd_dev,&root_fs);
  err = iso9660_open(&root_fs,&bd_dev);
  struct vfs_node_s *root_node;
  vfs_create_root(root_fs, &root_node);
  
  /*
  if (vfs_init(&bd_dev, VFS_VFAT_TYPE, 20, 20, NULL) != 0){
    printf("\033[1A  -- INPUT VFS INIT -- Aborted : problem during the initialization\n");
    abort();
  }
  printf("\033[1A  -- INPUT VFS INIT -- VFS Initialized\033[K\n");
  */
  //#endif
}

