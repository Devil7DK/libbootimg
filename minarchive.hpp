#include <iostream>

#include <archive.h>
#include <archive_entry.h>

using namespace std;

static int copy_data(struct archive *ar, struct archive *aw);
int extract(string filename, string outdir);
