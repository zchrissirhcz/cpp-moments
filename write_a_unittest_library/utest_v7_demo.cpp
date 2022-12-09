#include <stdio.h>

#define escape_color_blue "\x1b[94m"
#define escape_color_red "\x1b[31m"
#define escape_color_green "\x1b[32m"
#define escape_color_yellow "\x1b[33m"
#define escape_color_purple "\x1b[35m"
#define escape_color_end "\x1b[0m"

#define LOGD(fmt, ...) \
    fprintf(stdout, ("%s[%-5s]\x1b[0m" fmt "\n"), log_colors_debug, "debug", ##__VA_ARGS__)

int main()
{
    //LOGD("%s", "hello");

    fprintf(stdout, ("%sred%s\n"), escape_color_red, escape_color_end);
    fprintf(stdout, ("%sgreen%s\n"), escape_color_red, escape_color_end);

    return 0;
}