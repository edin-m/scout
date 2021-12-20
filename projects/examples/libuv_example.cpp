// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <sstream>
#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <set>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "uv.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


typedef int TcpConnectionId;








template <class T>
class IdPool
{
 public:
  IdPool(T initial = 1) : id_(initial) {}

  T Next()
  {
    if (free_.size() > 0) {
      T id = free_.front();
      free_.pop_front();
      return id;
    }

    return id_++;
  }

  void Free(T id)
  {
    free_.push_back(id);
  }
 private:
  T id_;
  std::deque<T> free_;
};

using IntIdPool = IdPool<int>;














class TcpSocket2 {
    using OnConnectCb = std::function<void()>;
    using OnCloseCb = std::function<void()>;
    using OnDataCb = std::function<void(std::string)>;
public:
    typedef int SocketId;

    TcpSocket2(uv_loop_t* loop_)
        : loop(loop_)
    {
        on_connect = []() {};
        on_data = [](std::string) {};
        on_close_last = []() {};
    }

    TcpSocket2(SocketId id_, uv_loop_t* loop_, uv_stream_t* stream_)
        : TcpSocket2(loop_)
    {
        id = id_;
        stream = stream_;
    }

    void Write(std::string message) {
        std::cout << __FUNCTION__ << "|"
                  << name << "|"
                  << message << std::endl;
        uv_write_t* write;
        write = (uv_write_t*) malloc(sizeof *write);
        write->data = this;

        uv_buf_t buf;
        buf.base = (char*) message.c_str();
        buf.len = message.length();

        uv_write(write, stream, &buf, 1, &TcpSocket2::WriteCb);
    }

    void Read() {
        int res;

        res = uv_read_start(
                    stream,
                    &TcpSocket2::AllocCb,
                    &TcpSocket2::ReadCb);
    }

    void Connect(std::string ipv4, int port) {
        std::cout << __FUNCTION__ << std::endl;
        int res;
        struct sockaddr_in addr;
        uv_connect_t* req;

        uv_tcp_t* client = (uv_tcp_t*) malloc(sizeof uv_tcp_t);
        res = uv_tcp_init(loop, client);

        req = (uv_connect_t*) malloc(sizeof *req);
        client->data = this;
        req->data = client;
        res = uv_ip4_addr(ipv4.c_str(), port, &addr);

        stream = (uv_stream_t*) client;

        uv_tcp_nodelay(client, true);
        res = uv_tcp_connect(req, client, (const sockaddr*) &addr, &TcpSocket2::ConnectedCb);
    }

    void Close() {
        if (stream != nullptr) {
            uv_close((uv_handle_t*) stream, AfterCloseCb);
        }
    }

    void SetName(std::string name_) { name = name_; }

    const std::string& GetName() const { return name; }

    void OnConnect(OnConnectCb f) { on_connect = f; }

    void OnData(OnDataCb f) { on_data = f; }

    void OnClose(OnCloseCb f) { on_close.push_back(f); }

    // emits after all close cbs called; useful for deleting
    void OnCloseLast(OnCloseCb f) { on_close_last = f; }

    SocketId GetSocketId() { return id; }

private:
    SocketId id;
    uv_loop_t* loop;
    uv_stream_t* stream = nullptr;
    std::string name;

    OnConnectCb on_connect;
    OnDataCb on_data;
    std::vector<OnCloseCb> on_close;
    OnCloseCb on_close_last;

    static void ConnectedCb(uv_connect_t* req, int status) {
        uv_tcp_t* client = (uv_tcp_t*) req->handle;
        TcpSocket2* socket = (TcpSocket2*) client->data;

        free(req);

        socket->on_connect();
    }

    static void WriteCb(uv_write_t* req, int status) {
        std::cout << __FUNCTION__ << status << std::endl;
//        uv_stream_t* stream = req->handle;
        free(req);
//        uv_close((uv_handle_t*) stream, AfterCloseCb);
    }

    static void AllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
        buf->base = (char*) malloc(suggested_size);
        buf->len = suggested_size;
    }

    static void ReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
        uv_shutdown_t* sreq;
        TcpSocket2* self = (TcpSocket2*) stream->data;
        int res;

        if (nread < 0) {
            if (nread == UV_EOF) {
                free(buf->base);
                uv_close((uv_handle_t*) stream, AfterCloseCb);
                return;
            }

            // todo: error handle

            if (uv_is_writable(stream)) {
                sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
                sreq->data = self;
                uv_shutdown(sreq, stream, AfterShutdownCb);
            }

            // close or stop read on error

            return;
        }

        std::stringstream ss;
        for (int i = 0; i < nread; i++) {
            char c = buf->base[i];
            ss << c;
        }

        if (nread != 0) {
            std::cout << "RECV2:" << nread << uv_is_writable(stream) << " " << ss.str() << std::endl;
        }

        self->on_data(ss.str());
    }

    static void AfterShutdownCb(uv_shutdown_t* req, int status) {
        std::cout << __FUNCTION__ << std::endl;
        uv_close((uv_handle_t*) req->handle, AfterCloseCb);
        free(req);
    }

    static void AfterCloseCb(uv_handle_t* handle) {
        TcpSocket2* self = (TcpSocket2*) handle->data;
        std::cout << __FUNCTION__ << " "
                  << self->GetName() << " "
                  << self->on_close.size() << std::endl;
        free(handle);

        for (int i = 0; i < self->on_close.size(); i++) {
            self->on_close[i]();
        }
    }
};





