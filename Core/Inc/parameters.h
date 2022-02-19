#include <main.h>
#include "ff.h"

#define mount_SD(arg) f_mount(arg, "", 1)
#define unmount_SD() f_mount(0, "", 1)

#define MAX_PARAMS      30
#define MAX_LANG        10
#define LINE_SD_BUFFER  1024 



#define LANG_DIR "languages"
#define LANG_DIR_FILE "languages/"
#define FILE_FORMAT ".csv"

void load_params_SD();
void update_param_file(uint8_t ID);
FRESULT scan_files (char* path);
void load_language_pack();