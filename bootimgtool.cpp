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


extern "C"
{
  #include "bootimg.h"
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

int usage(string name) {
	cout<< "usage: " << name << " <options> <boot-image> <directory>" << endl << endl;
	cout<< "options:" << endl;
	cout<< "         x - Extract <boot-image-path> to <directory>" << endl;
	cout<< "         p - Repack files with <boot-image> basename from <directory> to <boot-image>" << endl << endl;
	cout<< "example:" <<endl;
	cout<< "         " << name << " x /sdcard/boot.img /sdcard/extracted-bootimage" << endl;
	cout<< "         " << name << " p /sdcard/boot.img /sdcard/extracted-bootimage" << endl;
	return 1;
}

int main (int argc, char** argv) {
	if (argc < 4)
		return usage(argv[0]);

	char *option = argv[1];

	string path = argv[2];
	string fname = basename(path.c_str());
	string dir = argv[3];

	if (!strcmp(option, "x")) {
		int result = unpackbootimg(path.c_str(), dir.c_str(), 0);

		if (result == 0)
			cout << endl << endl << "Successfully unpacked \"" << path << "\" to \"" << dir << "\"." << endl << endl;
		else
			cout << endl << endl << "Failed to unpack \"" << path << "\" to \"" << dir << "\"." << endl << endl;

		return result;
	} else if (!strcmp(option, "p")) {
		string obase = dir + "/" + fname;

		string bootimg = dir + "/" + "repacked-" + fname;
		string kernel_fn = obase + "-zImage";
		string ramdisk_fn = obase + "-ramdisk.gz";
		string second_fn = obase + "-second";
		string cmdline = Load_File(obase + "-cmdline");
		string dt_fn = obase + "-dt";
		string board = Load_File(obase + "-board");
		string base_v = Load_File(obase + "-base");
		string kernel_offset_v = "";
		string ramdisk_offset_v = Load_File(obase + "-ramdisk_offset");
		string second_offset_v = Load_File(obase + "-second_offset");
		string tags_offset_v = Load_File(obase + "-tags_offset");
		string pagesize_v = Load_File(obase + "-pagesize");
		string os_version_v = Load_File(obase + "-osversion");
		string os_patch_level_v = Load_File(obase + "-oslevel");
		string hash_v = Load_File(obase + "-hash");
		bool get_id = false;

		int result = makebootimg(bootimg.c_str(), kernel_fn.c_str(), ramdisk_fn.c_str(),
								 second_fn.c_str(), cmdline.c_str(), dt_fn.c_str(), board.c_str(), base_v.c_str(),
								 kernel_offset_v.c_str(), ramdisk_offset_v.c_str(), second_offset_v.c_str(),
								 tags_offset_v.c_str(), pagesize_v.c_str(), get_id, os_version_v.c_str(), os_patch_level_v.c_str(), hash_v.c_str());

		if (result == 0)
			cout << endl << endl << "Successfully repacked files from \"" << dir << "\" to \"" << path << "\"." << endl << endl;
		else
			cout << endl << endl << "Failed to repack files from \"" << dir << "\" to \"" << path << "\"." << endl << endl;

		return result;
	} else {
		cout<< "error: unknown option" << endl << endl;
		return usage(argv[0]);
	}

	return 0;
}
