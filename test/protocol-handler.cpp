// oodles
#include "common/Exceptions.hpp"

#include "net/core/Client.hpp"
#include "net/core/Server.hpp"
#include "net/core/OnConnect.hpp"
#include "net/core/ProtocolCreator.hpp"
#include "net/core/ProtocolHandler.hpp"

// Boost
#include <boost/bind.hpp>

// STL
#include <queue>
#include <string>
#include <iostream>

// libc
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>

// STL
using std::cout;
using std::cerr;
using std::endl;
using std::queue;
using std::string;

/* 
 * Gah! A global variable :p
 */
static const string tmp("/tmp/TCPFEX/");

/*
 * Test protocol handler abstraction; This protocol, set
 * by an Endpoint for either client or server, demonstrates
 * how to override the ProtocolHandler interface to provide
 * a bare-bones file transfer protocol with this format;
 *
 * name length|file name|file size|file data
 *
 * Where name length is a 16bit unsigned integer and file
 * size is a 32bit unsigned integer.
 */
class TCPFileExchange : public oodles::net::ProtocolHandler
{
    private:
        struct WiredFile; // Forward declaration for public methods below
    public:
        TCPFileExchange(const oodles::net::DialectCreator &d,
                        oodles::net::OnConnect *c) :
            ProtocolHandler(d, c),
            loaded(0),
            sent(0) {}

        bool load_file(const char *file_name);
        size_t pending() const { return outbound.size(); }
        void write_file(WiredFile &f, const char *buffer, size_t bytes);

        /* Override the pure virtual methods from ProtocolHandler */
        string name() const { return "TCPFEX"; }

        /* Writes */
        void bytes_transferred(size_t n);
        size_t message2buffer(char *buffer, size_t max);

        /* Reads */
        size_t buffer2message(const char *buffer, size_t max);
    private:
        struct WiredFile
        {
            int fd;
            string name;
            size_t size, buffered, transferred;
            /*
             * size is the size of a complete message, not just the file
             */

            WiredFile() : fd(-1), size(0), buffered(0), transferred(0) {}
            void close() { if (fd != -1) { ::close(fd); fd = -1; } }
            void reset() { close(); *this = WiredFile(); }
        };
        
        WiredFile incoming;
        size_t loaded, sent;
        queue<WiredFile> outbound;
        static const uint16_t name_size, file_size;
};

const uint16_t
TCPFileExchange::name_size = sizeof(uint16_t);

const uint16_t
TCPFileExchange::file_size = sizeof(uint32_t);

bool
TCPFileExchange::load_file(const char *file_name)
{
    int fd = -1;
    struct stat info;

    if ((fd = open(file_name, O_RDONLY)) == -1)
        return false;

    if (fstat(fd, &info) != 0)
        return false;

    if (info.st_size == 0)
        return false; // Do not, for this trivial example, load empty files

    WiredFile f;

    f.fd = fd;
    f.name = file_name;
    f.size = name_size + f.name.size() + file_size + info.st_size;

    ++loaded;
    outbound.push(f);

    return true;
}

void
TCPFileExchange::bytes_transferred(size_t n)
{
    assert(pending());

    size_t excess = 0;
    WiredFile &f = outbound.front();

    if (f.transferred + n > f.size)
        excess = n - (f.size - f.transferred);

    f.transferred += (excess > 0 ? excess : n);

    cout << "Transferred " << f.transferred
         << " bytes of " << f.size << " (" << n
         << " in this chunk) for " << f.name << ".\n";

    if (f.transferred == f.size) {
        assert(f.transferred == f.buffered);

        cout << "Transfer of '" << f.name << "' complete.\n";

        ++sent;
        f.close();
        outbound.pop();

        if (excess)
            bytes_transferred(excess);
    }
    
    /*
     * No need to call transfer_data() from here - the Endpoint does it for us
     */
    if (!pending()) {
        stop(); // Will close the connection
        assert(sent == loaded); // Assert we sent all we should have

        cout << "All " << loaded << " files transferred successfully.\n";
        cout << "Transfer metrics:\n";
        print_metrics(&cout);
    }
}

size_t
TCPFileExchange::message2buffer(char *buffer, size_t max)
{
    if (!pending())
        return 0;

    size_t offset = 0;
    WiredFile &f = outbound.front();

    if (f.buffered == 0) { // Send the header?
        const size_t header_size = name_size + file_size + f.name.size();
        const uint32_t i = htonl(f.size - header_size);
        const uint16_t j = htons(f.name.size());
        const void *name = f.name.data();

        if (max < header_size)
            return 0; // Wait until we have a buffer size worthy of writing to

        memcpy(buffer, &j, name_size); // Name length
        memcpy(buffer + name_size, name, f.name.size()); // Name 
        memcpy(buffer + header_size - file_size, &i, file_size); // File size

        offset = header_size;
    }

    size_t pending = f.size - f.buffered;

    if (max > pending)
        max = pending;

    max -= offset;

    const int n = read(f.fd, buffer + offset, max);

    if (n == -1)
        throw oodles::ReadError("TCPFileExchange::message2buffer",
                                errno,
                                "Failed to read %s into socket buffer.",
                                f.name.c_str());

    if (n != EOF)
        f.buffered += (n + offset);

    return n + offset; // Return the number of bytes in buffer we used
}

