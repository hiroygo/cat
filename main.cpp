#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <cstdio>
#include <filesystem>
#include <vector>

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

    std::vector<char> readBuff(1024, 0);
    while (true)
    {
        const auto readSize = read(fd, readBuff.data(), readBuff.size());
        if (readSize == 0)
        {
            break;
        }
        if (readSize == -1)
        {
            const std::string err = "read error, " + std::string(strerror(errno));
            close(fd);
            throw std::runtime_error(err);
        }

        const auto writeSize = write(STDOUT_FILENO, readBuff.data(), readSize);
        if (writeSize == -1)
        {
            const std::string err = "write error, " + std::string(strerror(errno));
            close(fd);
            throw std::runtime_error(err);
        }
        if (writeSize != readSize)
        {
            const std::string err = "write size error, " + std::to_string(writeSize);
            close(fd);
            throw std::runtime_error(err);
        }
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1;
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
            // perror(const char *s) だと、"s : strerror(errnum)" のような表記で標準エラー出力される
            fprintf(stderr, "%s: %s\n", path.c_str(), e.what());
        }
    }

    return 0;
}
