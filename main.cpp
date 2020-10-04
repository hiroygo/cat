#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <filesystem>
#include <vector>

// 標準出力にファイルディスクリプタの内容を出力する
// エラー時には std::runtime_error を発生させる
void printFD(const int fd)
{
    std::vector<char> rdBuf(1024, 0);
    while (true)
    {
        const auto rdSize = read(fd, rdBuf.data(), rdBuf.size());
        if (rdSize == -1)
        {
            const std::string err = "read error, " + std::string(strerror(errno));
            throw std::runtime_error(err);
        }
        if (rdSize == 0)
        {
            return;
        }

        const auto wrSize = write(STDOUT_FILENO, rdBuf.data(), rdSize);
        if (wrSize == -1)
        {
            const std::string err = "write error, " + std::string(strerror(errno));
            throw std::runtime_error(err);
        }
        if (wrSize != rdSize)
        {
            const std::string err = "write size error, " + std::to_string(wrSize);
            throw std::runtime_error(err);
        }
    }
}

// 標準出力にファイルの内容を出力する
// エラー時には std::runtime_error を発生させる
void printFile(const std::filesystem::path &path)
{
    const auto fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
    {
        const std::string err = "open error, " + std::string(strerror(errno));
        throw std::runtime_error(err);
    }

    try
    {
        printFD(fd);
    }
    catch (const std::runtime_error &e)
    {
        close(fd);
        const std::string err = "printFD error, " + std::string(e.what());
        throw std::runtime_error(err);
    }

    close(fd);
}

void printStdIn()
{
    while (true)
    {
        printFD(STDIN_FILENO);
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;

    if (argc < 2)
    {
        printStdIn();
        return ret;
    }

    for (int i = 1; i < argc; i++)
    {
        const std::filesystem::path path(argv[i]);
        try
        {
            printFile(path);
        }
        catch (const std::runtime_error &e)
        {
            // fprintf は今回は使わない
            // fprintf ではなくて perror(const char *s) でもいいけど
            // "s : strerror(errnum)" のような表記で標準エラー出力される
            ret = 1;
            const std::string err = std::string(path.c_str()) + ": " + std::string(e.what()) + "\n";
            write(STDERR_FILENO, err.c_str(), err.size());
        }
    }

    return ret;
}
