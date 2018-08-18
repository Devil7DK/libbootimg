/*
*==========================================================================*
*                                                                          *
*                    (C) Copyright 2018 Devil7 Softwares.                  *
*                                                                          *
* Licensed under the Apache License, Version 2.0 (the "License");          *
* you may not use this file except in compliance with the License.         *
* You may obtain a copy of the License at                                  *
*                                                                          *
*                http://www.apache.org/licenses/LICENSE-2.0                *
*                                                                          *
* Unless required by applicable law or agreed to in writing, software      *
* distributed under the License is distributed on an "AS IS" BASIS,        *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
* See the License for the specific language governing permissions and      *
* limitations under the License.                                           *
*                                                                          *
*==========================================================================*
*/

#include <iostream>
#include <fstream>
#include <sys/stat.h>

extern "C"
{
	#include "bootimg.h"
	#include "utils/flashutils.h"
}

using namespace std;

string Load_File(string path) {
	string line;
	ifstream File;
	File.open (path);
	if(File.is_open()) {
		getline(File,line);
		File.close();
	}
	return line;
}

bool dir_exists(const char* path) {
	struct stat statbuf;
	int isDir = 0;
	if (stat(path, &statbuf) != -1) {
		if (S_ISDIR(statbuf.st_mode)) {
			return true;
		}
	}
	return false;
}

int packimage(string bootimg, string dir) {
	string obase = dir + "/split_img/";
	
	if (!dir_exists(obase.c_str())) {
		cout << "Failed to find files to repack image..!" << endl;
		return 1;
	}

	string kernel_fn = obase + "zImage";
	string ramdisk_fn = obase + "ramdisk.gz";
	string second_fn = obase + "second";
	string cmdline = Load_File(obase + "cmdline");
	string dt_fn = obase + "dt";
	string board = Load_File(obase + "board");
	string base_v = Load_File(obase + "base");
	string kernel_offset_v = "";
	string ramdisk_offset_v = Load_File(obase + "ramdisk_offset");
	string second_offset_v = Load_File(obase + "second_offset");
	string tags_offset_v = Load_File(obase + "tags_offset");
	string pagesize_v = Load_File(obase + "pagesize");
	string os_version_v = Load_File(obase + "osversion");
	string os_patch_level_v = Load_File(obase + "oslevel");
	string hash_v = Load_File(obase + "hash");
	bool get_id = false;
	
	int result = makebootimg(bootimg.c_str(), kernel_fn.c_str(), ramdisk_fn.c_str(),
							 second_fn.c_str(), cmdline.c_str(), dt_fn.c_str(), board.c_str(), base_v.c_str(),
							 kernel_offset_v.c_str(), ramdisk_offset_v.c_str(), second_offset_v.c_str(),
							 tags_offset_v.c_str(), pagesize_v.c_str(), get_id, os_version_v.c_str(), os_patch_level_v.c_str(), hash_v.c_str());
			
	if (result == 0)
		cout << endl << endl << "Successfully repacked files from \"" << dir << "\" to \"" << bootimg << "\"." << endl << endl;
	else
		cout << endl << endl << "Failed to repack files from \"" << dir << "\" to \"" << bootimg << "\"." << endl << endl;
	
	return result;
}

int usage(string name) {
	cout<< "usage: " << endl;
	cout<< "         " << name << " <x/p> <image> <directory>" << endl << endl;
	cout<< "         " << name << " <b/r><x/p> <directory>" << endl << endl;
	cout<< "options:" << endl;
	cout<< "          x - Extract <image-path> to <directory>" << endl;
	cout<< "          p - Repack files from <directory> to <image>" << endl << endl;
	cout<< "         bx - Extract current boot partition to <directory>" << endl;
	cout<< "         bp - Repack files from <directory> and flash image to boot" << endl << endl;	
	cout<< "         rx - Extract recovery partition to <directory>" << endl;
	cout<< "         rp - Repack files from <directory> and flash image to recovery" << endl << endl;
	cout<< "example:" <<endl;
	cout<< "         " << name << " x /sdcard/boot.img /sdcard/extracted-image" << endl;
	cout<< "         " << name << " p /sdcard/boot.img /sdcard/extracted-image" << endl << endl;
	cout<< "         " << name << " bx /sdcard/extracted-bootimage" << endl;
	cout<< "         " << name << " bp /sdcard/extracted-bootimage" << endl << endl;
	cout<< "         " << name << " rx /sdcard/extracted-recoveryimage" << endl;
	cout<< "         " << name << " rp /sdcard/extracted-recoveryimage" << endl << endl;
	return 1;
}