class TcpServer2 {
    using OnConnectionCb = std::function<void(TcpSocket2*)>;
    using OnListeningCb = std::function<void()>;
    using OnErrorCb = std::function<void()>;
    using OnCloseCb = std::function<void()>;
    using SocketId = TcpSocket2::SocketId;
public:
    TcpServer2(uv_loop_t* loop_)
        : loop(loop_)
    {
        on_connection = [](TcpSocket2*) {};
    }

    void Listen(const char* address, int port) {
        int res;

        res = uv_tcp_init(loop, &server);

        struct sockaddr_in server_address;
        res = uv_ip4_addr(address, port, &server_address);

        res = uv_tcp_bind(&server, (const struct sockaddr*) &server_address, 0);

        server.data = this;

        res = uv_listen((uv_stream_t*) &server, 128, &TcpServer2::OnConnected);

        is_listening = true;
    }

    bool IsListening() { return is_listening; }

    void OnConnection(OnConnectionCb f) { on_connection = f; }
private:
    uv_loop_t* loop;
    uv_tcp_t server;

    std::map<SocketId, TcpSocket2*> clients;

    IdPool<SocketId> id_pool;

    // listeners
    OnConnectionCb on_connection;

    bool is_listening = false;

private:

    static void OnConnected(uv_stream_t* server, int status) {
        TcpServer2* self = (TcpServer2*) server->data;
        uv_stream_t* stream;
        int res;

        stream = (uv_stream_t*) malloc(sizeof(uv_tcp_t));
        res = uv_tcp_init(self->loop, (uv_tcp_t*)stream);

        res = uv_accept(server, stream);

        SocketId id = self->id_pool.Next();
        TcpSocket2* socket = new TcpSocket2(id, self->loop, stream);
        self->clients[id] = socket;

        socket->OnClose([=]() {
            self->clients.erase(socket->GetSocketId());
            self->id_pool.Free(socket->GetSocketId());
        });
        socket->OnCloseLast([socket]() {
            delete socket;
        });

        stream->data = socket;

        socket->Read();

        self->on_connection(socket);
    }
};






















class TcpConnection {
    using ConnectionClosedCb = std::function<void()>;
    using DataReceivedCb = std::function<void(std::string)>;
public:
    TcpConnection(TcpConnectionId id_, uv_stream_t* stream_)
        : id(id_), stream(stream_) {
        stream->data = this;
        on_data_received_cb = [](std::string) { };
    }

    TcpConnection(uv_loop_t* uvloop_)
        : uvloop(uvloop_)
    {}

    void SetMessage(std::string str) {
        message = str;
    }

    std::string GetMessage() { return message; }

    void Read() {
        uv_read_start(stream, &TcpConnection::AllocCb, &TcpConnection::ReadCb);
    }

    void Connect(std::string ipv4, int port) {
        std::cout << __FUNCTION__ << std::endl;
        int res;
        struct sockaddr_in addr;
        uv_connect_t* req;

        client = (uv_tcp_t*) malloc(sizeof uv_tcp_t);
        res = uv_tcp_init(uvloop, client);

        req = (uv_connect_t*) malloc(sizeof *req);
        client->data = this;
        req->data = this;
        res = uv_ip4_addr(ipv4.c_str(), port, &addr);
        std::cout << "ipaddr" << uv_err_name(res) << this << GetMessage() << std::endl;

        res = uv_tcp_connect(req, client, (const sockaddr*) &addr, &TcpConnection::ConnectedCb);
        std::cout << uv_err_name(res) << std::endl;
    }

    void Write(std::string message) {

    }

    static void Send(std::string message, const char* ip, int port) {
        TcpConnection* conn = new TcpConnection(uv_default_loop());
    }

    void OnConnectionClosed(ConnectionClosedCb cb) { on_conn_closed.push_back(cb); }
    void OnDataReceived(DataReceivedCb cb) { on_data_received_cb = cb; }

private:
    // server conn specific
    TcpConnectionId id;

    uv_loop_t* uvloop;
    uv_stream_t* stream;

    // client specific
    uv_tcp_t* client;
    uv_connect_t* req;
    std::string message;