void
TCPFileExchange::write_file(WiredFile &f, const char *buffer, size_t bytes)
{
    assert(f.size > 0);
    assert(!f.name.empty());

    if (f.fd == -1) {
        static const size_t e = string::npos; // end
        const size_t s = f.name.find_last_of('/'); // slash
        const string file_name(tmp + f.name.substr(s == e ? 0 : s + 1));

        f.fd = open(file_name.c_str(), O_WRONLY | O_CREAT, 0644);

        if (f.fd == -1)
            throw oodles::OpenError("TCPFileExchange::write_file",
                                    errno,
                                    "Failed to open %s for writing.",
                                    file_name.c_str());
    }

    const int n = write(f.fd, buffer, bytes);

    if (n == -1)
        throw oodles::WriteError("TCPFileExchange::write_file",
                                 errno,
                                 "Failed to write to %s from socket buffer.",
                                 f.name.c_str());

    f.buffered += n;
}

size_t
TCPFileExchange::buffer2message(const char *buffer, size_t max)
{
    size_t offset = 0, used = 0;

    /*
     * If no name is set we must retrieve it from the buffer
     */
    if (incoming.name.empty()) {
        uint16_t n = 0;

        if (max < name_size)
            return 0;

        memcpy(&n, buffer, name_size);
        used += name_size;
        n = ntohs(n);

        if (max - used < n)
            return used;

        used += n;
        incoming.name.reserve(n);
        incoming.name.assign(buffer + name_size, n);
    }

    const uint32_t header_size = name_size + incoming.name.size() + file_size;

    /*
     * If no message size is set we must retrieve it from the buffer
     */
    if (incoming.size == 0) {
        uint32_t s = 0;

        if (max - used < file_size)
            return used;

        used += file_size;
        offset = header_size;

        memcpy(&s, buffer + (offset - file_size), file_size);
        incoming.size = header_size + ntohl(s);
    }

    assert(!incoming.name.empty());

    size_t written = incoming.buffered, end = 0;

    if (written + (max - used) > incoming.size - header_size)
        end = (incoming.size - header_size) - written;

    write_file(incoming, buffer + offset, end > 0 ? end : max - used);
    used += incoming.buffered - written;

    if (incoming.buffered == incoming.size - header_size) {
        ++loaded;
        
        cout << "Download of '" << incoming.name << "' complete, "
             << loaded << " files received in total.\n";

        incoming.reset();
        cout << "Transfer metrics:\n";
        print_metrics(&cout);
    }

    return used;
}

struct BeginExchange : public oodles::net::OnConnect
{
    int argc;
    char **argv;

    OnConnect* create() const { return new BeginExchange(*this); } // Clone

    void operator() (oodles::net::ProtocolHandler &p)
    {
        TCPFileExchange &tcpfex = *p.get_dialect();
        
        for (int i = 0 ; i < argc ; ++i) {
            if (!tcpfex.load_file(argv[i]))
                cerr << "Failed to load file '" << argv[i] << "'.\n";
            else
                cout << "Opened file '" << argv[i] << "'.\n";
        }
        
        cout << "Attempting to send " << tcpfex.pending() << " files...\n";
    }
};


static void print_usage(const char *program)
{
    cerr << program << " [-h|-c <host:port>|-s <ip:port>] <file file...>\n\n";

    cerr << "Provide -c (or --client) and a host:port pair to run a client.\n";
    cerr << "Provide -s (or --server) and an ip:port pair to run a server.\n";
}

int main(int argc, char *argv[])
{
    int ch = -1;
    bool client_only = false,
         server_only = false;
    string listen_on("127.0.0.1:8888"),
           connect_to("localhost:8888");
    const char *short_options = "hc:s:";
    const struct option long_options[4] = {
        {"help", no_argument, NULL, short_options[0]},
        {"client", required_argument, NULL, short_options[1]},
        {"server", required_argument, NULL, short_options[3]},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv,
                             short_options,
                             long_options, NULL)) != -1)
    {
        switch (ch) {
            case 'c':
                connect_to = optarg;
                client_only = true;
                break;
            case 's':
                listen_on = optarg;
                server_only = true;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    argc -= optind;
    argv += optind;

    const bool send = (!client_only && !server_only) || client_only;

    if (send && argc == 0) {
        cerr << "Supply at least one file to transfer.\n";
        return 1;
    }

    try {
        BeginExchange *oncon = send ? new BeginExchange : NULL;
        const oodles::net::Protocol<TCPFileExchange> creator(oncon);
        oodles::net::Server *server = NULL;
        oodles::net::Client *client = NULL;
        boost::asio::io_service service;

        if (!client_only)
            server = new oodles::net::Server(service, creator);

        if (!server_only)
            client = new oodles::net::Client(service, creator);

        if (send) {
            oncon->argc = argc;
            oncon->argv = argv;
        }

        if (server) {
            cout << "NOTE: All received files can be found in " << tmp << ".\n";
            server->start(listen_on);
        }

        if (client)
            client->start(connect_to);

        service.run();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