int main (int argc, char** argv) {
	if (argc >= 4) {
		char *option = argv[1];
		
		string path = argv[2];		
		string dir = argv[3];
		string split_img = dir + "/split_img";
		
		if (!dir_exists(dir.c_str())) {
			if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
				cout << "Failed to create dir: " << dir << endl;
				return 1;
			}
		}
		
		if (!dir_exists(split_img.c_str())) {
			if (mkdir(split_img.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
				cout << "Failed to create dir: " << split_img << endl;
				return 1;
			}
		}
		
		if (!strcmp(option, "x")) {
			int result = unpackbootimg(path.c_str(), split_img.c_str(), 0);
			
			if (result == 0)
				cout << endl << endl << "Successfully unpacked \"" << path << "\" to \"" << dir << "\"." << endl << endl;
			else
				cout << endl << endl << "Failed to unpack \"" << path << "\" to \"" << dir << "\"." << endl << endl;

			return result;
		} else if (!strcmp(option, "p")) {
			return packimage(path, dir);
		} else {
			cout<< "error: unknown option" << endl << endl;
			return usage(argv[0]);
		}		
	} else if(argc == 3) {
		char *option = argv[1];
		
		string dir = argv[2];
		string split_img = dir + "/split_img";
		string newpath = dir + "/image-new.img";
		string partition = "boot";
		
		if (strlen(option) < 2) {
			cout<< "error: unknown option" << endl << endl;
			return usage(argv[0]);
		}
		
		if (option[0] == 'b')
			partition = "boot";
		else if (option[0] == 'r')
			partition = "recovery";
		else {
			cout<< "error: unknown option" << endl << endl;
			return usage(argv[0]);
		}
			
		
		string path = dir + "/" + partition + ".img";
		string partition_p = "/dev/block/bootdevice/by-name/" + partition;
		
		if (!dir_exists(dir.c_str())) {
			if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
				cout << "Failed to create dir: " << dir << endl;
				return 1;
			}
		}
		
		if (!dir_exists(split_img.c_str())) {
			if (mkdir(split_img.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
				cout << "Failed to create dir: " << split_img << endl;
				return 1;
			}
		}
		
		if (option[1] == 'x') {
			int result1 = backup_raw_partition(NULL, partition_p.c_str(), path.c_str());
			
			if (result1 == 0) {
				cout << endl << "Successfully dumped " + partition + " partition to \"" << path << "\"." << endl << endl;
			} else {
				cout << endl << "Failed to dump " + partition + " partition to \"" << path << "\"." << endl << endl;
				return 1;
			}
			
			int result2 = unpackbootimg(path.c_str(), split_img.c_str(), 0);
			
			if (result2 == 0)
				cout << endl << endl << "Successfully unpacked \"" << path << "\" to \"" << dir << "\"." << endl << endl;
			else
				cout << endl << endl << "Failed to unpack \"" << path << "\" to \"" << dir << "\"." << endl << endl;
			
			return result2;			
		} else if (option[1] == 'p') {
			int result1 = packimage(newpath, dir);
			
			if (result1 != 0) {
				return 1;
			}
			
			int result2 = restore_raw_partition(NULL, partition_p.c_str(), newpath.c_str());
			
			if (result2 == 0)
				cout << endl << endl << "Successfully flashed \"" << newpath << "\" to \"" + partition + "\" partition." << endl << endl;
			else
				cout << endl << endl << "Failed to flash \"" << newpath << "\" to \"" + partition + "\" partition." << endl << endl;
			
			return result2;			
		} else {
			cout<< "error: unknown option" << endl << endl;
			return usage(argv[0]);
		}
	} else {
		return usage(argv[0]);
	}

	return 0;
}
