// Copyright 2011 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// FatFS wrappers.

#include "avrlib/fs/filesystem.h"

#include "avrlib/third_party/ff/diskio.h"
#include "avrlib/time.h"

namespace avrlib {

/* static */
FATFS FileSystem::fs_;

/* static */
FileSystemStatus FileSystem::Init() {
  f_mount(0, &fs_);
  return disk_initialize(0) ? FS_DISK_ERROR : FS_OK;
}

/* static */
FileSystemStatus FileSystem::Init(uint16_t timeout_ms) {
  f_mount(0, &fs_);
  for (uint32_t t = milliseconds() + timeout_ms; milliseconds() < t; ) {
    if (!disk_initialize(0)) {
      return FS_OK;
    }
  }
  return FS_DISK_ERROR;
}

/* static */
FileSystemStatus FileSystem::Unlink(const char* file_name) {
  return static_cast<FileSystemStatus>(f_unlink(file_name));
}

/* static */
FileSystemStatus FileSystem::Mkdir(const char* dir_name) {
  return static_cast<FileSystemStatus>(f_mkdir(dir_name));
}

/* static */
FileSystemStatus FileSystem::Mkdirs(const char* path) {
  return static_cast<FileSystemStatus>(f_mkdir(path));
}

/* static */
FileSystemStatus FileSystem::Chmod(
    const char* file_name,
    uint8_t value,
    uint8_t mask) {
  return static_cast<FileSystemStatus>(f_chmod(file_name, value, mask));
}

/* static */
FileSystemStatus FileSystem::Rename(
    const char* old_name,
    const char* new_name) {
  return static_cast<FileSystemStatus>(f_rename(old_name, new_name));
}

/* static */
FileSystemStatus FileSystem::Mkfs() {
  return static_cast<FileSystemStatus>(f_mkfs(0, 0, 0));
}
  
/* static */
uint32_t FileSystem::GetFreeSpace() {
  FATFS* p;
  DWORD free_clusters = 0;
  f_getfree("/", &free_clusters, &p);
  return free_clusters * p->csize * 512;  // 0 if error.
}

/* static */
FileSystemStatus FileSystem::FileStatus(const char* file_name, FileInfo* info) {
  return static_cast<FileSystemStatus>(f_stat(file_name, &info->file_info));
}

/* static */
FileSystemStatus FileSystem::Utime(
    const char* file_name,
    uint16_t date,
    uint16_t time) {
  FILINFO f;
  f.fdate = date;
  f.ftime = time;
  return static_cast<FileSystemStatus>(f_utime(file_name, &f));
}

}  // namespace avrlib
