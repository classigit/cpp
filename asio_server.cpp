#include <cstdlib>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace std;
using boost::asio::ip::tcp;

const int THREAD_POOL_SIZE = 2;

class session
    // enables to get a valid shared_ptr to this
        : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)
            : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                /*
                 * "this" is passed to be able to call do_write
                 * self is passed in order to make sure that session
                 * object outlives the asynchronous operation
                */
                                [this, self](boost::system::error_code ec, std::size_t bytes_read)
                                {

                                    if (!ec)
                                    {
                                        do_write(bytes_read);
                                    }
                                    else
                                    {
                                        /* When the server closes the connection, the
                                         * ip::tcp::socket::read_some() function will exit with the
                                         * boost::asio::error::eof error
                                         */
                                        if (ec != boost::asio::error::eof)
                                        {
                                            cout << ec.message() << endl;
                                        }
                                    }
                                });
    }

    void do_write(std::size_t length)
    {
        data_[length - 1] = 0;

        stringstream ss;
        cout << this_thread::get_id() << endl;
        this_thread::sleep_for(chrono::seconds(5));
        ss << ' ' << this_thread::get_id() << '\n';
        length = length + ss.str().length();
        std::strcat(data_, ss.str().c_str());
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/)
                                 {
                                     if (!ec)
                                     {
                                         do_read();
                                     }
                                     else
                                     {
                                         cout << ec.message() << endl;
                                     }
                                 });
    }

    tcp::socket socket_;
    enum
    {
        max_length = 1024
    };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context &io_context, unsigned short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
                // "this" is passed to be able to call do_accept
                [this](boost::system::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        make_shared<session>(move(socket))->start();
                    }
                    else
                    {
                        cout << ec.message() << endl;
                    }

                    do_accept();
                });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char *argv[])
{
    try
    {
        boost::asio::io_context io_context;

        server s(io_context, 7777);
        vector<boost::shared_ptr<boost::thread> > threads;
        for (auto i = 0; i < THREAD_POOL_SIZE; ++i)
        {
            boost::shared_ptr<boost::thread> thread(new boost::thread(
                    boost::bind(&boost::asio::io_context::run, &io_context)));
            threads.emplace_back(thread);
        }

        // Wait for all threads in the pool to exit.
        for (auto &thread : threads)
            thread->join();

    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
