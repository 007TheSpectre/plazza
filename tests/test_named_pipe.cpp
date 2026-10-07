#include <criterion/criterion.h>
#include "ipc/NamedPipe.hpp"
#include <numeric>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

static std::string g_pipe_path;

static void pipe_setup(void)
{
    g_pipe_path = std::string("/tmp/plazza_np_") + std::to_string(::getpid());
}

static void pipe_teardown(void)
{
    NamedPipe::remove(g_pipe_path);
}

Test(NamedPipeTest, CreateMakesFifo, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    struct stat st{};
    cr_assert_eq(::stat(g_pipe_path.c_str(), &st), 0);
    cr_assert(S_ISFIFO(st.st_mode));
}

Test(NamedPipeTest, CreateIsIdempotent, .init = pipe_setup, .fini = pipe_teardown)
{
    cr_assert_no_throw(NamedPipe::create(g_pipe_path));
    cr_assert_no_throw(NamedPipe::create(g_pipe_path));
}

Test(NamedPipeTest, RemoveDeletesFile, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe::remove(g_pipe_path);
    struct stat st{};
    cr_assert_eq(::stat(g_pipe_path.c_str(), &st), -1);
}

Test(NamedPipeTest, SendReceiveSmallPayload, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe reader(g_pipe_path, PipeMode::ReadOnly);
    NamedPipe writer(g_pipe_path, PipeMode::WriteOnly);

    std::vector<uint8_t> received;
    std::thread t([&]() { received = reader.receive(); });

    std::vector<uint8_t> payload{1, 2, 3};
    writer.send(payload);
    t.join();
    cr_assert(received == payload);
}

Test(NamedPipeTest, SendReceiveLargePayload, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe reader(g_pipe_path, PipeMode::ReadOnly);
    NamedPipe writer(g_pipe_path, PipeMode::WriteOnly);

    std::vector<uint8_t> payload(2048);
    std::iota(payload.begin(), payload.end(), 0);

    std::vector<uint8_t> received;
    std::thread t([&]() { received = reader.receive(); });
    writer.send(payload);
    t.join();
    cr_assert(received == payload);
}

Test(NamedPipeTest, SendReceiveEmptyPayload, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe reader(g_pipe_path, PipeMode::ReadOnly);
    NamedPipe writer(g_pipe_path, PipeMode::WriteOnly);

    std::vector<uint8_t> received{0xFF};
    std::thread t([&]() { received = reader.receive(); });
    writer.send({});
    t.join();
    cr_assert(received.empty());
}

Test(NamedPipeTest, HasDataFalseWhenEmpty, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe reader(g_pipe_path, PipeMode::ReadOnly);
    NamedPipe writer(g_pipe_path, PipeMode::WriteOnly);
    cr_assert_not(reader.hasData(0));
}

Test(NamedPipeTest, HasDataTrueAfterSend, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe reader(g_pipe_path, PipeMode::ReadOnly);
    NamedPipe writer(g_pipe_path, PipeMode::WriteOnly);
    writer.send({0x42});
    cr_assert(reader.hasData(10));
}

Test(NamedPipeTest, MultipleMessagesReceivedInOrder, .init = pipe_setup, .fini = pipe_teardown)
{
    NamedPipe::create(g_pipe_path);
    NamedPipe reader(g_pipe_path, PipeMode::ReadOnly);
    NamedPipe writer(g_pipe_path, PipeMode::WriteOnly);

    std::vector<std::vector<uint8_t>> messages = {{1}, {2, 3}, {4, 5, 6}};
    std::vector<std::vector<uint8_t>> received(3);

    std::thread t([&]() {
        for (auto &r : received)
            r = reader.receive();
    });

    for (auto &msg : messages)
        writer.send(msg);
    t.join();
    cr_assert(received == messages);
}
