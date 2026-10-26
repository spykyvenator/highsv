#include "./preprocess.h"
#include "../tp/vbpp/common.h"
#include "../tp/vbpp/yacc_stuff.h"
#include "../tp/vbpp/vpp_yacc.h"

FILE*
preProcessString(const char *s)
{
    YY_BUFFER_STATE buffer = VPP_scan_string(s);
    VPPparse();
    VPP_delete_buffer(buffer);
}