    std::vector<ConnectionClosedCb> on_conn_closed;
    DataReceivedCb on_data_received_cb;
//    ClientConnectedCb client_connected_cb;

    static void ConnectedCb(uv_connect_t* req, int status) {
        std::cout << __FUNCTION__ << std::endl;
        TcpConnection* self = (TcpConnection*) req->data;
        // accept
        uv_write_t* write;
        write = (uv_write_t*) malloc(sizeof *write);
        write->data = req;

        uv_buf_t buf;
        buf.base = (char*) self->message.c_str();
        buf.len = self->message.length();

        uv_write(write, req->handle, &buf, 1, &TcpConnection::WriteCb);

//        client_connected_cb();
    }

    static void WriteCb(uv_write_t* req, int status) {
        std::cout << __FUNCTION__ << status << std::endl;
        uv_stream_t* stream = req->handle;
        uv_connect_t* creq = (uv_connect_t*) req->data;
        free(creq);
        free(req);
        uv_close((uv_handle_t*) stream, ClientCloseCb);
    }

    static void AllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
        buf->base = (char*) malloc(suggested_size);
        buf->len = suggested_size;
    }

    static void ReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
        uv_shutdown_t* sreq;
        TcpConnection* self = (TcpConnection*) stream->data;

        if (nread < 0) {
            if (nread == UV_EOF) {
                free(buf->base);
                uv_close((uv_handle_t*) stream, ClientCloseCb);
                return;
            }

            // todo: error handle

            if (uv_is_writable(stream)) {
                sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
                sreq->data = self;
                uv_shutdown(sreq, stream, AfterShutdownCb);
            }

            // close or stop read on error

            return;
        }

        std::stringstream ss;
        for (int i = 0; i < nread; i++) {
            char c = buf->base[i];
            ss << c;
        }

        if (nread != 0) {
            std::cout << "RECV2:" << nread << uv_is_writable(stream) << " " << ss.str() << std::endl;
        }

        self->on_data_received_cb(ss.str());
    }

    static void AfterShutdownCb(uv_shutdown_t* req, int status) {
        std::cout << __FUNCTION__ << std::endl;
        uv_close((uv_handle_t*) req->handle, ClientCloseCb);
        free(req);
    }

    static void ClientCloseCb(uv_handle_t* handle) {
        std::cout << __FUNCTION__ << std::endl;
        TcpConnection* self = (TcpConnection*) handle->data;
        free(handle);

        for (auto cb : self->on_conn_closed) {
            cb();
        }
    }

};


class TcpServer {
    using OnNewConnectionCb = std::function<void(TcpConnectionId, TcpConnection*)>;
public:
    TcpServer(uv_loop_t* loop_)
        : uvloop(loop_)
    {
        new_conn_listener = [](TcpConnectionId, TcpConnection*) { };
    }

    void Listen(const char* address, int port) {
        int res;

        uv_tcp_init(uvloop, &server);

        struct sockaddr_in server_address;
        uv_ip4_addr(address, port, &server_address);

        uv_tcp_bind(&server, (const struct sockaddr*) &server_address, 0);

        server.data = this;

        uv_listen((uv_stream_t*) &server, 128, &TcpServer::OnConnected);
    }

    void Process() {

    }

    void OnNewConnection(OnNewConnectionCb f) {
        new_conn_listener = f;
    }

private:
    uv_loop_t* uvloop;
    uv_tcp_t server;

    std::map<TcpConnectionId, TcpConnection*> clients;

    OnNewConnectionCb new_conn_listener;

    inline static TcpConnectionId clientIds = 0;

    static int NextClientId() {
        return ++clientIds;
    }

private:
    static void OnConnected(uv_stream_t* server, int status) {
        TcpServer* self = (TcpServer*) server->data;

        uv_stream_t* stream;

        // tcp
        stream = (uv_stream_t*) malloc(sizeof(uv_tcp_t));
        uv_tcp_init(self->uvloop, (uv_tcp_t*)stream);

        uv_accept(server, stream);

        TcpConnectionId id = self->NextClientId();
        TcpConnection* client = new TcpConnection(id, stream);
        self->clients[id] = client;

        stream->data = client;

        self->new_conn_listener(id, client);

        client->Read();
    }

};




















uv_loop_t* uvloop;


static void echo_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

static void echo_write(uv_write_t *req, int status) {
  if (status == -1) {
    fprintf(stderr, "Write error!\n");
  }
  char *base = (char*) req->data;
  free(base);
  free(req);
}

static void close_client_cb(uv_handle_t* handle) {
    std::cout << __FUNCTION__ << std::endl;
    free(handle);
}

static void after_shutdown(uv_shutdown_t* req, int status) {
    std::cout << __FUNCTION__ << std::endl;
    uv_close((uv_handle_t*) req->handle, close_client_cb);
    free(req);
}

