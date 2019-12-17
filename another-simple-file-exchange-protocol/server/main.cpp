#include "../common/common.h"
int main()
{
    FtpServer ftpServer = FtpServer(12005);
    ftpServer.apply();
}