static void after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    uv_shutdown_t* sreq;

    if (nread == 0) {
        free(buf->base);
        return;
    }

    if (nread < 0) {
        if (nread == UV_EOF) {
            free(buf->base);
            uv_close((uv_handle_t*) stream, close_client_cb);
            return;
        }

        // error handle

        if (uv_is_writable(stream)) {
            sreq = (uv_shutdown_t*) malloc(sizeof* sreq);
            uv_shutdown(sreq, stream, after_shutdown);
        }

        std::cout << "Error:" << nread << " " << EAGAIN << " " << UV_EOF << " " << EWOULDBLOCK << std::endl;
        return;
    }

    std::stringstream ss;
    for (int i = 0; i < nread; i++) {
        char c = buf->base[i];
        ss << c;
    }

    if (nread != 0) {
        std::cout << "RECV:" << nread << uv_is_writable(stream) << " " << ss.str() << std::endl;
    }
}


static void connection_cb(uv_stream_t * server, int status) {
    uv_stream_t* stream;

    // tcp
    stream = (uv_stream_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uvloop, (uv_tcp_t*)stream);
    stream->data = server;

    uv_accept(server, stream);

    uv_read_start(stream, echo_alloc, after_read);
}









void f(int a, int b, int c) {
    std::cout << __FUNCTION__ << a << b << c << std::endl;
}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "imgui_example Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    uvloop = uv_default_loop();

    std::set<TcpSocket2::SocketId> server_clients;

    TcpServer2* server = new TcpServer2(uvloop);
    server->OnConnection([&](TcpSocket2* conn) {
        conn->SetName("server side");

        server_clients.insert(conn->GetSocketId());

        conn->OnData([](std::string str) {
            std::cout << "SRV: ondata:" << str << std::endl;
        });

        conn->OnClose([=, &server_clients]() {
            std::cout << "SRV: OnClose:" << conn->GetSocketId() << std::endl;
            server_clients.erase(conn->GetSocketId());
        });

//        conn->Write("Hello, welcome to the server!");
    });
    server->Listen("0.0.0.0", 3001);


//    TcpServer tcp_server(uvloop);

//    uv_tcp_t uvserver;
//    uv_tcp_init(uvloop, &uvserver);

//    struct sockaddr_in server_address;
//    int portnum = 3000;
//    uv_ip4_addr("0.0.0.0", portnum, &server_address);

//    uv_tcp_bind(&uvserver, (const struct sockaddr*) &server_address, 0);

//    uv_listen((uv_stream_t *) &uvserver, 128, connection_cb);


//    uv_run(uvloop, UV_RUN_NOWAIT);

//    tcp_server.Listen("localhost", 3001);


    std::set<TcpSocket2*> sockets;



    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }



        {
            uv_run(uvloop, UV_RUN_NOWAIT);
        }


        {
            ImGui::Begin("Tcp Server");

            static int srvport = 3001;
            ImGui::InputInt("srvport", &srvport);

            if (server->IsListening()) {
                ImGui::LabelText("", "server is listening");
            }
            else {
                if (ImGui::Button("Start server")) {
                    server->Listen("0.0.0.0", 3001);
                }
            }

            for (auto& v : server_clients) {
                ImGui::LabelText("client", "socket:%d", v);
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Open Connection");

            if (ImGui::Button("Open connection")) {
                TcpSocket2* socket = new TcpSocket2(uvloop);
                socket->Connect("0.0.0.0", 3001);
                sockets.insert(socket);
            }

            ImGui::End();
        }

        {

            {
                ImGui::Begin("Client");

                for (auto it = sockets.begin(); it != sockets.end(); ++it) {
                    TcpSocket2* socket = *it;
                    ImGui::LabelText("socket", "socket:%d", socket->GetSocketId());
                }

                ImGui::End();
            }
        }


//        {
//            bool bres = ImGui::Begin("Tcp client");
//            std::cout << bres << std::endl;


//            static std::string ipv4 = "0.0.0.0";
//            ImGui::InputText("srvaddr", &ipv4);
//            static int port = 3001;
//            ImGui::InputInt("srvport", &port);
//            static std::string msg = "";
//            ImGui::InputText("msg", &msg);

//            if (ImGui::Button("send local")) {
////                TcpConnection* conn = new TcpConnection(uvloop);
////                conn->SetMessage(msg);

//                TcpSocket2* socket = new TcpSocket2(uvloop);
//                socket->SetName("client side");
//                socket->Connect("0.0.0.0", 3001);
//                socket->OnClose([socket]() {
//                    std::cout << socket->GetName() << " closed" << std::endl;
//                });
//                socket->OnConnect([socket]() {
//                    socket->Write("message from client to server");
//                });
//            }

//            ImGui::End();
//        }












        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
